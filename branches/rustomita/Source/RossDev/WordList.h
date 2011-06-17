#if !defined(AFX_WORDLIST_H__0AA09784_C726_11D2_A6E4_A290D9000000__INCLUDED_)
#define AFX_WORDLIST_H__0AA09784_C726_11D2_A6E4_A290D9000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WordList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWordList form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "StdAfx.h"
#include "RossDoc.h"
#include "Hierarchy.h"
#include "SizeFormView.h "
#include <functional>


struct CRossDevTermin {
	WORD   m_UnitNo;
	string m_TerminStr;

	CRossDevTermin(WORD UnitNo, string TerminStr) {
		m_TerminStr = TerminStr;
		m_UnitNo = UnitNo;
	};
   bool operator==(const CRossDevTermin& X) const
	{return       (m_TerminStr == X.m_TerminStr);}

     bool operator<(const CRossDevTermin& X) const
		{ return    (m_TerminStr  < X.m_TerminStr );}

};
struct Valency {
     WORD ValNo;
	 bool   A_C;
 	 bool operator==(const Valency& X) const
			{return       (ValNo == X.ValNo) 
			          &&  (A_C == X.A_C); }

     bool operator<(const Valency& X) const
			{ return    (ValNo < X.ValNo) 
			         || (    (ValNo == X.ValNo)
					      && (A_C < X.A_C)); 
			}
};

struct TreeNode;
struct TreeNode{
	vector<Valency>  Vals;
	vector<TreeNode> SubItems;
	vector<WORD> UnitNos;


	bool operator==(const vector<Valency>& _Vals) const
			{return Vals == _Vals; }

	bool operator==(const TreeNode& X) const
			{
		          return (SubItems.size() == X.SubItems.size()); 

			}


    bool operator<(const TreeNode& X) const
			{ 
			  return    (SubItems.size() > X.SubItems.size()); 
			}


	
    static void Normalize (CHierarchyHolder& D, vector<Valency>& V) 
	{
		/*for (size_t i = 0; i < V.size(); i++)
			V[i].ValNo = D.GetTopmostWork(V[i].ValNo);*/
	};

	CString GetVals (const CDictionary* Ross)  const
    {
       CString Line;
       for (size_t i =0; i < Vals.size(); i++)
	   { CString Q;
	     Q.Format("%s (%s) ", (const char*)Ross->GetDomItemStr(Vals[i].ValNo), (Vals[i].A_C) ? "A,C": "C,A");
		 Line += Q;
	   };
	   return Line;
	};


	bool IsEqualWithHiera (CHierarchyDoc& D, TreeNode& N)
	{
		vector<Valency> V1  = Vals;
		vector<Valency> V2  = N.Vals;
		Normalize(D, V1);
		Normalize(D, V2);
		return V1 == V2;
	};

};




class CIndex {
	public:
		WORD UnitNo;
		tm     modif_tm;

		bool operator < (const CIndex &C) const 
		{
		 tm tm1 = modif_tm;
		 tm tm2 = C.modif_tm;

		 if (tm1.tm_year < tm2.tm_year) 
			 return true;
		  else
		   if (tm1.tm_year == tm2.tm_year)
             if (tm1.tm_mon < tm2.tm_mon)
				 return true;
			 else
				 if (tm1.tm_mon == tm2.tm_mon)
					 if (tm1.tm_mday < tm2.tm_mday)
						 return true;
					 else
                       if (tm1.tm_mday == tm2.tm_mday)
					     if (tm1.tm_hour < tm2.tm_hour)
					 	   return true;
					     else
						   if (tm1.tm_hour == tm2.tm_hour)
							   if (tm1.tm_min < tm2.tm_min)
								   return true;
							   else
								   if (tm1.tm_min == tm2.tm_min)
									   return tm1.tm_sec < tm2.tm_sec;

		 
		 return false;
	     
		};
};


struct CWordAndFreq {
	string m_Word;
	int    m_Freq; 
	CWordAndFreq ()
	{
		m_Freq = 0;
	};
	bool operator==(const CWordAndFreq& X) const
			{return     m_Word == X.m_Word; }

    bool operator<(const CWordAndFreq& X) const
	  { return  (m_Word < X.m_Word); }
	
};


class CWordList : public CSizeFormView
{
protected:
	CWordList();           // protected constructor used by dynamic creation
	bool AddNewRecordToUnits (char* Word, bool bTalk, char* Comments);
	DECLARE_DYNCREATE(CWordList)
  
// Form Data
public:
	CMenu m_Menu;
	enum SortType {SortByLemma, SortByDateAndTime};
	SortType m_Sorted;
	vector <CRossDevTermin> m_Termins;

	vector<CIndex> m_Index;
	void BuildIndex();
	void UpdateIndex();


	bool IsFiltered() const
	{ return (m_Menu.GetMenuState(IDR_SET_FILTER, MF_BYCOMMAND) & MF_CHECKED) > 0; };

	WORD GetUnitNo (WORD i) const
	{
		if (m_Termins.empty())
		  return m_Index[i].UnitNo;
		else
			return m_Termins[i].m_UnitNo;
	};
	string GetEntryStr(WORD UnitNo) const;
	WORD GetUnitsSize () const
	{
	    return IsFiltered() ? GetRoss()->GetSelectedUnitsSize() : GetRoss()->GetUnitsSize();
	};

	bool GetSelectedUnitNo (WORD& UnitNo) const
	{
 		POSITION pos = m_WordList.GetFirstSelectedItemPosition();

  		if (pos == NULL)
			{ ::MessageBox(0, "No selection in the list", "Message Box", MB_OK);
			  return false;};

		UnitNo = GetUnitNo(m_WordList.GetNextSelectedItem(pos));

		return true;

	};
	
	virtual CRossDoc* GetDocument() const {return ((CRossDoc*)CView::GetDocument());}; 
	CDictionary* GetRoss() const 	{return GetDocument()->GetRoss();};

	//{{AFX_DATA(CWordList)
	enum { IDD = IDD_WORDLIST_FORM };
	CHotKeyCtrl	m_HotCtrl;
	CStatic	m_UnitSizeCtrl;
	CButton	m_DelButton;
	CButton	m_ChangeTitleButton;
	CButton	m_AddButton;
	CEdit	m_LemmaLocatorCtrl;
	CListCtrl	m_WordList;
	CString	m_UnitsSize;
	CString	m_LemmaLocator;
	//}}AFX_DATA

// Attributes
public:

	
// Operations
public:
    void Update();
	void SetSelected (bool Value);
	void SetCursor (int i);
	CDocument* FindArticle (WORD UnitNo);
	bool       SetArticle(WORD UnitNo, CString Value);
	void       BuildVals (vector<Valency>& Vals, WORD UnitNo);
	bool       PocketAndArticleDocTempalteAreEmpty();
	void	   UpdateCurrentPos();
	void	   SaveRossToTxt(CString FileName) const;
	afx_msg void OnReload();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWordList)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWordList();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message  functions
	//{{AFX_MSG(CWordList)
	afx_msg void OnGetdispinfoWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnWordlistAdd();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveWordlist() ;
	afx_msg void OnChangeLemmaLocator();
	afx_msg void OnSelectAllWords();
	afx_msg void OnInvertAllWords();
	afx_msg void OnDeselectAllWords();
	afx_msg void OnWordlistDel();
	afx_msg void OnSetFilter();
	afx_msg void OnSetSelectedButton();
	afx_msg void OnWordlistArticleBtn();
	afx_msg void OnSaveRossToTxt();
    afx_msg void OnSearchByArticle();
	afx_msg void OnShowScheme();
	afx_msg void OnStatistic();
	afx_msg void OnChangeTitle();
	afx_msg void OnComments();
	afx_msg void OnMenuitem32788();
	afx_msg void OnValencies();
	afx_msg void OnSemFet();
	afx_msg void OnStatisticFieldValue();
	afx_msg void OnSortByLemma();
	afx_msg void OnSortByDateAndTime();
	afx_msg void OnImport();
	afx_msg void OnArticleAppend();
	afx_msg void OnDelAllSelected();
	afx_msg void OnSetAuthor();
	afx_msg void OnSelectByAuthor();
	afx_msg void OnGXiStatistics();
	afx_msg void OnDblclkWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectDownward();
	afx_msg void OnEmptyArticles();
	afx_msg void OnReadonlyImage();
	afx_msg void OnFindByString();
	afx_msg void OnFindWrongRefs();
	afx_msg void OnClickWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownWordlistGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusLemmaLocator();
	afx_msg void OnExportAllDicts();
	afx_msg void OnEmptyAllDicts();
	afx_msg void OnImportAllDicts();
	afx_msg void OnDelTextDomains();
	afx_msg void OnAllDictEntries();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORDLIST_H__0AA09784_C726_11D2_A6E4_A290D9000000__INCLUDED_)
