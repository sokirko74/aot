// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"




/*
 Правило построения фрагмента необособленного согласованного опредления
 типа "готовый ко всему мальчик".
 Ищет прилагательное, затем после него существительное,
 полностью согласованное с ним. Если нашло, 
 создает фрагмент, который начинается с прилагательного и заканчиватся
 перед существительным
*/

bool CRusSentence::RuleForFragNounGroup(int iClauseNum)
{

	int ig_W1, ig_W2;
	const CGroup* ig_Grp1; 
	const CGroup* ig_Grp2; 

	CClause* pClause1 = &GetClause(iClauseNum);
	int iWord = pClause1->m_iFirstWord;


	for (int WordNo = pClause1->m_iLastWord; WordNo >= pClause1->m_iFirstWord; WordNo--) 
	{
		// проверка на вложенную клаузу, нужно уменьшить индекс WordNo на длину вложенной клаузы
		int iEncloseCl = GetMinClauseByLastWord(WordNo);
		if		(			( iEncloseCl != -1 ) 
					&&		(	iEncloseCl !=  iClauseNum)
						//  если (	iEncloseCl ==  iClauseNum), значит вложенной  клаузы не  было найдено
						//  и мы находимся на последнем слове данной  iClauseNum
				)

		{
			assert (GetClause(iEncloseCl).is_part_of(*pClause1));

			WordNo = GetClause(iEncloseCl).m_iFirstWord;
			continue;
		};

		const CSynWord& MainStartingAdj = GetWords()[WordNo];
		if( MainStartingAdj.GetHomonymsCount() > 1 )
			continue;

		if ( pClause1->GetMaxLastHost(WordNo))
			continue;

	
		int iPartAdj, iPartPartic;
		iPartAdj = MainStartingAdj.GetHomonymByPOS(ADJ_FULL);
		iPartPartic = MainStartingAdj.GetHomonymByPOS(PARTICIPLE);

		//  если у данной клаузы только один вариант вершины - причастие, которое  может стать  стать НСО
		//  Например, "перечислив ряд, вовлеченных в движение  митьков",
		//  тогда  мы не имеем право создавать НСО, поскольку при создании новой подклаузы  будет удалена
		//  единственная  вершина данной  клаузы (в пример "вовлеченных"), и  клауза "вовлеченных в движение  митьков"
		//  останется без вершины.
		if (pClause1->m_vectorTypes.size() == 1)
			if (pClause1->m_vectorTypes[0].m_Root.m_WordNo == WordNo)
				continue;

        if (iPartAdj != -1 || iPartPartic != -1)
		{
		    int k;
			for(CSVI j =  pClause1->m_SynVariants.begin() ; j != pClause1->m_SynVariants.end(); j++ )
			{
				const CMorphVariant& V = *j;
				ig_W1 = ig_W2 = 0;
				ig_Grp1 = NULL;
				ig_Grp2 = NULL;
				for ( k = WordNo+1; k <= pClause1->m_iLastWord; k++ ) 
				{
					const CSynWord& w2 = GetWords()[k];

					if (w2.IsInOborot() ) continue;

					int UnitNo = pClause1->UnitNoByWordNo(k);
					assert (UnitNo != -1);
					// подклауза внутри НСО
					if (V.m_SynUnits[UnitNo].m_Type == EClause) continue;
					// "важные с точки зрения автора." (оборот внутри НСО)
					if (w2.m_Homonyms[V.m_SynUnits[UnitNo].m_iHomonymNum].m_bInOb)  continue;

					
					
					const CGroup* Grp = pClause1->GetLastHost(k, j);
					if (		w2.CanBeSynNoun() 
							&&	Grp == NULL 
						)
					{
					  if (ig_W1 == 0) 
					  {  
						ig_W1 = k;
						continue;
					  }
					  else
					  {
						ig_W2 = k;
						break;
					  }
					}
					else
					  if (   Grp != NULL 
						  && (   Grp->m_GroupType == NOUN_GR 
						      || Grp->m_GroupType == NOUN_ADJ 
							  || Grp->m_GroupType == NUMERAL_NOUN)
							 )

						if (ig_W1 == 0)
						{
							ig_W1 = Grp->m_iLastWord;
							ig_Grp1 = Grp;
							k = Grp->m_iLastWord;
							continue;
						}
						else
						{
							ig_W2 = Grp->m_MainWordNo;
							ig_Grp2 = Grp;
							break;
						}
					 else
					   if (Grp != NULL && ig_W1 == 0 && Grp->m_GroupType == PREP_NOUN)
						{
							ig_W1 = Grp->m_iLastWord;
							ig_Grp1 = Grp;
							k = Grp->m_iLastWord;
							continue;
						}
					 else
					   if (Grp != NULL && ig_W1 != 0 && Grp->m_GroupType == GEN_NOUN)
						{
							ig_W2 = Grp->m_MainWordNo;
							ig_Grp2 = Grp;
							break;
						}
					 else
					 {
						ig_W1 = 0;
					    break;
					 }				
				} //  end of for


				if (ig_W1 != 0 && ig_W2 != 0) //проверка условия;
				{
					const CSynWord& CoordinatingNoun = GetWords()[ig_W2];
					int iPartN =0;
					for (; iPartN < CoordinatingNoun.m_Homonyms.size(); iPartN++)
						if (CoordinatingNoun.m_Homonyms[iPartN].IsSynNoun())
							break;

					// во главе некоторых именных групп может стоять прилагательное. например, "все необходимое"
					// тогда у главного слов мы не найдем омоним существительного, будем проупскать такие конструкции
					if (iPartN == CoordinatingNoun.m_Homonyms.size()) 
						continue;
					
					if (iPartN != CoordinatingNoun.m_Homonyms.size())
					{
						int AdjPartNo =  (iPartAdj != -1) ? iPartAdj : iPartPartic;

						if(	GetRusGramTab()->GleicheGenderNumberCase(
								CoordinatingNoun.m_Homonyms[iPartN].m_CommonGramCode.c_str(),
								CoordinatingNoun.m_Homonyms[iPartN].GetGramCodes().c_str(),
								MainStartingAdj.m_Homonyms[AdjPartNo].GetGramCodes().c_str()) 
							)
						{
							SClauseType ClType(UNDETACHED_ADJ_PATIC, WordNo, 0);
							if(CreateEnclosedNotStrongClause(iClauseNum, WordNo, ig_W1, ClType))
								return true;
						}
					};
				}
			}
		}
	}
    return false;	
}
