#include "config.h"
#include "option.h"
#include "configparser.h"
#include "logger.h"


void Option::load(int argc, char *argv[])
{
	ConfigParser file(LINQ_CONFIG_DIR"/linqd.conf");
	Profile *prof;

	prof = file.getProfile("linqd");

	prof = file.getProfile("logd");
	log_ip = prof->getIP("ip", "127.0.0.1");
	log_port = prof->getInteger("port", 8002);
	log_level = prof->getInteger("level", LOG_ERROR);

	prof = file.getProfile("c2s");
	c2s_ip = prof->getIP("ip", "127.0.0.1");
	c2s_port = prof->getInteger("port", 8000);

	prof = file.getProfile("db");
	db_impl = prof->getString("impl", "");
	db_info.host = prof->getString("host", "localhost");
	db_info.port = prof->getInteger("port", 0);
	db_info.user = prof->getString("user", "");
	db_info.passwd = prof->getString("password", "");
	db_info.db = prof->getString("db", "");
}
