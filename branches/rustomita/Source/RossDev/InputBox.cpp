// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "InputBox.h"

#ifdef _DEBUG
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


bool InputBox (char* Caption, char* OutBuffer, size_t OutBufferLen, char* UpperCaption, CWnd* Parent)
{
	CInputBox* C =  new CInputBox(Parent);
	C->m_Caption = Caption; 
	C->m_Edit = OutBuffer;
	C->m_UpperCaption = UpperCaption;

	if (C->DoModal() != IDOK) 
	{
		delete C;
		return false;
	};

	if (strlen (C->m_Edit) < OutBufferLen - 1)
		strcpy (OutBuffer, C->m_Edit);
    else
		{
		 strncpy (OutBuffer, C->m_Edit, OutBufferLen - 2);
		 OutBuffer[OutBufferLen - 1] = 0;
		};
	delete C;

	
	return true;
};

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_UpperCaption);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
