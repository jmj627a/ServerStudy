#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT_NUM      10200
#define BLOG_SIZE       50000
#define MAX_MSG_LEN 256

SOCKET SetTCPServer(short pnum, int blog);//대기 소켓 설정
void AcceptLoop(SOCKET sock);//Accept Loop
void DoIt(SOCKET dosock); //송수신

int main()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//윈속 초기화	
	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);//대기 소켓 설정
	if (sock == -1)
	{
		perror("대기 소켓 오류");
		WSACleanup();
		return 0;
	}
	AcceptLoop(sock);//Accept Loop
	WSACleanup();//윈속 해제화

	system("pause");
	return 0;
}


SOCKET SetTCPServer(short pnum, int blog)
{
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//소켓 생성
	if (sock == -1) { return -1; }

	IN_ADDR Addr;
	SOCKADDR_IN servaddr = { 0 };//소켓 주소

	servaddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(PORT_NUM);

	int ret = 0;
	//소켓 주소와 네트워크 인터페이스 결합
	ret = bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret == -1)
		return -1;

	ret = listen(sock, blog);//백 로그 큐 설정
	if (ret == -1)
		return -1;
	return sock;
}




void AcceptLoop(SOCKET sock)
{
	SOCKET dosock;
	SOCKADDR_IN* cliaddr = { 0 };
	int len = sizeof(cliaddr);
	while (true)
	{
		dosock = accept(sock, (SOCKADDR *)&cliaddr, &len);//연결 수락
		if (dosock == -1)
		{
			perror("Accept 실패");
			break;
		}

		char buf[100];
		inet_ntop(AF_INET, &(cliaddr->sin_addr), buf, sizeof(buf));
		printf("%s:%d의 연결 요청 수락\n", buf,
			ntohs(cliaddr->sin_port));
		DoIt(dosock);
	}
	closesocket(sock);//소켓 닫기
}
void DoIt(SOCKET dosock)
{
	char msg[MAX_MSG_LEN] = "";
	while (recv(dosock, msg, sizeof(msg), 0) > 0)//수신
	{
		printf("recv:%s\n", msg);
		send(dosock, msg, sizeof(msg), 0);//송신
	}
	closesocket(dosock);//소켓 닫기
}