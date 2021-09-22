#include "WorkItem.h"



CWorkItem::CWorkItem()
{
}


CWorkItem::~CWorkItem()
{
}


MyWorkItem::MyWorkItem(int nOrder, char* pOrder, SOCKET s)
{
    m_socket = s;
    m_nOrder = nOrder;
    m_szOrder = new char[strlen(pOrder) + 1];
    strcpy_s(m_szOrder, strlen(pOrder) + 1, pOrder);
}
MyWorkItem::~MyWorkItem()
{
    if (m_szOrder != nullptr)
    {
        delete[]m_szOrder;
    }
}

/*
1. ��������
2. ��������Ͳ������
3. ���ͽ�����ͻ���
*/
void  MyWorkItem::excute(MYSQL* pMysql)
{
    HEADER head;
    MYSQL_RES* pRes = nullptr;
    MYSQL_ROW row;

    //��������
    switch (m_nOrder)
    {
    case MYSQL_ADD:
    case MYSQL_DEL:
    case MYSQL_MODIFY:
    {
        int result = mysql_query(pMysql, m_szOrder);

        //�����ɾ��ʧ������ʧ��ָ�������
        if (result != 0)
        {
            const char* per = mysql_error(pMysql);
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //�ɹ�
        //��ȡ�������
        char* pCheck = "select * from student.t_stu;";
        result = mysql_query(pMysql, pCheck);

        //�����ȡ�������ʧ������ʧ��ָ�������
        if (result != 0)
        {
            const char* per = mysql_error(pMysql);
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //������
        if (pRes != nullptr)
        {
            mysql_free_result(pRes);
            pRes = nullptr;
        }
        pRes = mysql_store_result(pMysql);
        if (pRes == nullptr)
        {
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //�����ֶ�
        int nCount = mysql_num_fields(pRes);

        //������Ϣ
        row = mysql_fetch_row(pRes);
        char szBuf[0X1000] = { 0 };
        while (row != nullptr)
        {
            strcat_s(szBuf, 0X1000, row[0]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[1]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[2]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[3]);
            strcat_s(szBuf, 0X1000, "-");

            row = mysql_fetch_row(pRes);
        }
        printf("���ͽ�� ��%s\r\n", szBuf);
        //�������ݣ����ݸ�ʽXX-XXX-XX-XXX-XX-XXX-XX-XXXX-
        head.nCmd = MYSQL_OK;
        head.nLen = 0X1000;
        send(m_socket, (char*)&head, sizeof(head), 0);
        send(m_socket, szBuf, 0X1000, 0);

        break;
    }
    case MYSQL_CHECK:
    {
        int result = mysql_query(pMysql, m_szOrder);

        //�������ʧ������ʧ��ָ�������
        if (result != 0)
        {
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //�ɹ�
        //������
        if (pRes != nullptr)
        {
            mysql_free_result(pRes);
            pRes = nullptr;
        }
        pRes = mysql_store_result(pMysql);
        if (pRes == nullptr)
        {
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //�����ֶ�
        int nCount = mysql_num_fields(pRes);

        //������Ϣ
        row = mysql_fetch_row(pRes);
        char szBuf[0X1000] = { 0 };
        while (row != nullptr)
        {
            strcat_s(szBuf, 0X1000, row[0]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[1]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[2]);
            strcat_s(szBuf, 0X1000, "-");
            strcat_s(szBuf, 0X1000, row[3]);
            strcat_s(szBuf, 0X1000, "-");

            row = mysql_fetch_row(pRes);
        }

        //�������ݣ����ݸ�ʽXX-XXX-XX-XXX-XX-XXX-XX-XXXX-
        head.nCmd = MYSQL_OK;
        head.nLen = 0X1000;
        send(m_socket, (char*)&head, sizeof(head), 0);
        send(m_socket, szBuf, 0X1000, 0);

        break;
    }
    default: break;
    }
    return;
}