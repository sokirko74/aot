// VisualSynan.h : main header file for the VISUALSYNAN application
//

#pragma once

#include "Resource.h"       
#include "synan/SynanLib/SyntaxHolder.h"

class CVisualSynanApp : public CWinApp
{
public:
	CSyntaxHolder	m_SyntaxHolder;
	

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
	DECLARE_MESSAGE_MAP()
};

