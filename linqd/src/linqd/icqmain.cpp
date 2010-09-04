#include "config.h"
#include "icqmain.h"
#include "eventthread.h"
#include "proto.h"
#include "debug.h"


/*
 * Select a queue(round-robin)
 */
static int selectQueue()
{
	static int last = -1;

	// I do not want to waste lock on it;-)
	int i = last;
	if (++i >= NUM_THREADS)
		i = 0;
	return (last = i);
}

// session iterator for unregister a module
static void sessionWalker(ClientSession *s, void *data)
{
	s->moduleList.remove((Module *) data);
}


ICQMain icqMain;


ICQMain::ICQMain()
{
	Socket::socketRegistry = &socketRegistry;

	memset(modules, 0, sizeof(modules));

	logger = new LoggerImpl();
	sessionHash = new SessionHash;
	c2s = new C2S;
}

ICQMain::~ICQMain()
{
	destroyThreads();

	for (int i = 0; i < MAX_NUM_MODULES; i++) {
		if (modules[i])
			removeModule(i);
	}

	delete c2s;
	delete sessionHash;
	delete logger;
}

int ICQMain::getModuleIndex(const char *name)
{
	for (int i = 0; i < MAX_NUM_MODULES; i++) {
		Module *m = modules[i];
		if (m && m->name == name)
			return i;
	}
	return -1;
}

Module *ICQMain::getModule(const char *name)
{
	int i = getModuleIndex(name);
	if (i < 0)
		return NULL;

	return getModule(i);
}

void ICQMain::addSession(ClientSession *s, InPacket &in)
{
	PACKET p;
	p.cmd = CMD_LOGIN;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();
	p.from = NULL;
	p.to = NULL;

	// Select an event queue
	s->queue = selectQueue();
	// Increase reference count, so that we should not be delete after processed
	s->addRef();
	// Put it to the hash
	sessionHash->put(s);

	postEvent(EV_SESSION, s, &p);
}

void ICQMain::removeSession(const char *name)
{
	// Remove session from the hash.
	ClientSession *s = sessionHash->remove(name);
	if (!s) {
		ICQ_LOG("No session %s to remove\n", name);
		return;
	}

	// Do not increment reference count, so that after this event,
	// session will be destroyed
	postEvent(EV_S_END, s, NULL);
}

void ICQMain::onPacketReceived(const char *from, uint16 cmd, InPacket &in)
{
	ClientSession *s = sessionHash->get(from);
	if (!s)
		return;

	PACKET p;
	p.cmd = cmd;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();
	p.from = NULL;
	p.to = NULL;

	postEvent(EV_S_OUT, s, &p);
}

int ICQMain::addModule(Module *m)
{
	// Find an unused slot
	for (int i = 0; i < MAX_NUM_MODULES; i++) {
		if (!modules[i]) {
			modules[i] = m;
			return i;
		}
	}
	return -1;
}

void ICQMain::removeModule(int i)
{
	Module *m = getModule(i);

	ICQ_ASSERT(m != NULL);

	delete m;
	modules[i] = NULL;
}

bool ICQMain::initDB()
{
	string name = LINQ_LIB_DIR"/db/";
	name += option.db_impl;

	if (!dbModule.load(name.c_str())) {
		logger->log(LOG_ERROR, "Can not load db module: %s\n", name.c_str());
		return false;
	}

	return true;
}

bool ICQMain::init(int argc, char *argv[])
{
	srand(time(&curTime));

	option.load(argc, argv);

	// Open logger
	logger->open(option.log_ip, option.log_port, option.log_level, "linqd");

	if (!initDB() || !c2s->init())
		return false;

	// Enumerate all of the modules
	ConfigParser parser(LINQ_CONFIG_DIR"/modules.conf");
	parser.start(this);

	// Start all event threads
	startThreads();

	logger->log(LOG_INFORMATION, "linqd is now started");
	return true;
}

// Called when a module is found
void ICQMain::sectionParsed(Profile *prof)
{
	startModule(prof);
}

// Start a module by name
bool ICQMain::startModule(const char *name)
{
	Module *m = getModule(name);
	if (m) {
		logger->log(LOG_WARNING, "module %s already started\n", name);
		return false;
	}

	ConfigParser file(LINQ_CONFIG_DIR"/modules.conf");
	Profile *prof = file.getProfile(name);

	// Shutdown threads first to avoid race condition
	destroyThreads();
	bool ret = startModule(prof);
	startThreads();

	return ret;
}

// Stop a module by name
bool ICQMain::stopModule(const char *name)
{
	int i = getModuleIndex(name);
	if (i < 0) {
		ICQ_LOG("module %s not found\n", name);
		return false;
	}

	Module *m = getModule(i);
	if (!m)
		return false;

	// Stop all work first
	destroyThreads();

	// Remove it from module list
	for (int e = 0; e < NUM_EVENTS; e++)
		moduleList[e].remove(m);

	// Walk through all sessions to remove it
	if (m->hasSessionEvent)
		sessionHash->walk(sessionWalker, m);

	// Remove and delete this module from system
	removeModule(i);

	// Recover previous work
	startThreads();
	return true;
}

/*
 * Get all of the installed modules.
 * FORMAT: module1 + '\0' + module2 + '\0' + ... + '\0\0'
 */
void ICQMain::getModuleList(char *buf, int len)
{
	for (int i = 0; i < MAX_NUM_MODULES; i++) {
		Module *m = modules[i];
		if (!m)
			continue;

		int n = m->name.length() + 1;
		if (len <= n)	// Buffer is overflow
			break;

		memcpy(buf, m->name.c_str(), n);
		buf += n;
		len -= n;
	}

	*buf = '\0';
}

bool ICQMain::startModule(Profile *prof)
{
	int startOrder = prof->getInteger("start_order", 0);
	// If start_order is 0 or null, disable it
	if (!startOrder) {
		logger->log(LOG_INFORMATION, "module %s is disabled\n", prof->getName());
		return false;
	}

	Module *m = new Module;
	m->startOrder = startOrder;
	m->name = prof->getName();

	string name = LINQ_LIB_DIR"/modules/";
	name += prof->getName();

	if (!m->load(name.c_str())) {
		logger->log(LOG_WARNING, "Can not load module %s\n", name.c_str());
		delete m;
		return false;
	}

	int i = addModule(m);

	// If module initialization failed for some reason, remove it
	if (!m->listener->init(i, this, prof)) {
		ICQ_LOG("Module::init() failed\n");
		removeModule(i);
		return false;
	}

	logger->log(LOG_INFORMATION, "module %s is loaded", m->name.c_str());
	return true;
}

void ICQMain::registerEvent(int module, int e)
{
	Module *m = getModule(module);

	ICQ_ASSERT(m != NULL);
	ICQ_ASSERT(e >= 0 && e < NUM_EVENTS);

	moduleList[e].add(&memPool, m, NULL);
}

void ICQMain::deliver(PACKET *p)
{
	// If this is a local session, deliver it locally
	ClientSession *s = sessionHash->get(p->to);
	if (s) {
		postEvent(EV_S_IN, s, p);
		return;
	}

	// Otherwise, let some modules to handle it
	EVENT ev;
	ev.event = EV_DELIVER;
	ev.conn = NULL;
	ev.mapi = this;
	ev.s = NULL;
	ev.packet = p;

	if (dispatchEvent(&ev))
		return;

	// No module handled it, deliver it as if this is an offline event
	if (!p->online)
		postEvent(EV_S_IN, NULL, p);
}

void ICQMain::deliverFrom(PACKET *p)
{
	ClientSession *s = sessionHash->get(p->from);
	postEvent(EV_S_OUT, s, p);
}

void ICQMain::deliverLocal(PACKET *p)
{
	ClientSession *s = sessionHash->get(p->to);
	postEvent(EV_S_IN, s, p);
}

// Post an event to be dispatched later
void ICQMain::postEvent(int e, ClientSession *s, PACKET *p)
{
	// Select a queue if has not selected
	int i = (s ? s->queue : selectQueue());

	ICQ_ASSERT(i >= 0 && i < NUM_THREADS);
	ICQ_ASSERT(eventThreads[i] != NULL);

	EventItem *item = new EventItem(e, s, p);
	eventThreads[i]->addTask(item);
}

bool ICQMain::dispatchEvent(EVENT *ev)
{
	int e = ev->event;

	// If this is a session event
	if (ev->s && e < NUM_S_EVENTS) {
		ClientSession *s = (ClientSession *) ev->s;
		return moduleList[e].dispatch(&s->moduleList, ev);
	}
	return moduleList[e].dispatch(ev);
}

void ICQMain::startThreads()
{
	for (int i = 0; i < NUM_THREADS; i++) {
		EventThread *t = new EventThread;

		t->init(&dbModule, &option.db_info);
		t->start();

		eventThreads[i] = t;
	}
}

void ICQMain::destroyThreads()
{
	for (int i = 0; i < NUM_THREADS; i++) {
		delete eventThreads[i];
		eventThreads[i] = NULL;
	}

	ICQ_LOG("All threads are shutdown\n");
}

void ICQMain::run()
{
	while (true) {
		curTime = time(NULL);

		if (!socketRegistry.poll(curTime))
			break;
	}
}
