#include "config.h"
#include "option.h"
#include "configparser.h"


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR"/sm.conf");

	Profile *prof = parser.getProfile("sm");
	sm_ip = prof->getIP("ip", "127.0.0.1");
	sm_port = prof->getInteger("port", 8001);

	prof = parser.getProfile("s2s");
	s2s_ip = prof->getIP("ip", "127.0.0.1");
	s2s_port = prof->getInteger("port", 8003);

	prof = parser.getProfile("db");
	db_impl = prof->getString("impl", "");
	db_info.host = prof->getString("host", "localhost");
	db_info.port = prof->getInteger("port", 0);
	db_info.user = prof->getString("user", "");
	db_info.passwd = prof->getString("password", "");
	db_info.db = prof->getString("db", "");
}
