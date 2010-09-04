#ifndef _EVENT_H
#define _EVENT_H

#include "types.h"


// Event types...
enum {
	EV_S_OUT,		// Packets out of a session
	EV_S_IN,		// Packets route into a session
	EV_S_END,		// Session ends
	NUM_S_EVENTS,

	EV_SESSION = NUM_S_EVENTS,	// Session begins
	EV_DELIVER,					// Deliver packet to a session
	NUM_EVENTS,
};


struct PACKET {
	uint8 online;		// Only deliver to online session?
	const char *from;	// Source and Destination user
	const char *to;
	uint16 cmd;			// Packet command
	const char *data;	// Packet data
	int dataLen;
};


class MAPI;
class DBConn;
class Session;


struct EVENT {
	int event;		// event type
	MAPI *mapi;		// module api
	DBConn *conn;	// db connection of this thread
	Session *s;		// The session assosiated with the packet
	PACKET *packet;
};


#endif
