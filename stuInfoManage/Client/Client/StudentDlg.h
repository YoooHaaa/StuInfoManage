#pragma once
//#include "ClientDlg.h"
#include "protocol.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include <mysql.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "libmysql.lib")
using namespace std;
class CClientDlg;


class StudentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(StudentDlg)

public:
	StudentDlg(CClientDlg* pThis, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~StudentDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:

    static DWORD WINAPI workThread(LPVOID lpParameter);
    void printInfo(char* pInfo);//解析字符串并将字符串写进控件
    bool checkFile(char* pOrder);
    void releaseFile();//清空缓存文件
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    CClientDlg* m_pClient = nullptr;

    SOCKET m_socket;
    bool m_bIsCheck = false;//判断当前是否是在查找
    char m_szCheck[0X1000] = { 0 };//存放检查的指令
    FILE* m_pFIle = nullptr;//缓存文件指针

public:
    CListCtrl m_list;
    CComboBox m_cmbClass;
    CComboBox m_cmbSex;

    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDel();
    afx_msg void OnBnClickedModify();
    afx_msg void OnBnClickedCheck();
    afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
};
