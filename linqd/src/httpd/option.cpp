#include "config.h"
#include "option.h"
#include "configparser.h"
#include <string>


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR"/httpd.conf");
	Profile *prof;

	prof = parser.getProfile("https");
	https_ip = prof->getIP("ip", "127.0.0.1");
	https_port = prof->getInteger("port", 443);

	prof = parser.getProfile("c2s");
	c2s_ip = prof->getIP("ip", "127.0.0.1");
	c2s_port = prof->getInteger("port", 8000);
}
