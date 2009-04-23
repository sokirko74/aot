// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"



bool CRusSentence::CheckCoordinationOfNounAndParticiple(const CSynHomonym& Participle, const CClause* pClause, size_t NounWordNo, size_t NounHomonymNo) const
{
	const CSynHomonym& Noun  = m_Words[NounWordNo].GetSynHomonym(NounHomonymNo);
	
	// мальчик, ушедший домой, пришел
	if(	 GetRusGramTab()->GleicheGenderNumberCase(Noun.m_CommonGramCode.c_str(), Noun.m_GramCodes.c_str(), Participle.m_GramCodes.c_str()) )
		return true;

	// мальчик и девочка, ушедшие домой, пришли
	CSVI Dummy;
	if (	(pClause->IsInThisGroup(NounWordNo, SIMILAR_NOUN_GROUPS, Dummy) != NULL)
		&&  GetRusGramTab()->GleicheCase(Participle.m_GramCodes.c_str(), Noun.m_GramCodes.c_str())
	   )
	   return true;

	//Два дома ,построенных на холме, постепенно разрушались.
	const CGroup*  pNumerNounGroup =  pClause->IsInThisGroup(NounWordNo, NUMERAL_NOUN, Dummy);
	if (	( pNumerNounGroup != NULL) 
		&&  ( pNumerNounGroup->GetGrammems() & _QM(rNominativ) )
		&&  ( Participle.HasGrammem(rGenitiv) )
		&&  ( Participle.HasGrammem(rPlural) )
	   )
	   return true;
					 
	return false;
};

bool CRusSentence::FindNounFromTheRightOfParticiple(const CClause* pClause, const CSynHomonym& Participle) const
{
	int iType = pClause->FindType(PARTICIPLE_T);
	assert ( -1 != iType );

	// начинаем идти от причастия вправо
	for(int WordNo = pClause->m_vectorTypes[iType].m_Root.m_WordNo + 1 ; WordNo <= pClause->m_iLastWord ; WordNo++ )
	{
		const CSynWord& pWord = m_Words[WordNo];

		// проверка на вложенную клаузу, нужно увеличить индекс WordNo на длину вложенной клаузы
		int iEncloseCl = GetMinClauseByFirstWord(WordNo);
		if ( iEncloseCl != -1 )
		{
			assert (GetClause(iEncloseCl).is_inside_of(*pClause));

			WordNo = GetClause(iEncloseCl).m_iLastWord;
			continue;
		}
		
		CSVI Dummy;
		if ( pClause->IsInThisGroup(WordNo, PREP_NOUN, Dummy) ) continue;

		for(int j = 0 ; j < pWord.m_Homonyms.size() ; j++ )
		{
			const CSynHomonym& Noun = pWord.m_Homonyms[j];

			if( !GetRusGramTab()->IsMorphNoun(Noun.m_iPoses) ) continue;

			if (CheckCoordinationOfNounAndParticiple(Participle, pClause, WordNo, j))
				return true;
		}
	}

	return false;
}


int CRusSentence::FindNounFromTheLeftOfParticiple(const CClause* pClauseWithParticiple, const CSynHomonym& Participle) const
{
	
	// (причастный оборот должен быть отделен запятой)
	bool bFoundComma = m_Words[pClauseWithParticiple->m_iFirstWord].m_bComma;

	// начинаем идти по клаузе налево
	for(int WordNo = pClauseWithParticiple->m_iFirstWord ; WordNo >= 0; WordNo-- )
	{
		const CSynWord& pWord = m_Words[WordNo];

		if (m_Words[WordNo].m_bComma) bFoundComma = true;

		int CurrentClauseNum = GetMinClauseByWordNo(WordNo);
		assert (CurrentClauseNum != -1);
		if (m_Clauses[CurrentClauseNum].m_iLastWord + 1 !=  pClauseWithParticiple->m_iFirstWord) continue;

		// должны пройти хотя бы одну запятую ()
		if (bFoundComma)
			for(int j = 0 ; j < pWord.m_Homonyms.size() ; j++ )
			{
				const CSynHomonym& Noun = pWord.m_Homonyms[j];
				if	( Noun.IsSynNoun() )
					if (CheckCoordinationOfNounAndParticiple(Participle, &m_Clauses[CurrentClauseNum], WordNo, j))
					{
						return CurrentClauseNum;
					};
			}
	};

	return -1;
}



bool CRusSentence::RuleForParticiples(int iClauseNum)
{
	CClause* pAbstClause = &GetClause(iClauseNum);
	int iWord = pAbstClause->m_iFirstWord;

	if ( IsEnclosedClause (iClauseNum) )  return false;

	if ( pAbstClause->HasUnambiguousStrongRootWithoutWeakHomonyms()) return false;

	int iParticipleType; 
	if( (iParticipleType = pAbstClause->FindType(PARTICIPLE_T)) == -1 )
		return false;


	assert(!pAbstClause->m_vectorTypes[iParticipleType].m_Root.IsEmpty());

	CSynWord& pParticiple = m_Words[pAbstClause->m_vectorTypes[iParticipleType].m_Root.m_WordNo];

	
	int iPart = pParticiple.GetHomonymByPOS(PARTICIPLE);
	if( iPart == -1 )
		return false;

	//пытаемся найти слева
	int iPrev = FindNounFromTheLeftOfParticiple(pAbstClause, pParticiple.GetSynHomonym(iPart));

	//пытаемся найти слева подходящее сущ.(тогда причастие после определяемого слова)
	if	(			
					( m_Words[iWord].m_SubordinateConjNo == -1 )
			&&		(		(pAbstClause->m_iFirstWord != pAbstClause->m_iLastWord ) 
						||  (		(iWord+1<m_Words.size())  
								&&	(m_Words[iWord+1].m_SubordinateConjNo != -1 )
							)
					)
			&&		(iPrev != -1)
            &&      (       pAbstClause->m_iLastWord+1 == m_Words.size()  // проверка правой границы  (должен быть знак препинания)
                        ||  m_Words[pAbstClause->m_iLastWord+1].HasDes(OPun)
                     )
		)
	{

		/*
			Поскольку мы собираемся вкладывать клаузу, нам приходится удалять
			все вершины клаузы, кроме найденного причастия и все омонимы причастия
		*/
		// уничтожаем другие вершины клаузы
		int k = 0;
		for( ; k < pAbstClause->m_vectorTypes.size() ; k++)
		{
			if(iParticipleType == k) continue;


			if(k < iParticipleType)  iParticipleType--;

			int WordNo = pAbstClause->m_vectorTypes[k].m_Root.m_WordNo;

			//	не будем удалять омоним, если он единственный у слова
			if (pAbstClause->GetWords()[WordNo].GetHomonymsCount() > 1)
				pAbstClause->DeleteHomonym(pAbstClause->m_vectorTypes[k].m_Root.m_WordNo, pAbstClause->m_vectorTypes[k].m_Root.m_HomonymNo);
			else
				pAbstClause->DeleteClauseType( k );
			k--;
		}

		// заново надо найти омоними причастия, поскольку мы в предыдущем цикле удалили некоторые омонимы
		iPart = pParticiple.GetHomonymByPOS(PARTICIPLE);
		int PartWordNo = pAbstClause->m_vectorTypes[iParticipleType].m_Root.m_WordNo;
		// уничтожаем омонимы причастия (прилагательное или существительное),
		for(k = 0 ; k < pParticiple.GetHomonymsCount() ; k++)
		{
			if(iPart == k) continue;
			if(k < iPart)  iPart--;
			pAbstClause->DeleteHomonym(PartWordNo, k);
			k--;
		}


		return EncloseClauseAsWord(iClauseNum, iPrev);
	}
	else
		// пытаемся найти справа (причастие не отделено запятой)
		if( FindNounFromTheRightOfParticiple(pAbstClause, pParticiple.GetSynHomonym(iPart)  ) )
		{
			// тогда нужно удалить клаузный вариант причастия, а если вариант только один, тогда
			// нужно его отконвертировать в пустыху
			if(pAbstClause->m_vectorTypes.size() == 1)
			{
				assert (pAbstClause->m_vectorTypes[0].m_Type == PARTICIPLE_T);
				pAbstClause->ChangeAllClauseTypesToOneType( SClauseType(UnknownSyntaxElement,-1,-1) );		
			}
			else
			{
				assert( iParticipleType >= 0);
				pAbstClause->DeleteClauseType( iParticipleType );
			}
		}	

	return false;
}


