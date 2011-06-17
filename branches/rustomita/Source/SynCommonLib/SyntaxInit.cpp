// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma warning(disable:4786)

#include  "stdafx.h"
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <assert.h>

#include  "SyntaxInit.h"

extern COborDic* NewOborDicGerman (CSyntaxOpt* Opt);
extern COborDic* NewOborDicRussian (CSyntaxOpt* Opt);
extern COborDic* NewOborDicEnglish (CSyntaxOpt* Opt);
extern CThesaurusForSyntax* NewThesaurusForSyntaxRussian (CSyntaxOpt* Opt);
extern CThesaurusForSyntax* NewThesaurusForSyntaxGerman (CSyntaxOpt* Opt);
extern CSentence*	NewSentenceGerman (const CSyntaxOpt* pSyntaxOptions);
extern CSentence*	NewSentenceEnglish (const CSyntaxOpt* pSyntaxOptions);
extern CSentence*	NewSentenceRussian (const CSyntaxOpt* pSyntaxOptions);


CSyntaxOpt :: CSyntaxOpt (MorphLanguageEnum Langua) 
{	
	m_bSilentMode = false;
	m_KillHomonymsMode = CoverageKillHomonyms;
	m_SynAnLogFile = 0;
	m_pOborDic = 0;
	m_Language = Langua;
	m_pThesaurus = 0;
	m_bEnableFinThesaurus = true;
	m_bEnableCompThesaurus = true;
	m_bEnableOmniThesaurus = true;
	m_bEnableLocThesaurus = true;
    m_pProfessions = 0;
	
}

bool CSyntaxOpt :: IsValid() const
{
	return (m_pOborDic != 0) && (GetGramTab() != 0);
};


void CSyntaxOpt :: DestroyOptions ()
{
	if (m_pOborDic) delete m_pOborDic;
	if (m_pThesaurus) delete m_pThesaurus ;
   	if(m_pProfessions)
		delete m_pProfessions;

};




bool CSyntaxOpt::ReadListFile   (const string& FileName, StringVector& C)
{

	FILE* fp  = fopen(FileName.c_str(),"r");
	if( !fp )
	{
		char strMsg[200];
		sprintf(strMsg, "Can't open file \'%s\'.", FileName.c_str());
		OutputErrorString(strMsg);
		return false;
	}

	char buffer[500];

	while  (fgets (buffer,500,fp))
	{
		string s = buffer;
		int i = s.find("//");
		if (i != string::npos)
			s.erase(i);
		Trim(s);

		if (!s.empty())
			C.push_back (s);
	};

	fclose (fp);   

	sort (C.begin(), C.end());

	return true;

};



  
void CSyntaxOpt::OpenLogFile()
{
	/*if (!m_SynAnLogFile)
		m_SynAnLogFile = fopen("syn_err.log", "w");*/
}

void CSyntaxOpt::CloseLogFile()
{
	/*if (m_SynAnLogFile)
	{
		fclose (m_SynAnLogFile);
		m_SynAnLogFile = 0;
	};*/
}

void CSyntaxOpt::OutputErrorString(string strMsg) const
{
	if (!m_bSilentMode)
		ErrorMessage("Synan",strMsg);
}




bool GetRegString(string key, string& value)
{
	try
	{
		value = GetRegistryString(key);	
		if (value.empty())
			throw  CExpc("");
	}
	catch(CExpc& )
	{
		char strMsg[400];
		strcpy(strMsg,"Failed to read registry entry ");
		strcat(strMsg, key.c_str());		
		ErrorMessage("SynAn", strMsg);
		return false;
	}

	return true;
}





bool CSyntaxOpt::LoadTerminsForOneThesaurus(const char* ThesName)
{
	_share_pointer_t<const CThesaurus*>& P = GetThesPointerByThesId(GetThesTypeByStr(ThesName));

	if (P.m_Pointer == 0)
		P.SetPointer(m_pThesaurus->LoadThesaurus(ThesName), true);

	if (!P.m_Pointer) return false;


	return m_pThesaurus->ReadThesaurusForSyntax(ThesName, P.m_Pointer, m_pProfessions->m_vectorDatItems);
};

bool CSyntaxOpt::LoadTermins(const CDictionary* piOborDic)
{
	try
	{
	
		if (m_bEnableLocThesaurus)
			LoadTerminsForOneThesaurus("RML_THES_LOC");

		if( m_bEnableFinThesaurus )
			LoadTerminsForOneThesaurus("RML_THES_FIN");

		if( m_bEnableCompThesaurus )
			LoadTerminsForOneThesaurus("RML_THES_COMP");

		if( m_bEnableOmniThesaurus )
			LoadTerminsForOneThesaurus("RML_THES_OMNI");

		m_pThesaurus->SortIndexes();

	}
	catch(...)
	{
		return false;
	}
return true;
}
  


const char g_strRusRegOborDicPath[] = "Software\\Dialing\\Obor\\DictPath"; 
const char g_strGerRegOborDicPath[] = "Software\\Dialing\\GerObor\\DictPath"; 

bool CSyntaxOpt :: InitializeOptions()
{
	if( m_bSilentMode )
		OpenLogFile();				

    m_pProfessions = new SDatItems(0xffff);	 //NOUN
	//loading agramtab

	try
	{
		if (!GetGramTab())
		{
			CAgramtab* A = GetNewGramTab();
			if (!A) return false;
			if (!A->LoadFromRegistry()) return false;
			SetGramTab(A, true);
		}
	}
	catch(...)
	{
		OutputErrorString("Failed to load \"agramtab.dll\"");
		return false;			
	}		

	if (GetLemmatizer() == 0) 
	{
		CLemmatizer* R;
		if (m_Language == morphRussian)
			R = new CLemmatizerRussian;
		else
			if (m_Language == morphEnglish)
				R = new CLemmatizerEnglish;
		  else
				R = new CLemmatizerGerman;

		if (!R) return false;
		string strError;
		if (!R->LoadDictionariesRegistry(strError) )  return false;
		//R->m_bUsePrediction = false;
		SetLemmatizer(R, true);
	};


		//loading obor.dic
	try
	{
		assert (m_pOborDic == 0);

		if (m_Language == morphRussian)
			m_pOborDic = NewOborDicRussian(this);
		else
			if (m_Language == morphEnglish)
				m_pOborDic = NewOborDicEnglish(this);
			else
				m_pOborDic = NewOborDicGerman(this);

		
		//  Инициализация словаря оборотов, если он не было вызвано 
		//  функции SetOborDic.
		string strPath;
		if (m_piOborDictionary.m_Pointer == 0)
		{
			CDictionary* D = new CDictionary;
			if( !D )
			{
				OutputErrorString("Failed to load \"StructDict.dll\"");
				return false;
			}

			
			if (m_Language == morphRussian)
				strPath = GetRegistryString( g_strRusRegOborDicPath );
			else
				strPath = GetRegistryString( g_strGerRegOborDicPath );

			D->Load(strPath.c_str());

			m_piOborDictionary.SetPointer( D , true);
		}


		if( !m_pOborDic->ReadOborots(m_piOborDictionary.m_Pointer) )
		{						
			OutputErrorString("Failed to load \"StructDict.dll\"");	
			return false;
		}

	}

	catch(CExpc& )
	{
		OutputErrorString("Failed to find registry entry for oborot dictionary");
		return false;			
	}
	catch(...)
	{			
		OutputErrorString("Failed to load \"StructDict.dll\" (Obor.dic)");	
		return false;
	}

	
	
	//loading thesauruses
	if (!m_pThesaurus) delete m_pThesaurus;
	m_pThesaurus = 0;
	if (m_Language == morphRussian)
		m_pThesaurus = NewThesaurusForSyntaxRussian(this );
	else
		m_pThesaurus = NewThesaurusForSyntaxGerman(this);

	if( !LoadTermins(m_piOborDictionary.m_Pointer) )
		OutputErrorString("Failed to load Thesaurus");

	if( !InitOptionsLanguageSpecific() )
		return false;

	return true;
};

bool has_item (const StringVector* C, const char* item) 
{    
	if (!item) return false;
	return  binary_search(C->begin(), C->end(), string(item));
};




CSentence* CSyntaxOpt::NewSentence () const
{
	if (m_Language == morphGerman)
		return NewSentenceGerman(this);

	if (m_Language == morphEnglish)
		return NewSentenceEnglish(this);
	
	return NewSentenceRussian(this);
};

_share_pointer_t<const CThesaurus*>& CSyntaxOpt::GetThesPointerByThesId (UINT ThesId)
{
	switch (ThesId) {
		case LocThes : return  m_LocThes;
		case FinThes : return  m_FinThes;
		case CompThes : return  m_CompThes;
		case OmniThes : return  m_OmniThes;
	};
	return m_LocThes;

};
const CThesaurus* CSyntaxOpt::GetThesByThesId (UINT ThesId)
{
	return GetThesPointerByThesId(ThesId).m_Pointer;
};


const char*	CSyntaxOpt::GetGroupNameByIndex(long lType) const
{
	if (lType < 0) return 0;
	if (lType >= m_SyntaxGroupTypes.size()) return 0;
	return m_SyntaxGroupTypes[lType].c_str();
}


int CSyntaxOpt::GetGroupTypebyName(const char* TypeName) const
{

	if (!TypeName) return false;

	for(int i = 0 ; i < m_SyntaxGroupTypes.size(); i++ )
	{
		if (m_SyntaxGroupTypes[i] ==  TypeName)
		{
			return i;
		}
	}
	return -1;
	
}

int CSyntaxOpt::GetSyntaxGroupOrRegister(const char* TypeName) const
{

	int i = GetGroupTypebyName(TypeName);
	if (i != -1)
		return i;

	m_SyntaxGroupTypes.push_back(TypeName);

	return m_SyntaxGroupTypes.size() - 1;
	
}



