// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "InputBox.h"

#ifdef DETECT_MEMORY_LEAK
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBox dialog


CInputBox::CInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputBox)
	m_Edit = _T("");
	m_Caption = _T("");
	//}}AFX_DATA_INIT
}


void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBox)
	DDX_Text(pDX, IDC_EDIT1, m_Edit);
	DDX_Text(pDX, IDC_CAPTION, m_Caption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBox message handlers


bool InputBox (const char* Caption, string& OutBuffer)
{
	CInputBox* C =  new CInputBox(0);
	C->m_Caption = Caption; 
	C->m_Edit = OutBuffer.c_str();
	bool bResult = (C->DoModal() == IDOK);
	OutBuffer = C->m_Edit;
	delete C;
	return bResult;
};

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}
