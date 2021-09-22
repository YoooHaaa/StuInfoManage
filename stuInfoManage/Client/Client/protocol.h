#pragma once


enum MYSQLORDER
{

    //Clent发送的指令
    MYSQL_ADD,
    MYSQL_DEL,
    MYSQL_MODIFY,
    MYSQL_CHECK,

    //Server发送的指令
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
    int nCmd;        //命令
    int nLen;        //信息总长度
};