#ifndef _PROFILE_IMPL_H
#define _PROFILE_IMPL_H

#pragma warning(disable:4786)

#include "profile.h"
#include "list.h"
#include <map>
#include <string>

using namespace std;


class ProfileImpl : public Profile {
public:
	virtual const char *getName() { return sectionName.c_str(); }
	virtual bool getBoolean(const char *name, bool def);
	virtual int getInteger(const char *name, int def);
	virtual const char *getString(const char *name, const char *def);
	virtual uint32 getIP(const char *name, const char *def);

	void set(const char *name, const char *value);
	void setName(const char *name) { sectionName = name; }

	void reset() {
		keyValuePairs.clear();
		sectionName = "";
	}

private:
	// The properties map
	map<string, string> keyValuePairs;
	string sectionName;
};


#endif
