// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "GraphanAndMorphanHolder.h"

bool ProcessHyphenWords(const CLemmatizer* lemmatizer, CGraphmatFile* piGraphmatFile)
{
    try
    {
        size_t LinesCount = piGraphmatFile->GetTokensCount();
        for (int LineNo = 1; LineNo+1 < LinesCount; LineNo++)
        {
            if	(		piGraphmatFile->HasDescr(LineNo, OHyp)
                           &&	(lemmatizer->GetLanguage() == piGraphmatFile->GetUnits()[LineNo - 1].GetTokenLanguage()

                                  // and if no single space was deleted between the first word and the hyphen
                                  &&	!piGraphmatFile->GetUnits()[LineNo-1].HasSingleSpaceAfter()

                                  &&	!piGraphmatFile->HasDescr(LineNo-1, OSentEnd)
                                  &&	!piGraphmatFile->HasDescr(LineNo, OSentEnd)

                                  // it is not possible to create a hyphen word when this part of the text has an intersection
                                  // with an oborot which is marked as "fixed" in the dictionary
                                  &&	!piGraphmatFile->HasDescr(LineNo, OFixedOborot))
                           &&	!piGraphmatFile->HasDescr(LineNo-1, OFixedOborot)
                    )
            {
                //  creating a concatenation if it possible
                size_t NextWord = piGraphmatFile->PSoft(LineNo+1, LinesCount);
                if (NextWord == LinesCount) continue;

                // it is not possible to create a hyphen word  when this part of the text has an intersection
                // with an oborot which is marked as "fixed" in the dictionary
                if (piGraphmatFile->HasDescr(NextWord, OFixedOborot)) continue;

                if (lemmatizer->GetLanguage() != piGraphmatFile->GetUnits()[NextWord].GetTokenLanguage()) continue;
                std::string HyphenWord = piGraphmatFile->GetToken(LineNo-1)+"-"+piGraphmatFile->GetToken(NextWord);

                if (lemmatizer->IsInDictionary(HyphenWord, !piGraphmatFile->HasDescr(LineNo-1,OLw)))
                {
                    // uniting words LineNo-1, LineNo,  and NextWord
                    piGraphmatFile->MakeOneWord(LineNo-1, NextWord+1);
                    //  update LinesCount
                    LinesCount = piGraphmatFile->GetTokensCount();
                };
            };

        };

    }
    catch (...)
    {
        return false;
    }
    return true;

};


CGraphanAndMorphanHolder::CGraphanAndMorphanHolder()
{
	m_pLemmatizer = 0;
	m_pGramTab = 0;
    m_bUsePrediction = true;
};

CGraphanAndMorphanHolder::~CGraphanAndMorphanHolder()
{
	DeleteProcessors();
};


void CGraphanAndMorphanHolder::LoadGraphanAndLemmatizer(MorphLanguageEnum langua)
{
	DeleteProcessors();
	m_Graphan.FreeDicts();
    if (!m_Graphan.LoadDicts(langua))
	{	
		throw CExpc("Cannot load graphan");
	}
	LoadMorphology(langua);
	m_LemText.m_pLemmatizer = m_pLemmatizer;
	m_LemText.m_pGramTab = m_pGramTab;
};


bool CGraphanAndMorphanHolder::GetMorphology(std::string str, bool bFile, int& CountOfWords)
{
	clock_t t1,t2;
	CountOfWords = 0;

	try {
		// ============  Graphematics =======================
		t1 = clock();
		if (bFile) {
			m_Graphan.LoadFileToGraphan(str);
		}
		else {
			m_Graphan.LoadStringToGraphan(str);
		}

		IF_PLOG(plog::debug)
		{
			t2 = clock();
			size_t TokensCount = m_Graphan.GetTokensCount();
			for (int i = 0; i <TokensCount; i++)
				if (m_Graphan.GetUnits()[i].GetTokenLanguage() == m_CurrentLanguage )
						CountOfWords++;
			PLOGD << "CountOfWords: " << CountOfWords;
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			PLOGD <<  "Graphan: Ticks = " << t2 - t1 <<" Speed = " << speed;
		};

		// ============  Morphology =======================
		t1= clock();

		//m_Graphan.WriteGraphMat("graphan.gra");

		ProcessHyphenWords(m_pLemmatizer, &m_Graphan);

		m_LemText.CreateFromTokemized(&m_Graphan);

		IF_PLOG(plog::debug)
		{
			t2 = clock();
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			PLOGD << "Morphology: Ticks = " << t2 - t1 << " Speed = " << speed;
		};
		m_Graphan.FreeTable();
		return true;
	}
	catch (std::exception& e) {
		PLOGE << e.what();
		return false;	}
	catch (...)
	{
		PLOGE << "unknown exception in CGraphanAndMorphanHolder::GetMorphology";
		return false;
	};


};



