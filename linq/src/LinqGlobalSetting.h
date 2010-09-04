#ifndef __LINQ_GLOBAL_SETTING_H__
#define __LINQ_GLOBAL_SETTING_H__

#include <string>

#include "LinqDB.h"

class LinqGlobalSetting : public DBSerialize
{
public:
	LinqGlobalSetting();
	virtual ~LinqGlobalSetting();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	uint8 use_custom_font;
	std::string custom_font;
};

#endif
