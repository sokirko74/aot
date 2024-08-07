// ==========  This file is under  LGPL, the GNU Lesser General Public License
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
  краткого прилагательного у словоформы  "права".
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
							if( GetRusGramTab()->GleicheSubjectPredicate( hom1.GetGramCodes().c_str(), hom.GetGramCodes().c_str()  ))
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
}
