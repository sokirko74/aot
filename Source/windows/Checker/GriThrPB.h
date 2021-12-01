/////////////////////////////////////////////////////////////////////////////
// Class 'Thread Progress Bar'
// Version 1.10 Gri
/////////////////////////////////////////////////////////////////////////////

#ifndef __CTHRPB_H__
#define __CTHRPB_H__

/////////////////////////////////////////////////////////////////////////////
// CThrPB dialog

class CThrPB : public CDialog
{
#ifdef _DEBUG
	DECLARE_DYNAMIC(CThrPB)
#endif

// Construction
public:
	CThrPB::CThrPB(CWnd* pParent,
		AFX_THREADPROC pfnProc, // UINT (*pfn)(LPVOID)
		LPVOID pData = NULL);

// Dialog Data
	//{{AFX_DATA(CThrPB)
	enum { IDD = IDD_PROGRESS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThrPB)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message  functions
	//{{AFX_MSG(CThrPB)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Code
public:
	void SetHead(LPCTSTR pText); // Set new Dialog header
	void Percent(int iValue); // Set new value and update progress bar
	BOOL IsBreak(void); // Return break status
	void EndDone(void); // Close Dialog and return (TRUE)
	void EndExit(void); // Close Dialog and return (FALSE)

// Data
protected:
	UINT (*m_pfnProc)(LPVOID);
	LPVOID m_pData;

	LPCSTR m_pText;
	int m_fBreak;
	int m_iValue;

	CWinThread *m_cHandle; // Handle to the running CWinThread
};

/////////////////////////////////////////////////////////////////////////////
// Static Pointer to CThrPB object

extern CThrPB *theTpb;

/////////////////////////////////////////////////////////////////////////////
#endif //__CTHRPB_H__
