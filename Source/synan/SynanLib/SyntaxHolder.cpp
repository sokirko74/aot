#include "SyntaxHolder.h"
#include "synan/MAPostLib/PostMorphInterface.h"

extern CPostMorphInteface* NewRussianPostMorph();
extern CPostMorphInteface* NewGermanPostMorph();


CSyntaxHolder::CSyntaxHolder(MorphLanguageEnum l) : CLemTextCreator(l)
{
	m_pPostMorph = 0;
};


CSyntaxHolder::~CSyntaxHolder()
{
	if (m_pPostMorph) delete m_pPostMorph;

};

void CSyntaxHolder::ClearHolder() {
	if (m_pPostMorph) {
		delete m_pPostMorph;
		m_pPostMorph = nullptr;
	}
	m_Synan.ClearSentences();
	m_Synan.ClearOptions();
}



void CSyntaxHolder::LoadSyntax()
{
	InitGraphan();

    assert (!m_pPostMorph);

	if (m_Language == morphRussian)
		m_pPostMorph = NewRussianPostMorph();
	else
		m_pPostMorph = NewGermanPostMorph();
		
	if (!m_pPostMorph)
	{
		throw CExpc("Cannot load postmorphology\n");				
	}

	m_Synan.CreateOptions(m_Language);

	if (m_Language == morphGerman)
	{
			m_Synan.SetEnableAllThesauri(false);
	}

	m_Synan.SetOborDic(m_Graphan.GetOborDic());
	m_Synan.SetLemmatizer(GetMHolder(m_Language).m_pLemmatizer);
	m_Synan.InitializeProcesser();
};


bool CSyntaxHolder::GetSentencesFromSynAn(std::string utf8str, bool bFile)
{
	try {
		m_Synan.ClearSentences();
		m_LemText.m_LemWords.clear();
		int CountOfWords;

		if (!BuildLemText(utf8str, bFile, CountOfWords))
			return false;;

        #ifdef _DEBUG
			m_LemText.SaveToFile("before.lem");
        #endif

		m_pPostMorph->ProcessData(m_LemText);

#ifdef _DEBUG
		m_LemText.SaveToFile("after.lem");
#endif
		
		if (!m_Synan.ProcessData(&m_LemText))
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
	auto gramtab = GetMHolder(m_Language).m_pGramTab;
	return gramtab->GetClauseNameByType(C.m_vectorTypes[ClauseTypeNo].m_Type);
};
