// TCPClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCP_Client.h"
#include "TCPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define READSIZE 1024*20
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTCPClientDlg �Ի���
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

		int length = recv(pDlg->m_socket, (char*)(&pDlg->m_fileinfo.FileSize), sizeof(4), 0);//�����ļ�����
		if(length <=0){break;}
		length = recv(pDlg->m_socket, (char*)(&len), sizeof(4), 0);//�����ļ�������	
		if(length <=0){break;}
		length = recv(pDlg->m_socket, (char*)bufname, len+1, 0);//�����ļ���
		if(length <=0){break;}

		//�򿪽����ļ��Ի���
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"�����ļ� (*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
		{ 
			pDlg->m_FilePath=dlg.GetPathName();
		}

		int dwCount=0;	
		pDlg->m_ctrlProgress.SetRange32(0, pDlg->m_fileinfo.FileSize);
		length = 0;

		CFile file(pDlg->m_FilePath, CFile::modeCreate|CFile::modeWrite);
		pDlg->m_FileName=file.GetFileName();	//��ȡ�����ļ���
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
		file.Close();//�ر��ļ�

		//�����յ��ļ���ʾ���б���
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


// CTCPClientDlg ��Ϣ�������

BOOL CTCPClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_REQUEST)->EnableWindow(FALSE);
	//���ö˿ں�
	CString Port;
	Port.Format(L"%d",m_Port);
	GetDlgItem(IDC_PORT)->SetWindowText(Port);
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTCPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTCPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTCPClientDlg::FileSend()
{
	CFile file;
	if(!file.Open(m_FilePath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("�ļ���ʧ��"));
		return ;
	}
	char buf[READSIZE] = {0};
	file.Seek(0, CFile::begin);

	int nLen=0;//��ȡ����
	int nSize=0;//���ͳ���
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
	GetDlgItem(IDC_IPADDRESS2)->GetWindowText(m_strIPAddress);	//��ȡIP
	USES_CONVERSION;//���ú�����T2A��W2A��֧��ATL��MFC�е��ַ�ת��
	char * pFileName = T2A(m_strIPAddress);  
	svrAddr.sin_addr.s_addr = inet_addr(pFileName);
	m_Port = (DWORD)GetDlgItemInt(IDC_PORT);
	svrAddr.sin_port = htons(m_Port);        
	memset(&(svrAddr.sin_zero), 0, sizeof(svrAddr.sin_zero));    // ������������������� 
  
	if (connect(m_socket, (SOCKADDR *)&svrAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)      
	{  
		AfxMessageBox(L"connet fail!");
		GetDlgItem(IDC_BT_LINK)->EnableWindow(TRUE);
		return;       
	}
	//�׽��ִ���
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

		int nSize = send(m_socket, (const char *)&m_fileinfo.FileSize, 4, 0);//�����ļ���С

		int filenamelen=m_fileinfo.FileName.GetLength();	
		nSize = send(m_socket, (const char *)&filenamelen, 4, 0);//�����ļ�������
		
		USES_CONVERSION;
		bufname = T2A(m_fileinfo.FileName.GetBuffer(filenamelen));
		nSize =  send(m_socket, (const char *)bufname, filenamelen+1, 0);//�����ļ���

		FileSend();//������׼���÷����ļ�
	}
}

void CTCPClientDlg::OnBnClickedRequest()
{
	// TODO: Add your control notification handler code here
	int flag = 2;
	int nSize = send(m_socket, (const char *)&flag, 4, 0);
}
