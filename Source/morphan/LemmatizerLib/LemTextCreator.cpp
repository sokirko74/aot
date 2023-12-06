// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "LemTextCreator.h"

static bool ProcessHyphenWords(const CLemmatizer* lemmatizer, CGraphmatFile* piGraphmatFile)
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

CLemTextCreator::CLemTextCreator(MorphLanguageEnum l) : m_LemText(l)
{
	m_Language = l;
	if (!m_Graphan.LoadDicts(l))
	{
		throw CExpc("Cannot load graphan");
	}

};



bool CLemTextCreator::BuildLemText(std::string str, bool bFile, int& CountOfWords)
{
	auto lemmatizer = GetMHolder(m_Language).m_pLemmatizer;
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
				if (m_Graphan.GetUnits()[i].GetTokenLanguage() == m_Language )
						CountOfWords++;
			PLOGD << "CountOfWords: " << CountOfWords;
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			PLOGD <<  "Graphan: Ticks = " << t2 - t1 <<" Speed = " << speed;
		};

		// ============  Morphology =======================
		t1= clock();

		//m_Graphan.WriteGraphMat("graphan.gra");

		ProcessHyphenWords(lemmatizer, &m_Graphan);

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
		PLOGE << "unknown exception in CLemTextCreator::BuildLemText";
		return false;
	};


};



