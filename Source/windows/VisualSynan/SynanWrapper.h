#if !defined(AFX_SYNANWRAPPER_H__1C505B49_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
#define AFX_SYNANWRAPPER_H__1C505B49_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SynanWrapper.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSynanWrapper 

#include "synan.h"

//const char[] strInitSynanFile = "../../bin/synan.cfg";

class CSynanWrapper 
{
// Construction
public:
	CSynanWrapper();
	CSynanWrapper(CString strCommandLine);

// Attributes
public:

// Operations
public:
	BOOL BuildSyntaxByCommandLine();
	


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSynanWrapper)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSynanWrapper();

	// Generated message map functions
protected:
	CString m_strCommandLine;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNANWRAPPER_H__1C505B49_C4B2_11D2_8BB6_00105A68ADF3__INCLUDED_)
