// RossDoc.cpp : implementation of the CRossDoc class
//

#include "stdafx.h"
#include "..\rossdev\resource.h"
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

/////////////////////////////////////////////////////////////////////////////
// CRossDoc construction/destruction

CRossDoc::CRossDoc() : m_ExternalRossHolder(GetSemBuilder().m_RusStr.m_pData)
{
	m_Author = "user";
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
CRossHolder*	CRossDoc::GetRossHolder()
{
	if (m_RossId != NoneRoss)
		return GetSemBuilder().m_RusStr.m_pData->GetRossHolder(m_RossId);
	else
		return &m_ExternalRossHolder;
}

const CRossHolder*	CRossDoc::GetRossHolder() const
{
	if (m_RossId != NoneRoss)
		return GetSemBuilder().m_RusStr.m_pData->GetRossHolder(m_RossId);
	else
		return &m_ExternalRossHolder;
}

string CRossDoc::GetLockFileName()  const
{
	return MakeFName(GetRossHolder()->m_DictPath,"lck").c_str();
}



void CRossDoc::RemoveLock() const
{
   try 
   {
		string FileName = GetLockFileName();
		if (access(FileName.c_str(), 0) != -1)
			CFile::Remove(FileName.c_str());
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
			m_Author = "guest";
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
/////////////////////////////////////////////////////////////////////////////
// CRossDoc serialization


void CRossDoc::Serialize(CArchive& ar)
{

	m_FirstLoadReadonly =  ((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly; 
	CString S = SerializeInner(ar);
	BuildBasicDomItems();
	char Login[LoginSize];
	strcpy (Login, (const char*) S);
	if (strlen(Login) == 0)
		strcpy (Login, "guest");


	try {
		if (!ar.IsStoring())
			if (!m_IsDefault)
				if (!m_ReadOnly)
				{
					size_t i =0;
					for (; i < 3; i++)
					{
						if (   !InputBox("Input your login (\"guest\" - for ReadOnly):", Login, LoginSize-1, "Password Dialog", AfxGetApp()->m_pMainWnd)
							|| (strlen (Login) == 0)) 
						{
							AfxMessageBox("Your must enter your login!");
						}
						else
						{

							char* q = strtok (Login," \t");
							if (!stricmp (q, "guest"))
							{
								MakeReadOnly();
								break;
							};

							size_t k =0;
							for (; k < m_Authors.size(); k++)
								if (!stricmp (q, m_Authors[k]))
								{
									m_Author = m_Authors[k];

									break;
								};

							if (k !=  m_Authors.size()) break;
						};

						if (i < 2) AfxMessageBox ("Your login is incorrect");
					};

					if (i==3)
					{
						AfxMessageBox ("Cannot find your login in loginlist. Logging as guest...");
						MakeReadOnly();
						return;
					};
				}
				else
					m_Author = Login;
	}
	catch (...)
	{
		AfxMessageBox ("An exception while entering login");
		return;
	};

}

bool GetLine(const char* Buffer, int& CurrOffset, int BufferLength, CString& S)
{
	bool bSpace = true;
	for (int i = CurrOffset; i < BufferLength+1; i++)
	 if (    (i == BufferLength)
		  || (Buffer[i] == '\n')
		)
	 {
		 if (bSpace)
		 {
			 continue;
		 };
		 char part[1000];
		 assert (i - CurrOffset < 1000);
		 strncpy(part,Buffer+CurrOffset, i - CurrOffset);
		 part[i-CurrOffset] = 0;
		 S = part;
		 if (S.GetLength()> 1)
			 if (S[S.GetLength() -1] == '\r')
				 S.Delete(S.GetLength() -1);
		 S.TrimLeft();
		 S.TrimRight();
 		 CurrOffset = i+1;
		 return true;
	 }
	 else
		 bSpace = bSpace && isspace((unsigned char)Buffer[i]);
	 
	 return false;

};


void CreateLockFile(CString FileName)
{
	FILE* fp = fopen (FileName, "wb");
	try {
        string strPath = GetRegistryString("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName\\ComputerName");
        fprintf (fp, "MachineName = %s \r\n", strPath.c_str());
		fprintf (fp,"Date = %s\r\n", CTime::GetCurrentTime().Format( "%A, %B %d, %Y " )); 
		fprintf (fp,"Time = %s\r\n", CTime::GetCurrentTime().Format( "%H : %M" )); 
		CString UserName = getenv("USERNAME");
        if (!UserName.IsEmpty())
			fprintf (fp, "UserName = %s \r\n", UserName);

	}
	catch (...) {
		// UNDER Windows 95/98 it doesn't work
        //AfxMessageBox("Cannot write lock file");
	};
	fclose(fp);

};

void CRossDoc::ReadConfig(CArchive& ar, CString& Login)
{
	m_bRussianFields = false;
	CFile* File;
	CString S;
	int FileLength = ar.GetFile()->GetLength();
	int CurrByte = 0;
	char* Buffer = new char [FileLength+1];
	ar.Read(Buffer, FileLength);
	int CurrBytes = 0;

	try {
		File = ar.GetFile();
		string FilePath = (const char*)File->GetFilePath();
		m_ReadOnly  =  (access(GetLockFileName().c_str(), 0) != -1) || m_FirstLoadReadonly;


		//  чтение зоны авторов 
		m_IsDefault  = false;

		Login = "";
		GetLine(Buffer, CurrByte, FileLength,  S);
		m_Authors.clear();
		while (GetLine(Buffer, CurrByte, FileLength,  S))
		{
			// начинается новая зона 
			if (S[0] == '[')
				break;

			if (S.Find ("Last", 0) != -1)
			{
				S = S.Left(S.GetLength()- 4);
				S.TrimRight();
				Login =  S;
			};

			if (S.Find ("Default", 0) != -1)
			{
				S = S.Left(S.GetLength()- 7);
				S.TrimRight();
				Login = S;
				m_IsDefault = true;
			};

			if (S.GetLength() > LoginSize - 1)
				throw CExpc("The length of login cannot be more than 10!");

			m_Authors.push_back (S);
		};
	}
	catch (...)
	{
		string Mess = "Cannot read user zone "+File->GetFileName();
		AfxMessageBox (Mess.c_str());
	};


		// чтение зоны настроек доменов
	try
	{
		m_DomainParams.clear();

		if (S == "[DomensParams]")
			while (GetLine(Buffer, CurrByte, FileLength,  S))
			{
				// начинается новая зона 
				if (S[0] == '[')
					break;
				CDomainParam C;
				C.InitFromString(S);
				m_DomainParams.push_back(C);
			};

		m_OptionsComments.clear();
		if (S == "[Options]")
			while (GetLine(Buffer, CurrByte, FileLength,  S))
			{
				// начинается новая зона 
				if (S[0] == '[')
					break;
				size_t i = 0;


				if (S.Mid (0,2) == "//")
					m_OptionsComments.push_back(S);

				if (sscanf (S,"DoNotLock %u", &i) == 1)
				{
					m_DoNotLock = (i!=0) ? true : false;
				};

				if (sscanf (S,"ArticleInitTextMode %u", &i) == 1)
				{
					m_bArticleInitTextMode = (i!=0) ? true : false;
				};

				if (sscanf (S,"DoNotSaveLastUserName %u", &i) == 1)
				{
					m_bDoNotSaveLastUserName = (i!=0) ? true : false;
				};


				char s[200];

				if (sscanf (S,"Language  %s", s) == 1)
				{
					if (stricmp (s, "English") == 0)
						m_LanguageId = English;
					if (stricmp (s, "Russian") == 0)
						m_LanguageId = Russian;
				};

				if (S == "RussianFields")
				{
					m_bRussianFields = true;
				};

			};
		}
		catch (...)
		{
			string Mess = "Cannot read options zone "+File->GetFileName();
			AfxMessageBox (Mess.c_str());
		};
	delete Buffer;
}

void CRossDoc::WriteConfig(CArchive& ar)
{
	ar.WriteString("[Configuration]\r\n");
	for (size_t k =0; k < m_Authors.size(); k++)
	{
		ar.WriteString(m_Authors[k]);
		if (!stricmp (m_Author, m_Authors[k]))
			if (m_IsDefault)
				ar.WriteString(" Default");
			else
				if (!m_bDoNotSaveLastUserName)
					ar.WriteString(" Last");
		ar.WriteString("\r\n");
	};
	ar.WriteString("[DomensParams]\r\n");
	for (size_t k =0; k < m_DomainParams.size(); k++)
	{
		CString S;
		S.Format("%s %i %i\r\n", m_DomainParams[k].DomStr, m_DomainParams[k].Color, m_DomainParams[k].DropDownCount);
		ar.WriteString(S);

	};
	ar.WriteString("[Options]\r\n");
	CString Q;
	if (m_LanguageId == Russian)
		ar.WriteString("Language Russian\r\n");
	else
		ar.WriteString("Language English\r\n");

	if (GetRoss()->m_bRussianFields)
		ar.WriteString("RussianFields\r\n");



	Q.Format("DoNotLock %i\r\n", m_DoNotLock ?  1 : 0);
	ar.WriteString(Q);

	Q.Format("DoNotSaveLastUserName %i\r\n", m_bDoNotSaveLastUserName ?  1 : 0);
	ar.WriteString(Q);

	Q.Format("ArticleInitTextMode %i\r\n", m_bArticleInitTextMode ?  1 : 0);
	ar.WriteString(Q);

	for (long j=0;j < m_OptionsComments.size();j++)
		ar.WriteString(m_OptionsComments[j]);
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

CString CRossDoc::SerializeInner(CArchive& ar)
{
	if (ar.IsStoring())
	{
		
		if (!GetRoss()->Save())
			throw CExpc (Format ("Cannot save %s", GetRossHolder()->m_DictPath.c_str()));

		WriteConfig(ar);	
		return "";
	}
	else
	{       
		m_bSerialized = true;

		CString Login;
		string DictFileName = (const char*)ar.GetFile()->GetFilePath();  
		ReadConfig(ar, Login);
		CAllRossesHolder* Trans = GetSemBuilder().m_RusStr.m_pData;
		Trans->m_LastUpdateTime++;

		m_RossId = Trans->GetRegisteredRossId(DictFileName);
		GetRossHolder()->m_DictPath = DictFileName;
		GetRossHolder()->m_LastUpdateTime =  Trans->m_LastUpdateTime;

		try
		{
			GetRossHolder()->OpenRossHolder(DictFileName.c_str(), false);
			GetRoss()->m_bRussianFields = m_bRussianFields;
		}
		catch(...)
		{
			string error  = "Cannot load "+DictFileName+":"+GetRoss()->m_LastError;
			TRACE (error.c_str());
			throw CExpc( error );
			return "";
		};

		if (   !m_ReadOnly && !m_DoNotLock	)
			CreateLockFile(GetLockFileName().c_str());

		m_FirstLoadReadonly = false;

		return Login;
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
	CDocument::SetTitle(GetRossHolder()->m_DictName.c_str());
}


BOOL CRossDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_ReadOnly)
	{
		::MessageBox(0, "Cannot write: the dictionary is read only.", "Error", MB_OK);
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
	catch (CExpc c)
	{
       AfxMessageBox  (c.m_strCause.c_str() );
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
		GetRossHolder()->m_LastUpdateTime = ++GetSemBuilder().m_RusStr.m_pData->m_LastUpdateTime;
	}
};




void CRossDoc::BuildBasicDomItems()
{
	m_DomParamsPtr.clear();
	m_DomParamsPtr.resize(GetRoss()->m_Domens.size());
    for (size_t i=0; i<GetRoss()->m_Domens.size(); i++)
	{
		vector<CDomainParam>::iterator It = find(m_DomainParams.begin(), m_DomainParams.end(), 
			GetRoss()->m_Domens[i].DomStr);
		if (It == m_DomainParams.end())
			m_DomParamsPtr[i]  = ErrUChar;
		else
			m_DomParamsPtr[i]  = It - m_DomainParams.begin();

	};
  
 
	GetRoss()->UpdateConstDomens();

	for (int i=0; i<GetRoss()->GetDomItemsSize(); i++)
	{
		 BYTE DomNo  = GetRoss()->GetDomItemDomNo(i);

  	     if (		(GetRoss()->m_Domens[DomNo].Source == dsMetaText)
				|| (GetRoss()->m_Domens[DomNo].Source == dsExpres)
			 )
		 { 
			if (DomNo  == GetRoss()->FieldDomNo)
				continue;
			TBaseDomItem I;
			strcpy (I.ItemStr,GetRoss()->GetDomItemStr(i));
			I.DomNo = GetRoss()->GetDomItemDomNo(i);
			m_BasicDomItems.push_back(I);

		 };
	}
	sort (m_BasicDomItems.begin(), m_BasicDomItems.end());

	//  adding all fields to m_Fields
	for (int i=0; i < GetRoss()->Fields.size(); i++)
	{
		const CField& F = GetRoss()->Fields[i];
		m_Fields.push_back(F.FieldStr);
		if  (F.IsApplicToActant)
			for (size_t k=1; k < MaxNumAct+1; k++)
				m_Fields.push_back(Format("%s%i", F.FieldStr, k));

	};

	sort ( m_Fields.begin(), m_Fields.end() );

	

	GetRossHolder()->InitConsts();
};

void CRossDoc::DelTextDomains()
{
	size_t count = GetRoss()->GetDomItemsSize();
	vector<BYTE> DomNos;
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_RLE"));
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_COLLOC"));
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_ENGL"));
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_ABBR"));
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_EXM"));
	DomNos.push_back(GetRoss()->GetDomenNoByDomStr("D_THES"));


	size_t i = 0;
	try {
		for (; i<count; i++)
		{
			BYTE DomNo  = GetRoss()->GetDomItemDomNo(i);

			if (find (DomNos.begin(),DomNos.end(),DomNo) == DomNos.end()) continue;

			GetRoss()->DelDomItem (i); 
			count--;
			i--;
		};
	}
	catch (...)
	{
		AfxMessageBox ("an exception occured");
	}
	SetModifiedFlag();

}



const char* CRossDoc::GetGramFetFieldName() const
{

	if (GetRoss()->m_bRussianFields)
		return "ГХ";
	else
		return "GF";

};

const char* CRossDoc::GetReferenceFieldName() const
{

	if (GetRoss()->m_bRussianFields)
		return "ОТС";
	else
		return "REF";

};


const char* CRossDoc::GetEnglishFieldName() const
{

	if (GetRoss()->m_bRussianFields)
		return "АНГ";
	else
		return "ENG";

};
