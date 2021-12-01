/////////////////////////////////////////////////////////////////////////////
// Class CGriIni - realizes Loading Message
// Version 2.0 Gri
//////////////////////////////////////////////////////////////////////////////

#ifndef __CGRIINI_H__
#define __CGRIINI_H__

/////////////////////////////////////////////////////////////////////////////
// CGriIni window

class CGriIni : public CWnd
{
// Construction
public:
	CGriIni(LPCTSTR message = NULL);
// Defaut message is: "Loading, please wait..."

// Attributes
protected:
	CString m_text;

// Operations
public:
	BOOL Init(CWnd *pWnd = NULL);
	void Exit(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGriIni)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGriIni();

	// Generated message  functions
protected:
	//{{AFX_MSG(CGriIni)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //__CGRIINI_H__
