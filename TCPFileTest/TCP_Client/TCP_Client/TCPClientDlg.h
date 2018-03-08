// TCPClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

struct Fileinfo{
	CString FileName;
	unsigned int FileSize;
};

// CTCPClientDlg 对话框
class CTCPClientDlg : public CDialog
{
// 构造
public:
	CTCPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCP_CLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	int m_FileNumber;
	void FileSend();
	CString m_FilePath;
	CString m_FileName;
	CString m_FileSize;
	Fileinfo m_fileinfo;

	SOCKET m_socket;
	SOCKADDR_IN  svrAddr;
	UINT m_Port;
	CString m_strIPAddress;

	CWinThread* m_ClientThread;

	CProgressCtrl m_ctrlProgress;
	CListCtrl m_ListCtrl;

	afx_msg void OnBnClickedBtLink();
	afx_msg void OnBnClickedBtEnd();
	afx_msg void OnBnClickedBtFile();
	afx_msg void OnBnClickedRequest();
};
