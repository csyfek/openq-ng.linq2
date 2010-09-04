#include "config.h"
#include "servicemain.h"
#include "ntservice.h"
#include "debug.h"
#include <winsock.h>


extern ServiceMain *serviceMain;


static bool initArgs(int &argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		const char *opt = argv[i];
		if (*opt != '-' || strlen(opt) != 2)
			continue;

		switch (opt[1]) {
		case 'i':
			NTService::install(serviceMain->getName());
			return true;

		case 'r':
			NTService::remove(serviceMain->getName());
			return true;

		case 'v':
			printf("%s-"VERSION", Win32\n", serviceMain->getName());
			return true;
		}
	}
	return false;
}


int main(int argc, char *argv[])
{
	// serviceMain has been set by its constructor
	ICQ_ASSERT(serviceMain != NULL);

	if (initArgs(argc, argv))
		return 0;

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

#ifndef _DEBUG
	NTService service(serviceMain);
	if (!service.start())
#endif
	{
		if (serviceMain->init(argc, argv))
			serviceMain->run();
	}

	WSACleanup();
	return 0;
}
