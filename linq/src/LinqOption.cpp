#include "config.h"

#include "LinqOption.h"
#include "intl.h"

#include <linq/udpsession.h>

LinqOption::LinqOption()
{
	login_status = STATUS_ONLINE;
	server_host = "myicq.cosoft.org.cn";
	server_port = 8000;
	proxy_type = 0; //no proxy.
	proxy_port = 0;
	proxy_resolve = false;
}

LinqOption::~LinqOption()
{
}

void LinqOption::load(DBInStream &in)
{
	in >> server_host >> server_port;	
	in >> login_status >> saved_passwd >> passwd_hash;		
	in >> proxy_type >> proxy_host >> proxy_port >> proxy_username >> proxy_passwd >> proxy_resolve;	
}

void LinqOption::save(DBOutStream &out)
{
	out << server_host << server_port;	
	out << login_status << saved_passwd << passwd_hash;		
	out << proxy_type << proxy_host << proxy_port << proxy_username << proxy_passwd << proxy_resolve;
}
