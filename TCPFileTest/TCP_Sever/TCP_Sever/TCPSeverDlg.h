// TCPSeverDlg.h : header file
//

#pragma once
#include "afxcmn.h"

struct Fileinfo{
	CString FileName;
	unsigned int FileSize;
};

// CTCPSeverDlg dialog
class CTCPSeverDlg : public CDialog
{
// Construction
public:
	CTCPSeverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TCP_SEVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString m_FileName;
	CString m_FilePath;
	Fileinfo m_fileinfo;
	void FileSend();
	int m_FileNumber;
	CString m_FileSize;

	SOCKET m_socket;
	SOCKET m_clientSocket;
	SOCKADDR_IN svrAddr;
	SOCKADDR_IN addrClient;

	DWORD m_wPort;
	CString strHostName,strIPAddress,strPort;

	CWinThread* m_SeverThread;

	CListCtrl m_ListCtrl;
	CProgressCtrl m_ProgressCtrl;
	int InitSocket(SOCKET &m_sock);
	afx_msg void OnBnClickedBtEnd();
	afx_msg void OnBnClickedBtListen();
	int GetLocalHostInfo(CString &strHostName, CString &strIPAddress);
	afx_msg void OnBnClickedRequest();
	afx_msg void OnBnClickedBtFile();
};
