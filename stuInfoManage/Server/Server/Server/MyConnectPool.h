#pragma once
#include <mysql.h>
#include "MyCreateSemaphore.h"
#include "MyCriticalSection.h"



struct MYSQLPOOL
{
    MYSQLPOOL()
    {
        isUse = false;
    }
    MYSQL mysql;
    bool  isUse;
};

class CMyConnectPool
{
public:
    CMyConnectPool();
    ~CMyConnectPool();

public:
    int create(int maxConnect);

    MYSQL* getConnect();                  //��ȡ����
    void releaseConnect(MYSQL* pMysql);   //�黹����


private:
    MYSQLPOOL* m_pMySqlPool = nullptr;
    int m_nMaxConnect;  //���������

    CMyCriticalSection m_cs;
    CMySemaphore m_sem;
};

