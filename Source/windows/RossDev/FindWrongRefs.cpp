// FindWrongRefs.cpp : implementation file
//

#include "StdAfx.h"
#include "RossDev.h"
#include "FindWrongRefs.h"
#include "PocketForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindWrongRefs dialog


CFindWrongRefs::CFindWrongRefs(CWnd* pParent, CRossDoc* ActiveRossDoc)
	: CDialog(CFindWrongRefs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindWrongRefs)
	m_FieldStr = _T("");
	//}}AFX_DATA_INIT
	m_pActiveRossDoc = ActiveRossDoc;
}


void CFindWrongRefs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindWrongRefs)
	DDX_Text(pDX, IDC_FIELD_STR, m_FieldStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindWrongRefs, CDialog)
	//{{AFX_MSG_MAP(CFindWrongRefs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindWrongRefs message handlers

void CFindWrongRefs::OnOK() 
{
	// TODO: Add extra validation here
try{
	UpdateData(TRUE);
	CTempArticle A(m_pActiveRossDoc->GetRoss());

	std::string field_u8 = wstring_to_utf8((const wchar_t*)m_FieldStr);
	BYTE EngFieldNo = m_pActiveRossDoc->GetRoss()->GetFieldNoByFieldStr(field_u8.c_str());

	std::vector<CRossDoc*> RossDocs;
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while( pos )
			RossDocs.push_back ((CRossDoc*)pRossDocTemplate->GetNextDoc(pos));	

    CWaitCursor C;
	std::vector<CRossPocketItem> PocketItems;		

	for (size_t UnitNo= 0;  UnitNo< m_pActiveRossDoc->GetRoss()->GetUnitsSize(); UnitNo++)
	{
		if (m_pActiveRossDoc->GetRoss()->IsEmptyArticle(UnitNo) == TRUE) continue;

		size_t k = m_pActiveRossDoc->GetRoss()->GetUnitStartPos(UnitNo);
		for (; k <= m_pActiveRossDoc->GetRoss()->GetUnitLastPos(UnitNo); k++)
		  {
			TCortege C = m_pActiveRossDoc->GetRoss()->GetCortege(k);
			if (C.m_FieldNo != EngFieldNo) continue;
			std::string Q = m_pActiveRossDoc->GetRoss()->WriteToString(C);
			if (   (Q.find("TR")!= string::npos) 
				|| (Q.find("PR")!= string::npos)
				|| (Q.find("SR")!= string::npos)
				|| (Q.find("[")!= string::npos)
			   ) continue;
			if (!C.is_null(0))
			{
				std::string UnitStr =  m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.GetItem(0));
				if (UnitStr.find(" ") != string::npos) continue;
			};

			if ( C.is_null(0) || C.is_null(1)) break;
			std::string MeanNumStr =  m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.GetItem(1));
			if (MeanNumStr.length() != 1)  break;
			BYTE MeanNum = MeanNumStr[0] - '0';
			std::string UnitStr =  m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.GetItem(0));
			int RossDocNo=0;
			for (; RossDocNo<RossDocs.size();  RossDocNo++)
			  if (RossDocs[RossDocNo]->GetRoss()->LocateUnit(UnitStr.c_str(), MeanNum)) 
				  break;

            if (RossDocNo ==  RossDocs.size())  break; // не нашлось
			// 
		 };

		if (k <= m_pActiveRossDoc->GetRoss()->GetUnitLastPos(UnitNo))
		{
			PocketItems.push_back(CRossPocketItem(UnitNo, m_pActiveRossDoc));
		};

	};

	C.Restore();

	OpenPocket(PocketItems, CString("Invalid references") );	
}
catch (...)
{
	AfxMessageBox (_T("Something is wrong"));
};

	
	CDialog::OnOK();
}
