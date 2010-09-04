#ifndef _DEBUG_H
#define _DEBUG_H

#include "config.h"
#include <stdio.h>
#include <stdlib.h>


#ifdef DEBUG

// Just like normal printf call. Should I write a function that works like printf,
// but can print to whatever stream (a file, eg.)
#define ICQ_LOG			printf

// Assert x is true. Otherwise, it prints the current file:line, and exits noisily
#define ICQ_ASSERT(x)	\
	if (!(x)) {	\
		ICQ_LOG("ASSERT FAILED (%s:%d):\n"#x"\n", __FILE__, __LINE__);	\
		abort();	\
	}

#else

#define ICQ_LOG			if (0) printf
#define ICQ_ASSERT(x)

#endif


#endif
