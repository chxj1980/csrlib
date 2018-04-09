#ifndef _CSR_FILE_SYSTEM_H_
#define _CSR_FILE_SYSTEM_H_

#include "comheader.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

typedef BOOL(CALLBACK *FINDFILESPROC)(LPCTSTR, LPARAM);

BOOL CALLBACK ShowFoundFiles(LPCTSTR pszFile, LPARAM lParam);

BOOL WINAPI FindAllFilesRecursively(LPCTSTR, FINDFILESPROC, LPARAM);

//��ȡ�ļ�·���������ļ�����
std::string GetModlePath();

//��ȡ�ļ�����·��
std::string GetModleFullPath();

//��ȡ�ļ�����
std::string GetModleName();

//��ȡ�����̷�
void GetAllDrive(std::vector<std::string>& allDri);

//�����ļ���
void TraverseDir(WCHAR* strDir);

void FindFiles(CString &strPath);

#endif // _CSR_FILE_SYSTEM_H_
