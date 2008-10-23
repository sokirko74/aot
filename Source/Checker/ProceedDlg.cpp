// ProceedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Checker.h"
#include "CheckerDlg.h"
#include "ProceedDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProceedDlg dialog


CProceedDlg::CProceedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProceedDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CProceedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProceedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProceedDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProceedDlg, CDialog)
	//{{AFX_MSG_MAP(CProceedDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADD_ALL, &CProceedDlg::OnBnClickedAddAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProceedDlg message handlers

BOOL CProceedDlg::OnInitDialog() 
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();


	if (pDlg->m_bResultsAreSmall)
	{
		RECT MainRect,r;
		GetWindowRect( &MainRect );
		GetDlgItem(IDC_TEXT1)->GetWindowRect( &r);
		GetDlgItem(IDC_TEXT1)->SetWindowPos(&wndBottom,r.left,r.top, r.right-r.left, (r.bottom - r.top)*3, SWP_SHOWWINDOW|SWP_NOMOVE);
		int offset =  r.bottom - r.top+70;
		GetDlgItem(IDC_TEXT2)->GetWindowRect( &r);
		GetDlgItem(IDC_TEXT2)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+offset-MainRect.top, r.right-r.left, (r.bottom - r.top)/2, SWP_SHOWWINDOW);
		GetDlgItem(IDC_COMMENTS)->GetWindowRect( &r);
		GetDlgItem(IDC_COMMENTS)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+offset-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		GetDlgItem(IDC_STATIC6)->GetWindowRect( &r);
		GetDlgItem(IDC_STATIC6)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+offset-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		GetDlgItem(IDC_STATIC3)->GetWindowRect( &r);
		GetDlgItem(IDC_STATIC3)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+offset-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		int Offest2 = 70;
		GetDlgItem(IDC_TEXT3)->GetWindowRect( &r);
		GetDlgItem(IDC_TEXT3)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+Offest2-MainRect.top, r.right-r.left, (r.bottom - r.top)/2, SWP_SHOWWINDOW);
		GetDlgItem(IDC_PREV)->GetWindowRect( &r);
		GetDlgItem(IDC_PREV)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+Offest2-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		GetDlgItem(IDC_NEXT)->GetWindowRect( &r);
		GetDlgItem(IDC_NEXT)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+Offest2-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		GetDlgItem(IDC_BASENO)->GetWindowRect( &r);
		GetDlgItem(IDC_BASENO)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+Offest2-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
		GetDlgItem(IDC_STATIC4)->GetWindowRect( &r);
		GetDlgItem(IDC_STATIC4)->SetWindowPos(&wndBottom,r.left-MainRect.left,r.top+Offest2-MainRect.top, r.right-r.left, (r.bottom - r.top), SWP_SHOWWINDOW);
	}

	CDialog::OnInitDialog();

	m_CurrentBadSentence = -1;
	PostMessage(WM_COMMAND,IDC_SKIP);

	return TRUE;
}


string ConvertEOLN (string s)
{
	for (int i =0; i < s.length(); i++)
		if (s[i] == 1)
		{
			s[i] = '\n';
			s.insert(s.begin() +i, '\r');
		};
	return s;
};
/////////////////////////////////////////////////////////////////////////////

void CProceedDlg::SetBaseVariant(int ExampleNo)
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();
	
	m_BaseStartNo = -1;
	m_BaseCurrentNo = -1;

	// идем по всем сохраненным результатам, ищем начало "ExampleNo"
	int i1=0;
	for( ; i1<pDlg->m_base.size(); i1++ )
	{
		if( pDlg->m_base[i1].num == ExampleNo )
			break;
	}
	if( i1 >= pDlg->m_base.size() )
		return;

   // идем по всем сохраненным результатам, ищем конец "ExampleNo"
	int i2=i1;
	for( ; i2<pDlg->m_base.size(); i2++ )
	{
		if( pDlg->m_base[i2].num != ExampleNo )
			break;
	}

	m_BaseStartNo = i1;
	m_BaseCurrentNo = i1;
	m_BaseLastNo = i2 - 1;
}

/////////////////////////////////////////////////////////////////////////////

void CProceedDlg::OnAdd() 
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();

	CBase b;
	b.num = pDlg->m_nbad[m_CurrentBadSentence]+1;
	b.txt = pDlg->m_tran[pDlg->m_nbad[m_CurrentBadSentence]];
	pDlg->m_base.push_back(b);
	sort(pDlg->m_base.begin(),pDlg->m_base.end());
	pDlg->m_changed = TRUE;
	PostMessage(WM_COMMAND,IDC_SKIP);
}

/////////////////////////////////////////////////////////////////////////////

StringVector ConvertToVector(string s)
{
	size_t  start = 0;
	StringVector r;
	for (size_t i =0; i < s.length(); i++)
	{
		 if (s[i] == '\n')
		 {
			if (i > start)
				r.push_back(s.substr(start, i-start+1));

			start = i+1;
		 };
	};

	return r;
};



void CProceedDlg::ShowDifferences() 
{
	char old_base[10000];
	char new_base[10000];
	::GetDlgItemText(this->m_hWnd, IDC_TEXT2,new_base, 10000);
	::GetDlgItemText(this->m_hWnd, IDC_TEXT3,old_base, 10000);
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();

	if (		(pDlg->m_CheckerType == SyntaxCheck) 
			||	(pDlg->m_CheckerType == MorphCorpusCheck)
			||	(pDlg->m_CheckerType == ExternalExeFileCheck)
			
		)
	{
		// showing differences with "!!!"
		StringVector t1 = ConvertToVector(new_base);
		StringVector t2 = ConvertToVector(old_base);
		string tran  = "";
		for (int i = 0; i < t1.size(); i++)
		{
			string s;
			if (find(t2.begin(),t2.end(), t1[i]) == t2.end())
				s = " !!! ";
			s += t1[i];
			tran += s;

		};
		SetDlgItemText(IDC_TEXT2,tran.c_str());


		tran = "";
		for (int i = 0; i < t2.size(); i++)
		{
			string s;
			if (find(t1.begin(),t1.end(), t2[i]) == t1.end())
				s = " !!! ";
			s += t2[i];
			tran += s;
		};
		SetDlgItemText(IDC_TEXT3,tran.c_str());
	}
	else
	{
		int MinEqual = 10000;
		long j =0;
		for (; j < strlen(old_base) && j <  strlen(new_base); j++)
				if (old_base[j] != new_base[j])
					break;
		if (j < MinEqual)
				MinEqual = j;

		string tran = new_base;
		if  (   (MinEqual > 0) 
			 && (MinEqual <  tran.length()) 
			)
		tran.insert(MinEqual, "(!!!)");
		SetDlgItemText(IDC_TEXT2,tran.c_str());
	}

};


void CProceedDlg::UpdateOldBaseOnTheScreen() 
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();

	string old_base;
	if (m_BaseStartNo != -1)
		old_base = ConvertEOLN(pDlg->m_base[m_BaseCurrentNo].txt);
	SetDlgItemText(IDC_TEXT3,old_base.c_str());		

	char tmp[256];
	sprintf(tmp," %i of %i ",m_BaseCurrentNo - m_BaseStartNo+1, m_BaseLastNo - m_BaseStartNo + 1);
	SetDlgItemText(IDC_BASENO,tmp);

	int no = pDlg->m_nbad[m_CurrentBadSentence];
	SetDlgItemText(IDC_TEXT2,ConvertEOLN(pDlg->m_tran[no]).c_str());

	ShowDifferences();
};

void CProceedDlg::OnSkip() 
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();

	m_CurrentBadSentence++;
	if( m_CurrentBadSentence == pDlg->m_nbad.size() )
	{
		PostMessage(WM_CLOSE);
		return;
	}

	int no = pDlg->m_nbad[m_CurrentBadSentence];
	char tmp[256];
	sprintf(tmp," %i    (%i of %i bad) ",no+1,m_CurrentBadSentence+1,pDlg->m_nbad.size());
	SetDlgItemText(IDC_SENTNO,tmp);

//
	SetBaseVariant(no+1);

	// GetBaseVar(no+1);

	SetDlgItemText(IDC_TEXT1,pDlg->m_CheckExamples[no].m_Text.c_str());

	SetDlgItemText(IDC_COMMENTS,pDlg->m_CheckExamples[no].m_Comments.c_str());
	

	UpdateOldBaseOnTheScreen();

}

/////////////////////////////////////////////////////////////////////////////

void CProceedDlg::OnNext() 
{
	if (m_BaseStartNo == -1)
		return;

	if (m_BaseCurrentNo == m_BaseLastNo)
		return;

	m_BaseCurrentNo++;

	UpdateOldBaseOnTheScreen();
}

void CProceedDlg::OnPrev() 
{
	if (m_BaseStartNo == -1)
		return;

	if (m_BaseCurrentNo == m_BaseStartNo)
		return;

	m_BaseCurrentNo--;

	UpdateOldBaseOnTheScreen();
}


void CProceedDlg::OnBnClickedAddAll()
{
	CCheckerDlg *pDlg = (CCheckerDlg *)AfxGetMainWnd();
	for (; m_CurrentBadSentence < pDlg->m_nbad.size(); m_CurrentBadSentence++)
	{
		CBase b;
		b.num = pDlg->m_nbad[m_CurrentBadSentence]+1;
		b.txt = pDlg->m_tran[pDlg->m_nbad[m_CurrentBadSentence]];
		pDlg->m_base.push_back(b);
		sort(pDlg->m_base.begin(),pDlg->m_base.end());
		int no = pDlg->m_nbad[m_CurrentBadSentence];
		SetBaseVariant(no+1);
	}

	pDlg->m_changed = TRUE;
	PostMessage(WM_CLOSE);
}