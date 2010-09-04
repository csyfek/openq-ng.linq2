#include "LinqGlobalSetting.h"
#include "intl.h"

LinqGlobalSetting::LinqGlobalSetting()
{
#ifdef __WIN32
	use_custom_font = 1;
	custom_font = _("simsun 8");
#else
	use_custom_font = 0;
#endif
}

LinqGlobalSetting::~LinqGlobalSetting()
{
}

void LinqGlobalSetting::load(DBInStream &in)
{
	in >> use_custom_font >> custom_font;	
}

void LinqGlobalSetting::save(DBOutStream &out)
{
	out << use_custom_font << custom_font;	
}
