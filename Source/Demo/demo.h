// demo.h : main header file for the DEMO application
//

#if !defined(AFX_DEMO_H__59A50E5A_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_)
#define AFX_DEMO_H__59A50E5A_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "../common/utilit.h"

/////////////////////////////////////////////////////////////////////////////
// CDemoApp:
// See demo.cpp for the implementation of this class
//

class CDemoApp : public CWinApp
{
public:
	MorphLanguageEnum m_Language;

	CDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMO_H__59A50E5A_1A5C_11D3_9CCC_00105A68BAF2__INCLUDED_)
