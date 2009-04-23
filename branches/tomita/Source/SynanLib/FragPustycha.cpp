// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"


bool CRusSentence::HasStrictestLeftClauseBorder(int ClauseNum) const
{
	int iWord = m_Clauses[ClauseNum].m_iFirstWord;

	if (IsStrictLeftClauseBorder(m_Clauses[ClauseNum])) return true;

   	/*
	 В случае типа "zzzz, и zzzz",т.е. когда есть соч. союз и запятая, тогда
	 налево объединять не надо, поскольку  клауза ", и zzzz" скорее всего однородная клаузы,
	 а ее продолжение находится справа, а не слева.
	 отдельно приходится рассматривать случаи разрывного союза (но уже в других правилах) типа:
			"И Иван, и Мария ушли."

	*/

		if(     ( GetWords()[iWord].m_strWord[0] ==  ',' )
			&&  (iWord  < GetWords().size() - 2 )
			&&  (   GetWords()[iWord+1].FindLemma("И") 
				 || GetWords()[iWord+1].FindLemma("ИЛИ")  
				 || GetWords()[iWord+1].FindLemma("НО")  
				 || GetWords()[iWord+1].FindLemma("А")  
				) 
		  )
		  return true;

	return false;
}


int HasSubConjWithInfinitiveGovernment(const CClause& C) 
{
	for(int i = 0; i < C.m_vectorConjs.size() ; i++ )
	{
		const SConjIndex& conj = C.m_vectorConjs[i];


		if(		(conj.m_FromWhere == FROM_OBOR_DIC)
			&&	(C.GetOpt()->m_pOborDic->m_Entries[conj.m_index].m_ConjType == sub_conj) 
			&&  (C.GetOpt()->m_pOborDic->m_Entries[conj.m_index].m_bConjWithInfinitiveSubord) 
		  )
			return i;
	}

	return -1;	
}


/*
Эта функция вызывается только из RuleForEmptyClauses. Проверяет, что
поданная клауза может быть частью клаузы, которая находится слева от нее, т.е. на ней неправильно 
сработала первичная фрагментация.  Обрабатываются ПУСТЫХИ, ИНФИНИТИВЫ, СРАВН (без подл) 
Т.е. эта функция определяет, что поданная клауза на самом деле не должна существовать
отдельно, а должна быть включена в соседние клаузы, если те позволят это сделать
*/
bool CRusSentence::IsGoodEmptyClause(const CClause* pAbstClause) const 
{
	if (pAbstClause->m_vectorTypes.size() > 1)   return false;

	/*
	будем рассматривать только клаузы,у которых один тип либо ИНФ, либо СРАВН
	*/
	if (pAbstClause->m_vectorTypes.size() == 1)   
		if (			(pAbstClause->m_vectorTypes[0].m_Type != INFINITIVE_T)
				&&		(pAbstClause->m_vectorTypes[0].m_Type != COMPARATIVE_T)
			)
			return false;


	/*
	 если в клаузе есть слово "какой", тогда будем считать это плохой 
	 Например:
	   "Вас спросят, в какой город ехать"
	*/
	for (int jj = pAbstClause->m_iFirstWord; jj < pAbstClause->m_iLastWord; jj++)
		if ( GetWords()[jj].FindLemma("КАКОЙ") )
			return false;

	int iFirstWord = pAbstClause->m_iFirstWord;


    /*
	  если уж клауза начинается со знака препинания, тогда этот знак препинания
	  должен быть либо запятой, либо двоеточием, либо тире:
	  Например:
	    В районе села Устьпорочи, в 60 км северо-западнее гавани, он рухнул. 
		 (здесь "в 60 км северо-западнее гавани" начинается с запятой)
		Я знаю двух людей: Петю и Васю
		 ("Петю и Васю")
		Он уехал на поезде Москва - Северная гавань
		 ("- Северная гавань")

	*/
	bool FirstWordIsComma =  GetWords()[iFirstWord].m_strWord[0] == ',';
    if(      GetWords()[iFirstWord].HasDes(OPun)
		&& ! (    FirstWordIsComma
		       || (GetWords()[iFirstWord].m_strWord[0] == ':') 
			   || (GetWords()[iFirstWord].m_strWord[0] == '-') 
		     ) 
	  )
	return false;

	/*
	 если клауза типа ИНФ начинается с подч. союза, который долже вводить инфинитив,
	 тогда  будем считать, что эта клауза полноценна
	*/

	if (pAbstClause->HasType(INFINITIVE_T))
		if (-1 != HasSubConjWithInfinitiveGovernment(*pAbstClause) )
			return false;

   	/*
	 если в клаузе типа СРАВН не было найдено подлежащего,
	 тогда  будем считать, что эта клауза не полноценна
	*/
	if (pAbstClause->HasType(COMPARATIVE_T))
		if ( ClauseHasSubjPredik(*pAbstClause) )
			return false;

	
	return true;
}


/*
 Эта функция вызывается только из RuleForEmptyClauses. Проверяет, может ли 
 клауза войти в левостоящую. Если не может, тогда возвращает истину.
*/
bool CRusSentence::HasStrictBorderLeft(int iClauseNum)
{
	CClause* pAbstClause = &GetClause(iClauseNum);

	if( HasStrictestLeftClauseBorder(iClauseNum) )
		return true;

	if( iClauseNum == 0)
		return true;

	if( pAbstClause->HasLeftStarter() )
		return true;

	return false;
}

/*
 Эта функция вызывается только из RuleForEmptyClauses. Проверяет, может ли 
 главная клауза включить в себя левостоящую клаузу. Если  может, тогда возвращает истину.
*/

bool CRusSentence::CanBeMainClauseOfTheLeftGoodEmptyClause(const CClause* pAbstClause1, bool bStrictPrevClauseBorder) const 
{
	bool bGoodClause;
	const string& strPunct = GetWords()[pAbstClause1->m_iFirstWord].m_strWord;
	bGoodClause =     
					  //  Иван и Петр пришли 
				      ( strPunct[0] != ',') 
					  // Иван, Петр пришли
		           || !pAbstClause1->HasOnlyOneSimConj()
					   // Иван, а не Петр пришел домой
				   ||  (   pAbstClause1->HasOnlyOneSimConjFromOborDic() 
					    && bStrictPrevClauseBorder 
						&& ( strPunct[0] == ',')
					   );

	/*
	 Не будем увеличивать влево деепричастные, причастные и вводные обороты.
	*/
	bGoodClause &= !(    pAbstClause1->HasType(ADVERB_PARTICIPLE_T) 
		              || pAbstClause1->HasType(INP_T) 
					  || pAbstClause1->HasType(PARTICIPLE_T)
					);

	/*
	 Если  у клаузы есть подчин. союз, тогда ее нельзя расширять влево
	*/
	bGoodClause &= !pAbstClause1->HasLeftStarter();


	return bGoodClause;
}

/*
  В районе населенного пункта Устьпорочи, в 60 км северо-западнее Советской гавани, он зацепился за линию электропередач и рухнул. 
  Маша, Саша и Леша

*/

bool CRusSentence::RuleForEmptyClauses(int iClauseNum)
{
	const CClause& Clause = GetClause(iClauseNum);
	int iWord = Clause.m_iFirstWord; 

	if( !IsGoodEmptyClause( &Clause ) ) return false;

	bool bStrictPrevClauseBorder = HasStrictBorderLeft(iClauseNum);	

	//берем тот фрагмент, который стоит справа от найденной цепочки	
	//  если в текущую клаузу была вложена другая клауза, которая заканчивается там же, где текущая клауза
	// тогда  не будем добавлять пустыху направо (неоднозначность границ ),
	int iNext = GetMaxClauseByFirstWord(m_Clauses[iClauseNum].m_iLastWord+1);
	CClause* pAbstClauseRight = NULL;
	if( iNext != -1 )
		pAbstClauseRight = &GetClause(iNext);
	bool bCanConnectRight =		pAbstClauseRight 
							&& 	CanBeMainClauseOfTheLeftGoodEmptyClause(pAbstClauseRight, bStrictPrevClauseBorder)
							&&  (GetMinClauseByLastWord(Clause.m_iLastWord) == iClauseNum)
							&& !HasStrictBorderLeft(iNext);

	//берем тот фрагмент, который стоит слева от найденной цепочки
	CClause* pAbstClauseLeft = NULL;
	int iPrev = GetMinClauseByLastWord(Clause.m_iFirstWord-1);
	if( iPrev != -1 ) 
		pAbstClauseLeft = &GetClause(iPrev);

	//  если предыщая клауза была вложена в другую клаузу, тогда  не будем добавлять пустыху налево,
	//  поскольку  пустыха  может войти обе клаузы (в меньшую и в большую"),
	//   например:
	//  "интервью, опубликованное в четверг, 12 апреля" // "12 апреля"  надо вкладывать в причастный  оборот
	//  "упал кран, который  нам подарили японцы, 12 апреля" // "12 апреля"  надо вкладывать в главную клаузу
	// мы имеем здесь  абсолютныю неоднозначноть
	bool bCanConnectLeft =		pAbstClauseLeft 
							&&  !pAbstClauseLeft->HasType(INP_T)
							&&  !Clause.HasLeftStarter()
							&&  !IsEnclosedClause(iPrev)
							&&  !HasStrictestLeftClauseBorder(iClauseNum);


	// соединение влево будем считать более приоритетным
	if(bCanConnectLeft)
	{
		/* если объединяется пустыха  с инфинитивом,
		   тогда  тип объединенной клаузы должен быть инфинитовом 
		   если объединяется пустыха  со сравном,
		   тогда  тип объединенной клаузы должен быть СРАВН 
		*/
		if  ( (    Clause.HasType(INFINITIVE_T) 
			       || Clause.HasType(COMPARATIVE_T) 
			      ) 
				&& pAbstClauseLeft->m_vectorTypes.empty() 
				&& 1 == pAbstClauseLeft->m_vectorTypes.size() 
			)
			UniteClauses(iPrev, iClauseNum, RightClauseParams);
		else
			UniteClauses(iPrev, iClauseNum, LeftClauseParams);			

		return true;	
	}

	if(bCanConnectRight) 
	{
		UniteClauses(iClauseNum, iNext, RightClauseParams);
		return  true;
	} 



	return false;
}

