#include "servicemain.h"
#include "debug.h"

ServiceMain *serviceMain;


ServiceMain::ServiceMain()
{
	ICQ_ASSERT(serviceMain == NULL);
	
	serviceMain = this;
}

ServiceMain::~ServiceMain()
{
	ICQ_ASSERT(serviceMain != NULL);

	serviceMain = NULL;
}

