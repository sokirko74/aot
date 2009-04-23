// VisualSynan.h : main header file for the VISUALSYNAN application
//

#if !defined(AFX_VISUALSYNAN_H__1C505B37_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_VISUALSYNAN_H__1C505B37_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols



#include "../common/COMSyntaxHolder.h"

/////////////////////////////////////////////////////////////////////////////
// CVisualSynanApp:
// See VisualSynan.cpp for the implementation of this class
//

class CVisualSynanApp : public CWinApp
{
public:
	CCOMSyntaxHolder	m_SyntaxHolder;
	

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualSynanApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVisualSynanApp)
	afx_msg void OnAppAbout();
	afx_msg void OnSynFileNew();
	//afx_msg LRESULT OnProcessTxtFile(WPARAM, LPARAM);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALSYNAN_H__1C505B37_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
