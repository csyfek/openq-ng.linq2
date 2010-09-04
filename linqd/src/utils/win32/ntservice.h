#ifndef _NT_SERVICE_H
#define _NT_SERVICE_H

#include <windows.h>


class ServiceMain;


/*
 * A utility class for NT service
 */
class NTService {
public:
	NTService(ServiceMain *main);
	~NTService();

	// Call backs...
	void serviceMain(DWORD argc, LPTSTR *argv);
	void serviceCtrlHandler(DWORD opcode);

	BOOL start();

	static BOOL install(LPCTSTR name);
	static BOOL remove(LPCTSTR name);

private:
	void setStatus(DWORD status);

	ServiceMain *service;
	SERVICE_STATUS_HANDLE hServiceStatus;
};


#endif
