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
	StudentDlg(CClientDlg* pThis, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~StudentDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:

    static DWORD WINAPI workThread(LPVOID lpParameter);
    void printInfo(char* pInfo);//�����ַ��������ַ���д���ؼ�
    bool checkFile(char* pOrder);
    void releaseFile();//��ջ����ļ�
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    CClientDlg* m_pClient = nullptr;

    SOCKET m_socket;
    bool m_bIsCheck = false;//�жϵ�ǰ�Ƿ����ڲ���
    char m_szCheck[0X1000] = { 0 };//��ż���ָ��
    FILE* m_pFIle = nullptr;//�����ļ�ָ��

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
