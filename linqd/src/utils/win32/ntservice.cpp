#include <windows.h>
#include "ntservice.h"
#include "servicemain.h"


/*
 * Store the unique NTService instance, so we can come back from outside the c++ world.
 *
 * TODO: For now, only one service is supported
 */
static NTService *ntService;


NTService::NTService(ServiceMain *main)
{
	ntService = this;

	service = main;
	hServiceStatus = NULL;
}

NTService::~NTService()
{
	ntService = NULL;
}

/*
 * Notify SCM to change service status
 */
void NTService::setStatus(DWORD status)
{
	SERVICE_STATUS serviceStatus;

	ZeroMemory(&serviceStatus, sizeof(serviceStatus));
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	serviceStatus.dwCurrentState = status;
	SetServiceStatus(hServiceStatus, &serviceStatus);
}

static VOID WINAPI serviceCtrlHandler(DWORD opcode)
{
	ntService->serviceCtrlHandler(opcode);
}

static VOID WINAPI serviceMain(DWORD argc, LPTSTR *argv)
{
	ntService->serviceMain(argc, argv);
}


void NTService::serviceMain(DWORD argc, LPTSTR *argv)
{
	hServiceStatus = RegisterServiceCtrlHandler(service->getName(), ::serviceCtrlHandler);
	if (!hServiceStatus)
		return;

	setStatus(SERVICE_START_PENDING);

	// Initialize and start it
	if (service->init(argc, argv)) {
		setStatus(SERVICE_RUNNING);
		service->run();
	}

	setStatus(SERVICE_STOPPED);
}

void NTService::serviceCtrlHandler(DWORD opcode)
{
	switch (opcode) {
	case SERVICE_CONTROL_STOP:
		if (service->stop())
			setStatus(SERVICE_STOPPED);
		break;
	}
}

/*
 * Install this executable file as a service
 */
BOOL NTService::install(LPCTSTR serviceName)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCM)
		return FALSE;

	// Get the exe file name
	char pathName[_MAX_PATH];
	GetModuleFileName(NULL, pathName, _MAX_PATH);

	SC_HANDLE hService = CreateService(
		hSCM,
		serviceName,
		serviceName,	// display name
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		pathName,
		NULL,
		NULL,
		NULL,	// Dependencies, should this be set?
		NULL,
		NULL
	);

	if (hService)
		CloseServiceHandle(hService);

	CloseServiceHandle(hSCM);

	return (hService != NULL);
}

/*
 * Remove ourselves from SCM
 */
BOOL NTService::remove(LPCTSTR name)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, 0);
	if (!hSCM)
		return FALSE;

	BOOL ret = FALSE;

	SC_HANDLE hService = OpenService(hSCM, name, DELETE);
	if (hService) {
		if (DeleteService(hService))
			ret = TRUE;
		CloseServiceHandle(hService);
	}

	CloseServiceHandle(hSCM);
	return ret;
}

BOOL NTService::start()
{
	static SERVICE_TABLE_ENTRY dispatchTable[] = {
		{ (LPTSTR) service->getName(), ::serviceMain },
		{ NULL, NULL }
	};

	return StartServiceCtrlDispatcher(dispatchTable);
}
