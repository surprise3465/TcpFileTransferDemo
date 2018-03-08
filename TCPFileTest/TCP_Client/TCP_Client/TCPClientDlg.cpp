// TCPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP_Client.h"
#include "TCPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define READSIZE 1024*20
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTCPClientDlg 对话框
CTCPClientDlg::CTCPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPClientDlg::IDD, pParent)
{
	m_socket=NULL;
	m_Port=6000;
	m_strIPAddress="";
	m_fileinfo.FileSize=0;
	m_FileNumber=0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

UINT ClientWaitThread(LPVOID para)
{
	CTCPClientDlg* pDlg=(CTCPClientDlg*)para;
	
	char bufname[1024];
	char buf[READSIZE];
	memset(buf, 0, sizeof(buf));

	while(1)
	{
		if(pDlg->m_socket==NULL)
			break;
		int len=0;

		int length = recv(pDlg->m_socket, (char*)(&pDlg->m_fileinfo.FileSize), sizeof(4), 0);//接收文件长度
		if(length <=0){break;}
		length = recv(pDlg->m_socket, (char*)(&len), sizeof(4), 0);//接收文件名长度	
		if(length <=0){break;}
		length = recv(pDlg->m_socket, (char*)bufname, len+1, 0);//接收文件名
		if(length <=0){break;}

		//打开接收文件对话框
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"所有文件 (*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
		{ 
			pDlg->m_FilePath=dlg.GetPathName();
		}

		int dwCount=0;	
		pDlg->m_ctrlProgress.SetRange32(0, pDlg->m_fileinfo.FileSize);
		length = 0;

		CFile file(pDlg->m_FilePath, CFile::modeCreate|CFile::modeWrite);
		pDlg->m_FileName=file.GetFileName();	//获取保持文件名
		for(;;)
		{
			length = recv(pDlg->m_socket, (char*)buf, READSIZE, 0);
			if(length==0)
				break;

			file.Write(buf, length);
			dwCount += length;

			pDlg->m_ctrlProgress.SetPos(dwCount);
			if(dwCount==pDlg->m_fileinfo.FileSize)
				break;

		}//for(;;)

		pDlg->m_ctrlProgress.SetPos(0);	
		file.Close();//关闭文件

		//将接收的文件显示在列表中
		pDlg->m_ListCtrl.InsertItem(pDlg->m_FileNumber,L"");
		pDlg->m_ListCtrl.SetItemText(pDlg->m_FileNumber,0,pDlg->m_FileName);
		CString m_FileSize;
		m_FileSize.Format(L"%dk",pDlg->m_fileinfo.FileSize);
		pDlg->m_ListCtrl.SetItemText(pDlg->m_FileNumber,1,m_FileSize);
		pDlg->m_ListCtrl.SetItemText(pDlg->m_FileNumber,2,pDlg->m_FilePath);

		pDlg->m_FileNumber++;

	}//whlie

	return 0;

}

void CTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress);
	DDX_Control(pDX, IDC_LIST_FILE, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_LINK, &CTCPClientDlg::OnBnClickedBtLink)
	ON_BN_CLICKED(IDC_BT_END, &CTCPClientDlg::OnBnClickedBtEnd)
	ON_BN_CLICKED(IDC_BT_FILE, &CTCPClientDlg::OnBnClickedBtFile)
	ON_BN_CLICKED(IDC_REQUEST, &CTCPClientDlg::OnBnClickedRequest)
END_MESSAGE_MAP()


// CTCPClientDlg 消息处理程序

BOOL CTCPClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_REQUEST)->EnableWindow(FALSE);
	//设置端口号
	CString Port;
	Port.Format(L"%d",m_Port);
	GetDlgItem(IDC_PORT)->SetWindowText(Port);
	//初始化列表
	m_ListCtrl.SetExtendedStyle(LVS_EX_FLATSB|
		LVS_EX_FULLROWSELECT|
		LVS_EX_HEADERDRAGDROP|
		LVS_EX_ONECLICKACTIVATE|
		LVS_EX_GRIDLINES);
	//设置表头
	m_ListCtrl.InsertColumn(0,L"名称",LVCFMT_LEFT,80,0);
	m_ListCtrl.InsertColumn(1,L"大小",LVCFMT_LEFT,80,2);
	m_ListCtrl.InsertColumn(2,L"路径",LVCFMT_LEFT,130,3);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTCPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTCPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTCPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTCPClientDlg::FileSend()
{
	CFile file;
	if(!file.Open(m_FilePath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("文件打开失败"));
		return ;
	}
	char buf[READSIZE] = {0};
	file.Seek(0, CFile::begin);

	int nLen=0;//读取长度
	int nSize=0;//发送长度
	DWORD dwCount=0;

	m_ctrlProgress.SetRange32(0, m_fileinfo.FileSize);

	for(;;)
	{
		nLen = file.Read(buf, READSIZE);
		if(nLen == 0)
			break;
		nSize = send(m_socket, (const char *)buf, nLen, 0);

		dwCount += nSize;
		m_ctrlProgress.SetPos(dwCount);
	}
	file.Close();
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_BT_FILE)->EnableWindow(TRUE);
}

void CTCPClientDlg::OnBnClickedBtLink()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BT_LINK)->EnableWindow(FALSE);

	m_socket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	svrAddr.sin_family = AF_INET; 
	GetDlgItem(IDC_IPADDRESS2)->GetWindowText(m_strIPAddress);	//获取IP
	USES_CONVERSION;//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char * pFileName = T2A(m_strIPAddress);  
	svrAddr.sin_addr.s_addr = inet_addr(pFileName);
	m_Port = (DWORD)GetDlgItemInt(IDC_PORT);
	svrAddr.sin_port = htons(m_Port);        
	memset(&(svrAddr.sin_zero), 0, sizeof(svrAddr.sin_zero));    // 向服务器发出连接请求 
  
	if (connect(m_socket, (SOCKADDR *)&svrAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)      
	{  
		AfxMessageBox(L"connet fail!");
		GetDlgItem(IDC_BT_LINK)->EnableWindow(TRUE);
		return;       
	}
	//套接字创建
	m_ClientThread=AfxBeginThread(ClientWaitThread,this);
	m_ClientThread->m_bAutoDelete=TRUE;
}

void CTCPClientDlg::OnBnClickedBtEnd()
{
	// TODO: Add your control notification handler code here
	m_socket=NULL;
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_BT_LINK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_FILE)->EnableWindow(TRUE);
}

void CTCPClientDlg::OnBnClickedBtFile()
{
	char* bufname;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"所有文件 (*.*)|*.*||", this);
	if(dlg.DoModal() == IDOK)
	{
		m_FilePath = dlg.GetPathName();//获取文件路径	
		m_fileinfo.FileName=dlg.GetFileName();//获取文件名	
		CFile file(m_FilePath, CFile::modeRead);//打开文件	
		m_fileinfo.FileSize = file.GetLength();//获取文件大小	
		file.Close();//关闭文件

		//在列表中显示发送文件
		m_FileSize.Format(L"%dk", m_fileinfo.FileSize);
		m_ListCtrl.InsertItem(m_FileNumber,L"");
		m_ListCtrl.SetItemText(m_FileNumber,0,m_fileinfo.FileName);
		m_ListCtrl.SetItemText(m_FileNumber,1,m_FileSize);
		m_ListCtrl.SetItemText(m_FileNumber,2,m_FilePath);
		m_FileNumber++;

		GetDlgItem(IDC_BT_FILE)->EnableWindow(FALSE);

		int nSize = send(m_socket, (const char *)&m_fileinfo.FileSize, 4, 0);//发送文件大小

		int filenamelen=m_fileinfo.FileName.GetLength();	
		nSize = send(m_socket, (const char *)&filenamelen, 4, 0);//发送文件名长度
		
		USES_CONVERSION;
		bufname = T2A(m_fileinfo.FileName.GetBuffer(filenamelen));
		nSize =  send(m_socket, (const char *)bufname, filenamelen+1, 0);//发送文件名

		FileSend();//服务器准备好发送文件
	}
}

void CTCPClientDlg::OnBnClickedRequest()
{
	// TODO: Add your control notification handler code here
	int flag = 2;
	int nSize = send(m_socket, (const char *)&flag, 4, 0);
}
