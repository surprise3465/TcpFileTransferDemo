// TCPSeverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TCP_Sever.h"
#include "TCPSeverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define READSIZE 1024*20
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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

// CTCPSeverDlg dialog
CTCPSeverDlg::CTCPSeverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPSeverDlg::IDD, pParent)
{
	m_wPort=6000;
	m_socket=NULL;
	m_clientSocket=NULL;
	m_FileNumber = 0;
	m_fileinfo.FileName="";
	m_fileinfo.FileSize=0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CTCPSeverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
}

BEGIN_MESSAGE_MAP(CTCPSeverDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_END, &CTCPSeverDlg::OnBnClickedBtEnd)
	ON_BN_CLICKED(IDC_BT_LISTEN, &CTCPSeverDlg::OnBnClickedBtListen)
	ON_BN_CLICKED(IDC_REQUEST, &CTCPSeverDlg::OnBnClickedRequest)
	ON_BN_CLICKED(IDC_BT_FILE, &CTCPSeverDlg::OnBnClickedBtFile)
END_MESSAGE_MAP()


// CTCPSeverDlg message handlers

BOOL CTCPSeverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GetDlgItem(IDC_REQUEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_END)->EnableWindow(FALSE);
	//��ʼ���б�
	m_ListCtrl.SetExtendedStyle(LVS_EX_FLATSB|
		LVS_EX_FULLROWSELECT|
		LVS_EX_HEADERDRAGDROP|
		LVS_EX_ONECLICKACTIVATE|
		LVS_EX_GRIDLINES);

	//���ñ�ͷ
	m_ListCtrl.InsertColumn(0,L"����",LVCFMT_LEFT,80,0);
	m_ListCtrl.InsertColumn(1,L"��С",LVCFMT_LEFT,80,2);
	m_ListCtrl.InsertColumn(2,L"·��",LVCFMT_LEFT,130,3);

	//��ʾIP���˿ں�
	if(GetLocalHostInfo(strHostName, strIPAddress))
		return	TRUE ;
	GetDlgItem(IDC_IP4)->SetWindowText(strIPAddress);
	strPort.Format(L"%d",6000);
	GetDlgItem(IDC_PORT)->SetWindowText(strPort);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTCPSeverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTCPSeverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTCPSeverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT ServerWaitThread(LPVOID para)
{
	CTCPSeverDlg* pDlg=(CTCPSeverDlg*)para;

	int size = sizeof(SOCKADDR_IN);;
	pDlg->m_clientSocket=accept(pDlg->m_socket, (sockaddr*)&pDlg->addrClient,&size);

	char bufname[1024];
	char buf[READSIZE];
	memset(buf, 0, sizeof(buf));

	while(1)
	{
		if(pDlg->m_clientSocket==NULL)
			break;
		int len=0;
		int length = recv(pDlg->m_clientSocket, (char*)(&pDlg->m_fileinfo.FileSize), sizeof(4), 0);//�����ļ�����
		if(length <=0){break;}
		length = recv(pDlg->m_clientSocket, (char*)(&len), sizeof(4), 0);//�����ļ�������	
		if(length <=0){break;}
		length = recv(pDlg->m_clientSocket, (char*)bufname, len+1, 0);//�����ļ���
		if(length <=0){break;}

		//�򿪽����ļ��Ի���
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"�����ļ� (*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
		{ 
			pDlg->m_FilePath=dlg.GetPathName();
		}
	
		int dwCount=0;	
		pDlg->m_ProgressCtrl.SetRange32(0, pDlg->m_fileinfo.FileSize);
		length = 0;

		CFile file(pDlg->m_FilePath, CFile::modeCreate|CFile::modeWrite);
		pDlg->m_FileName=file.GetFileName();	//��ȡ�����ļ���
		for(;;)	//��ʼ�����ļ�
		{
			length = recv(pDlg->m_clientSocket, (char*)buf, READSIZE, 0);
			if(length==0)
				break;

			//�����յ����ļ�д���½����ļ���ȥ
			file.Write(buf, length);
			dwCount += length;

			pDlg->m_ProgressCtrl.SetPos(dwCount);
			if(dwCount==pDlg->m_fileinfo.FileSize)
				break;
		}
		file.Close();//�ر��ļ�

		pDlg->m_ProgressCtrl.SetPos(0);	//�����յ��ļ���ʾ���б���				
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

void CTCPSeverDlg::OnBnClickedBtListen()
{
	//�����������׽���
	GetDlgItem(IDC_BT_LISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_END)->EnableWindow(TRUE);

	if (InitSocket(m_socket) == FALSE)
	{
		AfxMessageBox(L"��ʼ���׽���ʧ��!");
		exit(0);
	}

	USES_CONVERSION;//���ú�����T2A��W2A��֧��ATL��MFC�е��ַ�ת��
	char * pFileName = T2A(strIPAddress);   
	svrAddr.sin_family			= AF_INET;
	svrAddr.sin_addr.s_addr		= inet_addr(pFileName);
	m_wPort = (DWORD)GetDlgItemInt(IDC_PORT);
	svrAddr.sin_port			= htons(m_wPort);
	if (bind(m_socket,(sockaddr *)&svrAddr,sizeof(sockaddr)) == SOCKET_ERROR)
	{
		AfxMessageBox(L"�󶨴���!");
		exit(0);
	}
	if (listen(m_socket, 5)< 0)
	{
		AfxMessageBox(L"��������!");
		exit(0);
	}

	m_SeverThread=AfxBeginThread(ServerWaitThread,this);
	m_SeverThread->m_bAutoDelete=TRUE;

}

int CTCPSeverDlg::InitSocket(SOCKET &m_sock)
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
	{
		return FALSE;
	}

	if ( (m_sock = socket(AF_INET,SOCK_STREAM,0)) == SOCKET_ERROR )
	{
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}

void CTCPSeverDlg::OnBnClickedBtEnd()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BT_LISTEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_END)->EnableWindow(TRUE);
	m_socket=NULL;
	m_clientSocket=NULL;
}

int CTCPSeverDlg::GetLocalHostInfo(CString &strHostName, CString &strIPAddress)
{
	char szHostName[256];

	if(gethostname(szHostName, sizeof(szHostName)))
	{
		strHostName = _T("");
		AfxMessageBox(L"��ȡ����IP����", MB_ICONHAND|MB_OK);
		return -1;
	}

	PHOSTENT hostinfo;
	if((hostinfo = gethostbyname(szHostName)) == NULL)
		return GetLastError();

	LPCSTR ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);

	strIPAddress = ip;
	strHostName = szHostName;

	return 0;	
}

void CTCPSeverDlg::OnBnClickedRequest()
{
	// TODO: Add your control notification handler code here
	int flag = 2;
	//int nSize = send(m_clientSocket, (const char *)&flag, 4, 0);
}

void CTCPSeverDlg::FileSend()
{
	CFile file;
	if(!file.Open(m_FilePath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("�ļ���ʧ��"));
		return ;
	}
	char buf[READSIZE] = {0};
	file.Seek(0, CFile::begin);

	//��ʼ�����ļ�
	int nLen=0;//��ȡ����
	int nSize=0;//���ͳ���
	DWORD dwCount=0;
	m_ProgressCtrl.SetRange32(0, m_fileinfo.FileSize);
	
	for(;;)//��ʼ�����ļ�
	{
		//һ�ζ�ȡREADSIZE��С���ļ�����
		nLen = file.Read(buf, READSIZE);
		if(nLen == 0)
			break;
		nSize = send(m_clientSocket, (const char *)buf, nLen, 0);
		dwCount += nSize;
		m_ProgressCtrl.SetPos(dwCount);
	}

	file.Close();
	m_ProgressCtrl.SetPos(0);
	GetDlgItem(IDC_BT_FILE)->EnableWindow(TRUE);
}

void CTCPSeverDlg::OnBnClickedBtFile()
{
	char* bufname;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"�����ļ� (*.*)|*.*||", this);
	if(dlg.DoModal() == IDOK)
	{
		
		m_FilePath = dlg.GetPathName();//��ȡ�ļ�·��	
		m_fileinfo.FileName=dlg.GetFileName();//��ȡ�ļ���	
		CFile file(m_FilePath, CFile::modeRead);//���ļ�	
		m_fileinfo.FileSize = file.GetLength();//��ȡ�ļ���С	
		file.Close();//�ر��ļ�

		//���б�����ʾ�����ļ�
		m_FileSize.Format(L"%dk", m_fileinfo.FileSize);
		m_ListCtrl.InsertItem(m_FileNumber,L"");
		m_ListCtrl.SetItemText(m_FileNumber,0,m_fileinfo.FileName);
		m_ListCtrl.SetItemText(m_FileNumber,1,m_FileSize);
		m_ListCtrl.SetItemText(m_FileNumber,2,m_FilePath);
		m_FileNumber++;

		GetDlgItem(IDC_BT_FILE)->EnableWindow(FALSE);

		//int recvlen=1;
		//int nSize = send(m_clientSocket, (const char *)&recvlen, 4, 0);//֪ͨ�Է������ļ�	

		int nSize = send(m_clientSocket, (const char *)&m_fileinfo.FileSize, 4, 0);//�����ļ���С

		int filenamelen=m_fileinfo.FileName.GetLength();
		nSize = send(m_clientSocket, (const char *)&filenamelen, 4, 0);//�����ļ�������
	
		USES_CONVERSION;
		bufname = T2A(m_fileinfo.FileName.GetBuffer(filenamelen));
		nSize =  send(m_clientSocket, (const char *)bufname, filenamelen+1, 0);//�����ļ���

		//�ȴ�������׼���÷���1���	
		/*int flag;
		int length = recv(m_clientSocket, (char *)&flag, 4, 0);
		if(flag==1)*/
		FileSend();//������׼���÷����ļ�
	}
}
