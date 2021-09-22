
#include "MyThreadPool.h"
//#include <queue>



CMyThreadPool::CMyThreadPool()
    :m_sem(0, 10000)
{
    m_nCount = 0;
    m_nIdle = 0;
    m_nMaxThreads = 0;
    m_bIsQuit = false;
    m_ConnectPool.create(3);//���ӳ��ܹ����3������
}

DWORD WINAPI CMyThreadPool::workThread(LPVOID lpParameter)
{
    CMyThreadPool *pool = (CMyThreadPool*)lpParameter;

    while (!pool->m_bIsQuit)
    {
        InterlockedIncrement((LPLONG)&pool->m_nIdle);
        pool->m_sem.WaitForSingleObject(INFINITE);
        InterlockedDecrement((LPLONG)&pool->m_nIdle);
        while (!pool->m_bIsQuit)
        {
            CWorkItem *task = pool->getTask();
            if (task == NULL)
            {
                break;
            }
            
            MYSQL* pMySql = nullptr;
            while (!pMySql)
            {
                //��ȡ����
                pMySql = pool->m_ConnectPool.getConnect();
            }

            task->excute(pMySql);

            //�黹����
            pool->m_ConnectPool.releaseConnect(pMySql);
            delete task;
        }
    }
    return 0;
}

CMyThreadPool::~CMyThreadPool()
{
}

int CMyThreadPool::create(int maxThreads)
{
    if (m_bIsQuit)
    {
        return 0;
    }

    m_nMaxThreads = maxThreads;
    return 0;
}

int CMyThreadPool::destroy()
{
    if (m_bIsQuit)
    {
        return 0;
    }

    m_bIsQuit = true;

    m_sem.ReleaseSemaphore(m_nCount);

    return 0;
}

int CMyThreadPool::addTask(CWorkItem* task)
{
    if (m_bIsQuit)
    {
        return 0;
    }

    m_cs.lock();
    m_tasks.push_back(task);
    m_cs.unlock();


    //1.�п����߳�
    if (m_nIdle > 0)
    {
        m_sem.ReleaseSemaphore(1);
    }
    else
    {
        //�̴߳ﵽ�������
        //2.û�п����̣߳������߳�
        if (m_nCount < m_nMaxThreads)
        {
            ::CreateThread(NULL, 0, CMyThreadPool::workThread, this, 0, NULL);
            m_nCount++;//Ҫ����
        }
        m_sem.ReleaseSemaphore(1);
    }
    return 0;
}

CWorkItem* CMyThreadPool::getTask()
{
    m_cs.lock();

    if (m_tasks.empty())
    {
        m_cs.unlock();
        return NULL;
    }

    CWorkItem* task = m_tasks.front();
    m_tasks.pop_front();
    m_cs.unlock();

    return task;
}