#include "config.h"
#include "option.h"
#include "configparser.h"


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR"/logd.conf");

	Profile *prof = parser.getProfile("logd");
	prefix = prof->getBoolean("prefix", true);
	file = prof->getString("file", "");
	ip = prof->getIP("ip", "127.0.0.1");
	port = prof->getInteger("port", 8002);
}
