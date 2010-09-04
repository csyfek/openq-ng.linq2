#include "c2s.h"
#include "icqmain.h"
#include "proto.h"
#include "debug.h"


bool C2S::init()
{
	ICQ_LOG("Connecting to c2s...\n");

	return connect(icqMain.option.c2s_ip, icqMain.option.c2s_port, 30);
}

void C2S::deliver(uint32 ip, uint16 port, uint16 cmd, const char *data, int n)
{
	// packet header
	OutPacket out;
	out.write32(ip);
	out.write16(port);
	out << cmd;

	ICQ_LOG("Deliver packet to c2s\n");
	sendPacket(out, data, n);
}

void C2S::onAddSession(const char *name, InPacket &in)
{
	ICQ_LOG("Add session %s\n", name);

	uint32 id;
	in >> id;
	uint32 ip = in.read32();
	uint16 port = in.read16();

	// A new user logged in
	ClientSession *s = new ClientSession(id, name, ip, port);
	icqMain.addSession(s, in);
}

void C2S::onRemoveSession(const char *name, InPacket &in)
{
	ICQ_LOG("Remove session %s\n", name);

	// User logged out, remove it
	icqMain.removeSession(name);
}

void C2S::onDeliverPacket(const char *name, InPacket &in)
{
	uint16 cmd;
	in >> cmd;

	// Received session packet from c2s
	icqMain.onPacketReceived(name, cmd, in);
}

void C2S::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	// All packets from c2s begin with cmd + session_name
	ICQ_STR name;
	uint16 cmd;
	in >> cmd >> name;

	switch (cmd) {
	case C2S_ADD_SESSION:
		onAddSession(name.text, in);
		break;

	case C2S_REMOVE_SESSION:
		onRemoveSession(name.text, in);
		break;

	case C2S_DELIVER:
		onDeliverPacket(name.text, in);
		break;

	default:
		ICQ_LOG("unknown c2s command\n");
	}
}
