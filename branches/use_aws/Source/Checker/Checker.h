// Checker.h : main header file for the CHECKER application
//

#if !defined(AFX_CHECKER_H__506BF514_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
#define AFX_CHECKER_H__506BF514_3AE1_11D5_92A5_005004035BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#import "../../bin/lemmatizer.tlb"
using namespace LEMMATIZERLib;

#import "../../bin/agramtab.tlb"
using namespace AGRAMTABLib;

/////////////////////////////////////////////////////////////////////////////
// CCheckerApp:
// See Checker.cpp for the implementation of this class
//

class CCheckerApp : public CWinApp
{
public:
	CCheckerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCheckerApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKER_H__506BF514_3AE1_11D5_92A5_005004035BFC__INCLUDED_)
