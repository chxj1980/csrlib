#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI ThreadFun(LPVOID pParams) 
{
	printf("�߳�ID %d\n", GetCurrentThreadId());
	return 0;
}

//VOID WINAPI InitializeSRWLock(
//	_Out_ PSRWLOCK SRWLock
//);
////�������ܣ���ʼ��һ����д����
////����������һ��ָ���д����ָ�롣
//
//VOID WINAPI AcquireSRWLockExclusive(
//	_Inout_ PSRWLOCK SRWLock
//);
////�������ܣ��Զ�ռģʽ��ȡ����
////����������һ��ָ���д����ָ�롣
//
//VOID WINAPI ReleaseSRWLockExclusive(
//	_Inout_ PSRWLOCK SRWLock
//);
////�������ܣ��ͷ�һ���Զ�ռģʽ��ȡ������
////����������һ��ָ���д����ָ�롣
//
//
//VOID WINAPI AcquireSRWLockShared(
//	_Inout_ PSRWLOCK SRWLock
//);
////�������ܣ��Թ���ģʽ��ȡ����
////����������һ��ָ���д����ָ�롣
//
//
//VOID WINAPI ReleaseSRWLockShared(
//	_Inout_ PSRWLOCK SRWLock
//);
////�������ܣ��ͷ�һ���Թ���ģʽ��ȡ������
////����������һ��ָ���д����ָ�롣

BOOL SetConsoleColor(WORD wAttributes)
{
	
}


int main(void) 
{
	printf("���߳�\n");
	HANDLE hHandle = CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
	WaitForSingleObject(hHandle, INFINITE);
	system("pause");
	return 0;
}