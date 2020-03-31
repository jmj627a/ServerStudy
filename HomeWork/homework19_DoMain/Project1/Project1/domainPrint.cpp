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

SOCKET SetTCPServer(short pnum, int blog);//��� ���� ����
void AcceptLoop(SOCKET sock);//Accept Loop
void DoIt(SOCKET dosock); //�ۼ���

int main()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//���� �ʱ�ȭ	
	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);//��� ���� ����
	if (sock == -1)
	{
		perror("��� ���� ����");
		WSACleanup();
		return 0;
	}
	AcceptLoop(sock);//Accept Loop
	WSACleanup();//���� ����ȭ

	system("pause");
	return 0;
}


SOCKET SetTCPServer(short pnum, int blog)
{
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//���� ����
	if (sock == -1) { return -1; }

	IN_ADDR Addr;
	SOCKADDR_IN servaddr = { 0 };//���� �ּ�

	servaddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(PORT_NUM);

	int ret = 0;
	//���� �ּҿ� ��Ʈ��ũ �������̽� ����
	ret = bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret == -1)
		return -1;

	ret = listen(sock, blog);//�� �α� ť ����
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
		dosock = accept(sock, (SOCKADDR *)&cliaddr, &len);//���� ����
		if (dosock == -1)
		{
			perror("Accept ����");
			break;
		}

		char buf[100];
		inet_ntop(AF_INET, &(cliaddr->sin_addr), buf, sizeof(buf));
		printf("%s:%d�� ���� ��û ����\n", buf,
			ntohs(cliaddr->sin_port));
		DoIt(dosock);
	}
	closesocket(sock);//���� �ݱ�
}
void DoIt(SOCKET dosock)
{
	char msg[MAX_MSG_LEN] = "";
	while (recv(dosock, msg, sizeof(msg), 0) > 0)//����
	{
		printf("recv:%s\n", msg);
		send(dosock, msg, sizeof(msg), 0);//�۽�
	}
	closesocket(dosock);//���� �ݱ�
}