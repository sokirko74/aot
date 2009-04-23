// FindByStrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "rossdev.h"
#include "FindByStrDlg.h"
#include "PocketForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindByStrDlg dialog


CFindByStrDlg::CFindByStrDlg(CWnd* pParent /*=NULL*/, CRossDoc* ActiveRossDoc)
	: CDialog(CFindByStrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindByStrDlg)
	m_FindMode = -1;
	m_FindString = _T("");
	//}}AFX_DATA_INIT
	m_pActiveRossDoc = ActiveRossDoc;
}


void CFindByStrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindByStrDlg)
	DDX_Radio(pDX, IDC_FIND_IN_ALL_DICTS, m_FindMode);
	DDX_CBString(pDX, IDC_COMBO1, m_FindString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindByStrDlg, CDialog)
	//{{AFX_MSG_MAP(CFindByStrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindByStrDlg message handlers

BOOL CFindByStrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CheckRadioButton(IDC_FIND_IN_ALL_DICTS,IDC_FIND_IN_ACTIVE_DICT,IDC_FIND_IN_ALL_DICTS);


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindByStrDlg::OnOK() 
{
	// TODO: Add extra validation here
try{
	UpdateData(TRUE);
	vector<CRossDoc*> RossDocs;

	if(GetCheckedRadioButton(IDC_FIND_IN_ALL_DICTS,IDC_FIND_IN_ACTIVE_DICT)==IDC_FIND_IN_ALL_DICTS)
	{
		CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
		POSITION pos = pRossDocTemplate->GetFirstDocPosition();
		while( pos )
			RossDocs.push_back ((CRossDoc*)pRossDocTemplate->GetNextDoc(pos));	
	}
	else
	{
		RossDocs.push_back(m_pActiveRossDoc);
	};

	CWaitCursor C;
	vector<CRossPocketItem> PocketItems;
	for (int RossDocNo=0; RossDocNo<RossDocs.size();  RossDocNo++)
	{
		CRossDoc* pRossDoc = RossDocs[RossDocNo];
		CTempArticle A;
		A.m_pRoss  = pRossDoc->GetRoss();
		size_t UnitNo; 
		
		try{
			for (UnitNo = 0;  UnitNo < pRossDoc->GetRoss()->GetUnitsSize(); UnitNo++)
			{
				if (pRossDoc->GetRoss()->IsEmptyArticle(UnitNo)) continue;
				A.ReadFromDictionary(UnitNo, false, true);
				if (A.GetArticleStr().find(m_FindString) != -1)
					PocketItems.push_back(CRossPocketItem(UnitNo, pRossDoc));
			};
		}
		catch (...)
		{
			string Mess = string ("Errors in article ") + pRossDoc->GetRoss()->GetEntryStr(UnitNo) ;
			AfxMessageBox (Mess.c_str());
		};

	};
	C.Restore();

	OpenPocket(PocketItems, CString("Search Results for ") + m_FindString);	
}
catch (...)
{
	AfxMessageBox ("Something is wrong");
};

	CDialog::OnOK();
}
