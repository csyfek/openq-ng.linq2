#
# global.mak
#
# This file should be included by all Makefile.mingw files for project
# wide definitions.
#

CC = gcc.exe
CPPC = g++.exe


# Don't use -g flag when building Plugin DLLs
#CFLAGS += -O2 -Wall -mwindows -mno-cygwin -fnative-struct -mms-bitfields
CFLAGS += -Wall -O2 -mno-cygwin -mms-bitfields

