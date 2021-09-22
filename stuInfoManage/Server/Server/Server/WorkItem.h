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
    1. ��������
    2. ��������Ͳ������
    3. ���ͽ�����ͻ���
    */
    virtual void  excute(MYSQL* pMysql);


private:
    SOCKET m_socket;
    int m_nOrder = -1;
    char* m_szOrder = nullptr;
};