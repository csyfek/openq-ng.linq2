#ifndef _TYPES_H
#define _TYPES_H


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


struct ICQ_STR {
	int len;
	const char *text;
};


// Used by dll module
#ifdef _WIN32
#define ICQ_EXPORT	extern "C" __declspec(dllexport)
#else
#define ICQ_EXPORT	extern "C"
#endif


#endif
