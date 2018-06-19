// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"

bool IsEmptyOrInfinitive(const CClause* P)
{
	return		P->m_vectorTypes.empty() 
			||	P->HasType(INFINITIVE_T);
};

bool CRusSentence::RuleForSimilarNounGroupClauses(int iClauseNum, int MinCountOfSimilarMembers)
{
	CClause* pClauseFirst = &GetClause(iClauseNum);
	WORD ClauseFirstFirstWord =   pClauseFirst->m_iFirstWord;

	int iWord = pClauseFirst->m_iFirstWord;

	int iSecond = GetMaxClauseByFirstWord(pClauseFirst->m_iLastWord+1);
	if (iSecond == -1) return false; 

	CClause* pClauseSecond = &GetClause(iSecond);
	
	if ( IsWordClause(*pClauseFirst, pClauseFirst->m_iLastWord) ) return false;

	if (	! GetWords()[pClauseFirst->m_iLastWord].CanBeSynNoun()
		 &&  GetWords()[pClauseFirst->m_iLastWord].GetHomonymByPOS(ADV) == -1 //строить однородные наречия
		)
		return false;
	
	if ( pClauseSecond->HasLeftStarter() ) return false; 

	int CountOfNonPustych =  (IsEmptyOrInfinitive(pClauseFirst)?0:1) + (IsEmptyOrInfinitive(pClauseSecond)?0:1);
	//int CountOfNonPustych = 2;
	if (	//(CountOfNonPustych > 1)		 || 
			pClauseFirst->HasType(INP_T) 
		||  pClauseSecond->HasType(INP_T)
	   ) 
	  return false;
		
	CClause* pClauseThird;
	CPeriod  ClauseThirdPeriod;
	//int iThird  = -1;
	int iThird = GetMaxClauseByFirstWord(pClauseSecond->m_iLastWord+1);
	if ( iThird != -1 ) 
	{

		pClauseThird = &GetClause(iThird);
		
		//	приходится  отдельно сохранять координаты клаузы в переменную ClauseThirdPeriod, поскольку 
		//	ссылка на клаузу (pClauseThird) через некоторое время перестанет работать.
		
		ClauseThirdPeriod = (CPeriod)*pClauseThird;

		if ( pClauseThird->HasLeftStarter() ) iThird = -1;

		if ( pClauseThird->HasType(INP_T)) iThird = -1;		

		if	(		(CountOfNonPustych >= 1) 
				&&  !IsEmptyOrInfinitive(pClauseThird)
			)
			iThird = -1;

	     	 
	}
		
	// создаем пробную клаузу
	CClause prClause(pClauseFirst->m_pSent, pClauseFirst->m_iFirstWord, -1);
	if (-1 == iThird) 
		prClause.m_iLastWord = pClauseSecond->m_iLastWord;
	else
		prClause.m_iLastWord = pClauseThird->m_iLastWord;
	int dummi = 100;

	CRusFormatCaller FormatCaller(GetOpt());
	

	FormatCaller.AddSimilarNounGroupRules();

	pClauseFirst->m_bIgnoreWhileBuildingSynVariants	 = true;
	pClauseSecond->m_bIgnoreWhileBuildingSynVariants	 = true;
	if (-1 != iThird) 
		pClauseThird->m_bIgnoreWhileBuildingSynVariants	 = true;

	prClause.BuildGroups(FormatCaller,  false);

	pClauseFirst->m_bIgnoreWhileBuildingSynVariants	 = false;
	pClauseSecond->m_bIgnoreWhileBuildingSynVariants	 = false;
	if (-1 != iThird) 
		pClauseThird->m_bIgnoreWhileBuildingSynVariants	 = false;
	
	
	for (CSVI it = prClause.m_SynVariants.begin(); it!=prClause.m_SynVariants.end(); it++)
    {
        const CGroup*  it_second = NULL;
		for (size_t i=0; i< it->m_vectorGroups.GetGroups().size(); i++)
		{
			const CGroup& group = it->m_vectorGroups.GetGroups()[i];

			//  it_second будет  указывать на максимальную группу ОДНОР_ИГ, которая была создана  на границе двух клауз
			if( (group.m_iFirstWord <= pClauseFirst->m_iLastWord) && (group.m_iLastWord > pClauseSecond->m_iFirstWord))
				if (group.m_GroupType == SIMILAR_NOUN_GROUPS)
					it_second = &group;

			
			// если нашли группу, которая покрывает все три клаузы, надо все три клаузы объединить 
			if ( -1 != iThird )
				if	(		(group.m_iFirstWord <= pClauseFirst->m_iLastWord) 
						&&	(group.m_iLastWord > ClauseThirdPeriod.m_iFirstWord) 
					)
				{
					UniteClauses( iSecond, iThird, RightClauseParams);

					iClauseNum = GetMaxClauseByFirstWord(ClauseFirstFirstWord);
					if ( -1 == iClauseNum ) return true; 
					pClauseFirst = &GetClause(iClauseNum);
					iSecond = GetMaxClauseByFirstWord(pClauseFirst->m_iLastWord+1);
					if (iSecond == -1) return true; 
					pClauseSecond = &GetClause(iSecond);
					
					ParametersSourceEnum iwhose_chars_to_copy;
					if ( !pClauseFirst->m_vectorTypes.empty() && !pClauseFirst->HasType(INFINITIVE_T) )
						iwhose_chars_to_copy = LeftClauseParams;
					else if ( !pClauseSecond->m_vectorTypes.empty() && !pClauseSecond->HasType(INFINITIVE_T) )
						iwhose_chars_to_copy = RightClauseParams;
					else if ( pClauseFirst->m_vectorTypes.size() > pClauseSecond->m_vectorTypes.size() )
						iwhose_chars_to_copy = LeftClauseParams;
					else iwhose_chars_to_copy = RightClauseParams;

					UniteClauses( iClauseNum, iSecond,  iwhose_chars_to_copy);

					return true;				
				}
			}

        	if ( it_second != NULL )
	        {
		        const CGroup& group = *it_second;
		        int irel_count = 0;
		        for ( int rr = 0; rr < it->m_vectorGroups.m_Relations.size(); rr++ )
                    if ( it->m_vectorGroups.m_Relations[rr].is_part_of(group))
			            if ( SIMILAR_NOUN_GROUPS == it->m_vectorGroups.m_Relations[rr].type )
				            irel_count++;
        		

		        if ( irel_count < MinCountOfSimilarMembers) 
		        {
			        // если была собрана группа не по винительному, родительному или им. падежу, тогда  неважно сколько членов она содержит,
			        // такая  группа - редкость, поэтому  по ней  нужно объединять клаузы
			        if (		(group.GetGrammems() & (1 << rNominativ))  
					        ||	(group.GetGrammems() & (1 << rAccusativ))  
					        ||	(group.GetGrammems() & (1 << rGenitiv))  
				        )
				        return false;
		        };

		        if (MinCountOfSimilarMembers == 2)
			        if (group.m_bCommaDelimited) 
				        return false;

		        ParametersSourceEnum iwhose_chars_to_copy;
		        if ( !pClauseFirst->m_vectorTypes.empty() && !pClauseFirst->HasType(INFINITIVE_T) )
			        iwhose_chars_to_copy = LeftClauseParams;
		        else if ( !pClauseSecond->m_vectorTypes.empty() && !pClauseSecond->HasType(INFINITIVE_T) )
			        iwhose_chars_to_copy = RightClauseParams;
		        else if ( pClauseFirst->m_vectorTypes.size() > pClauseSecond->m_vectorTypes.size() )
			        iwhose_chars_to_copy = LeftClauseParams;
		        else iwhose_chars_to_copy = RightClauseParams;

		        UniteClauses( iClauseNum, iSecond, iwhose_chars_to_copy);

		        return  true;
	        }

    }

		

	return false;
}



bool CRusSentence::RuleForSimilarNounGroupClauses2(int iClauseNum)
{
	return RuleForSimilarNounGroupClauses(iClauseNum, 2);
};

bool CRusSentence::RuleForSimilarNounGroupClauses3(int iClauseNum)
{
	return RuleForSimilarNounGroupClauses(iClauseNum, 3);
};
