#include "csrfilesystem.h"

//��ȡ�ļ�����·��
std::string GetModleFullPath()
{
	char lpBuf[MAX_PATH];
	wchar_t *lpstr1 = new wchar_t[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, lpstr1);

	//get modle name
	CString strCurPath;
	GetModuleFileName(NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH);

	CString strBuf = lpstr1;
	strBuf += "\\";
	strBuf += strCurPath;

	USES_CONVERSION;
	return W2A(strBuf.GetBuffer());
}

//��ȡ�ļ�·���������ļ�����
std::string GetModlePath()
{
	char lpBuf[MAX_PATH];
	CString strBuf;
	wchar_t *lpstr1 = new wchar_t[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, lpstr1);
	
	USES_CONVERSION;
	strBuf = lpstr1;
	strBuf += "\\";
	return W2A(strBuf.GetBuffer());
}

//��ȡ�ļ�����
std::string GetModleName()
{
	//get modle name
	CString strCurPath;
	GetModuleFileName(NULL, strCurPath.GetBuffer(MAX_PATH), MAX_PATH);

	USES_CONVERSION;
	return W2A(strCurPath.GetBuffer());
}
#if 1
//�����ļ���
 void TraverseDir(WCHAR* strDir)
{
	WIN32_FIND_DATA wfd = { 0 };
	WCHAR szText[256] = { 0 };
	wsprintf(szText, L"%s/*.*", strDir);
	HANDLE hFind = FindFirstFileW(szText, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Invalid file handle. Error is %u \n", GetLastError());
		return;
	}
	if((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
	//if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
	{
		wprintf(L"%s\n", wfd.cFileName);
	}
	else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY&& _tcscmp(wfd.cFileName, L".") != 0&& _tcscmp(wfd.cFileName, L".") != 0)
	{
		wprintf(L"[%s]\n", wfd.cFileName);
		WCHAR szTextr[256] = { 0 };
		//strcat(szTextr, strDir);
		wsprintf(szText, L"%s/%s", strDir, wfd.cFileName);
		//sprintf(szTextr, "/%s", wfd.cFileName);
		TraverseDir(szTextr);
	}
	while (FindNextFile(hFind, &wfd))
	{
		if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			wprintf(L"%s\n", wfd.cFileName);
		}
		else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && _tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0)
		{
			wprintf(L"[%s]\n", wfd.cFileName);
			WCHAR szTexts[256] = { 0 };
			wcscat(szTexts, strDir);
			wsprintf(szTexts, L"/%s", wfd.cFileName);
			TraverseDir(szTexts);
		}
	}
}
#endif
//���� shlwapi.dll �е� Windows API ϵ�У�
/*
// ��չ����ز���
PathAddExtension
PathFindExtension
PathRemoveExtension
PathRenameExtension
// ����·�����������·��
PathAppend
PathCombine
// �ж��Ƿ��Ŀ¼���ļ���
PathIsRoot
PathIsDirectory
// �ļ��Ƿ����
PathFileExists
// �ļ����Ƿ�Ϸ�
PathIsFileSpec
// ��ȡ���Ƴ������в���
PathGetArgs
PathRemoveArgs
// �ļ����Ƿ�ƥ��ͨ���
PathMatchSpec
// �õ�һ���ļ����������һ�ļ��е����·��
PathRelativePathTo
// �����ļ���
PathFindFileName
// �Ƴ��ļ���
PathRemoveFileSpec
*/
BOOL CALLBACK ShowFoundFiles(LPCTSTR pszFile, LPARAM lParam)
{
	_tprintf(TEXT("%s\n"), pszFile);
	// ���� TRUE �������ң����� FALSE ��ֹ��
	return TRUE;
}

BOOL WINAPI FindAllFilesRecursively(LPCTSTR pszRootPath,FINDFILESPROC ffproc,LPARAM lpParam)
{
	TCHAR szFileFind[MAX_PATH] = { 0 };
	if (PathCombine(szFileFind, pszRootPath, TEXT("*.*")) == NULL)
	{
		SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFindFile = FindFirstFile(szFileFind, &wfd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	__try {
		do {
			if ((lstrcmpi(wfd.cFileName, TEXT(".")) != 0) &&
				(lstrcmpi(wfd.cFileName, TEXT("..")) != 0))
			{
				BOOL fResult = FALSE;
				TCHAR szNextPath[MAX_PATH] = { 0 };
				if (PathCombine(szNextPath, pszRootPath, wfd.cFileName) == NULL)
				{
					return FALSE;
				}
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					fResult = FindAllFilesRecursively(szNextPath, ffproc, lpParam);
				}
				else
				{
					fResult = ffproc(szNextPath, lpParam);
				}
				if (!fResult)
				{
					return FALSE;
				}
			}
		} while (FindNextFile(hFindFile, &wfd));
	}
	__finally {
		FindClose(hFindFile);
	}
	return TRUE;
}


//��������̷�
void GetAllDrive(std::vector<std::string>& allDri)
{
	WCHAR Drive[MAX_PATH] = { 0 };
	int i = 0;
	GetLogicalDriveStrings(MAX_PATH, Drive);
	char strDri[10];
	while (Drive[i - 1] != '\0' || Drive[i] != '\0')
	{
		sprintf(strDri, "%s", &Drive[i]);
		i += 2;
		allDri.push_back(std::string(strDri));
	}
}

#if 0
void list_current_dir()
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch = NULL;
	BOOL finished = FALSE;

	printf("%d\n", sizeof(FILETIME));
	printf("%d\n", sizeof(WIN32_FIND_DATA));

	hSearch = FindFirstFile("*", &FileData);  // find for all files
	if (hSearch == INVALID_HANDLE_VALUE)
		return;   // No file found

	while (!finished)
	{
		//puts(FileData.cFileName);    // print file name
		printf("%s\n", FileData.cFileName);
		finished = !FindNextFile(hSearch, &FileData);   // find next file
	}
	FindClose(hSearch);  // finish searching
}
#endif

void FindFiles(CString &strPath)
{
	WIN32_FIND_DATA findData;

	CString strTemp = strPath;
	strTemp.Format(_T("%s\\*.*"), strPath);//����ָ��Ŀ¼�µ�ֱ�ӵ������ļ���Ŀ¼

	HANDLE hFile = FindFirstFile(strTemp, &findData);
	while (hFile != INVALID_HANDLE_VALUE)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//�����Ŀ¼
		{
			if (findData.cFileName[0] != _T('.'))//�ų�.��..�ļ���
			{

				strTemp = strPath + CString("\\") + findData.cFileName;//�������·��
				FindFiles(strTemp);//�ݹ���ҵ�ǰĿ¼���ļ�
			}
		}
		else
		{
			strTemp = strPath + CString("\\") + findData.cFileName;
		}

		if (!FindNextFile(hFile, &findData))
		{
			break;
		}
	}
}