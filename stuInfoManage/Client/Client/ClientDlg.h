
// ClientDlg.h : ͷ�ļ�
/*********************************************************/
//�޸�VC++Ŀ¼�� ����Ŀ¼�Ϳ�Ŀ¼
//64λ����
/*********************************************************/

#pragma once
#include "StudentDlg.h"




// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();


public:
    StudentDlg* m_pStuDlg = nullptr;
    int m_nTime = 0;


    afx_msg void OnClose();
};
