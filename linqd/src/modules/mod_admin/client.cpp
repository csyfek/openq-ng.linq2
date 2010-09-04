#include "client.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#define vsnprintf	_vsnprintf
#endif


#define TOKEN_SEP	" \t"


Client::Client(MAPI *api, int fd) : Socket(fd)
{
	mapi = api;
	bufLen = 0;

	addEvent(SOCKET_READ);

	printBanner();
	printPrompt();
}

void Client::print(const char *fmt, ...)
{
	char buf[1024];

	va_list args;
	va_start(args, fmt);

	int n = vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);

	send(sockfd, buf, n, 0);
}

void Client::printBanner()
{
	const char *banner =
		"\r\n"
		"Welcome to linqd admin module.\r\n"
		"Copyright(C) Zhang Yong.\r\n"
		"\r\n";

	print(banner);
}

void Client::printPrompt()
{
	print("admin> ");
}

bool Client::onSocketRead()
{
	int n = recv(sockfd, recvBuf + bufLen, MAX_CMD_LEN - bufLen, 0);
	if (n <= 0) {
		onExit();
		return false;
	}

	bufLen += n;
	if (bufLen >= 2 && *(uint16 *) (recvBuf + bufLen - 2) == 0x0a0d) {
		recvBuf[bufLen - 2] = '\0';
		bufLen = 0;
		processCmd(recvBuf);
	}
	return true;
}

void Client::processCmd(char *line)
{
	char *cmd = strtok(line, TOKEN_SEP);
	if (!cmd)
		return;

	if (strcmp(cmd, "ls") == 0)
		onListModules();
	else if (strcmp(cmd, "start") == 0)
		onStartModule();
	else if (strcmp(cmd, "stop") == 0)
		onStopModule();
	else if (strcmp(cmd, "help") == 0)
		onHelp();
	else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
		onExit();
	else
		onError();

	printPrompt();
}

void Client::onHelp()
{
	const char *help = "This is the admin module.\r\n";
	print(help);
}

void Client::onExit()
{
	list.remove();
	delete this;
}

void Client::onError()
{
	const char *error = "Unknown command.\r\n";
	print(error);
}

void Client::onListModules()
{
	char buf[8192];

	mapi->getModuleList(buf, sizeof(buf));

	char *name = buf;
	while (*name) {
		int len = strlen(name);
		print("%.*s\n", len, name);
		name += len + 1;
	}
}

void Client::onStartModule()
{
	char *name = strtok(NULL, TOKEN_SEP);
	if (!name) {
		print("Usage: start module_name\r\n");
		return;
	}

	if (!mapi->startModule(name))
		print("Start module %s failed.\r\n", name);
}

void Client::onStopModule()
{
	char *name = strtok(NULL, TOKEN_SEP);
	if (!name) {
		print("Usage: stop module_name\r\n");
		return;
	}

	// Do not unload ourselves, otherwise we can never be back here!!
	if (strcmp(name, "mod_admin") == 0) {
		print("Can not unload ourselves!\n");
		return;
	}

	if (!mapi->stopModule(name))
		print("Can not stop module %s.\r\n", name);
}
