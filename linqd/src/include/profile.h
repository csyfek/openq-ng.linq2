#ifndef _PROFILE_H
#define _PROFILE_H

#include "types.h"


/*
 * A cluster of configuration options
 */
class Profile {
public:
	virtual const char *getName() = 0;
	virtual bool getBoolean(const char *name, bool def) = 0;
	virtual int getInteger(const char *name, int def) = 0;
	virtual const char *getString(const char *name, const char *def) = 0;
	virtual uint32 getIP(const char *name, const char *def) = 0;
};


#endif
