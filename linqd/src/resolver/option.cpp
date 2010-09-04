#include "config.h"
#include "option.h"
#include "configparser.h"
#include <string>


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR"/resolver.conf");

	Profile *prof = parser.getProfile("resolver");
	ip = prof->getIP("ip", "127.0.0.1");
	port = prof->getInteger("port", 8004);
}
