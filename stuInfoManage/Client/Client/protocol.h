#pragma once


enum MYSQLORDER
{

    //Clent���͵�ָ��
    MYSQL_ADD,
    MYSQL_DEL,
    MYSQL_MODIFY,
    MYSQL_CHECK,

    //Server���͵�ָ��
    MYSQL_OK,
    MYSQL_ERROR
};

#pragma pack(1)
struct HEADER
{
    HEADER()
    {
        nCmd = -1;
        nLen = 0;
    }
    int nCmd;        //����
    int nLen;        //��Ϣ�ܳ���
};