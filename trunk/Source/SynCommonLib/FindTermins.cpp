// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "Sentence.h"



bool CSentence::EqToTermin(const CSynWord& Word,   const string& termin, const CSynPlmLine& word_scheme, int iHom) const
{
	
	assert(iHom < Word.m_Homonyms.size());
	const CSynHomonym& hom = Word.m_Homonyms[iHom];
	const string& strLemma = hom.m_strLemma;
	if( !strLemma.size() || !Word.m_strUpperWord[0] )
		return false;

	if (strLemma == termin)
	{
		
		return		WordSchemeEqForThesaurus(hom, word_scheme) 
            ||	(word_scheme.HasFlag(fl_dg_ch) && (Word.HasDes(ONumChar)));
	}
	else
		if (Word.m_strUpperWord == termin)
		{
            return !word_scheme.HasFlag(fl_is_russian) &&  Word.HasDes(OLLE);
		}
		else
			return false;
}


void CSentence::BuildWordsSegments(CIntPairVector& WordsSegments)
{
	WordsSegments.clear();
	if( m_vectorTermins.size() == 0 )
	{
		WordsSegments.push_back(CIntPair(0, m_Words.size() - 1));
		return;
	}
	
	
	sort(m_vectorTermins.begin(), m_vectorTermins.end());
	int ii = 0;
	for( int i = 0 ; i < m_vectorTermins.size() ; i++ )
	{
		if( (m_vectorTermins[i].m_iFirstWord - ii) > 1 )
			WordsSegments.push_back(CIntPair(ii, m_vectorTermins[i].m_iFirstWord - 1 ));

		if( i == m_vectorTermins.size() - 1)
		{
			if( (m_Words.size() - 1 - m_vectorTermins[i].m_iLastWord ) > 1 )
				WordsSegments.push_back(CIntPair(m_vectorTermins[i].m_iLastWord + 1, m_Words.size() - 1));
		}
		
		ii = m_vectorTermins[i].m_iLastWord + 1;
	}
}

static void SetAllOtherHomsDelIfNotGood(CSynWord& W, int iHom)
{
	for(int i = 0 ; i < W.m_Homonyms.size() ; i++ )
	{
		W.m_Homonyms[i].m_bDelete =  !W.m_Homonyms[i].m_bGoodHomonym && (i != iHom);
	}
}



bool CSentence::FindTerminsHelper(	CWordVector::iterator words, 
									CWordVector::const_iterator end_of_words, 
									CTermin::const_iterator termin, 
									CTermin::const_iterator end_of_termin, 
									CPlmVector::const_iterator word_scheme,  
									CPlmVector::const_iterator word_scheme_end,
									bool& bError ) 
{
	

	bError = false;
	if(termin == end_of_termin)
		return true;
	else	
		if(words == end_of_words  )
			return false;

	//assert(word_scheme != word_scheme_end );
	if( word_scheme == word_scheme_end )
	{
		bError = true;
		return false;
	}

	bool bWordEq = false;
	string str_word =  words->m_strUpperWord;
	
	// раньше считалось, что в оборотоах нет лемм ( а теперь они есть),
	// поэтому я добавил следующий if (Сокирко)
	int i = 0;
    if (		!IsInNonAtomicOborPairs(words - m_Words.begin()) ) 
	for(  ; i < words->GetHomonymsCount() ; i++ )
	{
		bWordEq = (*termin == words->m_strWord);
		
		if( EqToTermin(*words, *termin, *word_scheme, i) )
		{
			words->SetGoodHomonym(i);	
			SetAllOtherHomsDelIfNotGood(*words, i);
			words++;
			termin++;
			word_scheme++;
			return FindTerminsHelper(words, end_of_words, termin, end_of_termin, word_scheme, word_scheme_end,bError);
		}
	}
	
	//в термине с оборотом слов может быть меньше , чем в модели
	bool bIsInObor = false;	
	while(	   (termin != end_of_termin)
		    && (str_word == *termin) 
		    &&  IsInNonAtomicOborPairs(words - m_Words.begin()) 
			&&	(words	!= end_of_words)
		 )
	{
		bIsInObor = true;
		++words;
		++termin;
		//nim
	    if(termin == end_of_termin) return true;
		if ( words	== end_of_words ) return false;
		//nim
		str_word =  words->m_strUpperWord;	
	}

	if( bIsInObor )
		return FindTerminsHelper(words, end_of_words, termin, end_of_termin, ++word_scheme, word_scheme_end,bError);



	//если вдруг в термине есть какая-нибудь запятая или что-то, у чего не может быть омонимов, то сравниваем с m_strWord
	//пустая лемма м.б. у слов из оборотов
	int lem_len;
	const char* lemma;
	if(i == 1)
	{
		lemma = words->m_Homonyms[0].m_strLemma.c_str();
		lem_len = strlen(lemma);
	}
	else
		lem_len = -1;

	if( bWordEq && ((i == 0) ||  ((i > 0) && (lem_len == 0))))
		return FindTerminsHelper(++words, end_of_words, ++termin, end_of_termin, ++word_scheme, word_scheme_end, bError);
	
	return false;
}


static void ResetMarkedHomonymsForPeriod(CSentence& S, int i1, int i2)
{
	for(int i = i1 ; (i <= i2) && (i < S.m_Words.size()) ; i++ )
	{
		for(int k = 0 ; k < S.m_Words[i].m_Homonyms.size() ; k++ )
		{
				S.m_Words[i].m_Homonyms[k].m_bGoodHomonym = false;
				S.m_Words[i].m_Homonyms[k].m_bDelete = false;
		}

	};
}


static void DeleteMarkedHomonymsAndFIOForTermins(CWordVector& Words, size_t start, size_t end)
{
	for (size_t i=start; i < end; i++)
	{
		if( Words[i].GetHomonymsCount() > 1 )
			Words[i].DeleteMarkedHomonymsBeforeClauses();
		
		// delete FIO1 and FIO2 if we have found a thesaurus entry 
        if (Words[i].HasDes(OFAM1) )
		{
			size_t k=i;
			for (; k < Words.size(); k++)
				if (Words[k].HasDes(OFAM2))
					break;

			assert (k < Words.size());
			Words[i].DelDes(OFAM1);
			Words[k].DelDes(OFAM2);
		};


        if (Words[i].HasDes(OFAM2))
		{
			int k=i;
			for (; k >= 0; k--)
                if (Words[k].HasDes(OFAM1))
					break;

			assert (k >= 0);
            Words[k].DelDes(OFAM1);
            Words[i].DelDes(OFAM2);
		};
	}
}




void CSentence::FindTermins()
{

	const CThesaurusForSyntax& Thesaurus = m_pSyntaxOptions->GetThesaurus();
	const CVectorOfTermins* pTermins = Thesaurus.GetTermins();

	CTerminSort_less pred(pTermins);

	bool bFound = false;
	bool bFoundAtLeastOneTermin = false;
	CIntPairVector WordsSegments;
	BuildWordsSegments( WordsSegments );
	

	CInt2Vector::const_reverse_iterator iter;
	for(iter= Thesaurus.m_TerminsGrouppedByLength.rbegin();  iter !=Thesaurus.m_TerminsGrouppedByLength.rend(); iter++)
	{
		int TerminWordCount = 	iter->first;
		CIntVector& TerminsIndexes = const_cast<CIntVector&>(iter->second);
		//		printf ("search for termins (length = %i)  \n",TerminWordCount);

		
		CIntVector::iterator termin_index_iter;

		if( WordsSegments.size() == 0 )
			return;

		for(int n = 0 ; n < WordsSegments.size() ; n++)
		{
			for(int j = WordsSegments[n].first ; j <= WordsSegments[n].second ; j++ )
			{
				CSynWord& word = m_Words[j];
				for(int k = 0 ; k < word.m_Homonyms.size() ; k++ )
				{
					CSynHomonym& hom = word.m_Homonyms[k];					
					string strLemma = hom.m_strLemma;
					
					if( !strLemma.size() )
						continue;
					RmlMakeUpper(strLemma, GetOpt()->m_Language);
					pred.SetStringToCompare(strLemma.c_str());					

					// Ищем термины, которые начинаются со слова strLemma
					termin_index_iter = lower_bound(TerminsIndexes.begin(), TerminsIndexes.end(), -1, pred);

					//  если не нашлось ни одного термина, который  начинается с этой леммы, переходим к след. омониму
					if(termin_index_iter == TerminsIndexes.end()) continue;

					const char* debug_current_termin  = (*pTermins)[*termin_index_iter].m_strTermin;
					//printf (" check termin  %s \n", debug_current_termin);

					const CGroups* pModel = (*pTermins)[*termin_index_iter].m_pModel;

					// идем по всем словам, которые начинаются с леммы  "strLemma"
					while( EqToTermin(word, (*pTermins)[*termin_index_iter][0], pModel->sent[0], k) )
					{
						
						const char* debug_current_termin  = (*pTermins)[*termin_index_iter].m_strTermin;
						

						if( TerminWordCount > 1 )
						{
							for (int kk = 0; kk < word.m_Homonyms.size() ; kk++ )
								if (word.m_Homonyms[kk].IsOb1() || word.m_Homonyms[kk].IsOb2())
									word.SetGoodHomonym(kk);

							word.SetGoodHomonym(k);	
							SetAllOtherHomsDelIfNotGood(word, k);	
						}
						bool bError;
						if( FindTerminsHelper(m_Words.begin() + j , //начало отрезка слов, где искать
												m_Words.begin() + WordsSegments[n].second + 1, //его конец
												(*pTermins)[*termin_index_iter].begin() , // начало цепочки слов, входящих в термин
												(*pTermins)[*termin_index_iter].end(), // ее конец
												pModel->sent.begin() , //начало цепочки, в которой хранятся грам. инфо. про слово в термине
												pModel->sent.end() , // её конец
												bError))
						{
							bFound = true;
							bFoundAtLeastOneTermin = true;
							//printf ("Found Termin %s\n", debug_current_termin);
							break;
						}
						else
						{
							ResetMarkedHomonymsForPeriod(*this, j, j + TerminWordCount - 1);
							if( bError )
							{
								char strBuf[500];
								sprintf(strBuf,"Termin \"%s\" with id %d maybe has bad model!", (*pTermins)[*termin_index_iter].GetTerminStr().c_str(),(*pTermins)[*termin_index_iter].m_TerminId);
								OutputErrorString(strBuf);
							}
						}
						
						if(++termin_index_iter == TerminsIndexes.end())
							break;

						pModel = (*pTermins)[*termin_index_iter].m_pModel;
					}

					if(bFound)
						break;
				} //  цикл по омонимам 

				if( bFound )
				{
					const CTermin& T = (*pTermins)[*termin_index_iter];
					rml_TRACE("Found termin \"%s\"\n", T.m_strTermin);
					m_vectorTermins.push_back( SFoundTermin(T.m_pModel, T.m_ThesType, T.m_TerminId, j, j + TerminWordCount - 1) );
					if (TerminWordCount > 1)
						DeleteMarkedHomonymsAndFIOForTermins(m_Words, j, j + TerminWordCount);
					else
						ResetMarkedHomonymsForPeriod(*this, j, j + TerminWordCount - 1);

					j += TerminWordCount - 1;
					bFound = false;
				}
			}
		}

		if( bFoundAtLeastOneTermin )
		{
			BuildWordsSegments(WordsSegments);
			bFoundAtLeastOneTermin = false;
		}
	}
}


void CSentence::FindAllTermins()
{
	try
	{
		FindTermins();
	}
	catch(...)
	{
		OutputErrorString("Can't find termins");
	}


}



void CSentence::InitWordsInTermins()
{
	for(int iTermin = 0 ; iTermin < m_vectorTermins.size() ; iTermin++ )
	{
		const SFoundTermin& foundTermin = m_vectorTermins[iTermin];
		
		for(int iW = foundTermin.m_iFirstWord ; iW <= foundTermin.m_iLastWord ; iW++ )
		{
			m_Words[iW].m_bFirstWordInTermin = (iW == foundTermin.m_iFirstWord);
			m_Words[iW].m_bLastWordInTermin = (iW == foundTermin.m_iLastWord);
			m_Words[iW].m_iTerminID = foundTermin.m_TerminId;
			m_Words[iW].m_ThesType = foundTermin.m_ThesType;
			m_Words[iW].m_bInTermin = true;
		}	
	}
}
