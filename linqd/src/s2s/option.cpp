#include "config.h"
#include "option.h"
#include "configparser.h"


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR"/s2s.conf");
	Profile *prof;

	prof = parser.getProfile("s2s");
	s2s_ip = prof->getIP("ip", "127.0.0.1");
	s2s_port = prof->getInteger("port", 8222);
	domain = prof->getString("domain", "localhost");

	prof = parser.getProfile("sm");
	sm_ip = prof->getIP("ip", "127.0.0.1");
	sm_port = prof->getInteger("port", 8003);

	prof = parser.getProfile("resolver");
	resolver_ip = prof->getIP("ip", "127.0.0.1");
	resolver_port = prof->getInteger("port", 8004);
}
