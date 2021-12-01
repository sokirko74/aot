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
                           &&	(lemmatizer->GetLanguage() == piGraphmatFile->GetTokenLanguage(LineNo-1)

                                  // and if no single space was deleted between the first word and the hyphen
                                  &&	!piGraphmatFile->GetUnits()[LineNo-1].HasSingleSpaceAfter()

                                  &&	!piGraphmatFile->HasDescr(LineNo-1, OSentEnd)
                                  &&	!piGraphmatFile->HasDescr(LineNo, OSentEnd)

                                  // it is not possible to create a hyphen word when this part of the text has an intersection
                                  // with an oborot which is marked as "fixed" in the dictionary
                                  &&	!piGraphmatFile->StartsFixedOborot(LineNo))
                           &&	!piGraphmatFile->StartsFixedOborot(LineNo-1)
                    )
            {
                //  creating a concatenation if it possible
                size_t NextWord = piGraphmatFile->PSoft(LineNo+1, LinesCount);
                if (NextWord == LinesCount) continue;

                // it is not possible to create a hyphen word  when this part of the text has an intersection
                // with an oborot which is marked as "fixed" in the dictionary
                if (piGraphmatFile->StartsFixedOborot(NextWord)) continue;

                if (lemmatizer->GetLanguage() != piGraphmatFile->GetTokenLanguage(NextWord)) continue;
                std::string HyphenWord = piGraphmatFile->GetToken(LineNo-1)+"-"+piGraphmatFile->GetToken(NextWord);

                if (lemmatizer->IsInDictionary(HyphenWord,!piGraphmatFile->HasDescr(LineNo-1,OLw)))
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
	m_bTimeStatis = false;
    m_bUsePrediction = true;
};

CGraphanAndMorphanHolder::~CGraphanAndMorphanHolder()
{
	DeleteProcessors();
};


bool CGraphanAndMorphanHolder::LoadGraphanAndLemmatizer(MorphLanguageEnum langua)
{
	try
	{
		DeleteProcessors();
		m_Graphan.FreeDicts();
		m_Graphan.m_Language = langua;
        if (!m_Graphan.LoadDicts())
		{	
			ErrorMessage("Cannot load graphan\n");
			return false;
		}
        if (! LoadLemmatizer(langua) )
            return false;
        m_PlmLines.m_pLemmatizer = m_pLemmatizer;

        return true;
	}
	catch(...)
	{
		return false;
	};

};


bool CGraphanAndMorphanHolder::GetMorphology(std::string str, bool bFile, int& CountOfWords)
{
	clock_t t1,t2;
	CountOfWords = 0;

	try {
		// ============  Graphematics =======================
		if (m_bTimeStatis) t1= clock();
		if (bFile) {
			m_Graphan.LoadFileToGraphan(str);
		}
		else {
			m_Graphan.LoadStringToGraphan(str);
		}

		if (m_bTimeStatis) 
		{
			t2 = clock();
			size_t TokensCount = m_Graphan.GetTokensCount();
			for (int i = 0; i <TokensCount; i++)
				if (m_Graphan.GetTokenLanguage(i) == m_CurrentLanguage )
							CountOfWords++;
			fprintf (stderr,"CountOfWords = %i\n",CountOfWords);

			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Graphan: Ticks = %li Speed = %6.0f\n", t2-t1, speed );
		};

		// ============  Morphology =======================
		if (m_bTimeStatis) t1= clock();

		//m_Graphan.WriteGraphMat("graphan.gra");

		ProcessHyphenWords(m_pLemmatizer, &m_Graphan);

		if (!m_PlmLines.ProcessPlmLines(&m_Graphan))
		{
			fprintf (stderr, "  Cannot get morph. interpretation from Lemmatizer\n");;
			return false;
		};

		if (m_bTimeStatis) 
		{
			t2 = clock();
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Morphology: Ticks = %li Speed = %6.0f\n", t2-t1, speed );
		};
		m_Graphan.FreeTable();
		return true;
	}
	catch (CExpc e) {
		std::cerr << e.m_strCause << "\n";
		return false;	}
	catch (...)
	{
		return false;
	};


};



