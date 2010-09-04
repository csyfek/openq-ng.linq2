#include "config.h"
#include "session.h"
#include "c2s.h"
#include "debug.h"
#include "proto.h"
#include "realserver.h"
#include "dbconn.h"
#include "sqlstmt.h"
#include <malloc.h>
#include <ctype.h>


/*
 * +-----------------+
 * +  C2S_PACKET     +
 * +-----------------+
 * +  C2S_HEADER     +
 * +-----------------+
 * +      data       +
 * +-----------------+
 */
#define C2S_PACKET_SIZE(n)	(sizeof(C2S_PACKET) + sizeof(C2S_HEADER) + n)

#define C2S_VER				1
// Client should send CMD_KEEPALIVE within every 3 minutes
#define KA_TIMEOUT			(3 * 60)
// Maximum resend attempts before it is dropped
#define MAX_SEND_ATTEMPTS	2

#define MAX_ANNO_MSG		8

// Login reply
enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_USER,
	LOGIN_WRONG_PASSWD,
	LOGIN_ERROR_UNKNOWN,
};

// Registration reply
enum {
	REG_SUCCESS,
	REG_USER_EXISTS,
	REG_ERROR_UNKNOWN,
};

struct ANNO_MSG {
	string from;
	time_t when;
	string text;
};


static ANNO_MSG annoMsg[MAX_ANNO_MSG];
static int lastMsg = -1;

int Session::numSessions = 0;


/*
 * Get user id by its name
 */
static uint32 getUserID(DBConn *conn, ICQ_STR &name)
{
	DBResult *res;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT id FROM user_tbl WHERE name=");
	sql << name;
	if (!conn->query(sql.getData(), sql.getLength()) ||
		!(res = conn->getResult()))
		return 0;

	DB_ROW row = res->fetchRow();
	uint32 id = (row ? atol(row[0]) : 0);
	res->destroy();

	return id;
}


Session::Session(C2S_HEADER *header)
{
	sessionID = header->sid;

	// send sequence can be randomly selected, but why waste time doing it? :-)
	recvSeq = sendSeq = header->seq;

	realServer = NULL;

	*userName = '\0';
	userID = 0;
	sessionIP = 0;
	sessionPort = 0;
	seqWindow = 0;
	isDead = true;
	isOnline = false;
	lastTimeDirty = false;

	expiry = c2s.curTime + KA_TIMEOUT;

	// Add it to the list, so we could remove it if timeouts
	c2s.sessionList.add(&listItem);
}

Session::~Session()
{
	// Clean up send queue
	while (!sendQueue.isEmpty()) {
		C2S_PACKET *p = LIST_ENTRY(sendQueue.removeHead(), C2S_PACKET, sendItem);
		p->globalSendItem.remove();
		free(p);
	}

	// Remove from the list and hash
	listItem.remove();
	hashItem.remove();

	// Remove from the real server
	if (realServer)
		realServer->removeSession(this);

	if (isOnline) {
		numSessions--;
		saveLastTime();
	}
}

void Session::saveLastTime()
{
	if (!lastTimeDirty)
		return;

	SQLStmt sql(c2s.dbConn);
	SQL_APPEND(sql, "UPDATE user_tbl SET lasttime=");
	sql << (uint32) lastTime;
	SQL_APPEND(sql, " WHERE id=");
	sql << userID;

	c2s.dbConn->query(sql.getData(), sql.getLength());
}

/*
 * Determine if seq is duplicated
 */
bool Session::setSeq(uint16 seq)
{
#define SEQ_WIN_SIZE	(sizeof(seqWindow) * 8)
#define MAX_SEQ			0x8000

	if (seq >= MAX_SEQ)
		return false;

	int d = seq - recvSeq;
	if (d < 0)
		d += MAX_SEQ;	// Wrap it around
	if (d > SEQ_WIN_SIZE) {
		ICQ_LOG("Error packet sequence(recv=%d, seq=%d)\n", recvSeq, seq);
		return false;
	}

	uint32 mask = (1 << d);
	if (seqWindow & mask) {
		ICQ_LOG("packet %d is duplicated\n", seq);
		return false;
	}

	seqWindow |= mask;

	// Slide the window
	while (seqWindow & 0x01) {
		recvSeq = (recvSeq + 1) & (MAX_SEQ - 1);
		seqWindow >>= 1;
	}
	return true;
}

void Session::fillHeader(C2S_HEADER *header, uint16 cmd, uint16 seq)
{
	header->ver = htons(C2S_VER);
	header->reserved = 0;
	header->sid = htonl(sessionID);
	header->seq = htons(seq);
	header->cmd = htons(cmd);
}

bool Session::deliverDirect(const char *data, int n)
{
	return c2s.sendPacket(data, n, sessionIP, sessionPort);
}

void Session::deliver(uint16 cmd, const char *data, int n)
{
	ICQ_ASSERT(n >= 0);

	C2S_PACKET *p = (C2S_PACKET *) malloc(C2S_PACKET_SIZE(n));

	// Initialize structure
	p->size = sizeof(C2S_HEADER) + n;
	p->seq = ++sendSeq;
	p->attempts = MAX_SEND_ATTEMPTS;
	p->expiry = c2s.curTime + SEND_TIMEOUT;
	p->session = this;

	C2S_HEADER *header = (C2S_HEADER *) (p + 1);
	fillHeader(header, cmd, p->seq);

	if (n > 0)
		memcpy(header + 1, data, n);

	// Really send it out
	if (!deliverDirect(p)) {
		ICQ_LOG("session deliver failed\n");
		free(p);
		return;
	}

	// Queue it so that we can resend it if time outs
	sendQueue.add(&p->sendItem);
	c2s.sendQueue.add(&p->globalSendItem);
}

/*
 * Send an ack packet
 */
void Session::sendAck(uint16 seq)
{
	char data[sizeof(C2S_HEADER) + sizeof(numSessions)];

	C2S_HEADER *header = (C2S_HEADER *) data;
	fillHeader(header, CMD_ACK, seq);

	// Should it be here?
	*(uint32 *) (header + 1) = htonl(numSessions);

	deliverDirect(data, sizeof(data));
}

/*
 * Called when an ack packet is received
 */
void Session::onAck(uint16 seq)
{
	ListHead *pos;

	LIST_FOR_EACH(pos, &sendQueue) {
		C2S_PACKET *p = LIST_ENTRY(pos, C2S_PACKET, sendItem);
		if (p->seq == seq) {
			ICQ_LOG("packet(seq=%d) is acked\n", seq);

			// Remove and delete it
			pos->remove();
			p->globalSendItem.remove();
			free(p);

			// We are here only waiting for an ack?
			if (isDead) {
				ICQ_LOG("session ends\n");
				delete this;
			}
			return;
		}
	}

	ICQ_LOG("ack packet %d is ignored\n", seq);
}

void Session::onKeepAlive(InPacket &in)
{
	expiry = c2s.curTime + KA_TIMEOUT;

	// Append it to the end of the session list
	listItem.remove();
	c2s.sessionList.add(&listItem);

	// Check announce message
	if (lastMsg < 0 || annoMsg[lastMsg].when <= lastTime)
		return;

	OutPacket out;
	int i = lastMsg;
	do {
		ANNO_MSG *msg = annoMsg + i;
		if (lastTime >= msg->when)
			break;

		out.reset();
		out << msg->from.c_str() << (uint32) msg->when;
		out << (uint8) MSG_ANNOUNCE << msg->text.c_str();
		deliver(CMD_SRV_MESSAGE, out.data, out.getLength());

		if (--i < 0)
			i = MAX_ANNO_MSG - 1;
	} while (i != lastMsg);

	lastTimeDirty = true;
	lastTime = annoMsg[lastMsg].when;
}

bool Session::onMessage(InPacket &in)
{
	ICQ_STR to;
	uint8 type;
	ICQ_STR text;

	in >> to >> type >> text;
	if (to.len > 0 || type != MSG_TEXT) {
		in.reset();
		return false;
	}

	string &admin = c2s.option.adminUser;
	if (admin.empty() || admin != userName) {
		ICQ_LOG("%s has no privilege to announce\n", userName);
		return true;
	}

	if (++lastMsg >= MAX_ANNO_MSG)
		lastMsg = 0;

	ANNO_MSG *msg = annoMsg + lastMsg;
	msg->from = userName;
	msg->when = c2s.curTime;
	msg->text = text.text;
	return true;
}

void Session::onLogin(InPacket &in)
{
	ICQ_STR name, passwd;
	in >> name >> passwd;

	if (name.len <= 0 || name.len > MAX_NAME_LEN) {
		ICQ_LOG("Invalid user name length\n");
		return;
	}

	// Authentication...
	DBConn *conn = c2s.dbConn;
	DBResult *res;
	DB_ROW row;
	uint8 error;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT id, passwd, lasttime FROM user_tbl WHERE name=");
	sql << name;

	if (!conn->query(sql.getData(), sql.getLength()) || !(res = conn->getResult()))
		error = LOGIN_ERROR_UNKNOWN;
	else {
		if (!(row = res->fetchRow()))
			error = LOGIN_INVALID_USER;
		else if (strcmp(passwd.text, row[1]) != 0)
			error = LOGIN_WRONG_PASSWD;
		else {
			error = LOGIN_SUCCESS;
			userID = atol(row[0]);
			lastTime = atol(row[2]);
		}
		res->destroy();
	}

	OutPacket out;
	out << error;
	// Append the client ip from our point of view(that is, the client's outer ip)
	out.write32(sessionIP);

	deliver(CMD_LOGIN, out.data, out.getLength());

	if (error != LOGIN_SUCCESS) {
		ICQ_LOG("%s logged in failed\n", name.text);
		return;
	}

	// By now, we are successfully logged in
	ICQ_LOG("%s logged in\n", name.text);

	numSessions++;
	isOnline = true;
	isDead = false;

	// Convert user name to lower case
	const char *src = name.text;
	char *dst = userName;
	while (*src)
		*dst++ = tolower(*src++);
	*dst = '\0';

	// Choose a server for this session
	realServer = c2s.chooseServer();
	if (!realServer) {
		ICQ_LOG("Can not choose a server to route packets to\n");
		return;
	}

	realServer->addSession(this, in.cursor, in.getBytesLeft());
}

void Session::onLogout(InPacket &in)
{
	ICQ_LOG("%s logged out\n", userName);
	delete this;
}

void Session::onRegister(InPacket &in)
{
	if (!c2s.option.enableReg) {
		ICQ_LOG("Register is currently disabled\n");
		return;
	}

	ICQ_STR name, passwd;
	in >> name >> passwd;

	if (name.len <= 0 || name.len > MAX_NAME_LEN) {
		ICQ_LOG("Invalid user name length %d\n", name.len);
		return;
	}
	// MD5 hashed string is always 16 bytes
	if (passwd.len != 32) {
		ICQ_LOG("Password does not seem to be MD5-hashed\n");
		return;
	}

	if (!c2s.checkUserName(name.text))
		return;

	// Insert a new record into db
	DBConn *conn = c2s.dbConn;
	uint8 error = REG_ERROR_UNKNOWN;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "INSERT INTO user_tbl (name, passwd) VALUES(");
	sql << name << ',' << passwd << ')';

	if (!conn->query(sql.getData(), sql.getLength()))
		error = REG_USER_EXISTS;
	else if (conn->getAffectedRows())
		error = REG_SUCCESS;

	OutPacket out;
	out << error;
	deliver(CMD_REGISTER, out.data, out.getLength());

	if (error != REG_SUCCESS) {
		ICQ_LOG("%s register failed\n", name.text);
		return;
	}

	// By now, we are successfully registered
	ICQ_LOG("%s registered\n", name.text);

	uint32 id = conn->lastInsertID();
	// Retrieve the last insert id by name, if not supported
	if (!id)
		id = getUserID(conn, name);

	// Insert corresponding records into the two other tables
	sql.reset();
	SQL_APPEND(sql, "INSERT INTO user_basic (id) VALUES(");
	sql << id << ')';
	conn->query(sql.getData(), sql.getLength());

	sql.reset();
	SQL_APPEND(sql, "INSERT INTO user_ext (id) VALUES(");
	sql << id << ')';
	conn->query(sql.getData(), sql.getLength());
}

void Session::onPacketReceived(InPacket &in, C2S_HEADER *header)
{
	// To prevent spoofing
	if (header->sid != sessionID) {
		ICQ_LOG("session id %x(cmd=%d) does not match\n", header->sid, header->cmd);
		return;
	}
	if (header->cmd == CMD_ACK) {
		onAck(header->seq);
		return;
	}

	sendAck(header->seq);
	if (!setSeq(header->seq))
		return;

	if (isOnline) {
		switch (header->cmd) {
		case CMD_KEEPALIVE:
			onKeepAlive(in);
			break;

		case CMD_LOGOUT:
			onLogout(in);
			break;

		case CMD_MESSAGE:
			if (onMessage(in))
				break;

		default:
			if (realServer)
				realServer->deliver(this, header->cmd, in.cursor, in.getBytesLeft());
		}
	} else {
		switch (header->cmd) {
		case CMD_LOGIN:
			onLogin(in);
			break;

		case CMD_REGISTER:
			onRegister(in);
			break;
		}
	}
}
