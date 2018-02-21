#ifndef _CSR_CURL_DOWNLOAD_H_
#define _CSR_CURL_DOWNLOAD_H_

/*
����curl�����ļ�
*/
#include <stdlib.h>

//д�ļ��ص�����
typedef size_t(*WriteFileCallBack)(void* contents, size_t size, size_t nmem, void *usrp);

//���ع�����
class CsrCurlDownload {
public:
	CsrCurlDownload();
	~CsrCurlDownload();

	int Process(const char* url, WriteFileCallBack callback_fun, void *pParams);
};


#endif  //_CURL_DOWNLOAD_H_
