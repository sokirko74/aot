// SLFEdiView.cpp : implementation file
//

#include "stdafx.h"
#include "Morphwizard.h"
#include "SLFView.h"
#include "SLFDocument.h"
#include "MorphwizardView.h"
#include "ChangeAllDlg.h"




//////////////////////////////////////////////////////////////////////////////
// CSLFView class implementation
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSLFView, CSizeFormView)

CSLFView::CSLFView() :
 CFormView(CSLFView::IDD)
{
	m_pRichView = 0;
	m_bRunAccentWizard = false;
}

CSLFView::~CSLFView()
{
}

MorphoWizard* CSLFView::GetWizard() 
{
	return  GetDocument()->GetWizard();
};

CSLFDocument* CSLFView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSLFDocument)));
	return (CSLFDocument*)m_pDocument;
}

//----------------------------------------------------------------------------
// CSLFView diagnostics
//----------------------------------------------------------------------------
#ifdef _DEBUG
void CSLFView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSLFView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG




void CSLFView::DoDataExchangeTypeGrammems(bool bSaveAndValidate)
{
	if (bSaveAndValidate)
	{
		m_TypeGrammemsList.GetWindowText(GetDocument()->m_CommonGrammems);
	}
	else
	if (m_TypeGrammemsList.GetCount() > 0)
	{
		CString grams = GetDocument()->m_CommonGrammems;
		int nIndex = m_TypeGrammemsList.FindStringExact(0, grams);
		if (grams.IsEmpty()) // FindStringExact cannot find an empty string, why?
			nIndex = 0;

		if (nIndex == CB_ERR)
		{
			if (m_TypeGrammemsList.GetCount() != 0)
				ErrorMessage (string("Bad type grammems: ")+(const char*)GetDocument()->m_CommonGrammems);
		}
		else
		{
			m_TypeGrammemsList.SetCurSel(nIndex);
		}
	};
};

void CSLFView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSLFView)
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_POSLIST, m_PosList);
	DDX_Control(pDX, IDC_GRAM_LIST, m_GramList);
	DDX_Control(pDX, IDC_GRAM_TITLE, m_GramTitle);
	DDX_Control(pDX, IDC_POS_TITLE, m_PosTitle);
	DDX_Control(pDX, IDC_PRD_NO_TITLE, m_PrdNoTitle);
	DDX_Control(pDX, IDC_PRD_NO, m_PrdNoEdit);
	DDX_Control(pDX, IDC_ACC_MDL_NO_TITLE, m_AccMdlNoTitle);
	DDX_Control(pDX, IDC_ACC_MDL_NO, m_AccMdlNoEdit);
	DDX_Text(pDX, IDC_PREFIXES, GetDocument()->m_Prefixes);
	DDX_Control(pDX, IDC_CHANGE_PARADIGM, m_ParadigmBtn);
	DDX_Control(pDX, IDC_CHANGE_ALL, m_ChangeAllBtn);
	DDX_Control(pDX, IDC_FIX_ACCENT_BTN, m_fixAccentBtn);
	DDX_Text(pDX, IDC_PARADIGM_COMMENTS, GetDocument()->m_ParadigmComments);
	DDX_Control(pDX, IDC_TYPE_GRAMMEMS, m_TypeGrammemsList);
	//}}AFX_DATA_MAP
	DoDataExchangeTypeGrammems(pDX->m_bSaveAndValidate==TRUE?true:false);


	if (m_pRichView)
	{
		if (pDX->m_bSaveAndValidate)
		{
			GetDocument()->m_ParadigmText = m_pRichView->GetText();

			int i;
			DDX_Text(pDX, IDC_PRD_NO, i);
			GetDocument()->m_Paradigm.m_FlexiaModelNo = i;
			DDX_Text(pDX, IDC_ACC_MDL_NO, i);
			GetDocument()->m_Paradigm.m_AccentModelNo = i;
			
		}
		else
		{
			m_pRichView->SetText(GetDocument()->m_ParadigmText);
			int i = GetDocument()->m_Paradigm.m_FlexiaModelNo;
			DDX_Text(pDX, IDC_PRD_NO, i);
			i = GetDocument()->m_Paradigm.m_AccentModelNo;
			DDX_Text(pDX, IDC_ACC_MDL_NO, i);
			DDX_Text(pDX, IDC_BASE, CString(GetDocument()->GetBase().c_str()));
		
		}
	}
}

BEGIN_MESSAGE_MAP(CSLFView, CFormView)
//{{AFX_MSG_MAP(CSLFView)
	ON_LBN_DBLCLK(IDC_POSLIST, OnDblclkPoslist)
	ON_LBN_DBLCLK(IDC_GRAM_LIST, OnDblclkGramList)
	ON_WM_SIZE()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_ADD_PREFIX, OnBnClickedAddPrefix)
	ON_BN_CLICKED(IDC_CHANGE_PARADIGM, OnBnClickedChangeParadigm)
	ON_EN_CHANGE(IDC_RICHEDIT21, OnChangeRichedit1)
//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHANGE_ALL, OnBnClickedChangeAll)
	ON_BN_CLICKED(IDC_FIX_ACCENT_BTN, OnBnClickedFixAccentBtn)
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
// CSLFView message handlers
//----------------------------------------------------------------------------

bool IsKeyWord (const CString& word, COLORREF& C, void* Data)
{
	try {
		CSLFView* V = (CSLFView*)Data;
		std::string w = (const char*)word;
		
		// part of speech?
		if (std::binary_search(V->GetWizard()->get_poses().begin(), V->GetWizard()->get_poses().end(), w) )
		{
			C = RGB(255,0,0);
			return true;
		};


		// grammem?
		const StringVector& v = V->GetWizard()->get_grammems();
		if (std::binary_search(v.begin(), v.end(), w) )
		{
			C = RGB(0,0,255);
			return true;
		};
	}
	catch(...)
	{
	};
	return false;
};


void SetComboBoxWidth(CComboBox* pmyComboBox)
{
	// Find the longest string in the combo box.
	CString      str;
	CSize      sz;
	int      dx = 0;
	TEXTMETRIC   tm;
	CDC*      pDC = pmyComboBox->GetDC();
	CFont*      pFont = pmyComboBox->GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < pmyComboBox->GetCount(); i++)
	{
	pmyComboBox->GetLBText(i, str);
	sz = pDC->GetTextExtent(str);

	// Add the avg width to prevent clipping
	sz.cx += tm.tmAveCharWidth;
	   
	if (sz.cx > dx)
		dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	pmyComboBox->ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	pmyComboBox->SetDroppedWidth(dx);

	CRect r;

	pmyComboBox->GetWindowRect(&r);
	pmyComboBox->SetWindowPos(0, 0, 0, dx+100, 200, SWP_SHOWWINDOW|SWP_NOMOVE);
	pmyComboBox->SetHorizontalExtent(400);
};

void CSLFView::set_lists()
{
	const StringVector &poses = GetWizard()->get_poses();
	for(size_t i = 0; i < poses.size(); i++){
		m_PosList.AddString(poses[i].c_str());
	}
	const StringVector &grammems = GetWizard()->get_grammems();
	for(size_t i = 0; i < grammems.size(); i++){
		std::string s = grammems[i]+ ",";
		m_GramList.AddString(s.c_str());
	}

	const StringVector& type_grammems = GetWizard()->get_type_grammems();
	for(size_t i = 0; i < type_grammems.size(); i++)
	{
		std::string s = type_grammems[i];

		m_TypeGrammemsList.AddString(s.c_str());
	}
	m_TypeGrammemsList.AddString("");

	SetComboBoxWidth(&m_TypeGrammemsList);
	DoDataExchangeTypeGrammems(false);
	

}

CRichEditCtrl& CSLFView::GetRichEditCtrl()
{
	return m_pRichView->GetRichEditCtrl();
}

void CSLFView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	m_pRichView = new CColorRichEditView(*GetWizard());

	m_pRichView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
      CRect(10,10, 100,150), this, AFX_IDW_PANE_FIRST + 1, NULL);
  
	GetRichEditCtrl().SetDlgCtrlID(IDC_RICHEDIT21);
	m_pRichView->m_Colorizer.InitializeParser(IsKeyWord, (void*)this);
	
	DWORD op = ECO_AUTOVSCROLL|ECO_AUTOHSCROLL|ECO_NOHIDESEL|ECO_WANTRETURN;
	

	GetRichEditCtrl().SetOptions(ECOOP_SET, op);

//	m_pRichView->m_bUseParse2 = false;

	UpdateData(FALSE);

	m_pRichView->RedrawLines(false);

	set_lists();
	
	if (GetDocument()->m_GotoLine != -1)
	{
		long nBegin = GetRichEditCtrl().LineIndex(GetDocument()->m_GotoLine);
		long nEnd = GetRichEditCtrl().LineIndex(GetDocument()->m_GotoLine+1);
		GetRichEditCtrl().SetSel(nBegin,nEnd);
	}
	else
	{
		GetRichEditCtrl().SetSel(0,0);
	}

//	CMorphwizardApp* App = (CMorphwizardApp*)AfxGetApp();
//	App->m_PointerCount[GetDocument()->m_pParent]++;

	{
//		CHARFORMAT2 cf;
//		cf.bCharSet = CHINESEBIG5_CHARSET;
//		cf.dwMask = CFM_CHARSET;

//		m_pRichView->GetRichEditCtrl().SetDefaultCharFormat(cf);
	}
   
}


void CSLFView::OnDblclkPoslist() 
{
	int index = m_PosList.GetCurSel();
	if(index == -1) return;
	CString pos;
	m_PosList.GetText(index, pos);
	GetRichEditCtrl().ReplaceSel(pos+" ");
	GetRichEditCtrl().SetFocus();
}

void CSLFView::OnDblclkGramList() 
{
	int index = m_GramList.GetCurSel();
	if(index == -1) return;
	CString gra;
	m_GramList.GetText(index, gra);
	GetRichEditCtrl().ReplaceSel(gra);	
	GetRichEditCtrl().SetFocus();
}

void CSLFView::ChangeSize(UINT nType, int cx, int cy)
{
	if( m_pRichView && cx > 0 && cy > 0 && m_pRichView->m_hWnd )
	{
//		TRACE("CSLFView::ChangeSize: %d,%d,%d\n",nType, cx, cy);
		int wr = 230,	// ширина панели справа от RichEdit
			d=8,		// расстояние между контролами
			h = 25;		// высота строк ввода

		/*if( GetWizard()->m_ReadOnly ) 
			wr = d;*/

		if( cx/2<wr ) wr=cx/2;
		int wl = cx - wr;


		int x=d, y=d;
		GetDlgItem(IDC_COMMON_GRM_LBL)->SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		CRect r;
		GetDlgItem(IDC_COMMON_GRM_LBL)->GetWindowRect(&r);

		x += r.Width() + d;
		GetDlgItem(IDC_TYPE_GRAMMEMS)->SetWindowPos(&wndBottom, x, y, wl-x, h, SWP_SHOWWINDOW|SWP_NOSIZE);
		GetDlgItem(IDC_TYPE_GRAMMEMS)->GetWindowRect(&r);

		x += r.Width() + d;
		GetDlgItem(IDC_PRD_LABEL_COMMENTS)->SetWindowPos(&wndBottom, x, y, wl-x, h, SWP_SHOWWINDOW|SWP_NOSIZE);
		GetDlgItem(IDC_PRD_LABEL_COMMENTS)->GetWindowRect(&r);
		x += r.Width() + d;
		GetDlgItem(IDC_PARADIGM_COMMENTS)->SetWindowPos(&wndBottom, x, y, wl-x, h, SWP_SHOWWINDOW|SWP_NOSIZE);

		y += h + d;
		int hy = cy-y-h-2*d;
		GetRichEditCtrl().SetWindowPos(&wndBottom,d,y,wl-d,hy, SWP_SHOWWINDOW);


		y += hy + d;
		GetDlgItem(IDC_PREFIXES_LBL)->SetWindowPos(&wndBottom,d,y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		GetDlgItem(IDC_PREFIXES_LBL)->GetWindowRect(&r);
		x = d + r.Width() + d;
		GetDlgItem(IDC_PREFIXES)->SetWindowPos(&wndBottom,x, y, wl-x, h, SWP_SHOWWINDOW);

		//if( !GetWizard()->m_ReadOnly ) 
		{
			m_Cancel.GetWindowRect(&r);
			x = cx - d - r.Width();
			y = d;
			m_Cancel.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			m_Ok.GetWindowRect(&r);
			x -= 2*d + r.Width();
			m_Ok.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			m_fixAccentBtn.GetWindowRect(&r);
			x -= 2*d + r.Width();
			m_fixAccentBtn.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			y += r.Height() + d;
			m_ParadigmBtn.GetWindowRect(&r);
			x = cx - d - r.Width();
			m_ParadigmBtn.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			m_ChangeAllBtn.GetWindowRect(&r);
			x -= 2*d + r.Width();
			m_ChangeAllBtn.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			wr -= d;
			x = cx - wr;
			y += r.Height() + d;
			m_PosTitle.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
			int wx = wr*5/9;
			int x1 = x + wx + d;
			m_GramTitle.SetWindowPos(&wndBottom, x1, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
			y += h;
			hy = cy-y-2*d-h;
			m_PosList.SetWindowPos(&wndBottom, x, y, wx, hy, SWP_SHOWWINDOW);
			m_GramList.SetWindowPos(&wndBottom, x1, y, cx-x1-d, hy, SWP_SHOWWINDOW);
			y += hy + d;
		}
		/*else
		{
			y = cy - d - h;
		}*/

		d = 4;
		{
			m_AccMdlNoEdit.GetWindowRect(&r);
			x = cx - d - r.Width();
			m_AccMdlNoEdit.SetWindowPos(&wndBottom, x,y, r.Width(), h, SWP_SHOWWINDOW);

			m_AccMdlNoTitle.GetWindowRect(&r);
			x -= d + r.Width();
			m_AccMdlNoTitle.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		}

		{
			m_PrdNoEdit.GetWindowRect(&r);
			x -= 2*d + r.Width();
			m_PrdNoEdit.SetWindowPos(&wndBottom, x,y, r.Width(), h, SWP_SHOWWINDOW);

			m_PrdNoTitle.GetWindowRect(&r);
			x -= d + r.Width();
			m_PrdNoTitle.SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		}

		{
			GetDlgItem(IDC_BASE)->GetWindowRect(&r);
			x -= d + r.Width();
			GetDlgItem(IDC_BASE)->SetWindowPos(&wndBottom, x, y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

			GetDlgItem(IDC_BASE_TITLE)->GetWindowRect(&r);
			x -= 2*d + r.Width();
			GetDlgItem(IDC_BASE_TITLE)->SetWindowPos(&wndBottom, x,y, r.Width(), h, SWP_SHOWWINDOW);

		}
		d = 8;


		//if( GetWizard()->m_ReadOnly ) 
		{
			GetDlgItem(IDC_PREFIXES)->GetWindowRect(&r);
			r.right = x-2*d;
			GetDlgItem(IDC_PREFIXES)->SetWindowPos(&wndBottom,r.left, y, r.Width(), h, SWP_SHOWWINDOW);
		}

//		Invalidate();
	}
	CFormView::OnSize(nType, cx, cy);
}

void CSLFView::OnSize(UINT nType, int cx, int cy) 
{
	ChangeSize(nType, cx, cy);	
}

BOOL CSLFView::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	AfxGetApp()->WinHelp(IDH_SLF_EDIT);
	return TRUE;
}


extern bool InputBox (const char* Caption, string& OutBuffer);

void CSLFView::OnBnClickedAddPrefix()
{
	UpdateData();
	string prefix;
	if ( !InputBox("Input prefix:", prefix) )
		return;

	std::string sprfix = prefix;
	if (sprfix.empty()) return;
	std::string new_slf;
	StringTokenizer T(GetDocument()->m_ParadigmText, "\n");
	while (T())
	{
		std::string Line = T.val();
		Trim(Line);
		if (Line.empty()) continue;
		new_slf += sprfix + Line+"\n";

	};
	GetDocument()->m_ParadigmText = new_slf.c_str();
	UpdateData(FALSE);
	m_pRichView->RedrawLines();
}

//----------------------------------------------------------------------------
void CSLFView::OnBnClickedChangeParadigm()
{
	UpdateData();
	string s;
	if ( !InputBox("Input prototype lemma or paradigm number: ", s) )
		return;
	
	RmlMakeUpper(s, GetWizard()->m_Language);
	Trim(s);
	if (s.empty()) return;

	WORD ParadigmNo;
	if( isdigit((BYTE)s[0]))
	{
		ParadigmNo = atoi(s.c_str());
	}
	else
	{
		std::vector<lemma_iterator_t> found_paradigms;
		GetWizard()->find_lemm(s.c_str(), false, found_paradigms);
		if (found_paradigms.empty())
		{
			ErrorMessage(Format("% is not found",s.c_str()));
			return;
		}
		ParadigmNo = found_paradigms[0]->second.m_FlexiaModelNo;
	};
	if ( ParadigmNo >= GetWizard()->m_FlexiaModels.size() )
	{
			ErrorMessage(Format("Paradigm no %i is not found",ParadigmNo));
			return;
	}

	string Lemma = (const char*)GetDocument()->GetLemma();

	const CFlexiaModel &new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
	if (GetDocument()->m_Paradigm.m_FlexiaModelNo != UnknownParadigmNo)
	{
		const CFlexiaModel &old_par = GetWizard()->m_FlexiaModels[GetDocument()->m_Paradigm.m_FlexiaModelNo];

		std::string new_flexia = new_par.get_first_flex();
		std::string lemma_suffix = Lemma.substr(Lemma.length() - new_flexia.length());
		RmlMakeUpper(lemma_suffix, GetWizard()->m_Language);

		if (new_flexia != lemma_suffix)
		{
			ErrorMessage ("Your choice will change lemma of the edited word. Is is not allowed.");
			return;
		};
	};
	GetWizard()->change_prd_info(GetDocument()->m_Paradigm, Lemma,  ParadigmNo,UnknownParadigmNo,true);
	GetDocument()->m_ParadigmText = GetWizard()->mrd_to_slf(Lemma.c_str(), new_par, GetDocument()->m_Paradigm.m_AccentModelNo, GetDocument()->m_Paradigm.m_AuxAccent, 50).c_str();	
	UpdateData(FALSE);
	m_pRichView->RedrawLines();
	
}

//----------------------------------------------------------------------------
void CSLFView::OnChangeRichedit1() 
{
	assert(m_pRichView);
	m_pRichView->RedrawLine();
}

//----------------------------------------------------------------------------
void CSLFView::OnBnClickedOk()
{
	UpdateData();
	if( GetDocument()->SaveModified() )
	{
		if( !GetDocument()->IsModified() )
		{
			if( m_bRunAccentWizard ) 
				GetDocument()->m_pParent->PostMessage(WM_NEXT_NONACCENTED_PARA);
			else
				GetDocument()->m_pParent->OnFind();
		}
		GetDocument()->m_bShouldBeClosed = true;
		GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
	}
}

//----------------------------------------------------------------------------
void CSLFView::OnBnClickedCancel()
{
/*
	UpdateData();
	if (GetDocument()->IsModified())
	{
		if (AfxMessageBox ("Exit without save?", MB_OKCANCEL) == IDCANCEL)
			return;        
	};
	GetDocument()->m_bShouldBeClosed = true;
*/
	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
}

//----------------------------------------------------------------------------
void CSLFView::OnBnClickedChangeAll()
{
	UpdateData();

	CParadigmInfo paraOld = GetDocument()->m_SaveParadigm;
	CString lemmaOld = GetDocument()->GetSavedLemma();

	if( GetDocument()->IsModified() )
	{
		if( !GetDocument()->SaveModified() )
			return;

		if( GetDocument()->IsModified() ) 
		{
			GetDocument()->m_bShouldBeClosed = true;
			GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
			return;
		}
	}

	WORD ParadigmNo = GetDocument()->m_Paradigm.m_FlexiaModelNo;
	WORD AccentMdlNo = GetDocument()->m_Paradigm.m_AccentModelNo;
	ASSERT(ParadigmNo<GetWizard()->m_FlexiaModels.size());

	std::vector<lemma_iterator_t> found_paradigms;
	GetWizard()->find_lemm_by_prdno(paraOld.m_FlexiaModelNo,found_paradigms);

	// удаляем из списка парадигму-шаблон (исправлялись только ударения)
	if( paraOld.m_FlexiaModelNo==ParadigmNo ) 
	{
		for( int i=0; i<found_paradigms.size(); ++i ) 
		{
			if( found_paradigms[i]->second==GetDocument()->m_Paradigm 
				&& found_paradigms[i]->first==std::string(GetDocument()->GetSavedLemma()) ) 
			{
				found_paradigms.erase(found_paradigms.begin()+i);
				break;
			}
		}
	}

	if( !found_paradigms.empty()) 
	{	
		CChangeAllDlg dlg(*GetWizard(),found_paradigms,paraOld.m_AccentModelNo,AccentMdlNo);
		if( dlg.DoModal()==IDOK )
		{
			if( dlg.m_usedAccentModel==0 ) 
				AccentMdlNo = paraOld.m_AccentModelNo;

			bool keepAccentModel = (dlg.m_usedAccentModel!=2);

			const CFlexiaModel &new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
			size_t ErrorsCount = 0;

			for (size_t i=0; i<found_paradigms.size(); i++)
			{
				if( dlg.m_anyAccentModels==0 
					&& found_paradigms[i]->second.m_AccentModelNo != paraOld.m_AccentModelNo )
					continue;

				const CFlexiaModel &old_par = GetWizard()->m_FlexiaModels[found_paradigms[i]->second.m_FlexiaModelNo];

				std::string new_flexia = new_par.get_first_flex();
				std::string lemma = GetWizard()->get_lemm_string(found_paradigms[i]);
				std::string lemma_suffix = lemma.substr(lemma.length() - new_flexia.length());

				if (new_flexia == lemma_suffix)
				{
					GetWizard()->change_prd_info(found_paradigms[i]->second, lemma, 
						ParadigmNo, AccentMdlNo, keepAccentModel);
				}
				else
				{
					ErrorsCount++;
				}
			}
			if (ErrorsCount > 0)
			{
				ErrorMessage (Format("This last operation cannot change lemma in %i cases.", ErrorsCount));
			}
		}
	}

	if( m_bRunAccentWizard ) 
		GetDocument()->m_pParent->PostMessage(WM_NEXT_NONACCENTED_PARA);
	else
		GetDocument()->m_pParent->OnFind();

	GetParent()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 1);
}

//----------------------------------------------------------------------------
void CSLFView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	SetInputLanguage(GetWizard()->m_Language);
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//----------------------------------------------------------------------------
void CSLFView::OnBnClickedFixAccentBtn()
{
	CRichEditCtrl& re = GetRichEditCtrl();
	long nStartChar, nEndChar;
	re.GetSel(nStartChar,nEndChar);
	int lineStart=0, lineEnd=re.GetLineCount()-1;
	bool hasSel = (nEndChar>=0 && nEndChar!=nStartChar);
	if( hasSel ) 
	{
		lineStart = re.LineFromChar(nStartChar);
		lineEnd = re.LineFromChar(nEndChar-1)+1;
		nStartChar = re.LineIndex(lineStart);
		nEndChar = re.LineIndex(lineEnd);
		if( nEndChar==-1 ) 
			nEndChar = re.GetTextLength();
	}
	else
	{
		nStartChar = 0;
		nEndChar = re.GetTextLength();
	}

	if( lineEnd-lineStart<1 )
		return;

	CString text;
	re.GetWindowText(text);
	int lineCount = text.Replace("\r\n","\n");

	CString t = text.Mid(nStartChar);
	int acc = t.FindOneOf(" '\n");
	if( acc<=0 || t[acc]!='\'' 
		|| !is_lower_vowel(t[acc-1],GetWizard()->m_Language) ) 
	{
//		if( !hasSel ) re.SetSel(nStart,nStart);
		return; 
	}

	if( hasSel ) 
		re.SetSel(nStartChar,nEndChar);

	StringTokenizer	tok(text,"\n");
	CStringArray lines;
	while( tok() ) lines.Add(tok.val());

//	ASSERT(lineEnd<=lines.GetSize());
	if( lineEnd>lines.GetSize() )
		lineEnd = lines.GetSize();

	int badAccCount=0;
	int i=lineStart+1;
	for( ; i<lineEnd; ++i ) 
	{
		int e = lines[i].Find(" ");
		if( e<=0 ) continue;

		int a = lines[i].Find("'");
		if( a<e && a>0 && a!=acc 
			&& is_lower_vowel(lines[i].GetAt(a-1),GetWizard()->m_Language) )
			++badAccCount;
	}

	CString s;
	if( badAccCount>0 ) 
	{
		s.Format("Reset accents in %d wordforms?",badAccCount);
		if( MessageBox(s,NULL,MB_YESNO)==IDYES )
			badAccCount = 0;
	}

	if( hasSel ) 
		re.SetSel(nStartChar,nStartChar);

	CRichEditRedrawer redrawer(*m_pRichView);

	for( i=lineStart+1; i<lineEnd; ++i )
	{
		int e = lines[i].Find(" ");
		if( e<=0 ) continue;

		s = lines[i];
		int a = lines[i].Find("'");
		bool bChange=false;
		if( a<e && a>=0 )
		{
			if( (badAccCount==0 && a!=acc) || a==0 || 
				!is_lower_vowel(s[a-1],GetWizard()->m_Language) )
			{
				s.Delete(a);
				a = -1;
				bChange = true;
			}
		}

		if( a==-1 && is_lower_vowel(s[acc-1],GetWizard()->m_Language) ) 
		{
			s.Insert(acc,'\'');
			bChange = true;
		}

		if( bChange ) 
		{
			s += "\r\n";
			nStartChar = re.LineIndex(i);
			nEndChar = re.LineIndex(i+1);
			if( nEndChar==-1 ) 
				nEndChar = re.GetTextLength();

			re.SetSel(nStartChar,nEndChar);
			re.ReplaceSel(s);
			redrawer.RedrawLine(i);
		}
	}
}

