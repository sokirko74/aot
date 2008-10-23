// MorphwizardView.h : interface of the CMorphwizardView class
//

#ifndef AFX_MORHWIZARDVIEW
#define AFX_MORHWIZARDVIEW


#include "afxcmn.h"
#include "SizeFormView.h"
#include "smart_ptr.h"
#include "afxwin.h"
#include "list"
#include "ListCtrlToolTip.h"

#define WM_NEXT_NONACCENTED_PARA (WM_USER + 1)

//----------------------------------------------------------------------------
class CSLFDocument;
class CMorphwizardDoc;
class FreqDict;


//----------------------------------------------------------------------------
class CMorphwizardView : public CSizeFormView
{

	std::vector<lemma_iterator_t> found_paradigms;
	bool			m_inFind;
	bool			m_inPredict;
	bool			m_inFilter;
	HICON			m_hIcon;
	list<string>	m_LastQueries;


	void	ShowFoundParadigms();
	void	FilterFoundParadigms();
	int		GetPredictOrderId(const CPredictSuffix& S)  const;
	void	LoadHistory();
	void	SaveHistory();
	void	ChangeHistory(string query);

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
	CSLFDocument* NewSLFDocument ();

// Operations
public:
	enum { IDD = IDD_MORHWIZARD_VIEW };
	CComboBox			m_FindWhat;
	CListCtrlToolTip	m_FoundList;
	CListCtrlToolTip	m_PredictedList;
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
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
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
	afx_msg void OnNMDblclkFoundList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFindParadigm();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEnKillfocusFilter();
	afx_msg void OnEnSetfocusFilter();
	afx_msg void OnHelpHelp();
	afx_msg void OnLvnColumnclickFoundList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolsPackMrd();
	afx_msg void OnToolsSaveListFile();
	afx_msg void OnToolsExport();
	afx_msg void OnToolsImport();
	afx_msg void OnToolsShowparadigmdifferences();
	afx_msg void OnToolsSelectByFile();
	afx_msg void OnToolsSetParaNo();
	afx_msg void OnToolsAccentWizard();
	afx_msg void OnUpdateToolsExport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsImport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsPackMrd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsSetParaNo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsAccentWizard(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsSaveListFile(CCmdUI *pCmdUI);

	afx_msg LRESULT OnNextNonAccentedPara( WPARAM wp, LPARAM lp );
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);

	bool	OpenExistingParadigm( lemma_iterator_t it, bool bRunAccentWizard=false );
	lemma_iterator_t FindNonAccentedPara( bool partialAccentedPara );

// Generated message map functions
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

	bool m_partialAccentedPara;
public:
	master_ptr<FreqDict>	m_pFreqDict;
	
	int m_PredictSuffLength;
	afx_msg void OnBnClickedSetPrdComments();
};


//----------------------------------------------------------------------------
// for AccentWizard
//----------------------------------------------------------------------------
class FreqDict
{
	struct FreqWord: std::string
	{
		int freq;
		bool operator !() const	{ return empty() || freq<=0; }
	};
public:
	FreqDict( CFileMeterRML* pMeter=NULL ) : m_pMeter(pMeter), m_minFreq(2)	{}

	bool operator!() const						{ return  m_words.empty(); }
	const int GetWordCount() const					{ return  m_words.size(); }
	const std::string& GetWord( int index ) const	{ return m_words[index]; }
	int GetFrequency( int index ) const			{ return m_words[index].freq; }
	bool Load( const CString&  fileName, int minFreq=2 );
	int GetMinFrequency() const					{ return m_minFreq; }
	const CString&	GetFileName() const			{ return m_fileName; }
private:
	std::vector<FreqWord>		m_words;
	init_ptr<CFileMeterRML>		m_pMeter;
	CString		m_fileName;
	int			m_minFreq;
};

//----------------------------------------------------------------------------
#ifndef _DEBUG  // debug version in MorphwizardView.cpp
inline CMorphwizardDoc* CMorphwizardView::GetDocument() const
   { return reinterpret_cast<CMorphwizardDoc*>(m_pDocument); }

#endif


#endif //AFX_MORHWIZARDVIEW

