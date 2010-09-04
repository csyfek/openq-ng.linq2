/*
base64.h
Copyright (C) 1999 Lars Brinkhoff.  See COPYING for terms and conditions.
*/

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t encode_base64 (const void *data, size_t length, char **code);

#ifdef __cplusplus
}
#endif 
