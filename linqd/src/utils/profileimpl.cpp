#include "profileimpl.h"

#ifdef _WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


int ProfileImpl::getInteger(const char *name, int def)
{
	const char *value = getString(name, NULL);
	if (!value)
		return def;

	return atoi(value);
}

bool ProfileImpl::getBoolean(const char *name, bool def)
{
	const char *value = getString(name, NULL);
	if (!value)
		return def;

	return (strcmp(value, "true") == 0);
}

const char *ProfileImpl::getString(const char *name, const char *def)
{
	map<string, string>::iterator it = keyValuePairs.find(name);
	if (it == keyValuePairs.end())
		return def;
	return (*it).second.c_str();
}

uint32 ProfileImpl::getIP(const char *name, const char *def)
{
	uint32 ip = INADDR_NONE;

	const char *val = getString(name, NULL);
	if (val)
		ip = inet_addr(val);
	if (ip == INADDR_NONE)
		ip = inet_addr(def);

	return ip;
}

void ProfileImpl::set(const char *name, const char *value)
{
	keyValuePairs[name] = value;
}
