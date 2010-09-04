# Microsoft Developer Studio Project File - Name="libutils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libutils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libutils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libutils.mak" CFG="libutils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libutils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libutils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libutils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I ".." /I "../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libutils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".." /I "../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libutils - Win32 Release"
# Name "libutils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\configparser.cpp
# End Source File
# Begin Source File

SOURCE=.\dbmodule.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\dllmodule.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\getopt.c
# End Source File
# Begin Source File

SOURCE=.\win32\getopt1.c
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\loggerimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\main.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\ntservice.cpp
# End Source File
# Begin Source File

SOURCE=.\packet.cpp
# End Source File
# Begin Source File

SOURCE=.\poolimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\profileimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\select.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\servicemain.cpp
# End Source File
# Begin Source File

SOURCE=.\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\sqlstmt.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\sync.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\tcpstream.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\workerthread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\configparser.h
# End Source File
# Begin Source File

SOURCE=.\dbmodule.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\dllmodule.h
# End Source File
# Begin Source File

SOURCE=.\win32\getopt.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\loggerimpl.h
# End Source File
# Begin Source File

SOURCE=.\win32\ntservice.h
# End Source File
# Begin Source File

SOURCE=.\packet.h
# End Source File
# Begin Source File

SOURCE=.\poolimpl.h
# End Source File
# Begin Source File

SOURCE=.\profileimpl.h
# End Source File
# Begin Source File

SOURCE=.\servicemain.h
# End Source File
# Begin Source File

SOURCE=.\socket.h
# End Source File
# Begin Source File

SOURCE=.\sqlstmt.h
# End Source File
# Begin Source File

SOURCE=.\tcpstream.h
# End Source File
# Begin Source File

SOURCE=.\thread.h
# End Source File
# Begin Source File

SOURCE=.\workerthread.h
# End Source File
# End Group
# End Target
# End Project
