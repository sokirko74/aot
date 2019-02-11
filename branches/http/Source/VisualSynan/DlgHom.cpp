// DlgHom.cpp : implementation file
//

#include "stdafx.h"
#include "visualsynan.h"
#include "DlgHom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHom dialog


CDlgHom::CDlgHom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHom::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHom)
	m_type = CoverageKillHomonyms;
	//}}AFX_DATA_INIT
}


void CDlgHom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHom)
	DDX_Radio(pDX, IDC_RADIO1, m_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHom, CDialog)
	//{{AFX_MSG_MAP(CDlgHom)
		// NOTE: the ClassWizard will add message  macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHom message handlers
