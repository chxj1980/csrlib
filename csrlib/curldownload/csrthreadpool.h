#ifndef __CSR_THREAD_POOL_H_
#define __CSR_THREAD_POOL_H_

/*
�̳߳ش���
----------------------------------------------------------------------------
---->�����̳߳�---->�������--->ѡ������̴߳�������--->���������ͷ��߳�
//�����̵߳Ĺ������ݺ�ÿ���̵߳Ķ�������
*/

#include <pthread/pthread.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include "csrtask.h"
#include "csrdef.h"

//�����߳�
class CsrThread;

//�̳߳�
class CsrThreadPool {
public:
	CsrThreadPool(size_t maxthreadnum);
	~CsrThreadPool();

	void AddAsynTask(TaskFunc taskfunc, void* pParams);		//�������
	void Activate();		//�����̳߳�
	void Destroy();			//�����̳߳�
	void WaitTaskFinishAndDestroy();				//���������߳��������
	void AddFreeThreadToQueue(CsrThread *thread);	//��ӿ����߳̽��̳߳�
	/*
	�������ܣ�ִ���߳�
	�������1��������ʣ���߳�����
	�������2��������ʣ������߳�����
	*/
	void Start(size_t *queue_remain_num, size_t *free_thread_num);

	bool bDestroy;		//�����߳�
	bool bWaitDestroy;	//�ȴ������߳�
	int GetQueueTaskCount();
	void WaitQueueTaskDignal();

private:
	TPGETSET(unsigned, WatiTime)		//	�ȴ�ʱ��
	TPGETSET(size_t, MaxThreadNum)		//	�̳߳�����
	TPGETSET(int, FreeThreadNum)		//	�����߳�

	static void* ScanTask(void* pParams);	//ɨ������

	void SignalTaskQueue();
	void LockTaskQueue();
	void UnLockTaskQueue();


	void LockFreeThreadQueue();
	void UnLockFreeThreadQueue();

	std::vector<CsrThread*> m_arrThreads;			//	�̳߳�***����
	std::queue<CsrThread*>  m_queFreeThreads;		//	�����̳߳ض���
	pthread_mutex_t			  m_freeThreadMutex;	//	�̳߳ػ�����

	std::queue<CsrTask*>  m_queTask;				//  �������
	pthread_mutex_t			  m_taskThreadMutex;	//  �̳߳ػ�����
	pthread_cond_t			  m_threadCond;			//	�̳߳ض����ź�

	pthread_t                 m_taskThread;			//	����ɨ���̣߳��ж��̳߳��Ƿ���δ�������
};

#endif		//_THREAD_POOL_H_