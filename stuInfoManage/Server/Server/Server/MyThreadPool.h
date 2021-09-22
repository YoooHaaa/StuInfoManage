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
    int m_nCount;      //当前线程数
    int m_nIdle;       //空闲线程数
    int m_nMaxThreads;  //最大线程数
    TASKLIST m_tasks; //任务列表
    CMyCriticalSection m_cs;
    CMySemaphore m_sem;
    bool m_bIsQuit;

    CMyConnectPool m_ConnectPool;//连接池
};

