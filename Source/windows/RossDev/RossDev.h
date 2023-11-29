#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "common/cortege.h"
#include "RossDoc.h"
#include "ArticleDoc.h"
#include "PocketForm.h"
#include "seman/SemanLib/HierarchyHolder.h"
#include "HierarchyDoc.h"
#include "afxrich.h"
#include "seman/SemanLib/SemStructureBuilder.h"
#include <tcl.h>
#include "TckTkWrapper.h"



class CRossDevApp : public CWinApp
{
	void CreateTclCommand(char* name, Tcl_CmdProc proc);
public:
	CSemStructureBuilder	m_SemBuilder;
	CTclTkWrapper			m_TclInterp;
	CString					m_TclClassWnd;

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
	afx_msg void OnSemfet();
	afx_msg void OnAppExit();
	afx_msg void OnSemAn();
	afx_msg void OnOpenAllRosses();
	afx_msg void OnFocusSeman();
	afx_msg void OnMinimizeAllDicts();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CRossDoc* FindRossDoc(DictTypeEnum RossType);
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


inline bool GlobalOpenArticle(CRossDoc*  pRossDoc, uint16_t UnitNo)
{
	CDocument* pDocument = GetRossArticleTemplate()->CreateNewDocument();
    ASSERT_VALID(pDocument);
	((CArticleDoc*)pDocument) -> OpenArticle (UnitNo, pRossDoc);
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
		AfxMessageBox (_T("Close all windows of this dictionary!"));
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
			AfxMessageBox (_T("Close the pockets that contain links to this dictionary!"));
			return false;
		};
  };

  return true;
};



class CRossDoc;


inline CString ConstructFldName (const CDictionary* pIRossForSyntax, BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId)
{
	CString S(pIRossForSyntax->Fields[FieldNo].FieldStr.c_str());

	if (LeafId == ErrUChar)
      S += "*";
	else
    if (LeafId > 0)
	 {
          CString  Q;
          Q.Format(_T("%i"), LeafId);
	      S +=  Q;
	 }

	 if (BracketLeafId == ErrUChar)
       S += "(*)";
	 else
      if (BracketLeafId > 0)
	  {
		CString  Q;
        Q.Format(_T("(%i)"), BracketLeafId);
	    S +=  Q;
	  };

    return S;
};


inline void GetCortegeList (const CDictionary* Ross, std::vector<TCortege>& L, LONG UnitNo)
{
  for (size_t i= Ross->GetUnitStartPos(UnitNo); i <= Ross->GetUnitLastPos(UnitNo); i++)
	  L.push_back(Ross->GetCortege(i));
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
		 if (D->GetView()->m_TopTKWindow.m_tkname == std::string(tkname))
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

inline std::string _U8(CString s) {
	return wstring_to_utf8((const TCHAR*)s);
}

inline CString _U16(std::string s) {
	return utf8_to_wstring(s.c_str()).c_str();
}
