// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <list>

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

//timegettime 식별자를 못찾는경우
#include <mmsystem.h>
#pragma comment (lib, "winmm")
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include "global.h"
#include "protocol.h"
#include "CPacket.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
