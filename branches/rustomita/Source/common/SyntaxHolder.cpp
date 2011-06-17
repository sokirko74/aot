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




bool CSyntaxHolder::GetSentencesFromSynAn(string str, bool bFile)
{
	clock_t t1,t2;
	int CountOfWords;
	
	
	try {
		m_Synan.ClearSentences();
		ClearVector(m_PlmLines.m_Items);

		if (!GetMorphology(str, bFile, CountOfWords))
			return false;;

        #ifdef _DEBUG
		    m_PlmLines.SaveToFile("before.lem");
        #endif
		// ============  Postmorphology =======================

		CPlmLineCollection MapostPlmLines;
		if (m_bTimeStatis) t1= clock();

		if (!m_pPostMorph->ProcessData(&m_PlmLines, MapostPlmLines))
		{
			fprintf (stderr, "  Cannot process Mapost\n");
			return false;

		};;
		if (m_bTimeStatis) 
		{
			t2 = clock();
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Mapost: Ticks = %i Speed = %6.0f\n", t2-t1, speed );
		};


#ifdef _DEBUG
		MapostPlmLines.SaveToFile("after.lem");
#endif
		
		    
		// ============  Syntax =======================
		if (m_bTimeStatis) t1= clock();
		if (!m_Synan.ProcessData(&MapostPlmLines))
		{
			fprintf (stderr, "  Synan has crushed!\n");
			return false;
		};

		if (m_bTimeStatis) 
		{
			t2 = clock();
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Synan: Ticks = %i Speed = %6.0f\n", t2-t1, speed );
		};
		

		return true;
	}
	catch (...)
	{
		return false;
	};
}


string  CSyntaxHolder::GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const
{
	if (ClauseTypeNo == -1)
		return " ";
	assert (ClauseTypeNo < C.m_vectorTypes.size() );
	return m_pGramTab->GetClauseNameByType(C.m_vectorTypes[ClauseTypeNo].m_Type);
};
