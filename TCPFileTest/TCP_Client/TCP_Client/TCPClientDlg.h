// TCPClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

struct Fileinfo{
	CString FileName;
	unsigned int FileSize;
};

// CTCPClientDlg �Ի���
class CTCPClientDlg : public CDialog
{
// ����
public:
	CTCPClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCP_CLIENT_DIALOG };

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
