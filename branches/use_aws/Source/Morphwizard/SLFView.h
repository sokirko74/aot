#pragma once


#include "../MorphWizardLib/wizard.h"
#include "ColorRichEditView.h"
#include "resource.h "
#include "afxwin.h"



class CSLFDocument;

//----------------------------------------------------------------------------
class CSLFView : public CFormView
{
	DECLARE_DYNCREATE(CSLFView)

public:
	CSLFView();           // protected constructor used by dynamic creation
	virtual ~CSLFView();

		
	enum { IDD = IDD_SLF_VIEW };

	MorphoWizard* GetWizard();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	CSLFDocument* GetDocument();
	CRichEditCtrl& GetRichEditCtrl();
	void ChangeSize(UINT nType, int cx, int cy);
	void set_lists();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedChangeAll();
	afx_msg void OnDblclkPoslist();
	afx_msg void OnDblclkGramList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	
	afx_msg void OnBnClickedAddPrefix();
	afx_msg void OnBnClickedChangeParadigm();
	afx_msg void OnChangeRichedit1();

	DECLARE_MESSAGE_MAP()

	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnBnClickedFixAccentBtn();
	void			DoDataExchangeTypeGrammems(bool bSaveAndValidate);
protected:
	CButton		m_Ok;
	CButton		m_Cancel;
	CListBox	m_PosList;
	CListBox	m_GramList;
	CStatic		m_PosTitle;
	CStatic		m_GramTitle;
	CStatic		m_PrdNoTitle;
	CEdit		m_PrdNoEdit;
	CStatic		m_AccMdlNoTitle;
	CEdit		m_AccMdlNoEdit;

	CButton		m_ParadigmBtn;
	CButton		m_ChangeAllBtn;
	CButton		m_fixAccentBtn;
public:
	CColorRichEditView* m_pRichView;
	bool		m_bRunAccentWizard;
	CComboBox m_TypeGrammemsList;
};


