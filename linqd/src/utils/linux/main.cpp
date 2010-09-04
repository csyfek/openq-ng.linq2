#include "servicemain.h"
#include "debug.h"
#include "daemon.h"
#include "pidfile.h"

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

extern ServiceMain *serviceMain;


int main(int argc, char *argv[])
{
	ICQ_ASSERT(serviceMain != NULL);

	PIDFile file(serviceMain->getName());
	pid_t pid = file.readPID();
	if (pid > 0)
		kill(pid, SIGINT);

#ifndef DEBUG
	if (daemon_init() < 0)
		fprintf(stderr, "Can not initialize daemon\n");
#endif
	file.writePID();

	if (serviceMain->init(argc, argv))
		serviceMain->run();

	return 0;
}

