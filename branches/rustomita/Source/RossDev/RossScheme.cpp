// RossScheme.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RossScheme.h"
#include "BasicDomenEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRossScheme dialog


CRossScheme::CRossScheme(CRossDoc* pRossDoc,
						 CWnd* pParent /*=NULL*/)
	: CDialog(CRossScheme::IDD, pParent),
	  m_pRossDoc(pRossDoc)
{
	//{{AFX_DATA_INIT(CRossScheme)
	//}}AFX_DATA_INIT
}


void CRossScheme::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRossScheme)
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRossScheme, CDialog)
	//{{AFX_MSG_MAP(CRossScheme)
	ON_BN_CLICKED(IDC_BUTTON1, OnEditDomain)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnHdnItemdblclickList1)
	ON_BN_CLICKED(IDC_SAVE_TO_FILES, OnBnClickedSaveToFiles)
	//}}AFX_MSG_MAP
	
	ON_BN_CLICKED(IDC_LOAD_CONST_FROM_FILES, OnBnClickedLoadConstFromFiles)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRossScheme message handlers



BOOL CRossScheme::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ListCtrl.InsertColumn( 0,"Name", LVCFMT_LEFT, 150);
	for (size_t i=0; i<GetRoss()->m_Domens.size(); i++)
	{
		int nPos = m_ListCtrl.InsertItem(i, GetRoss()->m_Domens[i].DomStr,0);
		m_ListCtrl.SetItemData(nPos, i);
	};

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CRossScheme::OnEditDomain() 
{
	// TODO: Add your control notification handler code here
    POSITION pos =  m_ListCtrl.GetFirstSelectedItemPosition();
    if (pos == NULL)
	{
		AfxMessageBox ("No object selected!");  
		return;
	};

	int nPos = m_ListCtrl.GetNextSelectedItem(pos);
    CBasicDomainEdit dlg (m_pRossDoc, m_ListCtrl.GetItemData(nPos), true);
    dlg.DoModal();

}



void CRossScheme::OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	OnEditDomain();
	*pResult = 0;
}

void CRossScheme::OnBnClickedSaveToFiles()
{
	CFileDialog  D (FALSE, 0,"D_", OFN_NOVALIDATE);
	if (!D.DoModal()) return;
	string Path = GetPathByFile((const char*)D.GetPathName());
	
	size_t i=0;

	for (; i<GetRoss()->m_Domens.size(); i++)
	{
		const CDomen& D = GetRoss()->m_Domens[i];
		if (D.IsFree) continue;
		
		string FileName = Path + (const char*)D.DomStr + string(".txt");
		FILE* fp = fopen (FileName.c_str(), "w");
		for (size_t k = 0; k < D.m_ItemsLength; k++)
			fprintf (fp, "%s\n", (const char*)D.m_Items[k]);
		fclose(fp);
	};

	if (::MessageBox(m_hWnd, "Export of metaconstants was finished! Delete all meta constants?" , "Question",MB_YESNO) == IDNO) return;

	for (int k = GetRoss()->m_Domens.size()-1; k>=0; k--)
	{
		const CDomen& D = GetRoss()->m_Domens[GetRoss()->GetDomItemDomNo(k)];
		if (D.IsFree) continue;
		GetRoss()->DelDomItem(k);
	};

	m_pRossDoc->SetModifiedFlag(TRUE);

}

void CRossScheme::OnBnClickedLoadConstFromFiles()
{
	CFileDialog  D (TRUE, 0,"D_", OFN_NOVALIDATE);
	if (!D.DoModal()) return;
	string Path = GetPathByFile((const char*)D.GetPathName());
	if  (Path.empty()) return;
	for (size_t i=0; i<GetRoss()->m_Domens.size(); i++)
	{
		const CDomen& D = GetRoss()->m_Domens[i];
		if (D.IsFree) continue;
		
		string FileName = Path + (const char*)D.DomStr + string(".txt");
		FILE* fp = fopen (FileName.c_str(), "r");
		if (!fp) continue;
		char buffer[100];
		while (fgets(buffer, 100, fp))
		{
			string q = buffer;
			Trim(q);
			int dummy;
			if (!GetRoss()->InsertDomItem(q.c_str(), i, dummy))
			{
				ErrorMessage(GetRoss()->m_LastError);
				fclose(fp);
				return;
			}
		};
		fclose(fp);
	};
	m_pRossDoc->SetModifiedFlag(TRUE);
	
}
