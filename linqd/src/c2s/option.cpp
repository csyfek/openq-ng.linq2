#include "config.h"
#include "option.h"
#include "configparser.h"
#include "logger.h"


void Option::load(int argc, char *argv[])
{
	ConfigParser parser(LINQ_CONFIG_DIR "/c2s.conf");
	Profile *prof;

	prof = parser.getProfile("c2s");
	c2s_ip = prof->getIP("ip", "0.0.0.0");
	c2s_port = prof->getInteger("port", 8000);
	enableReg = prof->getBoolean("register", true);
	adminUser = prof->getString("admin", "");

	prof = parser.getProfile("linqd");
	linqd_ip = prof->getIP("ip", "127.0.0.1");
	linqd_port = prof->getInteger("port", 8000);

	prof = parser.getProfile("logd");
	log_ip = prof->getIP("ip", "127.0.0.1");
	log_port = prof->getInteger("port", 8002);
	log_level = prof->getInteger("level", LOG_ERROR);

	prof = parser.getProfile("db");
	db_impl = prof->getString("impl", "");
	db_host = prof->getString("host", "localhost");
	db_port = prof->getInteger("port", 0);
	db_user = prof->getString("user", "");
	db_passwd = prof->getString("password", "");
	db_name = prof->getString("db", "");
}
