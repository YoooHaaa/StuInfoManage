#include "MyConnectPool.h"



CMyConnectPool::CMyConnectPool() :m_sem(0, 10000)
{
}

/*
�ͷ����ӳ�
*/
CMyConnectPool::~CMyConnectPool()
{
    for (int i = 0; i < m_nMaxConnect; i++)
    {
        mysql_close(&m_pMySqlPool->mysql);
    }
    delete[]m_pMySqlPool;
}

/*
�������ӳ�
*/
int CMyConnectPool::create(int maxConnect)
{
    m_nMaxConnect = maxConnect;
    m_pMySqlPool = new MYSQLPOOL[maxConnect];

    int nNum = 0;
    while (nNum < maxConnect)
    {
        MYSQL mysql;

        //��ʼ��
        if (mysql_init(&mysql) == nullptr)
        {
            continue;
        }

        //�����ַ���
        if (mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk") != 0)
        {
            continue;
        }

        //�������ݿ�
        if (mysql_real_connect(&mysql, "localhost", "root", "root",
            NULL, 3306, nullptr, 0) == nullptr)
        {
            continue;
        }

        m_pMySqlPool[nNum].isUse = false;
        m_pMySqlPool[nNum].mysql = mysql;

        nNum++;
    }

    //�ͷ��ź���������ָ����������
    m_sem.ReleaseSemaphore(maxConnect);

    return 0;
}

MYSQL * CMyConnectPool::getConnect()
{
    m_sem.WaitForSingleObject(INFINITE);

    m_cs.lock();
    for (int i = 0; i < m_nMaxConnect; i++)
    {
        if (m_pMySqlPool[i].isUse == false)
        {
            m_pMySqlPool[i].isUse = true;
            return &m_pMySqlPool[i].mysql;
        }
    }
    m_cs.unlock();
    return nullptr;
}

void CMyConnectPool::releaseConnect(MYSQL* pMysql)
{
    m_sem.ReleaseSemaphore(1);
    for (int i = 0; i < m_nMaxConnect; i++)
    {
        if (&m_pMySqlPool[i].mysql == pMysql)
        {
            m_pMySqlPool[i].isUse = false;
        }
    }
    
}

