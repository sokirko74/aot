#include "SyntaxHolder.h"
#include "../MAPostLib/PostMorphInterface.h"

extern CPostMorphInteface* NewRussianPostMorph(const CLemmatizer* RusLemmatizer, const CAgramtab* RusGramTab);
extern CPostMorphInteface* NewGermanPostMorph(const CLemmatizer* RusLemmatizer, const CAgramtab* RusGramTab);


CSyntaxHolder::CSyntaxHolder()
{
	m_pPostMorph = 0;
};


CSyntaxHolder::~CSyntaxHolder()
{
	if (m_pPostMorph) delete m_pPostMorph;

};


bool CSyntaxHolder::LoadSyntax(MorphLanguageEnum langua)
{
	try{
		LoadGraphanAndLemmatizer(langua);

        assert (!m_pPostMorph);

		if (langua == morphRussian)
			m_pPostMorph = NewRussianPostMorph(m_pLemmatizer, m_pGramTab);
		else
			m_pPostMorph = NewGermanPostMorph(m_pLemmatizer, m_pGramTab);
		
		if (!m_pPostMorph)
		{
			ErrorMessage ("Cannot load postmorphology\n");				
			return false;
		}

		m_Synan.CreateOptions(langua);

		if (langua == morphGerman)
		{
				m_Synan.SetEnableAllThesauri(false);
		}

		m_Synan.SetOborDic(m_Graphan.GetOborDic());
		m_Synan.SetLemmatizer(m_pLemmatizer);
		if (!m_Synan.InitializeProcesser())
		{
			fprintf (stderr, "Cannot load synan\n");				
			return false;
		};

		m_CurrentLanguage = langua;
		return true;

	}
	catch(...)
	{
		return false;
	};
};


bool CSyntaxHolder::RunMapost(CPlmLineCollection& mapostPlmLines)
{
	if (!m_pPostMorph->ProcessData(&m_PlmLines))
	{
		fprintf(stderr, "  Cannot process Mapost\n");
		return false;

	};
	mapostPlmLines.m_Items.clear();
	mapostPlmLines.m_pLemmatizer = m_PlmLines.m_pLemmatizer;
	m_pPostMorph->SwapResults(mapostPlmLines.m_Items);
	return true;
}

bool CSyntaxHolder::GetSentencesFromSynAn(std::string utf8str, bool bFile)
{
	try {
		m_Synan.ClearSentences();
		m_PlmLines.m_Items.clear();
		int CountOfWords;

		if (!GetMorphology(utf8str, bFile, CountOfWords))
			return false;;

        #ifdef _DEBUG
		    m_PlmLines.SaveToFile("before.lem");
        #endif

		CPlmLineCollection MapostPlmLines;
		if (!RunMapost(MapostPlmLines)) {
			return false;
		}

#ifdef _DEBUG
		MapostPlmLines.SaveToFile("after.lem");
#endif
		
		if (!m_Synan.ProcessData(&MapostPlmLines))
		{
			fprintf (stderr, "  Synan has crushed!\n");
			return false;
		};

		return true;
	}
	catch (...)
	{
		return false;
	};
}


std::string  CSyntaxHolder::GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const
{
	if (ClauseTypeNo == -1)
		return " ";
	assert (ClauseTypeNo < C.m_vectorTypes.size() );
	return m_pGramTab->GetClauseNameByType(C.m_vectorTypes[ClauseTypeNo].m_Type);
};
