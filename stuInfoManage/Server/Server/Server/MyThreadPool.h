#pragma once
#include "MyCriticalSection.h"
#include "MyCreateSemaphore.h"
#include "MyConnectPool.h"
#include "WorkItem.h"
#include <list>
using namespace std;




class CMyThreadPool
{
public:
    CMyThreadPool();
    ~CMyThreadPool();
public:
    int create(int maxThreads);
    int destroy();
    int addTask(CWorkItem* task);
private:
    CWorkItem* getTask();
    static DWORD WINAPI workThread(LPVOID lpParameter);
private:
    int m_nCount;      //��ǰ�߳���
    int m_nIdle;       //�����߳���
    int m_nMaxThreads;  //����߳���
    TASKLIST m_tasks; //�����б�
    CMyCriticalSection m_cs;
    CMySemaphore m_sem;
    bool m_bIsQuit;

    CMyConnectPool m_ConnectPool;//���ӳ�
};

