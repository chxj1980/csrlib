#include<winsock2.h>
#include<windows.h>
#include<Ws2tcpip.h>
#include<stdio.h>
#include<string>
#include<iostream>
#include<fstream>

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   5150
#define BUFFER_SIZE 4096

int main(int argc, TCHAR* argv[])
{
	int         iResult, rec;
	WSADATA     wsaData;
	HANDLE hout;
	ofstream    outfile;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	char    sendbuf[DEFAULT_BUFLEN] = "";
	char    szBuffer[BUFFER_SIZE];
	BOOL    issend = true;

	//��ʼ��Windows Sockets DLL
	if (iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		printf("WSAStartup failed with error : %d\n", iResult);
		return -1;
	}
	//����socket
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	////////////////////

	char addr[20];
	int prot;
	printf("�������е��ǿͻ��ˣ�\n");
	printf("**************************************************************\n");
	printf("�������������IP��ַ:");
	scanf("%s", addr);
	printf("������������Ķ˿�:");
	scanf("%d", &prot);



	///////////////////


	//����Ŀ�������ַ�����ӷ�����
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(addr);
	clientService.sin_port = htons(prot);
	iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR)
	{
		printf("connect failed with error : %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	printf("connect with server successfully!!!\n");
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hout,
		FOREGROUND_GREEN);
	cout << "��������:******************* help ��ȡ������ʾ**********************:\n";
	while (true)
	{



		//�������
		char cmd[20];
		char help[10] = "help";
		char exit[10] = "exit";
		char get[10] = "get";
		char dir[10] = "dir";
		char quit[10] = "quit";
		scanf("%s", &cmd);
		printf("�����������%s\n", cmd);
		if (strcmp(cmd, help) == 0) {


			printf("�������:\n help----------��ȡ�������\n dir----------��ȡ�ļ�Ŀ¼\n get----------�����ļ�\n exit----------�˳�\n");
			continue;
		}
		//  ��Ŀ¼
		if (strcmp(cmd, dir) == 0) {

			iResult = send(ConnectSocket, dir, strlen(dir), 0);

			if (SOCKET_ERROR == iResult)
			{
				printf("send failed with error : %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return -1;
			}
			//���մ�������Ŀ¼

			SetConsoleTextAttribute(hout,
				FOREGROUND_GREEN |
				FOREGROUND_BLUE);
			printf("**********************�ļ�Ŀ¼**********************\n");
			char dirs[30];


			while (true) {
				ZeroMemory(dirs, 30);
				iResult = recv(ConnectSocket, dirs, 30, 0);
				//���ݷ�����ɾ�����ѭ��������ʱ��������������quit
				if (strcmp(dirs, quit) == 0) { break; }

				else
					printf("%s\n", dirs);


				if (SOCKET_ERROR == iResult)
				{
					printf("recvieve failed with error : %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return -1;
				}//if

				 //�������ٴη���
				 //  continue;
			}//while
			continue;
		}//if



		if (strcmp(cmd, exit) == 0) { break; }

		if ((strcmp(cmd, dir) != 0) && (strcmp(cmd, exit) != 0) && (strcmp(cmd, help) != 0) && (strcmp(cmd, get) != 0)) {
			printf("�������:\n help----------��ȡ�������\n dir----------��ȡ�ļ�Ŀ¼\n get----------�����ļ�\n exit----------�˳�\n");
			continue;

		}


		if (strcmp(cmd, get) == 0) {
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
			printf("�������������get�������ļ�������������Ҫ���ص��ļ��������汾���ļ���\n");





			//�ļ�����   ����get��ʹ��
			char  str[20];
			//�ļ���
			scanf("%s", &str);
			ZeroMemory(sendbuf, DEFAULT_BUFLEN);
			strcpy(sendbuf, str);
			if (strcmp(str, quit) == 0) {



				printf("��������exit���˳�get\n");
				continue;
			}

			SetConsoleTextAttribute(hout,
				FOREGROUND_RED |
				FOREGROUND_GREEN |
				FOREGROUND_BLUE);
			iResult = send(ConnectSocket, sendbuf, strlen(sendbuf), 0);
			if (SOCKET_ERROR == iResult)
			{
				printf("send failed with error : %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return -1;
			}

			//�����ļ��ܴ�С
			int size;
			char sizestr[30];
			recv(ConnectSocket, sizestr, 30, 0);
			size = strtol(sizestr, NULL, 10);

			SetConsoleTextAttribute(hout,
				FOREGROUND_BLUE |
				FOREGROUND_RED);

			printf("�ܵ����ݴ�С��%ld\n", size);

			//cout<<"�������ļ���·��+�ļ�����:\n";
			char    CFileName[DEFAULT_BUFLEN] = "";
			scanf("%s", &CFileName);


			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

			printf("�����ļ���%s\n", CFileName);

			outfile.open(CFileName, ios::out | ios::binary);

			if (!outfile)   //���ļ�׼��д��
			{
				printf("Error of opening file !");

				iResult = shutdown(ConnectSocket, SD_SEND);
				iResult = closesocket(ConnectSocket);
				WSACleanup();
				return -1;
			}


			COORD   coord;

			CONSOLE_SCREEN_BUFFER_INFO csbi;

			hout = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hout, &csbi);
			coord.X = csbi.dwCursorPosition.X;
			coord.Y = csbi.dwCursorPosition.Y;

			float cnt = 0;

			while (issend)
			{
				//��ȡ��       

				rec = recv(ConnectSocket, szBuffer, BUFFER_SIZE, 0);
				cnt += rec;
				if (rec == 0)
				{
					SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED);
					printf("Download completed ! \n");
					break;
				}
				else if (rec == SOCKET_ERROR)
				{
					printf("Receive function failed with error : %d\n ", WSAGetLastError());
					break;
				}

				SetConsoleCursorPosition(hout, coord);
				SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);

				printf("Received %f%% bytes \n", cnt / size * 100);
				outfile.write(szBuffer, rec);

			}

			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			printf("Transfer is completed!\n");
			SetConsoleTextAttribute(hout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

			outfile.close();

		}

		// ��û������Ҫ���ͣ�����ر�����
		iResult = shutdown(ConnectSocket, SD_SEND);
		// Step 6: �ر�����
		iResult = closesocket(ConnectSocket);

		WSACleanup();
		return 0;

	}
}
