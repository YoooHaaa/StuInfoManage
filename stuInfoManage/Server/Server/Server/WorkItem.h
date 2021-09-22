#pragma once

#include <stdio.h>
#include "MyConnectPool.h"
#include "protocol.h"
#include <windows.h>

#include <list>
using namespace std;

class CWorkItem
{
public:
    CWorkItem();
    virtual ~CWorkItem();

public:
    virtual void excute(MYSQL* pMysql) = 0;
};


typedef list<CWorkItem*> TASKLIST;


class MyWorkItem :public CWorkItem
{
public:
    MyWorkItem(int nOrder, char* pOrder, SOCKET s);
    ~MyWorkItem();

    /*
    1. 处理命令
    2. 发送命令和查表命令
    3. 发送结果到客户端
    */
    virtual void  excute(MYSQL* pMysql);


private:
    SOCKET m_socket;
    int m_nOrder = -1;
    char* m_szOrder = nullptr;
};