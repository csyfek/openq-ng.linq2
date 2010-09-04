#ifndef _ICQ_MAIN_H
#define _ICQ_MAIN_H

#include "mapi.h"
#include "servicemain.h"
#include "configparser.h"
#include "socket.h"
#include "c2s.h"
#include "sessionhash.h"
#include "packet.h"
#include "modulelist.h"
#include "option.h"
#include "dbmodule.h"
#include "eventthread.h"
#include "loggerimpl.h"
#include "poolimpl.h"


#define MAX_NUM_MODULES		64
#define NUM_THREADS			10


/*
 * The main class for this service
 */
class ICQMain : public MAPI, public ServiceMain, public ConfigListener {
public:
	ICQMain();
	~ICQMain();

	// Interfaces in MAPI
	virtual time_t getTime() { return curTime; }
	virtual Logger *getLogger() { return logger; }
	virtual SocketRegistry *getSocketRegistry() {
		return &socketRegistry;
	}
	virtual void registerEvent(int module, int event);
	virtual void deliver(PACKET *p);
	virtual void deliverFrom(PACKET *p);
	virtual void deliverLocal(PACKET *p);

	virtual bool startModule(const char *name);
	virtual bool stopModule(const char *name);
	virtual void getModuleList(char *buf, int n);

	// Interfaces in ServiceMain
	virtual const char *getName() { return "linqd"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	// Interfaces in ConfigListener
	virtual void sectionParsed(Profile *prof);

	Module *getModule(int i) { return modules[i]; }
	int getModuleIndex(const char *name);
	Module *getModule(const char *name);

	void addSession(ClientSession *s, InPacket &in);
	void removeSession(const char *name);
	// Called when c2s has received a packet
	void onPacketReceived(const char *from, uint16 cmd, InPacket &in);
	bool dispatchEvent(EVENT *ev);

	Option option;
	SocketRegistryImpl socketRegistry;
	C2S *c2s;
	LoggerImpl *logger;

private:
	bool initDB();
	void postEvent(int event, ClientSession *s, PACKET *p);
	void startThreads();
	void destroyThreads();

	int addModule(Module *m);
	void removeModule(int i);
	bool startModule(Profile *prof);

	time_t curTime;
	EventThread *eventThreads[NUM_THREADS];
	Module *modules[MAX_NUM_MODULES];
	SessionHash *sessionHash;
	DBModule dbModule;
	ModuleList moduleList[NUM_EVENTS];
	PoolImpl memPool;
};


extern ICQMain icqMain;


#endif
