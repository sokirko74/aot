// RossDoc.cpp : implementation of the CRossDoc class
//

#include "StdAfx.h"
#include "resource.h"
#include "InputBox.h"
#include "RossDoc.h"

const int LoginSize = 10;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRossDoc

IMPLEMENT_DYNCREATE(CRossDoc, CDocument)

BEGIN_MESSAGE_MAP(CRossDoc, CDocument)
	//{{AFX_MSG_MAP(CRossDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const char guest_user_name[] = "guest";

CRossDoc::CRossDoc() : m_ExternalRossHolder(NoneRoss)
{
	m_Author = guest_user_name;
	m_ReadOnly = true;
	m_FirstLoadReadonly = false;
	m_DoNotLock = false;
	m_bArticleInitTextMode = false;
	m_bSerialized = false;
	m_bDoNotSaveLastUserName = false;
	m_RossId = NoneRoss;
}

CDictionary*	CRossDoc::GetRoss()
{
	return GetRossHolder()->GetRoss();
}

CView*  CRossDoc::GetWordList()
{
	POSITION p = GetFirstViewPosition();
	return GetNextView(p);  

};


const CDictionary*	CRossDoc::GetRoss() const
{
	return GetRossHolder()->GetRoss();
}
CStructDictHolder*	CRossDoc::GetRossHolder()
{
	if (m_RossId != NoneRoss)
		return GetSemBuilder().m_RusStr.m_pData->GetRossHolder(m_RossId);
	else
		return &m_ExternalRossHolder;
}

const CStructDictHolder*	CRossDoc::GetRossHolder() const
{
	if (m_RossId != NoneRoss)
		return GetSemBuilder().m_RusStr.m_pData->GetRossHolder(m_RossId);
	else
		return &m_ExternalRossHolder;
}

std::string CRossDoc::GetLockFileName()  const
{
	return MakeFName(GetRossHolder()->GetDictPath(), "lck").c_str();
}



void CRossDoc::RemoveLock() const
{
   try 
   {
		std::string FileName = GetLockFileName();
		if (fs::exists(FileName))
			fs::remove(FileName);
	}
	catch (...) 
	{
	};

}

void CRossDoc::MakeReadOnly() 
{
	  if    (!m_ReadOnly)
	  {	 
			m_ReadOnly = true;
			m_Author = guest_user_name;
			RemoveLock();
	  };
};


CRossDoc::~CRossDoc()
{
	RemoveLock();
};

BOOL CRossDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CRossDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	if (!GlobalPocketAndArticleDocTempalteAreEmpty(this)) return FALSE;	

	return CDocument::CanCloseFrame(pFrame);
};

std::string get_user_name(const std::vector<std::string>& users) {
	//todo: save last user name to registry
	size_t trial_count = 3;
	for (size_t i = 0; i < trial_count; i++)
	{
		char login[20];
		strcpy(login, guest_user_name);
		if (!InputBoxUtf8("Input your login (\"guest\" - for ReadOnly):", login, 20 - 1, "Password Dialog", AfxGetApp()->m_pMainWnd)
			|| (strlen(login) == 0))
		{
			if (i + 1 < trial_count) {
				AfxMessageBox(_T("Your must enter your login, try one more time!"));
				continue;
			}

		}
		std::string user = login;
		Trim(user);
		if (user == guest_user_name)
		{
			return user;
		};

		for (auto& u : users) {
			if (user == u) {
				return user;
			}
		}
		if (i + 1 < trial_count) {
			AfxMessageBox(_T("Your login is incorrect"));
		}
	};
	AfxMessageBox(_T("Cannot find your login in loginlist. Logging as guest..."));
	return guest_user_name;
}


void CRossDoc::Serialize(CArchive& ar)
{

	m_FirstLoadReadonly =  ((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly; 
	SerializeInner(ar);
	BuildBasicDomItems();

	try {
		if (!ar.IsStoring()) {
			if (!m_ReadOnly)
			{
				m_Author = get_user_name(GetRoss()->GetConfig().DictUsers).c_str();
			}
			if (m_Author == guest_user_name) {
				MakeReadOnly();
			}
		}
	}
	catch (...)
	{
		AfxMessageBox (_T("An exception while entering login"));
		return;
	};

}

void CreateLockFile(std::string fileName)
{
	FILE* fp = fopen (fileName.c_str(), "w");
	fprintf (fp,"Date = %s\n", CTime::GetCurrentTime().Format( "%A, %B %d, %Y " )); 
	fprintf (fp,"Time = %s\n", CTime::GetCurrentTime().Format( "%H : %M" )); 
	CString UserName = getenv("USERNAME");
    if (!UserName.IsEmpty())
		fprintf (fp, "UserName = %s\n", utf16_to_utf8((const TCHAR*)UserName).c_str());
	fclose(fp);
};
	


bool CRossDoc::IsThesRoss() const
{
	return ::IsThesRoss(m_RossId);
}

int	CRossDoc::GetThesId() const
{
	assert (::IsThesRoss(m_RossId));
	return GetThesIdByRossId(m_RossId);
}

void CRossDoc::SerializeInner(CArchive& ar)
{
	if (ar.IsStoring())
	{
		
		if (!GetRoss()->Save())
			throw CExpc (Format ("Cannot save %s", GetRossHolder()->GetDictPath().c_str()));

	}
	else
	{       
		m_bSerialized = true;
		std::string DictFileName = _U8(ar.GetFile()->GetFilePath());  
		std::string dict_directory = fs::path(DictFileName).parent_path().string();
		m_ReadOnly = FileExists(GetLockFileName().c_str()) || m_FirstLoadReadonly;
		m_DomainParams.clear(); // todo: restore colors for domains in rossdev
		m_OptionsComments.clear();

		CAllRossesHolder* Trans = GetSemBuilder().m_RusStr.m_pData;
		Trans->m_LastUpdateTime++;

		m_RossId = Trans->GetRegisteredRossId(dict_directory);
		GetRossHolder()->LoadStructDict(dict_directory, false, Trans->m_LastUpdateTime);

		if (   !m_ReadOnly && !m_DoNotLock	)
			CreateLockFile(GetLockFileName());

		m_FirstLoadReadonly = false;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CRossDoc diagnostics

#ifdef _DEBUG
void CRossDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRossDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRossDoc commands

void CRossDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::SetTitle(_U16(GetRossHolder()->GetDictName()));
}

DictTypeEnum CRossDoc::GetRossType() const {
	return m_RossId;
}

BOOL CRossDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_ReadOnly)
	{
		::MessageBox(0, _T("Cannot write: the dictionary is read only."), _T("Error"), MB_OK);
		return FALSE;
	};
	
	return CDocument::OnSaveDocument(lpszPathName);
}



BOOL CRossDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CRossDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	try
	{
	 if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	}
	catch (std::exception& c)
	{
       AfxMessageBox  (_U16(c.what()));
	   return FALSE;

	};
	// TODO: Add your specialized creation code here
	
	return TRUE;
}

void CRossDoc::SetModifiedFlag( BOOL bModified)
{
	if (m_bSerialized)
	{
		CDocument::SetModifiedFlag(bModified);
		auto t = ++GetSemBuilder().m_RusStr.m_pData->m_LastUpdateTime;
		GetRossHolder()->SetUpdateTime(t);
	}
};




void CRossDoc::BuildBasicDomItems()
{
	m_DomParamsPtr.clear();
	m_DomParamsPtr.resize(GetRoss()->m_Domens.size());
    for (size_t i=0; i<GetRoss()->m_Domens.size(); i++)
	{
		std::vector<CDomainParam>::iterator It = find(m_DomainParams.begin(), m_DomainParams.end(), 
			GetRoss()->m_Domens[i].GetDomStr().c_str());
		if (It == m_DomainParams.end())
			m_DomParamsPtr[i]  = ErrUChar;
		else
			m_DomParamsPtr[i]  = It - m_DomainParams.begin();

	};
  
 
	GetRoss()->UpdateConstDomens();

	for (BYTE dom_no=0; dom_no < GetRoss()->m_Domens.size(); dom_no++)
	{
		const CDomen& D = GetRoss()->m_Domens[dom_no];
  	     if ((D.GetDomainSource() == dsMetaText) || (D.GetDomainSource() == dsExpres))
		 { 
			for (auto s : D.GetAllStrings()) {
				m_BasicDomItems[s] = dom_no;
			}
		 };
	}

	//  adding all fields to m_Fields
	for (auto& f: GetRoss()->Fields)
	{
		m_Fields.insert(f.FieldStr);
		if  (f.IsApplicToActant)
			for (size_t k=1; k < MaxNumAct+1; k++)
				m_Fields.insert(Format("%s%i", f.FieldStr, k));

	};

	GetRossHolder()->InitConsts();
};




const char* CRossDoc::GetGramFetFieldName() const
{
		return "GF";
};

const char* CRossDoc::GetReferenceFieldName() const
{
		return "REF";
};


const char* CRossDoc::GetEnglishFieldName() const
{
		return "ENG";
};
