// Ross.h : main header file for the ROSSDEV application
//

#if !defined(AFX_ROSSDEV_H__553CC260_C720_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_ROSSDEV_H__553CC260_C720_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "..\common\cortege.h"       // main symbols
#include "RossDoc.h"       // main symbols
#include "ArticleDoc.h"       // main symbols
#include "PocketForm.h"       // main symbols
#include "../SemanLib/HierarchyHolder.h"       // main symbols
#include "HierarchyDoc.h"       // main symbols
#include "afxrich.h"       // main symbols
#include "../SemanLib/\SemStructureBuilder.h"




/////////////////////////////////////////////////////////////////////////////
// CRossDevApp:
// See RossDev.cpp for the implementation of this class


class CRossDevApp : public CWinApp
{
public:
	CSemStructureBuilder	m_SemBuilder;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CRossDevApp();
	~CRossDevApp();
	CFont					m_TextFont; 
	HANDLE                  m_hEventMinusListReport;
	CString                 m_strMinusListReport;
	bool                    m_OpenRossDocReadOnly;
	bool                    m_OnlyRoss;
	bool					m_bGraphanIsBusy;
  

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRossDevApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRossDevApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnImport();
	afx_msg void OnSemfet();
	afx_msg void OnAppExit();
	afx_msg void OnSemAn();
	afx_msg void OnOpenAllRosses();
	afx_msg void OnFocusSeman();
	afx_msg void OnMinimizeAllDicts();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline  CDocTemplate* GetTemplate (CString Name)
{
	// получение template документа статьи
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* tmpl;
	CString S;
	do {
		tmpl = AfxGetApp()->GetNextDocTemplate (pos);
		tmpl->GetDocString (S, CDocTemplate::regFileTypeId);
		}
	while ((tmpl != 0) &&  (S != Name));

	ASSERT (tmpl);

	return tmpl;
};

inline  CDocTemplate* GetRossArticleTemplate ()
{
	return GetTemplate("RossDev.ArticleRoss");
};

inline  CDocTemplate* GetRossDocTemplate ()
{
	return GetTemplate("RossDev.Ross");
};


inline  CDocTemplate* GetHierarchyTemplate ()
{
	return GetTemplate("RossDev.Hierarchy");
};

inline  CDocTemplate* GetReportTemplate ()
{
	return GetTemplate("RossDev.Report");
};

inline  CDocTemplate* GetSemanticStrTemplate ()
{
	return GetTemplate("RossDev.SemanticStructure");
};

inline  CDocTemplate* GetRossPocketTemplate ()
{
	return GetTemplate("RossDev.PocketRoss");
};

inline  CDocTemplate* GetSemanticStructureTemplate ()
{
	return GetTemplate("RossDev.SemanticStructure");
};





inline bool GlobalOpenArticle(CRossDoc*  pRossDoc, WORD UnitNo)
{
	CDocument* pDocument = GetRossArticleTemplate()->CreateNewDocument();
    ASSERT_VALID(pDocument);
	((CArticleDoc*)pDocument) -> OpenArticle (UnitNo, pRossDoc);
	return true;
};

inline bool GlobalOpenHierarchy(CRossDoc*  pRossDoc, CHierarchyEnum Type)
{
	
	CDocument* pDocument = GetHierarchyTemplate()->CreateNewDocument();
    ASSERT_VALID(pDocument);
	((CHierarchyDoc*)pDocument) -> OpenHierarchy ( pRossDoc, Type);
	return true;
};


extern bool GlobalOpenReport(CString S, CString Name);

inline bool GlobalPocketAndArticleDocTempalteAreEmpty(CRossDoc* RossDoc)
{
  CDocTemplate* tmpl = GetRossArticleTemplate();
  POSITION pos = tmpl->GetFirstDocPosition();
  while (pos != NULL)
  {
    CArticleDoc* Doc  = (CArticleDoc*)tmpl->GetNextDoc(pos);
	if (Doc->m_pRossDoc == RossDoc)
	{
		AfxMessageBox (" Сначала закройте статьи этого словаря!");
		return false;
	};
  };

  tmpl = GetRossPocketTemplate();
  pos = tmpl->GetFirstDocPosition();
  while (pos != NULL)
  {
    CDocument* Doc  = tmpl->GetNextDoc(pos);
	POSITION pos1 = Doc->GetFirstViewPosition();
	assert (pos1 != 0);
	CPocketForm* View = (CPocketForm*)Doc->GetNextView(pos1);
	assert (View != 0);
	for (long i=0; i < View->m_PocketItems.size(); i++)
		if (View->m_PocketItems[i].m_pRossDoc == RossDoc)
		{
			AfxMessageBox (" Сначала закройте карманы, в которых есть ссылки на этот словарь!");
			return false;
		};
  };

  return true;
};



inline CTime Tm2CTime (tm t)
{
	return CTime( t.tm_year + 1900, t.tm_mon + 1,t.tm_mday+1, t.tm_hour+1, t.tm_min, t.tm_sec, 0);
};

inline tm CTime2Tm (CTime t)
{
	tm out;
	out.tm_year = t.GetYear() - 1900;
	out.tm_mon = t.GetMonth() - 1;
	out.tm_mday = t.GetDay() - 1;
	out.tm_hour = t.GetHour()-2;
	out.tm_min = t.GetMinute();
	out.tm_sec = t.GetSecond();
	return out;
};

class CRossDoc;


inline CString ConstructFldName (const CDictionary* pIRossForSyntax, BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId)
{
	CString S((char*)pIRossForSyntax->Fields[FieldNo].FieldStr);

	if (LeafId == ErrUChar)
      S += "*";
	else
    if (LeafId > 0)
	 {
          CString  Q;
          Q.Format("%i",LeafId);
	      S +=  Q;
	 }

	 if (BracketLeafId == ErrUChar)
       S += "(*)";
	 else
      if (BracketLeafId > 0)
	  {
		CString  Q;
        Q.Format("(%i)",BracketLeafId);
	    S +=  Q;
	  };

    return S;
};





inline void GetCortegeList (const CDictionary* Ross, vector<TCortege>& L, LONG UnitNo)
{
  for (size_t i= Ross->GetUnitStartPos(UnitNo); i <= Ross->GetUnitEndPos(UnitNo); i++)
	  L.push_back(GetCortege (Ross, i));
};

template <class T>
T* FindDoc (char*  tkname, CDocTemplate* Template)
{
		POSITION p = Template->GetFirstDocPosition( );
		T* D;
		for (;;)
		{
		 D =  (T*)Template->GetNextDoc(p);
		 if (!D) break;
		 if (D->GetView()->m_tkname == CString(tkname))
		 {
			 break;
		 };
		};
		return D;
};

inline CSemStructureBuilder&	GetSemBuilder()
{
	CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
	return App->m_SemBuilder;
}



//const char szNodeWCL[] = "NodeClass";

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROSSDEV_H__553CC260_C720_11D2_A6E4_A290D9000000__INCLUDED_)
