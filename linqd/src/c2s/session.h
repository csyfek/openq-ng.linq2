#ifndef _SESSION_H
#define _SESSION_H

#include "packet.h"
#include "list.h"
#include <time.h>

#define MAX_NAME_LEN	16

#define SEND_TIMEOUT	7


class Session;

#pragma pack(1)

// Packet header from client
struct C2S_HEADER {
	uint16 ver;
	uint32 reserved;
	uint32 sid;
	uint16 seq;
	uint16 cmd;
};

struct C2S_PACKET {
	ListHead sendItem, globalSendItem;

	int size;		// packet size
	time_t expiry;
	int attempts;	// number of attempts retried
	uint16 seq;
	Session *session;	// The session this packet belongs to. Used to resend it
};

#pragma pack()


class RealServer;

class Session {
public:
	Session(C2S_HEADER *header);
	~Session();

	bool deliverDirect(C2S_PACKET *p) {
		return deliverDirect((const char *) (p + 1), p->size);
	}

	// Called when a new packet arrives
	void onPacketReceived(InPacket &in, C2S_HEADER *header);
	// Deliver packet to the user
	void deliver(uint16 cmd, const char *data, int n);

	ListHead listItem;
	ListHead hashItem;

	char userName[MAX_NAME_LEN + 1];
	uint32 userID;
	uint32 sessionIP;
	uint16 sessionPort;
	time_t expiry;				// When this session expires?
	RealServer *realServer;		// Which server does this session resides on?

private:
	void saveLastTime();
	bool setSeq(uint16 seq);
	void fillHeader(C2S_HEADER *header, uint16 cmd, uint16 seq);
	bool deliverDirect(const char *data, int n);
	void sendAck(uint16 seq);

	// Command handlers...
	void onAck(uint16 seq);
	void onKeepAlive(InPacket &in);
	void onRegister(InPacket &in);
	void onLogin(InPacket &in);
	void onLogout(InPacket &in);
	bool onMessage(InPacket &in);

	// A dead session is one that will be removed if an ACK is received
	bool isDead : 1;
	bool isOnline : 1;		// Has the user logged in?
	bool lastTimeDirty : 1;

	uint32 sessionID;		// Every session has a session id
	uint16 recvSeq;
	uint16 sendSeq;
	uint32 seqWindow;		// A slide window to check packet duplication
	ListHead sendQueue;		// sent but not acked packet list
	time_t lastTime;

	// Number of sessions currently online?
	static int numSessions;
};


#endif
