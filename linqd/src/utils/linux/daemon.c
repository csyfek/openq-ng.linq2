#include <signal.h>
#include <fcntl.h>
#include <unistd.h>


/*
 * Setup daemon
 * Stolen from somewhere, of course:-)
 */
int daemon_init()
{
	struct sigaction act;
	int i, maxfd;
	
	if (fork() != 0)
		exit(0);
	if (setsid() < 0)
		return -1;
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGHUP, &act, 0);
	if (fork() != 0)
		exit(0);
	chdir("/");
	umask(0);
	maxfd = sysconf(_SC_OPEN_MAX);
	for (i = 0; i < maxfd; i++)
		close(i);
	open("/dev/null", O_RDWR);
	dup(0);
	dup(1);
	dup(2);
	return 0;
}

