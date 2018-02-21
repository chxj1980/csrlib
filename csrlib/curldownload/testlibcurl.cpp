#include <iostream>  
#include <string.h>  
#include <curl/curl.h>  
using namespace std;

#pragma comment(lib, "libcurl.lib")

#if 0
/*
	http��������Ļص�
*/
static size_t WriteFunc(void *input, size_t uSize, size_t uCount, void *avg)
{
	//�����󷵻�����input������avg�У� avgΪһ��ʼcurl_easy_setopt���ò���
	size_t uLen = uSize*uCount;
	string *pStr = (string*)avg;
	pStr->append((char*)(input), uLen);
	return uLen;
}

int main()
{
	CURL *pCurl = NULL;
	CURLcode code;

	/* CURLcode curl_global_init(long flags)
	@ ��ʼ��libcurl��ȫ��ֻ���һ��
	@ flags: CURL_GLOBAL_DEFAULT     // ��ͬ��CURL_GLOBAL_ALL
	CURL_GLOBAL_ALL         // ��ʼ�����еĿ��ܵĵ���
	CURL_GLOBAL_SSL         // ��ʼ��֧�ְ�ȫ�׽��ֲ�
	CURL_GLOBAL_WIN32       // ��ʼ��win32�׽��ֿ�
	CURL_GLOBAL_NOTHING     // û�ж���ĳ�ʼ��
	......
	*/
	code = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (code != CURLE_OK) {
		cout << "curl_global_init() Err" << endl;
		return -1;
	}

	/* CURL *curl_easy_init()
	@ ��ʼ��curl����CURL *curlָ��
	*/
	pCurl = curl_easy_init();
	if (pCurl == NULL) {
		cout << "curl_easy_init() Err" << endl;
		return -1;
	}

	string sUrl = "www.baidu.com";
	curl_slist *pHeaders = NULL;
	string sBuffer;

	/* struct curl_slist *curl_slist_append(struct curl_slist * list, const char * string);
	@ ���Http��Ϣͷ
	@ ����string����ʽΪname+": "+contents
	*/
	string header = "username: andyawang";
	pHeaders = curl_slist_append(pHeaders, header.c_str());

	/* CURLcode curl_easy_setopt(CURL *handle, CURLoption option, parameter);
	@ �����������Լ����ò���
	*/
	curl_easy_setopt(pCurl, CURLOPT_URL, "www.baidu.com");  // ���ʵ�URL  
	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaders);  // ����ͷ��(Ҫ��pHeader��û����)  
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 20);           // ��ʱ(��λS)  
	curl_easy_setopt(pCurl, CURLOPT_HEADER, 1);             // �������ݰ���HTTPͷ��  

	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &WriteFunc); // !���ݻص�����  
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &sBuffer);           // !���ݻص������ĲΣ�һ��ΪBuffer���ļ�fd  

	/* CURLcode curl_easy_perform(CURL *handle);
	@ ��ʼ����
	*/
	code = curl_easy_perform(pCurl);
	if (code != CURLE_OK) {
		cout << "curl_easy_perform() Err" << endl;
		return -1;
	}

	/* CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... );
	@ ��ȡ������������Ϣ
	@ info��CURLINFO_RESPONSE_CODE    // ��ȡ���ص�Http��
	CURLINFO_TOTAL_TIME      // ��ȡ�ܵ���������ʱ��
	CURLINFO_SIZE_DOWNLOAD   // ��ȡ���ص��ļ���С
	......
	*/
	long retCode = 0;
	code = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &retCode);
	if (code != CURLE_OK) {
		cout << "curl_easy_perform() Err" << endl;
		return -1;
	}
	cout << "[Http Return Code] : " << retCode << endl;
	cout << "[Http Context] : " << endl << sBuffer.c_str() << endl;

	/* void curl_easy_cleanup(CURL * handle);
	@ �ͷ�CURL *curlָ��
	*/
	curl_easy_cleanup(pCurl);

	/* void curl_global_cleanup(void);
	@ ��ʼ��libcurl��ȫ��Ҳֻ���һ��
	*/
	curl_global_cleanup();
}

#endif