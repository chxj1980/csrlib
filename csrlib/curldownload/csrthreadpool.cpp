#include "csrthreadpool.h"
#include "csrtask.h"
#include "csrthread.h"
#include <stdlib.h>
#include <windows.h>
#include "filenameio.h"

using namespace std;

#pragma comment(lib,"pthreadVC2.lib")

#if 0
//����C++�ӿ�
//���������ļ�
std::string strIniFile = GetMoudlePath();
strIniFile += "GB28181.ini";
cosip soip(strIniFile);

//��ʼ������ ��jrtp�����
soip.StartPSStream(0);

while (true)
{
	//���ϻ�ȡ��Ƶ��
	//����

	//sdl��ʾ
}
#endif


//�̳߳ع��캯��
CsrThreadPool::CsrThreadPool(size_t maxthreadnum)
{
	MaxThreadNum = maxthreadnum;		//	����̸߳���
	bDestroy = false;					//	�Ƿ������̳߳�
	bWaitDestroy = false;               //	�Ƿ�ȴ�����
	m_taskThreadMutex = NULL;			//	�����߳�����ÿ����һ����������Ҫ�����������
	m_freeThreadMutex = NULL;           //  �����߳�����ÿ����һ���߳�ִ����������Ҫ�����̶߳���
	WatiTime = 500;						//	�ȴ�ʱ��
	pthread_mutex_init(&m_taskThreadMutex, NULL);	//	��ʼ�����������
	pthread_cond_init(&m_threadCond, NULL);			//	��ʼ����������
	pthread_mutex_init(&m_freeThreadMutex, NULL);	//	��ʼ���̶߳�����
}

//��������󼰳�Ա
CsrThreadPool::~CsrThreadPool()
{
	cout << "�̳߳���������" << endl;

	//�ֱ��ͷ��̶߳��������������������������
	pthread_mutex_destroy(&m_taskThreadMutex);
	pthread_cond_destroy(&m_threadCond);
	pthread_mutex_destroy(&m_freeThreadMutex);
}

//�������
void CsrThreadPool::AddAsynTask(TaskFunc taskfunc, void* pParams)
{
	//��������ʼ��һ���߳�����
	CsrTask *task = new CsrTask(taskfunc, pParams);
	LockTaskQueue();			//	��ס�������
	m_queTask.push(task);		//	������񵽶���
	SignalTaskQueue();			//	֪ͨ����ִ��
	UnLockTaskQueue();			//	�������
}

//�����̳߳�
void CsrThreadPool::Activate()
{
	for (int i = 0; i < MaxThreadNum; ++i)
	{
		CsrThread* thread = new CsrThread(this);
		m_arrThreads.push_back(thread);
		thread->Start();
	}

	//����ɨ���߳�
	pthread_create(&m_taskThread, NULL, &ScanTask, this);
}

//�����̳߳�
void CsrThreadPool::Destroy()
{
	cout << "Destroy begin" << endl;
	bDestroy = true;
	SignalTaskQueue();	//֪ͨ�߳��˳��ȴ���

						//ֹͣɨ���߳�
	pthread_join(m_taskThread, NULL);

	//ֹͣ�����߳�
	size_t size = m_arrThreads.size();
	for (size_t i = 0; i < size; ++i)
	{
		CsrThread* thread = m_arrThreads[i];
		thread->Destroy();
		delete thread;
	}

	size_t remain = m_queTask.size();
	for (size_t i = 0; i < remain; ++i)
	{
		CsrTask *task = m_queTask.front();
		m_queTask.pop();
		delete task;
	}

	cout << "Destroy end" << endl;
}

//���������߳��������
void CsrThreadPool::WaitTaskFinishAndDestroy()
{
	bWaitDestroy = true;
	pthread_join(m_taskThread, NULL);

	//ֹͣ�����߳�
	for (size_t i = 0; i < m_arrThreads.size(); ++i)
	{
		CsrThread* thread = m_arrThreads[i];
		thread->Destroy();
		cout << "thread->Destroy()" << endl;
		delete thread;
	}
}

//��ӿ����߳̽��̳߳�
void CsrThreadPool::AddFreeThreadToQueue(CsrThread *thread)
{
	//�ȴ���һ������
	LockFreeThreadQueue();
	m_queFreeThreads.push(thread);
	UnLockFreeThreadQueue();
}

//�ı�����
void CsrThreadPool::SignalTaskQueue()
{
	pthread_cond_signal(&m_threadCond);
}

//��ס�������
void CsrThreadPool::LockTaskQueue()
{
	pthread_mutex_lock(&m_taskThreadMutex);
}

void CsrThreadPool::UnLockTaskQueue()
{
	pthread_mutex_unlock(&m_taskThreadMutex);
}

//��ס�����̶߳���
void CsrThreadPool::LockFreeThreadQueue()
{
	pthread_mutex_lock(&m_freeThreadMutex);
}

void CsrThreadPool::UnLockFreeThreadQueue()
{
	pthread_mutex_unlock(&m_freeThreadMutex);
}
//ɨ������
void* CsrThreadPool::ScanTask(void* pParams)
{
	CsrThreadPool *pool = (CsrThreadPool*)pParams;
	size_t queNum = 0;
	size_t freeQueNum = 0;
	while (true)
	{
		if (pool->bDestroy)
			break;
		pool->Start(&queNum, &freeQueNum);

		//�����������״̬�����߳�������Ϊ���ֱ������
		if (pool->bWaitDestroy && !queNum && freeQueNum == pool->GetMaxThreadNum())
		{
			pool->bDestroy = true;
			break;
		}

		//���������Ϊ�����˳�
		if (queNum)
		{
			if (freeQueNum)
				continue;
			else
				Sleep(pool->GetWatiTime());		//�޿����߳�
		}
		else//�����������
		{
			pool->LockTaskQueue();
			cout << pool->GetQueueTaskCount() << endl;
			if (!pool->GetQueueTaskCount())
			{
				cout << "����ȴ�" << endl;
				pool->WaitQueueTaskDignal();
				cout << "�����ȴ�" << endl;
			}
			pool->UnLockTaskQueue();
		}
	}
	cout << "Scantask end" << endl;
	return nullptr;
}

int CsrThreadPool::GetQueueTaskCount()
{
	return m_queTask.size();
}
void CsrThreadPool::WaitQueueTaskDignal()
{
	pthread_cond_wait(&m_threadCond, &m_taskThreadMutex);
}

/*
�������ܣ�ִ���߳�
�������1��������ʣ����������
�������2��������ʣ������߳�����
*/
void CsrThreadPool::Start(size_t *queue_remain_num, size_t *free_thread_num)
{
	cout << "��ʼִ������" << endl;
	LockFreeThreadQueue();
	if (!m_queFreeThreads.empty())
	{
		LockTaskQueue();
		if (!m_queTask.empty())
		{
			CsrTask* task = m_queTask.front();
			m_queTask.pop();
			CsrThread* freeThread = m_queFreeThreads.front();
			m_queFreeThreads.pop();
			freeThread->m_task = task;
			freeThread->Notify();

			*queue_remain_num = m_queTask.size();
		}
		else
		{
			*queue_remain_num = 0;
		}

		UnLockTaskQueue();

		*free_thread_num = m_queFreeThreads.size();
	}
	else
	{
		*free_thread_num = 0;
	}
	UnLockFreeThreadQueue();
	cout << "����ִ�����" << endl;
}