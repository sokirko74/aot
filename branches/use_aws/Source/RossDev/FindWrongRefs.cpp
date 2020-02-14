// FindWrongRefs.cpp : implementation file
//

#include "stdafx.h"
#include "rossdev.h"
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
	CTempArticle A;
	A.m_pRoss = m_pActiveRossDoc->GetRoss();
	BYTE EngFieldNo = m_pActiveRossDoc->GetRoss()->GetFieldNoByFieldStr((const char*)m_FieldStr);

	vector<CRossDoc*> RossDocs;
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while( pos )
			RossDocs.push_back ((CRossDoc*)pRossDocTemplate->GetNextDoc(pos));	

    CWaitCursor C;
	vector<CRossPocketItem> PocketItems;		

	for (size_t UnitNo= 0;  UnitNo< m_pActiveRossDoc->GetRoss()->GetUnitsSize(); UnitNo++)
	{
		if (m_pActiveRossDoc->GetRoss()->IsEmptyArticle(UnitNo) == TRUE) continue;

		size_t k = m_pActiveRossDoc->GetRoss()->GetUnitStartPos(UnitNo);
		for (; k <= m_pActiveRossDoc->GetRoss()->GetUnitEndPos(UnitNo); k++)
		  {
			TCortege C = GetCortege(m_pActiveRossDoc->GetRoss(), k);
			if (C.m_FieldNo != EngFieldNo) continue;
			string Q = WriteToString(m_pActiveRossDoc->GetRoss(), m_pActiveRossDoc->GetRoss()->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt, C);
			if (   (Q.find("TR")!=-1) 
				|| (Q.find("PR")!=-1) 
				|| (Q.find("SR")!=-1)
				|| (Q.find("[")!=-1)
			   ) continue;
			if (C.m_DomItemNos[0] != -1)
			{
				string UnitStr =  (const char*)m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.m_DomItemNos[0]);
				if (UnitStr.find(" ") != -1) continue;
			};

			if ( (C.m_DomItemNos[0] == -1) ||  (C.m_DomItemNos[1] == -1)) break;
			string MeanNumStr =  (const char*)m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.m_DomItemNos[1]);
			if (MeanNumStr.length() != 1)  break;
			BYTE MeanNum = MeanNumStr[0] - '0';
			string UnitStr =  (const char*)m_pActiveRossDoc->GetRoss()->GetDomItemStr(C.m_DomItemNos[0]);
			int RossDocNo=0;
			for (; RossDocNo<RossDocs.size();  RossDocNo++)
			  if (RossDocs[RossDocNo]->GetRoss()->LocateUnit(UnitStr.c_str(), MeanNum)) 
				  break;

            if (RossDocNo ==  RossDocs.size())  break; // не нашлось
			// 
		 };

		if (k <= m_pActiveRossDoc->GetRoss()->GetUnitEndPos(UnitNo))
		{
			PocketItems.push_back(CRossPocketItem(UnitNo, m_pActiveRossDoc));
		};

	};

	C.Restore();

	OpenPocket(PocketItems, CString("Invalid references") );	
}
catch (...)
{
	AfxMessageBox ("Something is wrong");
};

	
	CDialog::OnOK();
}
