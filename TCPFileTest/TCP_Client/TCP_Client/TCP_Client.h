// TCP_Client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTCP_ClientApp:
// �йش����ʵ�֣������ TCP_Client.cpp
//

class CTCP_ClientApp : public CWinApp
{
public:
	CTCP_ClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTCP_ClientApp theApp;