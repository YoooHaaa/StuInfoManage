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
1. 处理命令
2. 发送命令和查表命令
3. 发送结果到客户端
*/
void  MyWorkItem::excute(MYSQL* pMysql)
{
    HEADER head;
    MYSQL_RES* pRes = nullptr;
    MYSQL_ROW row;

    //处理命令
    switch (m_nOrder)
    {
    case MYSQL_ADD:
    case MYSQL_DEL:
    case MYSQL_MODIFY:
    {
        int result = mysql_query(pMysql, m_szOrder);

        //如果增删改失败则发送失败指令，并返回
        if (result != 0)
        {
            const char* per = mysql_error(pMysql);
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //成功
        //获取表格数据
        char* pCheck = "select * from student.t_stu;";
        result = mysql_query(pMysql, pCheck);

        //如果获取表格数据失败则发送失败指令，并返回
        if (result != 0)
        {
            const char* per = mysql_error(pMysql);
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //保存结果
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

        //保存字段
        int nCount = mysql_num_fields(pRes);

        //保存信息
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
        printf("发送结果 ：%s\r\n", szBuf);
        //发送数据，数据格式XX-XXX-XX-XXX-XX-XXX-XX-XXXX-
        head.nCmd = MYSQL_OK;
        head.nLen = 0X1000;
        send(m_socket, (char*)&head, sizeof(head), 0);
        send(m_socket, szBuf, 0X1000, 0);

        break;
    }
    case MYSQL_CHECK:
    {
        int result = mysql_query(pMysql, m_szOrder);

        //如果查找失败则发送失败指令，并返回
        if (result != 0)
        {
            head.nCmd = MYSQL_ERROR;
            send(m_socket, (char*)&head, sizeof(head), 0);
            return;
        }

        //成功
        //保存结果
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

        //保存字段
        int nCount = mysql_num_fields(pRes);

        //保存信息
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

        //发送数据，数据格式XX-XXX-XX-XXX-XX-XXX-XX-XXXX-
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