// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#pragma comment(lib,"ws2_32")
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 6000
#define BUFSIZE 512