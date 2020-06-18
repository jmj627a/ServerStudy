#pragma once
#ifndef __PRINT__
#define __PRINT__

#include "protocol.h"

#define dfLOG_LEVEL_DEBUG	0
#define dfLOG_LEVEL_NOTICE	1
#define dfLOG_LEVEL_ERROR	2

void Log(const WCHAR *szString, int iLogLevel);

#define _LOG(LogLevel, fmt, ...) 					\
do {												\
	if (g_iLogLevel <= LogLevel)					\
	{												\
		wsprintf(g_szLogBuff, fmt, ##__VA_ARGS__);	\
		Log(g_szLogBuff, LogLevel);					\
	}												\
}while(0)													\

void ServerControl(void);

void ServerMornitor(void);



#endif