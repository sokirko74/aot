#include "COMSyntaxHolder.h"


#define COM_TRY try {
#define COM_CATCH(msg) }  catch(...) { ErrorMessage(msg); return FALSE; }

CCOMSyntaxHolder::CCOMSyntaxHolder()
{
};

CCOMSyntaxHolder::~CCOMSyntaxHolder()
{
	DeleteProcessors();
};



BOOL CCOMSyntaxHolder::LoadSyntaxModule(MorphLanguageEnum langua)
{
	try
	{	
		m_piSentCollection = 0;
		HRESULT hr = m_piSentCollection.CreateInstance(__uuidof(SYNANLib::SentencesCollection));
		if( FAILED(hr) )
		{
			ErrorMessage("SynAn has crushed, maybe \"SynAn.dll\" is not registered.");
			return FALSE;
		}
		
		m_piSentCollection->SyntaxLanguage = langua;
		m_piSentCollection->SetLemmatizer(m_piLemmatizer);
		
		
		if (langua != morphRussian)
		{
			m_piSentCollection->EnableThesauri = FALSE;
		};

		m_piSentCollection->KillHomonymsMode = CoverageKillHomonyms;

    	
		hr = m_piSentCollection->InitializeProcesser();
		if( FAILED(hr) )
		{
			ErrorMessage("Can not load syntax dictionaries.");
			return FALSE;
		}
		
	}
	catch(...)
	{
		ErrorMessage("SynAn has crushed, maybe \"SynAn.dll\" is not registered.");
		return FALSE;
	}
	return TRUE;
};

BOOL CCOMSyntaxHolder::LoadSyntax(MorphLanguageEnum langua)
{
	HRESULT hr;
	try
	{
		m_piGraphan = 0;
		m_piGraphan.CreateInstance(__uuidof(GRAPHANLib::GraphmatFile));
		m_piGraphan->Language = langua;
		m_piGraphan->LoadDicts();

		m_piLemmatizer = 0;
		if (langua == morphRussian)
			hr = m_piLemmatizer.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerRussian));	
		else
			if (langua == morphGerman)
				hr = m_piLemmatizer.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerGerman));	
			else
				if (langua == morphEnglish)
					hr = m_piLemmatizer.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerEnglish));	
		if( FAILED(hr) )
		{
			ErrorMessage("CCOMSyntaxHolder", "MorphAn has crushed, maybe \"lemmatizer.dll\" is not registered.");
			return FALSE;
		}

		
		hr = m_piLemmatizer->LoadDictionariesRegistry();
		if( FAILED(hr) )
		{
			ErrorMessage("Can not load morphology dictionaries.");
			return FALSE;
		}

		m_pGramTab = 0;
		if (langua == morphRussian)
			hr = m_pGramTab.CreateInstance(__uuidof(AGRAMTABLib::RusGramTab));
		else
			if (langua == morphGerman)
				hr = m_pGramTab.CreateInstance(__uuidof(AGRAMTABLib::GerGramTab));
			else
				if (langua == morphEnglish)
					hr = m_pGramTab.CreateInstance(__uuidof(AGRAMTABLib::EngGramTab));

		if( FAILED(hr) )
		{
			ErrorMessage("MorphAn has crushed, maybe \"agramtab.dll\" is not registered.");
			return FALSE;
		}

		hr = m_pGramTab->Load();
		if( FAILED(hr) )
		{
			ErrorMessage("Can not load rusgramtab .");
			return FALSE;
		}
		
	}
	catch(...)
	{
		ErrorMessage("MorphAn has crushed, maybe \"lemmatizer.dll\" is not registered.");
		return FALSE;
	}

	try
	{	
		m_piAfterMorphPlmLines = 0;
		m_piBeforeSyntaxPlmLines = 0;
		
		if( FAILED( m_piMAPost.CreateInstance(__uuidof(MAPOSTLib::MAPost)) ) )
			ErrorMessage("Mapost has crushed, maybe \"Mapost.dll\" is not registered.");

		if (langua != morphEnglish)
			m_piMAPost->Init(langua, m_piLemmatizer, m_pGramTab);

	}
	catch(...)
	{
		ErrorMessage("Postmorphology has crushed, maybe \"MApost.dll\" is not registered.");
		return FALSE;
	}

	
	if (!LoadSyntaxModule(langua))
		return FALSE;

	m_CurrentLanguage = langua;

	return TRUE;
};

BOOL CCOMSyntaxHolder::BuildBeforeSyntax(string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults)
{
	m_piBeforeSyntaxPlmLines = 0;	
	m_piAfterMorphPlmLines = 0;
	HRESULT hr =  m_piAfterMorphPlmLines.CreateInstance(__uuidof(LEMMATIZERLib::PLMLineCollection));
	m_piAfterMorphPlmLines->AttachLemmatizer(m_piLemmatizer);

	m_piSentCollection->ClearSentences();
	string log_path;
	string FileName = "rossdev.log";
	try {
		
		log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		log_path += "/";
	}
	catch (...) {
	};
	if (bFile)
		if (!FileExists(str.c_str()))
		{
			ErrorMessage (Format("file %s  is not found", str.c_str()));
			return FALSE;
		};


	COM_TRY
		
		
		if( bFile )
			m_piGraphan->LoadFileToGraphan(_bstr_t(str.c_str()).copy());
		else
			m_piGraphan->LoadStringToGraphan(_bstr_t(str.c_str()).copy());
		

		

	COM_CATCH( "GraphAn has crushed.");

	
	

	COM_TRY
		m_piAfterMorphPlmLines->ProcessHyphenWords(m_piGraphan);


		m_piAfterMorphPlmLines->ProcessPlmLines(m_piGraphan);				
		if (bWriteIntermFiles)
			m_piAfterMorphPlmLines->SaveToFile(string(log_path+"before.lem").c_str()); 

		if (!bSaveIntermResults)
			m_piGraphan->FreeTable();

	
	COM_CATCH( "MorphAn has crushed.");

	COM_TRY
		if (m_CurrentLanguage == morphEnglish)
		{
			m_piBeforeSyntaxPlmLines = 0;
			m_piBeforeSyntaxPlmLines .CreateInstance(__uuidof(LEMMATIZERLib::PLMLineCollection));
			m_piBeforeSyntaxPlmLines->AttachLemmatizer(m_piLemmatizer);			
			m_piBeforeSyntaxPlmLines->CopyItems(m_piAfterMorphPlmLines);
		}
		else
			m_piBeforeSyntaxPlmLines = m_piMAPost->ProcessData(m_piAfterMorphPlmLines);
		if (bWriteIntermFiles)
			m_piBeforeSyntaxPlmLines->SaveToFile(string(log_path+"after.lem").c_str()); 

	COM_CATCH( "PostMorphology has crushed.");
	
	if (!bSaveIntermResults)
		m_piAfterMorphPlmLines->Clear();

	return TRUE;
};

BOOL CCOMSyntaxHolder::BuildSyntax(BOOL bSaveIntermResults)
{
	COM_TRY
		
		assert( !(m_piSentCollection == NULL) );
		HRESULT hr = m_piSentCollection->raw_ProcessData(m_piBeforeSyntaxPlmLines);
		if( FAILED(hr) )
		{	
			m_piBeforeSyntaxPlmLines = 0;
			ErrorMessage("SynAn has crushed.");
			return FALSE;
		}
		
		if (!bSaveIntermResults)
			m_piBeforeSyntaxPlmLines->Clear();
		
	

	COM_CATCH( "SynAn has crushed.")

	return TRUE;
};

BOOL CCOMSyntaxHolder::GetSentencesFromSynAn(string str, BOOL bFile, BOOL bWriteIntermFiles, BOOL bSaveIntermResults)
{
	if (!BuildBeforeSyntax(str, bFile, bWriteIntermFiles, bSaveIntermResults)) 
			return FALSE;

	return BuildSyntax(bSaveIntermResults);
}

void CCOMSyntaxHolder::DeleteProcessors()
{
	m_piLemmatizer = NULL;	
	m_pGramTab = NULL;	
	m_piSentCollection = NULL;
	m_piGraphan = 0;
	m_piAfterMorphPlmLines = 0;
	m_piBeforeSyntaxPlmLines = 0;
	m_piMAPost = 0;
};


string  GetClauseTypeDescr(MorphLanguageEnum	Language, const SYNANLib::IClausePtr& piClause, int ClauseRootNo) 
{
	if (ClauseRootNo == -1)
	{
		if (Language == morphRussian)
			return "осяршую";
		else
			return "EMPTY";
	}
	else
	{
		return (const char*)piClause->ClauseRoots[ClauseRootNo]->GetDescription();
	}
};
