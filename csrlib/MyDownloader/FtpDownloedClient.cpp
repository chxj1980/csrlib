#include <WINSOCK2.H>
#include <stdio.h>
#include <iostream>
#include "package.h"
#pragma comment(lib,"ws2_32.lib")
using namespace ::std;

int getBytesLength(char* buf) {
	char len[5];
	for (int i = 1;i < 5;i++) {
		len[i - 1] = buf[i];
	}
	len[4] = '\0';
	return atoi(len);
}
int main(int argc, char **argv)
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired = MAKEWORD(1, 1);
	char str[30];
	int segema = 0;
	err = WSAStartup(versionRequired, &wsaData);//Э���İ汾��Ϣ
	printf("�������ӷ�������\n");
	char receiveBuf[BUFFERLENGTH + 6];
	char sendBuf[BUFFERLENGTH + 1];
	while (1)
	{


		SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

		SOCKADDR_IN clientsock_in;
		clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		clientsock_in.sin_family = AF_INET;
		clientsock_in.sin_port = htons(6000);
		connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));//��ʼ����


		recv(clientSocket, receiveBuf, sizeof(receiveBuf), 0);
		if (segema == 0)
		{
			printf("��������������������:%s\n", receiveBuf);
		}
		segema++;
		printf("�������ļ���������quit�˳�\n");
		cin >> str;
		if (strcmp(str, "quit") == 0)
		{
			break;
		}
		int len = strlen(str) + 1;
		send(clientSocket, str, len, 0);
		FILE *outfp;
		if ((outfp = fopen(str, "w")) == NULL)
		{
			printf("Can't save the %s\n", str);
			exit(2);
		}
		int bytesize = 0;
		while (1)
		{
			int len = recv(clientSocket, receiveBuf, BUFFERLENGTH + 5, 0);
			receiveBuf[BUFFERLENGTH + 5] = '\0';
			if (len > 0)
			{
				int flag = getBytesLength(receiveBuf);
				if (flag > 0)
				{
					if (flag == strlen(receiveBuf) - 5)
					{
						bytesize += flag;
						printf("\b\b\b\b\b\b\b\b\b\\b\b\b\b\b\b\b\b\b\b\b�ѽ���%d% Byte", bytesize);
						for (int i = 5;i < strlen(receiveBuf);i++)
						{
							putc(receiveBuf[i], outfp);
						}
					}
					if (receiveBuf[0] == '0'&&flag == strlen(receiveBuf) - 5)
					{
						printf("\n\n�ļ�%s�������\n\n\n", str);
						fclose(outfp);
						break;
					}
					if (receiveBuf[0] == '9')
					{
						printf("�ļ�%s������!\n", str);
						segema = 1;
						break;
					}
				}
			}
		}

		closesocket(clientSocket);

	}
	WSACleanup();
	return 0;
}