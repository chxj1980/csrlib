
#include<winsock2.h>
#include<windows.h>
#include<stdio.h>
#include<Ws2tcpip.h>
#include<iostream>
#include<fstream>

#include  <direct.h>

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   8088
#define BUFFER_SIZE 4096
char    SendBuffer[BUFFER_SIZE], RecvBuffer[BUFFER_SIZE];   //���ͻ����������ջ�����
SOCKET          AcceptSocket;
DWORD WINAPI ClientThread(LPVOID lp);
struct CLIENTINFO {
	SOCKET          clientsocket;
	sockaddr_in     sockaddr;
};

int main(int argc, TCHAR* argv[])
{
	int         Ret;
	WSADATA     wsaData;

	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != NO_ERROR)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		return -1;
	}//if

	 // Create a new socket to listening for client connections.
	SOCKET      ListenSocket;
	if ((ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
		== INVALID_SOCKET)
	{
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}//if

	sockaddr_in     service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(DEFAULT_PORT);

	if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}//if

	if (listen(ListenSocket, 10) == SOCKET_ERROR)
	{
		printf("Error listening on socket.\n");
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}//if

	SOCKET          AcceptSocket;
	sockaddr_in     ClientAddr;//���տͻ��˵�ַ
	int     addrClientlen = sizeof(ClientAddr);
	while (true)
	{

		CLIENTINFO      clientInfo;
		printf("waiting for client to connect......\n");
		AcceptSocket = accept(ListenSocket, (sockaddr*)&ClientAddr, &addrClientlen);
		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("accept failed : %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}//if
		clientInfo.clientsocket = AcceptSocket;
		memcpy(&clientInfo.sockaddr, &ClientAddr, sizeof(sockaddr_in));
		printf("a client is coming......\n");
		DWORD dwThreadID;
		CreateThread(NULL, 0, ClientThread, (LPVOID*)&clientInfo, 0, &dwThreadID);
	}//while
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
} //main


DWORD WINAPI ClientThread(LPVOID lp)
{
	CLIENTINFO *pClientInfo = (CLIENTINFO*)lp;
	SOCKET  clientsocket = pClientInfo->clientsocket;
	sockaddr_in sockaddr;
	memcpy(&sockaddr, &pClientInfo->sockaddr, sizeof(sockaddr_in));
	int     iResult, bytes;
	char    recvRFileName[DEFAULT_BUFLEN] = "";
	char    temp_buffer[BUFFER_SIZE];
	unsigned readlen;
	DWORD dwReadSize = 0;
	char   curPath[MAX_PATH];
	char quit[10] = "quit";
	//������·��
	GetModuleFileName(NULL, (LPSTR)curPath, sizeof(curPath));
	strrchr(curPath, '\\')[0] = 0;

	ifstream infile;
	while (true)
	{
		memset(recvRFileName, 0, sizeof(recvRFileName));
		iResult = recv(clientsocket, recvRFileName, DEFAULT_BUFLEN, 0);
		if (SOCKET_ERROR == iResult)
		{
			printf("recvieve failed with error : %d\n", WSAGetLastError());
			closesocket(clientsocket);
			WSACleanup();
			return -1;
		}//if


		 //������get help exit -s(�ļ���С)

		 //help
		char dir[10] = "dir";
		char get[10] = "get";
		printf("������������:%s", recvRFileName);

		if (strcmp(recvRFileName, dir) == 0) {
			//��������
			system("cmd /c dir /a-d /b >allfiles.txt");
			printf("�鿴allfiles�ļ�\n");

			//����ӡ�������ļ���Ϣ��ȡ����

			FILE *file;
			char buf[1024];
			int len = 0, i = 0;
			char *array[1024];
			char filedir[1024];//Ҫ����ȥ���ļ�Ŀ¼

			file = fopen("allfiles.txt", "r");//��TXST.TxT�ļ�
			if (!file)return -1;
			while (fgets(buf, 1024, file))//��ȡTXT���ַ�
			{
				len = strlen(buf);
				array[i] = (char*)malloc(len + 1);
				if (!array[i])break;
				strcpy(array[i++], buf);
			}//while

			fclose(file);
			i--;
			while (i >= 0 && array[i])
			{
				printf("%s\n", array[i]);//��ӡtest�ĵ����ַ�

				iResult = send(clientsocket, array[i], strlen(array[i]), 0);
				if (iResult == SOCKET_ERROR) { printf("����ʧ��\n"); }

				free(array[i--]);

			}//while
			printf("���ͽ���quit\n");
			send(clientsocket, quit, strlen(quit), 0);
			continue;

		}//if

		 //�����ļ���
		strcat(curPath, "\\");
		strcat(curPath, recvRFileName);
		strcpy(recvRFileName, curPath);



		//��ȡ�ļ���С

		FILE * pFile;
		long size;
		pFile = fopen(recvRFileName, "rb");
		if (pFile == NULL)
			perror("Error opening file");
		else
		{
			fseek(pFile, 0, SEEK_END);   ///���ļ�ָ���ƶ��ļ���β
			size = ftell(pFile); ///�����ǰ�ļ�ָ������ļ���ʼ���ֽ���
			fclose(pFile);
			printf("FileSize : %ld bytes.\n", size);
			char a[30];
			//��size�������ַ���a��
			sprintf(a, "%d", size);
			send(clientsocket, a, 30, 0);
			printf("�����ļ���С:%s", a);
		}


		printf("�ļ���%s\n", recvRFileName);
		infile.open(recvRFileName, ios::in | ios::binary);

		if (!infile)
		{    //δ�ɹ����ļ�
			printf("Sorry, cannot open %s. Please try again.\r\n", recvRFileName);
			//break;
		}//if

		else
		{
			printf("The file %s is found,ready to transfer.\n", recvRFileName);
			printf("Transfering\r\n");



			while (!infile.eof())
			{    //ѭ����ȡ�ļ���ͨ��h_NewSocket���͵��ͻ���
				 //printf(SendBuffer,"%s",temp_buffer); 
				infile.read(SendBuffer, BUFFER_SIZE);
				readlen = infile.gcount();
				send(clientsocket, SendBuffer, readlen, 0);
				//printf("."); 
			}
			infile.close();
			printf("Transfer completed... \r\n");


			//bytes = send(AcceptSocket, SendBuffer, strlen(SendBuffer), 0);

		}//else      




		 //���û�����ݷ���
		iResult = shutdown(clientsocket, SD_BOTH);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
		}//if

		closesocket(clientsocket);
		return 0;


	}//while
}//main

//////////////////////////////////////////////////////////////////////////
//time_t start, end;
//double cost;
//time(&start);
//printf("��ʼ���㴫��ʱ��\n");
////������ɡ����ʱ��
//time(&end);
//cost = difftime(end, start);
//printf("����ʱ��:%f\n", cost);