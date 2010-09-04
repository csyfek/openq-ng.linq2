#include "client.h"
#include "logapp.h"


Client::Client(int fd) : TCPStream(fd)
{
	logger = NULL;
}

Client::~Client()
{
	if (logger)
		delete logger;
}

void Client::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	uint8 level;
	ICQ_STR str;
	in >> level >> str;

	if (logger)
		logger->log(level, str.text);
	else {
		std::string file;
		const char *prefix;

		if (logApp.option.prefix) {
			file = logApp.option.file;
			prefix = str.text;
		} else {
			file = logApp.option.file + '/' + str.text + ".log";
			prefix = NULL;
		}

		logger = new Logger;
		logger->open(file.c_str(), level, prefix);
	}
}

void Client::onSocketClose()
{
	TCPStream::onSocketClose();

	list.remove();
	delete this;
}
