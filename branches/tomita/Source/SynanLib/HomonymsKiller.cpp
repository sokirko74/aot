// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"


/*

  Эта функция для каждого омонима краткого прилагательного или причастия
  идет по всему предложению и ищет существительное или местоимение
  в им. падеже, согласованное с этим прилагательным.
  Если во всем предложении такого слова нет, тогда удаляем этот омоним.
  Например, во фразе "Петя получил права", будет уничтожен омоним 
  краткого прилагательного у словоформы "права".
*/

void CRusSentence::SolveAmbiguityUsingRuleForShortAdj()
{
	for(int i = 0 ; i < m_Words.size(); i++ )
	{
		CSynWord& word = m_Words[i];
		
		bool bAdj = false;
		if (AllHomonymsArePredicates(word)) continue;
        int iHom = word.GetHomonymByPOS(ADJ_SHORT);
        if (iHom != -1)
            bAdj = true;
        else
            iHom = word.GetHomonymByPOS(PARTICIPLE_SHORT);

		if (iHom != -1)
		{
			bool bFound = false;
			CSynHomonym& hom = word.GetSynHomonym(iHom);

			for(int j = 0 ; j < m_Words.size(); j++ )
			{
				if( j != i)
				{
					const CSynWord& word1 = m_Words[j];					
					for(int k = 0 ; k < word1.GetHomonymsCount() ; k++)
					{
						const CSynHomonym& hom1 = word1.GetSynHomonym(k);
						if (  hom1.IsSynNoun()  )
							if( GetRusGramTab()->GleicheSubjectPredicate( hom1.m_GramCodes.c_str(), hom.m_GramCodes.c_str()  ))
							{
								bFound = true;
								break;
							}
					}
				}
			}
			
			if( !bFound )
				if( bAdj )
					word.KillHomonymOfPartOfSpeech(ADJ_SHORT);
				else
					word.KillHomonymOfPartOfSpeech(PARTICIPLE_SHORT);
		}
	}
}



void CRusSentence::SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord)
{
    return;
	/*
	for(int POS = 0 ; POS < GetRusGramTab()->GetPartOfSpeechesCount(); POS++)
	if ( GetRusGramTab()->IsStrongClauseRoot(1 << POS) )
	{
		int StrongRootWordNo = FindWordWithOneHomonym(iFirstWord, iLastWord, POS);

		if (StrongRootWordNo == -1) continue;

		for(int j = iFirstWord ; j <= iLastWord ; j++ )
		{

			// аналитические  формы еще не собраны, мы не имеем  право убивать омонимы у вспом. глаголов
			if (	HasAnalyticalBe( m_Words[j]) ) continue;

			if( j == StrongRootWordNo ) continue;


			for(int k = 0 ; k < GetRusGramTab()->GetPartOfSpeechesCount(); k++)
			if ( GetRusGramTab()->IsStrongClauseRoot(1 << k)  && m_Words[j].HasPos(k))
			{
				if(k == VERB)  // Дима не хотел удалять омонимы на личные формы глаголы. Я не знаю, почему.
					continue;


				// // аналитические  формы еще не собраны, мы не имеем  право убивать омонимы у главной глагольной части 
                if(		( k == PARTICIPLE_SHORT )
						||	( k == ADJ_SHORT )
						||	( k == PREDK) 
					  )
                {
				    if(	HasAnalyticalBe( m_Words[StrongRootWordNo]) )
					continue;
                    //автомобиль должен  быть красив
                    if ( j  > 0 && HasAnalyticalBe(m_Words[j-1]) )
                        continue;
                }

                

				m_Words[j].KillHomonymOfPartOfSpeech(k);
			}

		}
		break;	
	}
*/
}
