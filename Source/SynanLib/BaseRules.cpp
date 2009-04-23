// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusWord.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"


bool	CRusSentence::CanBeRelativeAntecedent(const CSynHomonym& H) const
{
	return			H.IsMorphNoun();
};


void CRusSentence::InitClauseVectorRules() 
{
	
    #define Abbr  m_vectorPrimitiveRules.push_back(CClauseRule((FirstGrRoule)&CRusSentence::
		Abbr RuleForSimilarNounGroupClauses3, "RuleForSimilarNounGroupClauses3"));	
		Abbr RuleForDashClause, "RuleForDashClause"));
		Abbr RuleForParenthesis, "RuleForParenthesis"));
		Abbr RuleForFragNounGroup, "RuleForFragNounGroup"));
		Abbr RuleForDisruptConjUnion, "RuleForDisruptConjUnion"));
	#undef Abbr

	#define Abbr  m_vectorMainEncloseRules.push_back(CClauseRule((FirstGrRoule)&CRusSentence::
		Abbr RuleForRelativeClause, "RuleForRelativeClause"));
		Abbr RuleForAdverbPart, "RuleForAdverbPart"));
		Abbr RuleForParticiples, "RuleForParticiples"));
		Abbr RuleForSubClause, "RuleForSubClause"));
	#undef Abbr

	#define Abbr  m_vectorDisruptRules.push_back(CClauseRule((FirstGrRoule)&CRusSentence::
		Abbr RuleForUnitingClausesBySubjectWeak, "RuleForUnitingClausesBySubjectWeak"));
		Abbr RuleForUnitingEmptyClauseWithStrongLeftBorder, "RuleForUnitingEmptyClauseWithStrongLeftBorder"));
		Abbr RuleForKTO, "RuleForKTO"));
		Abbr RuleForRelativeClause, "RuleForRelativeClause"));
		
		Abbr RuleForUnitingEmptyClauseWithStrongLeftBorder, "RuleForUnitingEmptyClauseWithStrongLeftBorder"));
		Abbr RuleForDisruptClausesBySubject, "RuleForDisruptClausesBySubject"));
		Abbr RuleForClauseDisruptWithSubordinateClauses, "RuleForClauseDisruptWithSubordinateClauses"));
		//
		Abbr RuleForSimilarNounGroupClauses2, "RuleForSimilarNounGroupClauses2"));	
		Abbr RuleForSubClauseWithCoordiantion, "RuleForSubClauseWithCoordiantion"));	
		
		Abbr RuleForEmptyClauses, "RuleForEmptyClauses"));
		Abbr RuleForDisruptClausesBySubject, "RuleForDisruptClausesBySubject"));
		
		
		Abbr RuleForParticiples, "RuleForParticiples"));
		Abbr RuleForAdverbPart, "RuleForAdverbPart"));
		//
		Abbr RuleForCommaDashClause, "RuleForCommaDashClause"));

		//  еще раз вызываем RuleForUnitingClausesBySubject
		Abbr RuleForUnitingClausesBySubjectStrong, "RuleForUnitingClausesBySubjectStrong"));
	#undef Abbr
    
}










struct CAntecedent {
	char m_Lemma[20];
	int		   m_Poses;
};

const int KTO_AntecedentCount = 12;

const CAntecedent KTO_Antecedents[KTO_AntecedentCount] =  {
	{"ТОТ",    1<<PRONOUN_P },
	{"КАЖДЫЙ", 1<<PRONOUN_P},
	{"ВСЯКИЙ", 1<<PRONOUN},
	{"ВСЯКАЯ", 1<<PRONOUN},
	{"ЛЮБОЙ", 1<<PRONOUN},
	{"ЛЮБАЯ", 1<<PRONOUN},
	{"ВСЕ", 1<<PRONOUN},
	{"НИКТО", 1<<PRONOUN},
	{"КТО-НИБУДЬ", 1<<PRONOUN},
	{"КТО-ТО", 1<<PRONOUN},
	{"КОЕ-КТО", 1<<PRONOUN},
	{"ПРОЧИЙ", 1<<PRONOUN}
};

/*
Примеры:
Тот, кто пришел на это митинг, получит футболку.
Пресс-секретарь министерства внутренних дел Англии заявил, что каждый, кто пожелает в переписи населения записаться джедаем, будет считаться исповедующим "другие религии". 

Эта функция рассматрвиать только случаи с антецедентом, а такие случаи 
не берутся:
"Я знаю, кто пришел."

*/
bool CRusSentence::RuleForKTO(int iClauseNum)
{
	CClause* pAbstClause = &GetClause(iClauseNum);

	int iWord = pAbstClause->m_iFirstWord;

	if( !pAbstClause )
		return false;

	if( IsEnclosedClause(iClauseNum) )
		return false;

	/*
		Ищем союзное слово "КТО".
	*/

	int i = 0;
	for( ; i < pAbstClause->m_vectorConjs.size() ; i++ )
	{
		const SConjIndex& conj = pAbstClause->m_vectorConjs[i];
		if(		(conj.m_FromWhere == FROM_SUB_CONJ)
			&&	(GetOpt()->m_pOborDic->GetSubConjs()[conj.m_index] == "КТО")
		  )
		break;
	}
	/*
		Не нашли - выходим 
	*/

	if( i == pAbstClause->m_vectorConjs.size() )
		return false;

 
	int PrevClauseNo = GetMinClauseByLastWord(pAbstClause->m_iFirstWord - 1);

	if( PrevClauseNo == -1 ) 
		return false;

	CClause* pPrevClause = &GetClause(PrevClauseNo);

	//пытаемся найти слева от КТО антецедент
	for(i = pPrevClause->m_iLastWord ; i >= pPrevClause->m_iFirstWord ; i-- )
	{
		const CSynWord& pWord = GetWords()[i];
        if (pWord.HasOborot1() != pWord.HasOborot2()) continue;
		for(int j = 0 ; j < pWord.m_Homonyms.size() ; j++ )
		{		
			
			const CSynHomonym& H = pWord.m_Homonyms[j];
            

			if (H.m_iGrammems & (_QM(rFeminum) |  _QM(rMasculinum) | _QM(rPlural)))
				for (long k=0; k < KTO_AntecedentCount; k++)
					if (KTO_Antecedents[k].m_Poses & H.m_iPoses)
						if (	H.IsLemma(KTO_Antecedents[k].m_Lemma) )
						{
							GetWords()[i].SetAllOtherHomsDel(j);
							DeleteMarkedHomonymsWithClauses(i);
							iWord = pAbstClause->m_iFirstWord;
							return EncloseClauseAsWord(iClauseNum, PrevClauseNo );
						}
		}

	}	

	return false;
}







bool CRusSentence::RuleForSubClause(int iClauseNum)
{
	CClause* pAbstClause = &GetClause(iClauseNum);
	int iWord = pAbstClause->m_iFirstWord;

	if ( IsEnclosedClause (iClauseNum) )  return false;


	//  проверка на неодн. границы
	if(GetMinClauseByLastWord(m_Clauses[iClauseNum].m_iLastWord) !=  iClauseNum)
		return false;


	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	if(iNext == -1)
		return false;

	CClause* pAbstClause1 = &GetClause(iNext);


	if(		pAbstClause1->m_vectorTypes.empty()
		||	pAbstClause1->HasType(INP_T) 
	  )
		return false;

	if( pAbstClause1->HasType(INFINITIVE_T) && !pAbstClause1->HasSubConj("ЧТОБЫ"))
		return false;

	
	if( pAbstClause1->size() < 2 )
		return false;

	int iFirstWord = pAbstClause1->m_iFirstWord;
	bool bSubConj =		(GetWords()[iFirstWord].m_strWord[0] == ',') 
						&& pAbstClause1->HasLeftStarter();
					
	if (!bSubConj) 
		return false;

	return EncloseClauseAsWord(iNext, iClauseNum);
}

bool CRusSentence::RuleForAdverbPart(int iClauseNum)
{
	const CClause* pAbstClause = &GetClause(iClauseNum);
	

	if ( IsEnclosedClause (iClauseNum) )  return false;

	int AdvPartType  = pAbstClause->FindType(ADVERB_PARTICIPLE_T);
	if (AdvPartType == -1) 
		return false;

	// если деепричастие  вошло в состав оборота, тогда не будем вкладывать его 
	// куда-либо 
	// Например, "принимая во внимание"
	int RootWordNo = pAbstClause->m_vectorTypes[AdvPartType].m_Root.m_WordNo;
	assert (RootWordNo != -1);
	if (m_Words[RootWordNo].IsInOborot())
		return false;


	// Деепричастный  оборот должен быть отделен запятой, если он не стоит в самом начале.
	//  В конструкции "Он шел, свистя и напевая" "напевая" должно вложиться в "свистя".
	if (pAbstClause->m_iFirstWord != 0)
		if (!m_Words[pAbstClause->m_iFirstWord].m_bComma)
			return false;


	int iPrev = -1;

	if(    (pAbstClause->m_iFirstWord + 1 >= GetWords().size())
		|| !GetWords()[pAbstClause->m_iFirstWord + 1].m_bSimilarConj 
	  )	
		iPrev = GetMaxClauseByLastWord(pAbstClause->m_iFirstWord - 1);

	

	
	if(			(iPrev != -1 )
			&&	(GetMinClauseByLastWord(pAbstClause->m_iFirstWord - 1) == iPrev)
		) 
	{
		if( GetClause(iPrev).HasType(VERB_PERS_T) )
		{			
			return EncloseClauseAsWord(iClauseNum, iPrev);
		}
	}

	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	CClause* pAbstClause1 = NULL;
	if( iNext != -1 )
		pAbstClause1 = &GetClause(iNext);

	if( !(pAbstClause1 == NULL) )
	{			
		if( !pAbstClause1->HasType(VERB_PERS_T) )
			return false;
		if( !pAbstClause1->CanHaveEncloseClauseFromTheLeft() )
			return false;
		return EncloseClauseAsWord(iClauseNum, iNext);
	}

	return false;
}





//новое правило для тире работает по алгоритму "Функции Copul" [Шалимов] - Правило1  
bool CRusSentence::RuleForDashClause(int iClauseNum)
{
	CClause* pClause1  = &GetClause(iClauseNum);
	int iWord = pClause1->m_iFirstWord;


	if( GetWords()[pClause1->m_iFirstWord].m_strWord != "-" ) return false;

	CClause* pClauseLeft = NULL;
	int iPrev = GetMaxClauseByLastWord(pClause1->m_iFirstWord - 1);
	if( iPrev != -1 ) 
		pClauseLeft = &GetClause(iPrev);
	else
		return false;
	
	int i = pClause1->m_iFirstWord;

	if (pClause1->HasUnambiguousStrongRootWithoutWeakHomonyms())
		return false;

	if (pClauseLeft->HasUnambiguousStrongRootWithoutWeakHomonyms())
		return false;

    long DashWordNo =  pClause1->m_iFirstWord;
	CClause& newClause = UniteClauses(iPrev, iClauseNum, LeftClauseParams);

	newClause.ChangeAllClauseTypesToOneType(SClauseType(DASH_T,DashWordNo, 0));		

	return true;
}




bool CRusSentence::RuleForClauseDisruptWithSubordinateClauses(int iClauseNum)
{
	int i;

	CPeriod pClause1 = GetClause(iClauseNum);

	int iWord = pClause1.m_iFirstWord;

	int iNxt = GetMaxClauseByFirstWord(pClause1.m_iLastWord+1);
	if (iNxt == -1) return false; 

	CPeriod pClause2 = GetClause(iNxt);
	
	if ( IsStrictLeftClauseBorder(pClause2) ) return false;

	if ( !GetClause(iClauseNum).HasLeftStarter())
		return false;

	if ( !GetClause(iClauseNum).m_vectorTypes.empty() ) return false;

	for (i = pClause1.m_iFirstWord; i <= pClause1.m_iLastWord; i++) 
		if ( HasNounInNom(GetWords()[i]) ) return false;

	if (!GetClause(iNxt).HasLeftStarter()) return false;

	vector<CPeriod> vClausesForEnclose;
	int iNum = -1;

	for ( i = pClause2.m_iLastWord+1; i < GetWords().size(); )
	{
		iNum = GetMaxClauseByFirstWord(i);
		if (iNum == -1) return false; 
			
		const CClause& pClauseN = GetClause(iNum);

		if ( IsStrictLeftClauseBorder(pClauseN) ) return false;
		
		if (pClauseN.HasLeftStarter() || pClauseN.HasType(PARTICIPLE_T) ||
			pClauseN.HasType(ADVERB_PARTICIPLE_T))
			vClausesForEnclose.push_back(pClauseN);
		else
		{
			if ( pClauseN.HasOnlyOneSimConjFromOborDic() ) return false;
			break;
		}

		i = pClauseN.m_iLastWord+1;
	}
	
	if (iNum == -1) return false;

	EncloseClauseAsWord(iNxt, iClauseNum);

	for ( i = 0; i < vClausesForEnclose.size(); i++)
	{
		int iCurrentNum = GetMaxClauseByFirstWord(vClausesForEnclose[i].m_iFirstWord); 
		int iClauseNum_New = GetMaxClauseByFirstWord(pClause1.m_iFirstWord);
		assert(iCurrentNum != -1 && iClauseNum_New != -1);
		EncloseClauseAsWord(iCurrentNum, iClauseNum_New);
	}

	if (vClausesForEnclose.size() > 0)
		iNum = GetMaxClauseByFirstWord(vClausesForEnclose[vClausesForEnclose.size()-1].m_iLastWord+1);

	if (iNum == -1) return false;

	int iClauseNum_New = GetMaxClauseByFirstWord(iWord);
	assert(iNum != -1 && iClauseNum_New != -1);

	if (m_Clauses[iClauseNum_New].m_iLastWord+1 == m_Clauses[iNum].m_iFirstWord)
		UniteClauses( iClauseNum_New, iNum, RightClauseParams);
	return  true;
}





//правило для запятая+тире работает по алгоритму "Функции Copul" [Шалимов] - Правило2 
//примеры: Дом, построенный на холме, - большой.
//         Дом, который построен на холме, - большой.
//         Дом,видимо, - большой.
bool CRusSentence::RuleForCommaDashClause(int iClauseNum)
{
	const CClause& pClause1 = GetClause(iClauseNum);

	int iWord = pClause1.m_iFirstWord;

	int iNxt = GetMaxClauseByFirstWord(pClause1.m_iLastWord+1);
	if (iNxt == -1) return false; 

	const CClause& pClause2 = GetClause(iNxt);		

	if(     !GetWords()[pClause2.m_iFirstWord].m_bComma 
		||  (pClause2.size() == 1)
		||  !GetWords()[pClause2.m_iFirstWord+1].m_bDash
	  )
		return false;
	
	if ( pClause1.HasUnambiguousStrongRootWithoutWeakHomonyms() ) return false;
	if ( pClause2.HasUnambiguousStrongRootWithoutWeakHomonyms() ) return false;


	int iEncClL = GetMaxChildByLastWord(pClause1,pClause1.m_iLastWord);

	if ( -1 == iEncClL) return false;
	
	const CClause& pEnclClause = GetClause(iEncClL);

	if (!pEnclClause.HasType(PARTICIPLE_T) && 
		!pEnclClause.HasType(ADVERB_PARTICIPLE_T) &&
		!pEnclClause.IsRelative() &&
		!pEnclClause.HasType(INP_T) &&
		!pEnclClause.HasType(PARTICIPLE_SHORT_T))
			return false;

	int DashWordNo = pClause2.m_iFirstWord+1;
	CClause& newClause = UniteClauses(iClauseNum, iNxt, LeftClauseParams);
	newClause.ChangeAllClauseTypesToOneType(SClauseType(DASH_T, DashWordNo, 0));		
	return true;
}

bool CRusSentence::ClauseHasSubjPredik(const CClause& Clause) const 
{
	for (CSVI i = Clause.m_SynVariants.begin(); i != Clause.m_SynVariants.end(); i++)
	{
		if ( i->HasSubj() &&  i->HasPredk())
			return true;
	}
	return false;
};

//правило для клауз с вводными словами и оборотами
//работает по алгоритму "Алгоритм Вкладывания Вводных" [Шалимов]
bool CRusSentence::RuleForParenthesis(int iClauseNum)
{
	CClause& pClauseParenthesis = GetClause(iClauseNum);

	int iWord = pClauseParenthesis.m_iFirstWord;

	if ( IsEnclosedClause (iClauseNum) )  return false;

	if ( !pClauseParenthesis.HasType(INP_T) ) return false;	

	int iNxt = GetMaxClauseByFirstWord(pClauseParenthesis.m_iLastWord+1);
	int iPrev = GetMaxClauseByLastWord(pClauseParenthesis.m_iFirstWord-1);

	if ( -1 == iNxt && -1 == iPrev ) return false;

	

	// если нет правой  клаузы, или это прид. предложение, тогда вкладываем влево
	if	(		(		( -1 == iNxt ) 
					||	!GetClause(iNxt).CanHaveEncloseClauseFromTheLeft()
				)
			&&	(iPrev != -1)
		)
	{
		const CClause& pPrevClause = GetClause(iPrev);
		iWord = pPrevClause.m_iFirstWord;
		return EncloseClauseAsWord(iClauseNum, iPrev);
	}
	else
	if (		( -1 == iPrev )
			&&	GetClause(iNxt).CanHaveEncloseClauseFromTheLeft()
		)
	{
		return EncloseClauseAsWord(iClauseNum, iNxt);
	}


	// далее рассматривается случай, только  когда можно вложить в обе клаузы
	if ( -1 == iNxt || -1 == iPrev ) return false;

	assert (iPrev != -1);
	assert (iNxt != -1);
	assert (GetClause(iNxt).CanHaveEncloseClauseFromTheLeft());

	const CClause& pNextClause = GetClause(iNxt);

	
	CClause pPrevClause = GetClause(iPrev);
	int prev_conj_size = pPrevClause.m_vectorConjs.size();
	int next_conj_size = pNextClause.m_vectorConjs.size();
	bool bPrev, bNext;
	bPrev = bNext = false;	

	 int i = pClauseParenthesis.m_iFirstWord;
     for (; i < pClauseParenthesis.m_iLastWord && GetWords()[i].HasDes(OPun); i++ )
		if ( !GetWords()[i].m_bComma )
			bNext = true;

	for ( i = pNextClause.m_iFirstWord; i < pNextClause.m_iLastWord && GetWords()[i].HasDes(OPun); i++ )
		if ( !GetWords()[i].m_bComma )
			bPrev = true;

	if ( bNext && !bPrev)
	{
		return EncloseClauseAsWord(iClauseNum, iNxt);
	}

	if ( !bNext && bPrev )
	{
		return EncloseClauseAsWord(iClauseNum, iPrev);
	}

	if ( (prev_conj_size > 0 && next_conj_size > 0) )
	{
		return  EncloseClauseAsWord(iClauseNum, iPrev);
	}	

	bool bParticleOrConj = false;
	for ( i = pClauseParenthesis.m_iFirstWord; i <= pClauseParenthesis.m_iLastWord; i++ )
		if ( GetWords()[i].GetHomonymByPOS(PARTICLE) > -1 ||
			 GetWords()[i].GetHomonymByPOS(CONJ) > -1)
			bParticleOrConj = true;
	if ( bParticleOrConj || pClauseParenthesis.m_vectorConjs.size() > 0 )
	{
		return EncloseClauseAsWord(iClauseNum, iNxt);
	}
	
	bPrev = bNext = false;
	if ( pPrevClause.HasType(VERB_PERS_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(VERB_PERS_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(ADJ_SHORT_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(ADJ_SHORT_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(PARTICIPLE_SHORT_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(PARTICIPLE_SHORT_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(PREDK_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(PREDK_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(ADVERB_PARTICIPLE_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(ADVERB_PARTICIPLE_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(PARTICIPLE_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(PARTICIPLE_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(INFINITIVE_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(INFINITIVE_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(COMPARATIVE_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(COMPARATIVE_T) && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.m_vectorTypes.empty() && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.m_vectorTypes.empty() && 0 == next_conj_size )
		bNext = true;
	else if ( pPrevClause.HasType(INP_T) && 0 == prev_conj_size )
		bPrev = true;
	else if ( pNextClause.HasType(INP_T) && 0 == next_conj_size )
		bNext = true;

	if (!bPrev && !bNext) return false;

	if ( bPrev )
	{
		EncloseClauseAsWord(iClauseNum, iPrev);
	}

	if ( bNext )
	{
		EncloseClauseAsWord(iClauseNum, iNxt);
	}

	if ( bNext && pPrevClause.m_vectorTypes.empty() )
	{
		iNxt = GetMaxClauseByFirstWord(iWord);
		iPrev = GetMaxClauseByFirstWord(pPrevClause.m_iFirstWord);
		if ( -1 == iNxt && -1 == iPrev) return true;
		UniteClauses( iPrev, iNxt, RightClauseParams);
		iWord = pNextClause.m_iLastWord+1;
		return true;	
	}

	if ( bPrev && pNextClause.m_vectorTypes.empty() && 0 == next_conj_size ) 
	{
		iPrev = GetMaxClauseByFirstWord(pPrevClause.m_iFirstWord);
		if ( -1 == iPrev ) return true;
		iNxt = GetMaxClauseByFirstWord(pNextClause.m_iFirstWord);
		if ( -1 == iNxt && -1 == iPrev) return true;
		int iLast = GetMaxClauseByFirstWord(pNextClause.m_iLastWord+1);
		if ( -1 == iLast )
		{
			UniteClauses( iPrev, iNxt, LeftClauseParams );
			iWord = pNextClause.m_iLastWord+1;
		}
		else 
		{
			const CClause& pLastClause = GetClause(iLast);
			if ( !pLastClause.HasLeftStarter() &&
				( pLastClause.HasType(VERB_PERS_T) || 
				  pLastClause.HasType(ADJ_SHORT_T) || 
				  pLastClause.HasType(PARTICIPLE_SHORT_T) ||
				  pLastClause.HasType(PREDK_T) ) )
			{
				UniteClauses( iPrev, iNxt, LeftClauseParams );
				iWord = pNextClause.m_iLastWord+1;
			}
		}
		
	}

	return true;
}


/*
 Функция  заполняет слот  m_ClausesRelations
 Узнавать приходится следующее:
	1.  Вложена клауза или нет.
	2.  Тип клаузы (все варианты типа по большей части должны быть удалены, поскольку  клауза
	вложилась).
	3. Вошла ли клауза в син. группу (если вошла, тогда отношение строить
	не надо).
*/

void CRusSentence::BuildClauseRelations()
{
	m_ClausesRelations.clear();

	for (int  ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		int FirstHostNum = GetMininalParentByClauseNo( ClauseNo );
		if (FirstHostNum == -1) continue;

		// если клауза  вошла в группы, тогда  отношения  будут проведены в соотв. с синтакс. группой

		if (m_Clauses[FirstHostNum].m_SynVariants.empty()) continue;
		if (m_Clauses[ClauseNo].m_SynVariants.empty()) continue;

		if (m_Clauses[FirstHostNum].GetLastHost(m_Clauses[ClauseNo], m_Clauses[FirstHostNum].m_SynVariants.begin()) != 0)
			continue;

		CRelation newRel;
		newRel.m_SourceType = EWord;
		newRel.m_TargetType = EWord;
		newRel.m_SourceClause.m_ClausePeriod = m_Clauses[FirstHostNum];
		newRel.m_TargetClause.m_ClausePeriod = m_Clauses[ClauseNo];
		


		//  отношение  между вводным и главной  клаузой
		if (m_Clauses[ClauseNo].HasType(INP_T))
		{
			newRel.m_TargetClause.m_ClauseTypes.push_back(INP_T);
			newRel.type = PARENTHESIS_CLAUSE;
		}
		else
		//  отношение  между неомонимичным причастным оборотом
		if (		m_Clauses[ClauseNo].HasType(PARTICIPLE_T) 
				&&  (		!m_Clauses[ClauseNo].HasLeftStarter() 
						||  (m_Clauses[ClauseNo].m_vectorTypes.size() == 1)
					)
			)
		{
			newRel.m_TargetClause.m_ClauseTypes.push_back(PARTICIPLE_T);
			newRel.type = SUB_CLAUSE;
		}
		else
		//  отношение  между неомонимичным деепричастным оборотом и главным предложением
		if (		m_Clauses[ClauseNo].HasType(ADVERB_PARTICIPLE_T) 
				&&  (		!m_Clauses[ClauseNo].HasLeftStarter() 
						||  (m_Clauses[ClauseNo].m_vectorTypes.size() == 1)
					)
			)
		{
			newRel.m_TargetClause.m_ClauseTypes.push_back(ADVERB_PARTICIPLE_T);
			newRel.type = SUB_CLAUSE;
		}
		else
		//  вложение с помощью  подчинительного или сочинительного союза
		if (    m_Clauses[ClauseNo].HasType(VERB_PERS_T) 
			 || m_Clauses[ClauseNo].HasType(PARTICIPLE_SHORT_T)
			 || m_Clauses[ClauseNo].HasType(ADJ_SHORT_T)
			 || m_Clauses[ClauseNo].HasType(PREDK_T)
			 || m_Clauses[ClauseNo].HasType(INFINITIVE_T)
			 || m_Clauses[ClauseNo].HasType(COMPARATIVE_T)
			 
		   ) 
		{
			// COMPARATIVE_T добавлен из-за примера "я знаю, что золото дороже бронзы"
			const EClauseType Predicative[6] =  {VERB_PERS_T, PARTICIPLE_SHORT_T, ADJ_SHORT_T, PREDK_T, INFINITIVE_T, COMPARATIVE_T };		
			newRel.m_TargetClause.m_ClauseTypes.insert(newRel.m_TargetClause.m_ClauseTypes.begin(), Predicative, Predicative+6);
			newRel.m_SourceClause.m_ClauseTypes.insert(newRel.m_SourceClause.m_ClauseTypes.begin(), Predicative, Predicative+6);
			newRel.type = SUB_CLAUSE;
		}
		else
			continue;


		m_ClausesRelations.push_back(newRel);

	};

};
