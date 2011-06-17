// SchemeRossChoicer.cpp : implementation file
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
#include "SchemeRossChoicer.h"
#include "FieldsListEditor.h"
#include "RossScheme.h"
#include "../common/utilit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SchemeRossChoicer dialog

SchemeRossChoicer::SchemeRossChoicer(CRossDoc* pRossDoc,
						 CWnd* pParent /*=NULL*/)
	: CDialog(SchemeRossChoicer::IDD, pParent),
	  m_pRossDoc(pRossDoc)
{
	//{{AFX_DATA_INIT(CRossScheme)
	//}}AFX_DATA_INIT
}




void SchemeRossChoicer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SchemeRossChoicer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SchemeRossChoicer, CDialog)
	//{{AFX_MSG_MAP(SchemeRossChoicer)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
    ON_BN_CLICKED(IDC_BUTTON2, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SchemeRossChoicer message handlers

void SchemeRossChoicer::OnButton1() 
{
	// TODO: Add your control notification handler code here
   CRossScheme dlg (m_pRossDoc);
   dlg.DoModal();	
}

void SchemeRossChoicer::OnButton2() 
{
	// TODO: Add your control notification handler code here
   CFieldsListEditor dlg (m_pRossDoc);
   dlg.DoModal();	
	
}

// удлаение всех констант из текстовых доменов
void SchemeRossChoicer::OnButton3() 
{
	CWaitCursor C;
	if (GetRoss()->_GetCortegesSize() > 0 )
	{
		 AfxMessageBox ("Before this operation you should delete all dictionary entries");
		 return;
	};

    m_pRossDoc->DelTextDomains();
}


void SchemeRossChoicer::OnOK() 
{
	try
	{
		GetRoss()->BuildFormats(GetRoss()->m_MaxNumDom);
	}
	catch (...)
	{
		return;
	};
	CDialog::OnOK();
             
}


