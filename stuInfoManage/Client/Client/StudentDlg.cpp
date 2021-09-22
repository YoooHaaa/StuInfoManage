// StudentDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "StudentDlg.h"
#include "afxdialogex.h"
#include <string.h>
#include "ClientDlg.h"


// StudentDlg 对话框

IMPLEMENT_DYNAMIC(StudentDlg, CDialogEx)

StudentDlg::StudentDlg(CClientDlg* pThis, CWnd* pParent /*=NULL*/)
	: CDialogEx(DLG_STU, pParent)
{
    m_pClient = pThis;

    //连接服务器
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //连接
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

    //创建线程用于接收消息
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

    //设置性别控件
    m_cmbSex.SetCurSel(0);
    m_cmbSex.AddString("");
    m_cmbSex.AddString("男");
    m_cmbSex.AddString("女");
    m_cmbSex.AddString("其他");
    m_cmbSex.AddString("未知");

    //设置班级控件
    m_cmbClass.SetCurSel(0);
    m_cmbClass.InsertString(0, "");
    m_cmbClass.InsertString(1, "科锐一班");
    m_cmbClass.InsertString(2, "科锐二班");
    m_cmbClass.InsertString(3, "科锐三班");
    m_cmbClass.InsertString(4, "科锐四班");
    m_cmbClass.InsertString(5, "科锐五班");
    m_cmbClass.InsertString(6, "科锐六班");

    //设置信息框list控件
    RECT  rect;
    m_list.GetClientRect(&rect); //获取list的客户区,方便调节每一列的宽度

    m_list.InsertColumn(0, _T("学生性别"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("学生班级"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("学生姓名"), LVCFMT_LEFT, rect.right / 4);
    m_list.InsertColumn(0, _T("学生ID"), LVCFMT_LEFT, rect.right / 4);

    m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    //刷出数据库的信息
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


// StudentDlg 消息处理程序
void StudentDlg::printInfo(char* pInfo)
{
    //删除原表内容
    int nCount = m_list.GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        m_list.DeleteItem(0);
    }

    //解析字符串 XX-XX-XX-XX-,并插入list控件
    char szMark[] = "-";
    char* token = strtok(pInfo, szMark);
    int nNum = 0;
    int nRow = -1;
    while (token != NULL)
    {
        if (nNum % 4 == 0)
        {
            nRow++;
            m_list.InsertItem(nRow, token);//插入项
        }
        else
        {
            m_list.SetItemText(nRow, nNum % 4, token);
        }

        token = strtok(NULL, szMark);
        nNum++;
    }
}

//接收消息
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
            //接收字符串，解析字符串
            char* pInfo = new char[head.nLen + 1];
            recv(pThis->m_socket, pInfo, head.nLen, 0);

            //看是否是查询的结果
            if (pThis->m_bIsCheck)
            {
                pThis->m_pFIle = fopen("cache.txt", "a+");

                //写指令
                fputs(pThis->m_szCheck, pThis->m_pFIle);
                fputs("\n", pThis->m_pFIle);

                //写查询结果
                fputs(pInfo, pThis->m_pFIle);
                fputs("\n", pThis->m_pFIle);

                //查询结束
                pThis->m_bIsCheck = false;
                fclose(pThis->m_pFIle);
            }

            //将结果解析并打印到控件
            pThis->printInfo(pInfo);

            delete[]pInfo;
            break;
        }
        case MYSQL_ERROR:
        {
            AfxMessageBox("数据发送错误！");
            break;
        }
        default:break;
        }

        //设置控件为空
        pThis->SetDlgItemText(EDT_ID, "");
        pThis->SetDlgItemText(EDT_NAME, "");
        pThis->m_cmbClass.SetCurSel(0);
        pThis->m_cmbSex.SetCurSel(0);
    }
    return 0;
}

//检查文件是否有该查询指令，有的话，输出结果并返回true，没有则返回false
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

    // TODO:  在此添加额外的初始化

    CFont font;
    font.CreatePointFont(120, "楷体");
    GetDlgItem(IDC_STA)->SetFont(&font);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

//#include "ClientDlg.h"
void StudentDlg::OnClose()
{
    m_pClient->OnClose();

    CDialogEx::OnClose();
}


void StudentDlg::OnBnClickedAdd()//添加信息
{
    //数据发生改变，清空缓存文件
    releaseFile();

    //获取客户输入的信息
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //添加的指令
    CString csInfo;
    csInfo.Format("insert into student.t_stu values('%s', '%s', '%s', '%s');",
        csID.GetBuffer(), csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer());

    //发送信息
    HEADER head;
    head.nCmd = MYSQL_ADD;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedDel()//删除信息
{
    //数据发生改变，清空缓存文件
    releaseFile();

    //获取待删除的信息
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    //获取行号
    int nRow = m_list.GetSelectionMark();

    //获取文本
    csID = m_list.GetItemText(nRow, 0);
    csName = m_list.GetItemText(nRow, 1);
    csClass = m_list.GetItemText(nRow, 2);
    csSex = m_list.GetItemText(nRow, 3);

    //删除的指令
    CString csInfo;
    csInfo.Format("delete from student.t_stu where stu_id = '%s' and stu_name = '%s' and stu_class =\
        '%s'and stu_sex = '%s';",csID.GetBuffer(), csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer());

    //发送信息
    HEADER head;
    head.nCmd = MYSQL_DEL;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedModify()//修改信息
{
    //数据发生改变，清空缓存文件
    releaseFile();

    //获取客户输入的信息
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //添加的指令
    CString csInfo;
    csInfo.Format("update student.t_stu set stu_name = '%s', stu_class ='%s', stu_sex = '%s' \
        where stu_id = '%s';",csName.GetBuffer(), csClass.GetBuffer(), csSex.GetBuffer(), csID.GetBuffer() );

    //发送信息
    HEADER head;
    head.nCmd = MYSQL_MODIFY;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}


void StudentDlg::OnBnClickedCheck()//查找
{
    //当前处于查找状态
    m_bIsCheck = true;

    //获取客户输入的信息
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    GetDlgItemText(EDT_ID, csID);
    GetDlgItemText(EDT_NAME, csName);
    GetDlgItemText(CMB_CLASS, csClass);
    GetDlgItemText(CMB_SEX, csSex);

    //添加的指令
    CString csIsID;
    CString csIsName;
    CString csIsClass;
    CString csIsSex;

    //判断客户是否输入了查询信息
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

    //将信息拼接起来
    CString csInfo;
    csInfo += "select * from student.t_stu where 1 = 1";
    csInfo += csIsID;
    csInfo += csIsName;
    csInfo += csIsClass;
    csInfo += csIsSex;
    csInfo += ";";

    bool ret = checkFile(csInfo.GetBuffer());
    if (ret)//有查询记录,查询结束，直接返回
    {
        m_bIsCheck = false;
        return;
    }

    //没有找到记录，则开始写记录
    memset(m_szCheck, '\0', 0X1000);
    strcpy_s(m_szCheck, 0X1000, csInfo.GetBuffer());

    //发送信息
    HEADER head;
    head.nCmd = MYSQL_CHECK;
    head.nLen = csInfo.GetLength();

    send(m_socket, (char*)&head, sizeof(head), 0);
    send(m_socket, csInfo.GetBuffer(), head.nLen, 0);
}



void StudentDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)//单机某一项，就提取该项的数据
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;


    //获取单击的列的信息
    CString csID;
    CString csName;
    CString csClass;
    CString csSex;

    //获取行号
    int nRow = pNMLV->iItem;

    //获取文本
    csID = m_list.GetItemText(nRow, 0);
    csName = m_list.GetItemText(nRow, 1);
    csClass = m_list.GetItemText(nRow, 2);
    csSex = m_list.GetItemText(nRow, 3);

    //显示到控件
    SetDlgItemText(EDT_ID, csID);
    SetDlgItemText(EDT_NAME, csName);
    m_cmbClass.SelectString(0, csClass);
    m_cmbSex.SelectString(0, csSex);
}
