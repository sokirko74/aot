// Morphwizard.h : main header file for the Morphwizard application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CMorphwizardApp:
// See Morphwizard.cpp for the implementation of this class
//

class CMorphwizardApp : public CWinApp
{
public:
//	map<void*, int> m_PointerCount;
	CMorphwizardApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	afx_msg void OnHelpHelp();
};

extern CMorphwizardApp theApp;

CDocTemplate* GetTemplate( CString Name );

extern void SetInputLanguage( MorphLanguageEnum lang );
