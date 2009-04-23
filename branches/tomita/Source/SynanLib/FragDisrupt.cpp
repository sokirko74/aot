// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"



/*
 Выдает все типы групп, которые накрывают слово WordNo, а если слово
 разделяет подлежащее и сказуемое, тогда  в выходное множество выдается еще и 
 тип SUBJ
*/
static void get_all_group_types_for_the_word(const CMorphVariant& F, int UnitNo, vector<int>& Types)
{
	Types.clear();
	for (size_t i =  0; i < F.m_vectorGroups.GetGroups().size(); i++)
	{
		const CGroup& G =  F.m_vectorGroups.GetGroups()[i];
		if  ((G.m_iFirstWord <= UnitNo) &&  (UnitNo <= G.m_iLastWord))
			Types.push_back(G.m_GroupType);
	};

	for (size_t i =0; i < F.m_Subjects.size(); i++)
		if  (		(F.m_iPredk < UnitNo &&  UnitNo < F.m_Subjects[i] )
				||	(F.m_Subjects[i]  < UnitNo &&  UnitNo < F.m_iPredk)
			)
		{
			Types.push_back(SUBJ);
			break;
		}
};



/*
  добавляет один морф. вариант к другому, если у добавляемого варианта 
  прописано подлежащее и сказуемое, тогда нужно 
  заимствовать у добавляемого эти слоты
*/
void AddSynVar(CMorphVariant& morphVar, const CMorphVariant& ToAdd)
{
	morphVar.m_SynUnits.insert
		(morphVar.m_SynUnits.end(), 
		 ToAdd.m_SynUnits.begin(), 
		 ToAdd.m_SynUnits.end());

	if( ToAdd.HasSubj() && (ToAdd.m_iPredk != -1) )
	{
		morphVar.ResetSubj();
		morphVar.m_Subjects.push_back( ToAdd.GetFirstSubject() + (morphVar.m_SynUnits.size() - ToAdd.m_SynUnits.size()));
		morphVar.m_iPredk = ToAdd.m_iPredk + (morphVar.m_SynUnits.size() - ToAdd.m_SynUnits.size());
	};

	if (morphVar.m_ClauseTypeNo == -1)
		morphVar.m_ClauseTypeNo = ToAdd.m_ClauseTypeNo;
}




bool CRusSentence::TryToFindCommonGroupsForUnitedSynVariants(const CClause& ClFirst, const CClause& ClSecond, vector<int>& Types, bool bFindSubjPredikate) 
{
	assert (ClFirst.m_iFirstWord < ClSecond.m_iFirstWord);

	if (			ClFirst.HasUnambiguousStrongRootWithoutWeakHomonyms() 
			&&		ClSecond.HasUnambiguousStrongRootWithoutWeakHomonyms()
		) return false;

	CRusFormatCaller FormatCaller(GetOpt());
	
	FormatCaller.AddAllRules();

	//следующий кусок во многом цитирует CClause::FillWithAlreadyBuiltGroups
    CMorphVariant synVariant(GetRusGramTab());
	int CountOfVariants = 0;
	for(CSVI pFirstSynVar = ClFirst.m_SynVariants.begin() ; pFirstSynVar != ClFirst.m_SynVariants.end() ; pFirstSynVar++)
	{	
		size_t debug = ClSecond.m_SynVariants.size();
		for(CSVI pSecondSynVar = ClSecond.m_SynVariants.begin() ; pSecondSynVar != ClSecond.m_SynVariants.end(); pSecondSynVar++ )
		{
			// если число общих вариантов больше 500, надо выйти, поскольку  приходится слишком долго ждать.
			CountOfVariants++;
			if (CountOfVariants > 500)
				break;
			int i;
			const CMorphVariant& synVariantLeft = *pFirstSynVar;
			const CMorphVariant& synVariantRight = *pSecondSynVar;			
			if (GetCountOfStrongRoots(ClFirst, synVariantLeft) + GetCountOfStrongRoots(ClSecond, synVariantRight)  > 1)
				continue;
			
			AddSynVar(synVariant, synVariantLeft);			

			// если клаузы  не стоят влотную, тогда  добавим пустую клаузу между  ними
			if (ClFirst.m_iLastWord+1 !=  ClSecond.m_iFirstWord)
			{
				CSynUnit S(GetRusGramTab());
				S.m_Type = EClause;
				S.m_SentPeriod = CPeriod (ClFirst.m_iLastWord+1, ClSecond.m_iFirstWord-1); 
				synVariant.m_SynUnits.push_back(S);
			};

			AddSynVar(synVariant, synVariantRight);
	
			FormatCaller.sent.clear();

			int iWordNum = ClFirst.m_iFirstWord;

			for(i = 0; i < synVariant.GetUnitsCount(); i++)
			{
				CSynUnit SynVarUnit = synVariant.m_SynUnits[i];
				CSynPlmLine plmWord;
				if( SynVarUnit.m_Type == EWord)
				{
					plmWord.m_UnitType = EWord;
					assert( SynVarUnit.m_iHomonymNum < GetWords()[iWordNum].GetHomonymsCount() );
					GetWords()[iWordNum].InitializePlmLine(plmWord, SynVarUnit.m_iHomonymNum);
					plmWord.SetGrammems(SynVarUnit.m_iGrammems);
					plmWord.m_gramcodes =  SynVarUnit.m_GramCodes.c_str();
					iWordNum++;
				}
				else
				{
					plmWord.m_UnitType = EClause;
					plmWord.m_Clause = SynVarUnit.m_SentPeriod;
					// если данная  клауза не пустыха
					if (SynVarUnit.m_iClauseTypeNum != -1)
					{
						int ii = FindClauseIndexByPeriod(SynVarUnit.m_SentPeriod);
						assert (ii != -1);
						const CClause& clause = GetClause(ii);
						assert(SynVarUnit.m_iClauseTypeNum < clause.m_vectorTypes.size() );
						if (clause.IsRelative())
						{
							plmWord.SetFlag(fl_relative_clause);
							const CSynHomonym& HomWhose = GetWords()[clause.m_RelativeWord.m_WordNo].GetSynHomonym(clause.m_RelativeWord.m_HomonymNo);
							plmWord.SetMorphByAncodePattern(HomWhose);
						}
						else
						{
							const CAncodePattern* P = clause.GetRootAncodePattern(SynVarUnit.m_iClauseTypeNum);
							if (P)
									plmWord.SetMorphByAncodePattern(*P);
						}
						plmWord.m_ClauseType = clause.m_vectorTypes[SynVarUnit.m_iClauseTypeNum].m_Type;
					};

					iWordNum += SynVarUnit.m_SentPeriod.size();
				}

				FormatCaller.AddWord(plmWord);	
			}
	
			
			for(i = 0 ; i < FormatCaller.sent.size() ; i++ )
			{
				if( FormatCaller.sent[i].m_UnitType == EClause )
					continue;
				assert(i <  synVariant.GetUnitsCount());
				FormatCaller.SetGrammems(i,  synVariant.m_SynUnits[i].m_iGrammems, synVariant.m_SynUnits[i].m_GramCodes.c_str());
			}

			vector<SFoundTermin> m_vTermins;
			int iSynInfrormativeCount = 0;
			
			FormatCaller.main_analyse();
		

			const SClauseType* pClauseType = 0;
			if (pFirstSynVar->m_ClauseTypeNo != -1)
				pClauseType = &ClFirst.m_vectorTypes[pFirstSynVar->m_ClauseTypeNo];
			else
				if (pSecondSynVar->m_ClauseTypeNo != -1)
					pClauseType = &ClSecond.m_vectorTypes[pSecondSynVar->m_ClauseTypeNo];


			int RootWordNo = (pClauseType == 0) ? -1 : pClauseType->m_Root.m_WordNo;
			int iNodeClType = -1;
			if (RootWordNo != -1)
				iNodeClType = synVariant.UnitNoByWordNo(RootWordNo);	
			EClauseType Type = (pClauseType == 0) ? UnknownSyntaxElement : pClauseType->m_Type;
			synVariant.m_vectorGroups = FormatCaller;
			BuildSubjAndPredRelation(synVariant,iNodeClType, Type);

			


	        //конец цитаты CClause::FillWithAlreadyBuiltGroups
				get_all_group_types_for_the_word(synVariant, synVariantLeft.GetUnitsCount(), Types);

			if (bFindSubjPredikate)
			{
				if (find (Types.begin(), Types.end(), SUBJ) != Types.end())
					return true;
			}
			else
				if (Types.size())
					return true;

			synVariant.Reset();
			FormatCaller.Reset();

		}

	}

	return false;
}

/*
Примеры:
 Дом, где живет Петров, сломался
 Мысль, что он придет, ее угнетала.
 Мысль, что он, в самом деле, придет, ее угнетала.


 Между первой и третьей клаузами должно быть построено отношение ПОДЛ-СКАЗУЕМОЕ.
*/

bool CRusSentence::RuleForDisruptClausesBySubject(int iClauseNum)
{
	const CClause& pClause1 = GetClause(iClauseNum);

	int iWord = pClause1.m_iFirstWord;


	int iNxt = GetMinClauseByFirstWord(pClause1.m_iLastWord+1);
	if (iNxt == -1) return false; 

	const CClause& pClause2 = GetClause(iNxt);		

	if ( IsStrictLeftClauseBorder(pClause2) ) return false; 


	

	int iThird = GetMaxClauseByFirstWord(pClause2.m_iLastWord+1);
	if (iThird == -1) return false; 

	const CClause& pClause3 = GetClause(iThird);		

	int ThirdFirstWordNo = pClause3.m_iFirstWord;

	if ( IsStrictLeftClauseBorder(pClause3) ) return false;

	if (    ClauseHasSubjPredik(pClause1)
		 && ClauseHasSubjPredik(pClause3)
	    )
    return false;   

	if (		(iWord != 0) 
			&&	!pClause1.HasLeftStarter()
	   ) return false;

	if ( !pClause1.m_vectorTypes.empty() ) return false;

	if ( !pClause2.HasLeftStarter() ) return false; //: nim 18.01.01

	// пустыхи нельзя вкладывать в другие клаузы
	if ( pClause2.m_vectorTypes.size() == 0 ) return false; 
	
	if ( pClause3.HasLeftStarter() ) return false;

	if ( pClause3.HasOnlyOneSimConjFromOborDic() ) return false;

	if ( !(pClause3.HasType(VERB_PERS_T)) && !(pClause3.HasType(PARTICIPLE_SHORT_T)) && 
		 !(pClause3.HasType(PARTICIPLE_SHORT_T)) && !(pClause3.HasType(ADJ_SHORT_T)) &&
		 !(pClause3.HasType(PREDK_T)) && !(pClause3.HasType(INFINITIVE_T)) ) return false;
	
	vector<int> Types;

	/*
	  пробуем найти  подлеажащее для сказуемого во  третьей клаузе
	*/
	if (!TryToFindCommonGroupsForUnitedSynVariants(pClause1, pClause3, Types, true))
		return false;
	
	EncloseClauseAsWord(iNxt, iClauseNum);

	int iClauseNum_New = GetMaxClauseByFirstWord(iWord);
	iThird = GetMaxClauseByFirstWord(ThirdFirstWordNo);
	assert(iThird != -1 && iClauseNum_New != -1);
		
	UniteClauses( iClauseNum_New, iThird, RightClauseParams);
	return true;
}

bool HasOnlyWeakTypes (const vector<SClauseType>& vectorTypes)
{
	for  (size_t i=0; i < vectorTypes.size(); i++)
	{
		EClauseType type = vectorTypes[i].m_Type;
		if (		(type != PARTICIPLE_T)
				&&	(type != COMPARATIVE_T)
				&&	(type != INFINITIVE_T)
			)
		return false;
	};
	return true;
};


bool CRusSentence::RuleForUnitingClausesBySubject(int iClauseNum, bool bIgnoreWeakTypes)
{
	const CClause& pClause1 = GetClause(iClauseNum);

	int iNxt = GetMaxClauseByFirstWord(pClause1.m_iLastWord+1);
	if (iNxt == -1) return false; 

	const CClause& pClause2 = GetClause(iNxt);		

	if ( IsStrictLeftClauseBorder(pClause2) ) return false;

	if (    ClauseHasSubjPredik(pClause1)
		 && ClauseHasSubjPredik(pClause2)
	    )
    return false;   

	if (		(pClause1.m_iFirstWord != 0) 

			&&	!pClause1.HasLeftStarter()
	   ) return false;

	if (!bIgnoreWeakTypes)
		if ( !pClause1.m_vectorTypes.empty() ) return false;
	else
		if (!HasOnlyWeakTypes(pClause1.m_vectorTypes))
			return false;


	if ( pClause2.HasLeftStarter() ) return false;

	if ( pClause2.HasOnlyOneSimConjFromOborDic() ) return false;

	

	if ( !(pClause2.HasType(VERB_PERS_T)) && !(pClause2.HasType(PARTICIPLE_SHORT_T)) && 
		 !(pClause2.HasType(PARTICIPLE_SHORT_T)) && !(pClause2.HasType(ADJ_SHORT_T)) &&
		 !(pClause2.HasType(PREDK_T)) && !(pClause2.HasType(INFINITIVE_T)) ) return false;
	
	vector<int> Types;

	/*
	  пробуем найти  подлежащее для сказуемого во второй клаузе
	*/
	if (!TryToFindCommonGroupsForUnitedSynVariants(pClause1, pClause2, Types, true))
		return false;

	UniteClauses( iClauseNum, iNxt, RightClauseParams);
	return true;
}
bool CRusSentence::RuleForUnitingClausesBySubjectWeak(int iClauseNum)
{
	return RuleForUnitingClausesBySubject(iClauseNum, false);
};

bool CRusSentence::RuleForUnitingClausesBySubjectStrong(int iClauseNum)
{
	return RuleForUnitingClausesBySubject(iClauseNum, true);
};




/*
эта функция проверяет, что в первой клаузе есть начало разрывного союза,
а во второй - конец. Если эта функция выдает ложь, значит на границе
не может быть построена группа с разрывным союзом. 
Эта функция сделана только для ускорения.
*/
bool CRusSentence::CanBeDisruptConjClause (const CClause& pClauseLeft, const CClause& pClauseRight) const
{
	for (long WordNo1=pClauseLeft.m_iFirstWord; WordNo1<=pClauseLeft.m_iLastWord; WordNo1++)
	{
		for (int ConjNo = 0; ConjNo < m_pSyntaxOptions->m_pOborDic->m_DisruptConj.size(); ConjNo++)
		{		
			const SDoubleConj& vConj = m_pSyntaxOptions->m_pOborDic->m_DisruptConj[ConjNo];
			if ( GetWords()[WordNo1].m_strUpperWord != vConj.m_FirstPart[0].m_item ) continue;
			for (long WordNo2=pClauseRight.m_iFirstWord; WordNo2<=pClauseRight.m_iLastWord; WordNo2++)
			{
			  if ( GetWords()[WordNo2].m_strUpperWord == vConj.m_SecondPart[0].m_item ) 
			  return true;
			};
		};
	};
	return false;
}

/*
 Примеры:
 Он изобрел и сад, и огород.
 И сад, и огород он изобрел. 
 Я хочу и пить, и курить.
 Не красивая, а желтая страница выпала из книги.
 будь то доступ в Интернет, код регистрации программы или нужная вам глава из книги 
 И красивая, и желтая страница.

*/

bool CRusSentence::RuleForDisruptConjUnion(int iClauseNum)
{
	CClause& pClauseLeft = GetClause(iClauseNum);

	int iWord = pClauseLeft.m_iFirstWord;

	int iNxt = GetMaxClauseByFirstWord(pClauseLeft.m_iLastWord+1);
	if (iNxt == -1) return false; 

	CClause& pClauseRight = GetClause(iNxt);		
	
	//специальная проверка для союза "будь то ... , будь то"
	bool bPredikatConj = false;
	if (!pClauseLeft.m_vectorTypes.empty())
		if ( pClauseLeft.m_vectorTypes[0].m_Root.m_WordNo == pClauseLeft.m_iFirstWord )
			if (GetWords()[pClauseLeft.m_vectorTypes[0].m_Root.m_WordNo].m_strUpperWord == "БУДЬ")
				bPredikatConj = true;

	vector<int> Types;

	if (!CanBeDisruptConjClause(pClauseLeft,pClauseRight))
		return false;

	if (!TryToFindCommonGroupsForUnitedSynVariants(pClauseLeft, pClauseRight, Types, false))
		return false;

	for (long i=0; i < Types.size(); i++)
		if (GetOpt()->m_DisruptConjGroupType == Types[i] )
		{
		  /*
		    если вершина преовй клаузы был "будь", тогда первую клаузу
			надо сдлеать пустыхой, поскольку она вошла в группа разрывных союзов
			и параметры объед. клаузы нужно брать от второй клаузы
		  */
          if (bPredikatConj)
		  {
			  pClauseLeft.ChangeAllClauseTypesToOneType( SClauseType(UnknownSyntaxElement, -1, -1) );
		  }
		  ParametersSourceEnum iwhose_chars_to_copy;
          if (   pClauseLeft.HasType(INFINITIVE_T) 
			  && pClauseRight.m_vectorTypes.empty()
			 )
			 iwhose_chars_to_copy = LeftClauseParams;
		  else
			  if (   pClauseRight.HasType(INFINITIVE_T) 
			      && pClauseLeft.m_vectorTypes.empty()
				 )
				 iwhose_chars_to_copy = RightClauseParams;
			  else
			  if (pClauseLeft.m_vectorTypes.empty())
				  iwhose_chars_to_copy = RightClauseParams;
			  else
				  iwhose_chars_to_copy = LeftClauseParams;

		  UniteClauses( iClauseNum, iNxt, iwhose_chars_to_copy);

		  return true;
		};

	return false;
}




