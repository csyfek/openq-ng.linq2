#ifndef _SHA_H
#define _SHA_H


char *shahash(char *str);    /* NOT THREAD SAFE */
void shahash_r(const char* str, char hashbuf[41]); /* USE ME */


#endif /* sha.h */
