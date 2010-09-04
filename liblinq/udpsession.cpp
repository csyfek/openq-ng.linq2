/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2003 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#include "udpsession.h"
#include "socket.h"
#include "md5.h"
#include "icqlog.h"


#define SEQ_MASK	0x7fff

#define UDP_VER		1
#define TCP_VER		1

// UDP protocol commands...
enum {
	CMD_ACK = 1,
	CMD_KEEPALIVE,
	CMD_REGISTER,
	CMD_LOGIN,
	CMD_LOGOUT,

	CMD_CHANGE_STATUS = 0x1000,
	CMD_ADD_CONTACT,
	CMD_DEL_CONTACT,
	CMD_MESSAGE,
	CMD_SEARCH,
	CMD_SEARCH_RANDOM,
	CMD_GET_CONTACT_INFO,
	CMD_GET_USER_INFO,
	CMD_UPDATE_USER_INFO,
	CMD_GET_CONTACT_LIST,
	CMD_CHANGE_PASSWD,

	// s2c or s2s commands
	CMD_SRV_USER_ONLINE = 0x2000,
	CMD_SRV_USER_OFFLINE,
	CMD_SRV_USER_STATUS,
	CMD_SRV_PROBE_STATUS,
	CMD_SRV_SEARCH_RESULT,
	CMD_SRV_ADD_CONTACT,
	CMD_SRV_DEL_CONTACT,
	CMD_SRV_CONTACT_AUTH,
	CMD_SRV_MESSAGE,
};


// Hash a string to md5
static void md5String(char result[33], const char *str)
{
	static char table[] = "0123456789abcdef";

	md5_context ctx;
	uint8 digest[16];

	md5_starts(&ctx);
	md5_update(&ctx, (uint8 *) str, strlen(str));
	md5_finish(&ctx, digest);

	for (int i = 0; i < 16; i++) {
		*result++ = table[digest[i] >> 4];
		*result++ = table[digest[i] & 0xf];
	}
	*result = '\0';
}

// Read contact info from packet
static void readContactInfo(InPacket &in, CONTACT_INFO &info)
{
	in >> info.nick >> info.gender >> info.birth >> info.email;
	in >> info.country >> info.city >> info.address;
	in >> info.postcode >> info.tel >> info.mobile;
	in >> info.realname >> info.occupation >> info.homepage >> info.intro;
}


UDPSession::UDPSession(SessionListener *l)
{
	listener = l;
	udpSocket = NULL;
	realIP = ourIP = 0;

	reset();
}

UDPSession::~UDPSession()
{
	clearSendQueue();

	if (udpSocket)
		delete udpSocket;
}

void UDPSession::connect(Socket *sock, const char *host, int port)
{
	if (udpSocket)
		delete udpSocket;

	udpSocket = sock;
	udpSocket->create(SOCK_DGRAM, this);

	udpSocket->addEvent(Socket::WRITE);
	udpSocket->addEvent(Socket::EXCEPTION);

	udpSocket->connect(host, port);
}

void UDPSession::reset()
{
	numClients = 0;

	sessionID = ((rand() << 16) | (rand() & 0xffff));
	sendSeq = (rand() & SEQ_MASK);
	recvSeq = 0;
	memset(seqWindow, 0, sizeof(seqWindow));

	clearSendQueue();
}

void UDPSession::clearSendQueue()
{
	list<UDPOutPacket *>::iterator it;
	for (it = sendQueue.begin(); it != sendQueue.end(); ++it)
		delete *it;

	sendQueue.clear();
}

// Check whether a seq is duplicated
bool UDPSession::checkSeq(uint16 seq)
{
	uint8 &byte = seqWindow[seq / 8];
	uint8 mask = (1 << (seq % 8));
	if (byte & mask)
		return false;

	byte |= mask;
	return true;
}

// Check if there are expired packets
void UDPSession::checkSendQueue()
{
	time_t now = time(NULL);

	while (!sendQueue.empty()) {
		UDPOutPacket *out = sendQueue.front();

		if (out->expiry > now)
			break;

		sendQueue.pop_front();

		out->attempts++;
		if (out->attempts >= MAX_SEND_ATTEMPTS) {
			ICQ_LOG("packet %d failed\n", out->seq);

			if (out->cmd == CMD_KEEPALIVE || out->cmd == CMD_LOGIN)
				listener->onConnect(false);
			else
				listener->onSendError(out->seq);

			delete out;

		} else {
			// Resend it
			sendDirect(out);

			out->expiry = now + SEND_TIMEOUT;
			sendQueue.push_back(out);
		}
	}
}

// Create a packet and fill its header
void UDPSession::createPacket(UDPOutPacket &out, uint16 cmd, uint16 seq)
{
	out.expiry = time(NULL) + SEND_TIMEOUT;
	out.cmd = cmd;
	out.seq = seq;

	out << (uint16) UDP_VER << (uint32) 0;
	out << sessionID << seq << cmd;
}

UDPOutPacket *UDPSession::createPacket(uint16 cmd)
{
	UDPOutPacket *out = new UDPOutPacket;

	sendSeq = (sendSeq + 1) & SEQ_MASK;
	createPacket(*out, cmd, sendSeq);
	return out;
}

void UDPSession::sendPacket(UDPOutPacket *out)
{
	sendDirect(out);
	sendQueue.push_back(out);
}

void UDPSession::sendAck(uint16 seq)
{
	UDPOutPacket out;
	createPacket(out, CMD_ACK, seq);
	sendDirect(&out);
}

void UDPSession::sendDirect(OutPacket *out)
{
	udpSocket->send(out->getData(), out->getLength());
}

void UDPSession::sendKeepAlive()
{
	UDPOutPacket *out = createPacket(CMD_KEEPALIVE);
	sendPacket(out);
}

void UDPSession::registerUser(const char *name, const char *passwd)
{
	ICQ_LOG("register %s...\n", name);
	reset();

	char buf[33];
	md5String(buf, passwd);

	UDPOutPacket *out = createPacket(CMD_REGISTER);
	*out << name << buf;
	sendPacket(out);
}

void UDPSession::login(const char *name, const char *passwd, uint32 status, uint16 port)
{
	reset();

	char buf[33];
	md5String(buf, passwd);

	UDPOutPacket *out = createPacket(CMD_LOGIN);
	*out << name << buf << status << (uint16) TCP_VER;
	*out << realIP << port;

	sendPacket(out);
}

void UDPSession::logout()
{
	UDPOutPacket out;
	createPacket(out, CMD_LOGOUT, ++sendSeq);
	sendDirect(&out);
}

void UDPSession::changeStatus(uint32 status)
{
	UDPOutPacket *out = createPacket(CMD_CHANGE_STATUS);
	*out << status;
	sendPacket(out);
}

void UDPSession::sendMessage(uint8 type, const char *to, const char *text)
{
	UDPOutPacket *out = createPacket(CMD_MESSAGE);
	*out << to << type << text;
	sendPacket(out);
}

void UDPSession::searchRandom()
{
	UDPOutPacket *out = createPacket(CMD_SEARCH_RANDOM);
	sendPacket(out);
}

void UDPSession::searchUser(const char *name, const char *nick, const char *email)
{
	UDPOutPacket *out = createPacket(CMD_SEARCH);
	*out << name << nick << email;
	sendPacket(out);
}

void UDPSession::addContact(const char *name)
{
	UDPOutPacket *out = createPacket(CMD_ADD_CONTACT);
	*out << name;
	sendPacket(out);
}

void UDPSession::delContact(const char *name)
{
	UDPOutPacket *out = createPacket(CMD_DEL_CONTACT);
	*out << name;
	sendPacket(out);
}

void UDPSession::getContactList()
{
	UDPOutPacket *out = createPacket(CMD_GET_CONTACT_LIST);
	sendPacket(out);
}

void UDPSession::getContactInfo(const char *name)
{
	UDPOutPacket *out = createPacket(CMD_GET_CONTACT_INFO);
	*out << name;
	sendPacket(out);
}

void UDPSession::getUserInfo()
{
	UDPOutPacket *out = createPacket(CMD_GET_USER_INFO);
	sendPacket(out);
}

void UDPSession::updateUserInfo(USER_INFO &info)
{
	UDPOutPacket *out = createPacket(CMD_UPDATE_USER_INFO);

	*out << info.auth << info.nick << info.gender << info.birth;
	*out << info.email << info.country << info.city << info.address;
	*out << info.postcode << info.tel << info.mobile;
	*out << info.realname << info.occupation << info.homepage << info.intro;

	sendPacket(out);
}

void UDPSession::changePasswd(const char *passwd)
{
	char buf[33];
	md5String(buf, passwd);

	UDPOutPacket *out = createPacket(CMD_CHANGE_PASSWD);
	*out << buf;
	sendPacket(out);
}

void UDPSession::onSocketRead()
{
	char buf[MAX_PACKET_SIZE];

	int n = udpSocket->receive(buf, sizeof(buf));
	if (n < (int) sizeof(UDP_PACKET_HDR))
		return;

	UDPInPacket in(buf, n);
	onPacketReceived(in);
}

void UDPSession::onSocketWrite()
{
	udpSocket->removeEvent(Socket::WRITE);
	udpSocket->addEvent(Socket::READ);

	int fd = udpSocket->getFd();
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	getsockname(fd, (sockaddr *) &addr, &len);

	realIP = ntohl(addr.sin_addr.s_addr);

	ICQ_LOG("my realIP is %s\n", inet_ntoa(addr.sin_addr));

	// We are connected to server
	listener->onConnect(true);
}

void UDPSession::onSocketException()
{
	// An error occured
	udpSocket->removeEvent(Socket::WRITE);
	
	listener->onConnect(false);
}

void UDPSession::onPacketReceived(UDPInPacket &in)
{
	if (sessionID != in.header.sid) {
		ICQ_LOG("session id does not match(sid = %08x, cmd = %d).\n",
			in.header.sid, in.header.cmd);
		return;
	}

	uint16 seq = in.header.seq;
	uint16 cmd = in.header.cmd;

	if (cmd == CMD_ACK) {
		onAck(in);
		return;
	}

	sendAck(seq);

	if (!checkSeq(seq)) {
		ICQ_LOG("packet %d is ignored.\n", seq);
		return;
	}

	switch (cmd) {
	case CMD_REGISTER:
		onRegisterReply(in);
		break;

	case CMD_LOGIN:
		onLoginReply(in);
		break;

	case CMD_ADD_CONTACT:
		onAddContactReply(in);
		break;

	case CMD_GET_CONTACT_LIST:
		onContactListReply(in);
		break;

	case CMD_SRV_USER_ONLINE:
		onUserOnline(in);
		break;

	case CMD_SRV_USER_OFFLINE:
		onUserOffline(in);
		break;

	case CMD_SRV_USER_STATUS:
		onUserStatus(in);
		break;

	case CMD_SRV_SEARCH_RESULT:
		onSearchResult(in);
		break;

	case CMD_SRV_MESSAGE:
		onRecvMessage(in);
		break;

	case CMD_GET_CONTACT_INFO:
		onContactInfoReply(in);
		break;

	case CMD_GET_USER_INFO:
		onUserInfoReply(in);
		break;
	}
}

void UDPSession::onAck(UDPInPacket &in)
{
	uint16 seq = in.header.seq;

	list<UDPOutPacket *>::iterator it;

	for (it = sendQueue.begin(); it != sendQueue.end(); ++it) {
		UDPOutPacket *p = *it;
		if (p->seq == seq) {
			ICQ_LOG("packet %d is acked.\n", seq);

			sendQueue.erase(it);
			delete p;

			in >> numClients;
			listener->onAck(seq);
			return;
		}
	}

	ICQ_LOG("Ack %d is ignored.\n", seq);
}

void UDPSession::onRegisterReply(UDPInPacket &in)
{
	uint8 error;
	const char *name;

	in >> error >> name;

	listener->onRegisterReply(error, name);
}

void UDPSession::onLoginReply(UDPInPacket &in)
{
	uint8 error;
	const char *domain;
	in >> error >> domain >> ourIP;

	in_addr addr;
	addr.s_addr = htonl(ourIP);
	ICQ_LOG("ourIP is %s.\n", inet_ntoa(addr));

	listener->onLoginReply(error, domain);
}

void UDPSession::onRecvMessage(UDPInPacket &in)
{
	ICQ_MSG msg;
	uint32 when;
	in >> msg.contact >> when >> msg.type >> msg.text;

	msg.when = when;
	msg.flags = MF_RELAY | MF_FROM;

	listener->onRecvMessage(msg);
}

void UDPSession::onUserOnline(UDPInPacket &in)
{
	ONLINE_INFO info;

	in >> info.name >> info.status;
	in >> info.tcpver >> info.ip >> info.realip >> info.msgport;

	listener->onUserOnline(info);
}

void UDPSession::onUserOffline(UDPInPacket &in)
{
	const char *name;
	in >> name;

	listener->onUserOffline(name);
}

void UDPSession::onUserStatus(UDPInPacket &in)
{
	const char *name;
	uint32 status;

	in >> name >> status;
	listener->onUserStatus(name, status);
}

void UDPSession::onSearchResult(UDPInPacket &in)
{
	uint16 n;
	in >> n;

	SEARCH_RESULT *results = new SEARCH_RESULT[n];

	for (int i = 0; i < n; i++) {
		SEARCH_RESULT *p = results + i;

		in >> p->name >> p->status >> p->auth;
		in >> p->nick >> p->gender >> p->age;
	}

	listener->onSearchResult(results, n);
	delete []results;
}

void UDPSession::onAddContactReply(UDPInPacket &in)
{
	const char *name;
	uint8 auth;

	in >> name >> auth;
	listener->onAddContactReply(name, auth);
}

void UDPSession::onContactListReply(UDPInPacket &in)
{
	uint16 n;
	in >> n;

	const char **contacts = new const char *[n];

	for (int i = 0; i < n; i++)
		in >> contacts[i];

	listener->onContactListReply(contacts, n);
	delete []contacts;
}

void UDPSession::onContactInfoReply(UDPInPacket &in)
{
	CONTACT_INFO c;

	in >> c.name;
	readContactInfo(in, c);

	listener->onContactInfoReply(c);
}

void UDPSession::onUserInfoReply(UDPInPacket &in)
{
	USER_INFO user;

	in >> user.auth;
	readContactInfo(in, user);

	listener->onUserInfoReply(user);
}
