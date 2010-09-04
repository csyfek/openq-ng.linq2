#ifndef _SERVICE_MAIN_H
#define _SERVICE_MAIN_H


/*
 * The main framework class that controls the life cycle of the service.
 * Applications using this framework must declare a global variable of this type
 */
class ServiceMain {
public:
	ServiceMain();
	~ServiceMain();

	// Returns the service name
	virtual const char *getName() = 0;

	// Initialize this service
	virtual bool init(int argc, char *argv[]) = 0;

	// Run it
	virtual void run() = 0;

	// Called when the framework wants to stop this service
	// Returns true if it is successfully stopped.
	virtual bool stop() { return true; }
};


#endif
