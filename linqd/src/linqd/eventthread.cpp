#include "eventthread.h"
#include "icqmain.h"
#include "debug.h"


EventItem::EventItem(int e, ClientSession *s, PACKET *p)
{
	event = e;
	session = s;
	packet = NULL;

	// Duplicate packet
	if (p) {
		packet = (PACKET *) memPool.alloc(sizeof(PACKET));
		packet->online = p->online;
		packet->from = memPool.strdup(p->from);
		packet->to = memPool.strdup(p->to);
		packet->cmd = p->cmd;
		if ((packet->dataLen = p->dataLen) <= 0)
			packet->data = NULL;
		else {
			packet->data = (char *) memPool.alloc(p->dataLen);
			memcpy((void *) packet->data, p->data, p->dataLen);
		}
	}
}

bool EventItem::run(Thread *t)
{
	EVENT ev;
	ev.conn = ((EventThread *) t)->dbConn;
	ev.event = event;
	ev.s = session;
	ev.mapi = &icqMain;
	ev.packet = packet;

	// If this packet is a deliver_from one, then route it to its destination
	if (!icqMain.dispatchEvent(&ev) && packet && packet->to && event == EV_S_OUT)
		icqMain.deliver(packet);

	// addRef() by SessionHash::get()
	if (session)
		session->release();

	return true;	// Not a quit signal
}


EventThread::EventThread()
{
	dbConn = NULL;
}

EventThread::~EventThread()
{
	exit();

	if (dbConn)
		dbConn->destroy();
}

bool EventThread::init(DBModule *db, DB_INFO *info)
{
	dbConn = db->getConn();
	if (!dbConn)
		return false;

	if (!dbConn->init(
		info->host.c_str(),
		info->user.c_str(),
		info->passwd.c_str(),
		info->db.c_str(),
		info->port))
		return false;

	return true;
}
