#ifndef __LINQ_OPTION_H__
#define __LINQ_OPTION_H__

#include "LinqDB.h"

#include <string>

class LinqOption : public DBSerialize {
public:
	LinqOption();
	virtual ~LinqOption();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	uint32 login_status;
	std::string saved_passwd;

	std::string passwd_hash;

	std::string server_host;
	uint16 server_port;

	// Proxy stuff
	uint8 proxy_type;
	std::string proxy_host;
	uint16 proxy_port;
	std::string proxy_username;
	std::string proxy_passwd;
	uint8 proxy_resolve;
};

#endif
