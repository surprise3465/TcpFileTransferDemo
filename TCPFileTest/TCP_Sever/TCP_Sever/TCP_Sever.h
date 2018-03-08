// TCP_Sever.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTCP_SeverApp:
// See TCP_Sever.cpp for the implementation of this class
//

class CTCP_SeverApp : public CWinApp
{
public:
	CTCP_SeverApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTCP_SeverApp theApp;