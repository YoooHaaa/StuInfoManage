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

    MYSQL* getConnect();                  //获取连接
    void releaseConnect(MYSQL* pMysql);   //归还连接


private:
    MYSQLPOOL* m_pMySqlPool = nullptr;
    int m_nMaxConnect;  //最大连接数

    CMyCriticalSection m_cs;
    CMySemaphore m_sem;
};

