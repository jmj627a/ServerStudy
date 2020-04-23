// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <list>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include "..\..\..\Client\Client\protocol.h"
#include "CRingBuffer.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "winmm")
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define WM_NETWORK WM_USER+1
#define SERVER_PORT 25000


using namespace std;