#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "QueryHistory.h"

#define WM_NEXT_NONACCENTED_PARA (WM_USER + 1)

//----------------------------------------------------------------------------
class CSLFDocument;
class CMorphwizardDoc;



//----------------------------------------------------------------------------
class CMorphwizardView : public CFormView, public CQueryHistory
{

	std::vector<lemma_iterator_t> found_paradigms;
	bool			m_inFind;
	bool			m_inPredict;
	bool			m_inFilter;
	HICON			m_hIcon;
	int				m_ControlMargin;
	std::vector<const CPredictSuffix*> m_PredictedParadigms;
	std::string m_LemmaToPredict;



	void	ShowFoundParadigms();
	void	FilterFoundParadigms();
	int		GetPredictOrderId(const CPredictSuffix& S)  const;
	int		PlaceSimpleControl(int id, int x, int y);

protected: // create from serialization only
	CMorphwizardView();

	DECLARE_DYNCREATE(CMorphwizardView)



	// Attributes
public:
	int		m_SortPredictColumn;

	void echo(CString s);
	CMenu   m_Menu;

	CMorphwizardDoc* GetDocument() const;
	MorphoWizard* GetWizard();
	CSLFDocument* NewSLFDocument();

	// Operations
public:
	enum { IDD = IDD_MORHWIZARD_VIEW };
	CComboBox			m_FindWhat;
	CListCtrl			m_FoundList;
	CListCtrl			m_PredictedList;
	CEdit				m_PredictWhat;
	CEdit				m_StatusCtrl;
	CString				m_Status;
	long				m_MinimalFrequence;
	BOOL				m_bOnlyMainPartOfSpeeches;

	// Overrides
public:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	std::string ToInnerEncoding(CString strText) const;
	CString FromInnerEncoding(std::string s) const;
protected:

	// Implementation
public:
	virtual ~CMorphwizardView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnFind();

protected:
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnRemove();
	afx_msg void OnPredict();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnSetfocusFindWhat();
	afx_msg void OnKillfocusFindWhat();
	afx_msg void OnReadonlyImage();
	afx_msg void OnColumnclickPredictedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPredictedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusPredictWhat();
	afx_msg void OnKillfocusPredictWhat();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnNMDblclkFoundList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindParadigm();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEnKillfocusFilter();
	afx_msg void OnEnSetfocusFilter();
	afx_msg void OnHelpHelp();
	afx_msg void OnLvnColumnclickFoundList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolsPackMrd();
	afx_msg void OnToolsSaveListFile();
	afx_msg void OnToolsExport();
	afx_msg void OnToolsImport();
	afx_msg void OnToolsShowparadigmdifferences();
	afx_msg void OnToolsShowAccentDifferences();
	afx_msg void OnToolsSelectByFile();
	afx_msg void OnToolsSetParaNo();
	afx_msg void OnToolsSetAccentModelNo();
	afx_msg void OnToolsAccentWizard();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnNextNonAccentedPara(WPARAM wp, LPARAM lp);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);

	bool	OpenExistingParadigm(lemma_iterator_t it, bool bRunAccentWizard = false);
	lemma_iterator_t FindNonAccentedPara(bool partialAccentedPara);

	// Generated message map functions
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

	bool m_partialAccentedPara;
public:

	int m_PredictSuffLength;
	afx_msg void OnBnClickedSetPrdComments();
};


//----------------------------------------------------------------------------
#ifndef _DEBUG  // debug version in MorphwizardView.cpp
inline CMorphwizardDoc* CMorphwizardView::GetDocument() const
{
	return reinterpret_cast<CMorphwizardDoc*>(m_pDocument);
}

#endif

