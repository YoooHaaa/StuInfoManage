
#include <Ws2tcpip.h>

#include "protocol.h"
#include <stdio.h>
#include "MyThreadPool.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

SOCKET g_socket;
SOCKET g_clentSocket;
CMyThreadPool g_pool;


void reportError(const char* pre)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    printf("%s:%s", pre, (char*)lpMsgBuf);


    LocalFree(lpMsgBuf);

}

//工作线程，用于不断接收消息
DWORD WINAPI WorkThread(LPVOID lpParameter)
{
    SYSTEM_INFO info;
    ::GetSystemInfo(&info); //获取cpu核心数

    g_pool.create(info.dwNumberOfProcessors);

    while (true)
    {
        HEADER head;
        int result = recv(g_clentSocket, (char*)&head, sizeof(head), 0);
        if (result < 0)
        {
            reportError("recv");
            break;
        }

        char szMsg[MAXBYTE] = { 0 };
        result = recv(g_clentSocket, szMsg, MAXBYTE, 0);
        if (result < 0)
        {
            reportError("recv");
            break;
        }
        g_pool.addTask(new MyWorkItem(head.nCmd, szMsg, g_clentSocket));
    }
    return 0;
}

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    g_socket = s;

    //绑定端口
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5588);
    if (inet_pton(AF_INET, "192.168.72.1", &addr.sin_addr) < 0)
    {
        reportError("inet_pton");
        return 0;
    }

    //绑定
    if (bind(g_socket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        reportError("bind");
        return 0;
    }
    else
    {
        printf("bind OK\n");
    }

    //监听
    if (listen(g_socket, SOMAXCONN) < 0)
    {
        reportError("listen");
        return 0;
    }
    else
    {
        printf("listen OK\n");
    }

    //接受连接
    sockaddr_in caddr;
    int len = sizeof(caddr);
    g_clentSocket = accept(g_socket, (sockaddr*)&caddr, &len);
    if (g_clentSocket < 0)
    {
        reportError("accept");
        return 0;
    }
    else
    {
        printf("accept OK\n");
    }

    //创建线程接收消息
    DWORD dwThreadId = 0;
    HANDLE hThread = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)WorkThread,
        (LPVOID)NULL,
        0, //立即运行
        &dwThreadId);
    if (hThread == NULL)
    {
        printf("create WorkThread fail!\n");
    }


    getchar();
    closesocket(g_socket);
    WSACleanup();
    return 0;
}

