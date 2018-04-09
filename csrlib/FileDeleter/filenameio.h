#pragma once

#include <stdio.h>
#include <windows.h>

//��ȡ�ļ���׺
char* CsrGetFileExt(char* strFile)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	printf("Path created with _makepath: %s\n\n", strFile);
	_splitpath(strFile, drive, dir, fname, ext);

	return ext;
}


//�ж��ļ���׺�Ƿ����Ҫ��
int CsrCmpExt(char* strFile, char* strExt)
{
	return strcmp(CsrGetFileExt(strFile), strExt);
}