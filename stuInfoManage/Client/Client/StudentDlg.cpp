// StudentDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "StudentDlg.h"
#include "afxdialogex.h"
#include <string.h>
#include "ClientDlg.h"


// StudentDlg �Ի���

IMPLEMENT_DYNAMIC(StudentDlg, CDialogEx)

StudentDlg::StudentDlg(CClientDlg* pThis, CWnd* pParent /*=NULL*/)
	: CDialogEx(DLG_STU, pParent)
{
    m_pClient = pThis;

    //���ӷ�����
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //����
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5588);
    if (inet_pton(AF_INET, "192.168.72.1", &addr.sin_addr) < 0)
    {
        AfxMessageBox(TEXT("inet_pton ERROR!"));
    }

    if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        int n = WSAGetLastError();
        AfxMessageBox(TEXT("connect ERROR!"));
        exit(1);
    }

    //�����߳����ڽ�����Ϣ
    ::CreateThread(NULL, 0, StudentDlg::workThread, this, 0, NULL);

}

StudentDlg::~StudentDlg()
{
}

void StudentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, CMB_CLASS, m_cmbClass);
    DDX_Control(pDX, CMB_SEX, m_cmbSex);
    DDX_Control(pDX, IDC_LIST, m_list);

    //�����Ա�ؼ�
    m_cmbSex.SetCurSel(0);
    m_cmbSex.AddString("");
    m_cmbSex.AddString("��");
    m_cmbSex.AddString("Ů");
    m_cmbSex.AddString("����");
    m_cmbSex.AddString("δ֪");

    //���ð༶�ؼ�
    m_cmbClass.SetCurSel(0);
    m_cmbClass.InsertString(0, "");
    m_cmbClass.InsertString(1, "����һ��");
    m_cmbClass.InsertString(2, "�������");
    m_cmbClass.InsertString(3, "��������");
    m_cmbClass.InsertString(4, "�����İ�");
    m_cmbClass.InsertString(5, "�������");
    m_cmbClass.InsertString(6, "��������");

    //������Ϣ��list�ؼ�
    RECT  rect;
    m_list.GetClientRect(&rect); //��ȡlist�Ŀͻ���,�������ÿһ�еĿ��

    m_list.InsertColumn(0, _T("ѧ���Ա�"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("ѧ���༶"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("ѧ������"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("ѧ��ID"), LVCFMT_LEFT, rect.right / 4);

    m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    //ˢ�����ݿ����Ϣ
    OnBnClickedCheck();

}


BEGIN_MESSAGE_MAP(StudentDlg, CDialogEx)
    ON_WM_CLOSE()
    ON_BN_CLICKED(BTN_ADD, &StudentDlg::OnBnClickedAdd)
    ON_BN_CLICKED(BTN_DEL, &StudentDlg::OnBnClickedDel)
    ON_BN_CLICKED(BTN_MODIFY, &StudentDlg::OnBnClickedModify)
    ON_BN_CLICKED(BTN_CHECK, &StudentDlg::OnBnClickedCheck)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &StudentDlg::OnLvnItemchangedList)
END_MESSAGE_MAP()


// StudentDlg ��Ϣ�������
void StudentDlg::printInfo(char* pInfo)
{
    //ɾ��ԭ������
    int nCount = m_list.GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        m_list.DeleteItem(0);
    }

    //�����ַ��� XX-XX-XX-XX-,������list�ؼ�
    char szMark[] = "-";
    char* token = strtok(pInfo, szMark);
    int nNum = 0;
    int nRow = -1;
    while (token != NULL)
    {
        if (nNum % 4 == 0)
        {
            nRow++;
            m_list.InsertItem(nRow, token);//������
        }
        else
        {
            m_list.SetItemText(nRow, nNum % 4, token);
        }

        token = strtok(NULL, szMark);
        nNum++;
    }
}

//������Ϣ
DWORD WINAPI StudentDlg::workThread(LPVOID lpParameter)
{
    StudentDlg* pThis = (StudentDlg*)lpParameter;

    while (true)
    {
        HEADER head;
        recv(pThis->m_socket, (char*)&head, sizeof(head), 0);

        switch (head.nCmd)
        {
        case MYSQL_OK:
        {
            //�����ַ����������ַ���
            char* pInfo = new char[head.nLen + 1];
            recv(pThis->m_socket, pInfo, head.nLen, 0);

            //���Ƿ��ǲ�ѯ�Ľ��
            if (pThis->m_bIsCheck)
            {
                pThis->m_pFIle = fopen("cache.txt", "a+");

                //дָ��
                fputs(pThis->m_szCheck, pThis->m_pFIle);
                fputs("\n", pThis->m_pFIle);

                //д��ѯ���
                fputs(pInfo, pThis->m_pFIle);
                fputs("\n", pThis->m_pFIle);

                //��ѯ����
                pThis->m_bIsCheck = false;
                fclose(pThis->m_pFIle);
            }

            //�������������ӡ���ؼ�
            pThis->printInfo(pInfo);

            delete[]pInfo;
            break;
        }
        case MYSQL_ERROR:
        {
            AfxMessageBox("���ݷ��ʹ���");
            break;
        }
        default:break;
        }

        //���ÿؼ�Ϊ��
        pThis->SetDlgItemText(EDT_ID, "");
        pThis->SetDlgItemText(EDT_NAME, "");
        pThis->m_cmbClass.SetCurSel(0);
        pThis->m_cmbSex.SetCurSel(0);
    }
    return 0;
}

//����ļ��Ƿ��иò�ѯָ��еĻ���������������true��û���򷵻�false
bool StudentDlg::checkFile(char * pOrder)
{
    m_pFIle = fopen("cache.txt", "r+");

    while (!feof(m_pFIle))
    {
        char szOrder[MAXWORD] = { 0 };
        fgets(szOrder, MAXWORD, m_pFIle);
        int nSize = (int)strlen(szOrder);
        if (nSize > 1)
        {
            szOrder[nSize - 1] = '\0';
        }

        if (strcmp(szOrder, pOrder) == 0)
        {
            char szInfo[MAXWORD] = { 0 };
            fgets(szInfo, MAXWORD, m_pFIle);
            printInfo(szInfo);
            fclose(m_pFIle);
            return true;
        }
    }
    fclose(m_pFIle);
    return false;
}

void StudentDlg::releaseFile()
{
    m_pFIle = fopen("cache.txt", "w+");
    fputc('\n', m_pFIle);

    fclose(m_pFIle);
}

BOOL StudentDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    CFont font;
    font.CreatePointFont(120, "����");
    GetDlgItem(IDC_STA)->SetFont(&font);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}

//#include "ClientDlg.h"
void StudentDlg::OnClose()
{
    m_pClient->OnClose();

    CDialogEx::OnClose();
}


void StudentDlg::OnBnClickedAdd()//�����Ϣ
{
    //���ݷ����ı䣬��ջ����ļ�
    releaseFile();

    //��ȡ�ͻ��������Ϣ
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //��ӵ�ָ��
    CString csInfo;
    csInfo.Format("insert into student.t_stu values('%s', '%s', '%s', '%s');",
        csID.GetBuffer(), csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer());

    //������Ϣ
    HEADER head;
    head.nCmd = MYSQL_ADD;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedDel()//ɾ����Ϣ
{
    //���ݷ����ı䣬��ջ����ļ�
    releaseFile();

    //��ȡ��ɾ������Ϣ
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    //��ȡ�к�
    int nRow = m_list.GetSelectionMark();

    //��ȡ�ı�
    csID = m_list.GetItemText(nRow, 0);
    csName = m_list.GetItemText(nRow, 1);
    csClass = m_list.GetItemText(nRow, 2);
    csSex = m_list.GetItemText(nRow, 3);

    //ɾ����ָ��
    CString csInfo;
    csInfo.Format("delete from student.t_stu where stu_id = '%s' and stu_name = '%s' and stu_class =\
        '%s'and stu_sex = '%s';",csID.GetBuffer(), csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer());

    //������Ϣ
    HEADER head;
    head.nCmd = MYSQL_DEL;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedModify()//�޸���Ϣ
{
    //���ݷ����ı䣬��ջ����ļ�
    releaseFile();

    //��ȡ�ͻ��������Ϣ
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //��ӵ�ָ��
    CString csInfo;
    csInfo.Format("update student.t_stu set stu_name = '%s', stu_class ='%s', stu_sex = '%s' \
        where stu_id = '%s';",csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer(), csID.GetBuffer() );

    //������Ϣ
    HEADER head;
    head.nCmd = MYSQL_MODIFY;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedCheck()//����
{
    //��ǰ���ڲ���״̬
    m_bIsCheck = true;

    //��ȡ�ͻ��������Ϣ
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //��ӵ�ָ��
    CString csIsID;
    CString csIsName;
    CString csIsClass;
    CString csIsSex;

    //�жϿͻ��Ƿ������˲�ѯ��Ϣ
    if (csID.GetLength() != 0)
    {
        csIsID += " and stu_id = '";
        csIsID += csID;
        csIsID += "'";
    }
    if (csName.GetLength() != 0)
    {
        csIsName += " and stu_name = '";
        csIsName += csName;
        csIsName += "'";
    }
    if (csClass.GetLength() != 0)
    {
        csIsClass += " and stu_class = '";
        csIsClass += csClass;
        csIsClass += "'";
    }
    if (csSex.GetLength() != 0)
    {
        csIsSex += " and stu_sex = '";
        csIsSex += csSex;
        csIsSex += "'";
    }

    //����Ϣƴ������
    CString csInfo;
    csInfo += "select * from student.t_stu where 1 = 1";
    csInfo += csIsID;
    csInfo += csIsName;
    csInfo += csIsClass;
    csInfo += csIsSex;
    csInfo += ";";

    bool ret = checkFile(csInfo.GetBuffer());
    if (ret)//�в�ѯ��¼,��ѯ������ֱ�ӷ���
    {
        m_bIsCheck = false;
        return;
    }

    //û���ҵ���¼����ʼд��¼
    memset(m_szCheck, '\0', 0X1000);
    strcpy_s(m_szCheck, 0X1000, csInfo.GetBuffer());

    //������Ϣ
    HEADER head;
    head.nCmd = MYSQL_CHECK;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}



void StudentDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)//����ĳһ�����ȡ���������
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    *pResult = 0;


    //��ȡ�������е���Ϣ
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    //��ȡ�к�
    int nRow = pNMLV->iItem;

    //��ȡ�ı�
    csID = m_list.GetItemText(nRow, 0);
    csName = m_list.GetItemText(nRow, 1);
    csClass = m_list.GetItemText(nRow, 2);
    csSex = m_list.GetItemText(nRow, 3);

    //��ʾ���ؼ�
    SetDlgItemText(EDT_ID, csID);
    SetDlgItemText(EDT_NAME, csName);
    m_cmbClass.SelectString(0, csClass);
    m_cmbSex.SelectString(0, csSex);
}
