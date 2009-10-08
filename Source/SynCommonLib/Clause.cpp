// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "Clause.h"
#include "Sentence.h"

/*
 сравнение CMorphVariant-ов по весу
*/
inline bool GreaterByWeight (const CMorphVariant& variant1, const CMorphVariant& variant2 )  
{
	if( variant1.m_iWeight == variant2.m_iWeight)
	{
		if(			(variant2.m_ClauseTypeNo == -1) 
				&&	(variant1.m_ClauseTypeNo != -1)
			)				
			return true;
		
		return false;
	}
	else
		return variant1.m_iWeight > variant2.m_iWeight;
}



CClause::CClause(CSentence* pSent, int iFirstWord, int iLastWord )
{
	m_iFirstWord = iFirstWord;
	m_iLastWord = iLastWord; 	
	m_iPunctSignsCount = 0;
	m_pSent = pSent; 
	m_bIgnoreWhileBuildingSynVariants = false;
	m_AntecedentWordNo = -1;
}

CWordVector& CClause::GetWords()
{
	return m_pSent->m_Words;
};

const CWordVector& CClause::GetWords() const 
{
	return m_pSent->m_Words;
};

const	CSyntaxOpt* CClause::GetOpt() const
{
	return m_pSent->GetOpt();
};



int CClause::FindType(EClauseType Type) const
{
	for(int i = 0 ; i < m_vectorTypes.size() ; i++ )
	{
		if( m_vectorTypes[i].m_Type == Type)
			return i;
	}
	return -1;
};

int CClause::FindCommonType  (const CClause& C ) const
{
	for(int i = 0 ; i < m_vectorTypes.size() ; i++ )
        for(int j = 0 ; j < C.m_vectorTypes.size() ; j++ )
	        if( m_vectorTypes[i].m_Type == C.m_vectorTypes[j].m_Type)
			    return m_vectorTypes[i].m_Type;
	    
	return -1;
};

bool CClause::HasType (EClauseType Type)  const
{ 
	return (FindType(Type) != -1); 
};



string  CClause::GetTraceStr() const
{
	string T;
	for (int i = 0; i < m_vectorTypes.size(); i++)
	{
		T += GetOpt()->GetGramTab()->GetClauseNameByType(m_vectorTypes[i].m_Type);
		if (i+1 < m_vectorTypes.size())
			T+= "|";
	};
	if (T == "") T = "no_type";
	return Format("%s(%i %i) ", T.c_str(), m_iFirstWord, m_iLastWord);
};


int CClause::FindClauseSubordConj(const char* ConjStr)  const
{
	const COborDic* D = GetOpt()->m_pOborDic;
	for(int i = 0; i < m_vectorConjs.size() ; i++ )
	{
		const SConjIndex& conj = m_vectorConjs[i];

		if	(		( conj.m_FromWhere == FROM_SUB_CONJ )
				&&	(		!ConjStr
						||	!strcmp( D->GetSubConjs()[m_vectorConjs[i].m_index].c_str(), ConjStr ) 
					)
			)
			return i;


		if	(conj.m_FromWhere == FROM_OBOR_DIC)
		{
			const COborotForSyntax& O =  D->m_Entries[m_vectorConjs[i].m_index];
			if (	(O.m_ConjType == sub_conj) 
				&&	(		!ConjStr
						||	!strcmp( O.m_OborotEntryStr.c_str(), ConjStr) 
					)
			)
			return i;
		};
	}

	return -1;
}

bool CClause::HasSubConj(const char* strConj) const 
{
	return FindClauseSubordConj(strConj) != -1;
}

bool CClause::HasLeftStarter() const 
{
	return HasSubConj() || (m_iFirstWord == 0) || IsRelative();
}


bool CClause::CanHaveEncloseClauseFromTheLeft() const
{
		return		!HasSubConj() 
				&&	!HasAtLeastOneSimConj()
				&&	!IsRelative();

};


bool CClause::HasOnlyOneSimConj() const 
{
	return !HasSubConj()  && (m_vectorConjs.size() == 1);
}

bool CClause::HasAtLeastOneSimConj() const
{
	return !HasSubConj() && (m_vectorConjs.size() >= 1);
}

bool CClause::HasOnlyOneSimConjFromOborDic() const
{
	int c = 0;
	for(int i = 0 ; i < m_vectorConjs.size() ; i++ )
	{
		const SConjIndex& conj = m_vectorConjs[i];
		if(		(conj.m_FromWhere == FROM_OBOR_DIC)
			&&	(GetOpt()->m_pOborDic->m_Entries[conj.m_index].m_ConjType != sub_conj) )
			c++;
	}
	return (c == 1);
}









const CMorphVariant* CClause::GetSynVariantByClauseType(const SClauseType& type) const 
{
	for(CSVI i =  m_SynVariants.begin() ; i != m_SynVariants.end(); i++ )
	{
		if (i->m_ClauseTypeNo != -1)
			if( type == m_vectorTypes[i->m_ClauseTypeNo] )
				return &(*i);
	}
	return 0;
}



vector<CMorphVariant*> CClause::GetSynVariantIndexesByTypeNum(int i_type) 
{
	vector<CMorphVariant*> res;

	for(SVI i =  m_SynVariants.begin() ; i != m_SynVariants.end(); i++ )
		if( i_type == i->m_ClauseTypeNo )
			res.push_back(&(*i));

	return res;

}




//	если слово iWord входит в клаузу, тогда выдается номер слова, которое стоит 
//	сразу после этой клаузы.
//	иначе выдается  iWord.
//	Выдаваемый iWord может снова указывать на клаузу, если клаузы стоят контактно, а
//	входной iWord указывал на первую клаузу.

int CClause::PassSubClause(int iWord) const
{
	if (m_SynVariants.empty())
		return iWord;

	const CMorphVariant& V =  *(m_SynVariants.begin());
	size_t UnitNo = V.UnitNoByWordNo(iWord);
	if (UnitNo == -1) return -1;

	if (V.m_SynUnits[UnitNo].m_Type == EClause)
		return V.m_SynUnits[UnitNo].m_SentPeriod.m_iLastWord+1;
	else
		return iWord;
}



int CClause::SynVarToPrmClForLastWord(int UnitNo) const
{
	if (m_SynVariants.empty())
		return UnitNo + m_iFirstWord;

	int n = m_SynVariants.begin()->GetUnitLastWord(UnitNo);
	assert (n != -1);
	return n;
}


int CClause::SynVarToPrmClForFirstWord(int UnitNo)  const
{
	if (m_SynVariants.empty())
		return UnitNo + m_iFirstWord;

	int n = m_SynVariants.begin()->GetUnitFirstWord(UnitNo);
	assert (n != -1);
	return n;
}

void CClause::TranslateCoordSynVarForGroup(CGroup& pGrp) const
{
	pGrp.m_iFirstWord = SynVarToPrmClForFirstWord(pGrp.m_iFirstWord);
	pGrp.m_iLastWord = SynVarToPrmClForLastWord(pGrp.m_iLastWord);
	pGrp.m_MainGroup.m_iFirstWord = SynVarToPrmClForFirstWord(pGrp.m_MainGroup.m_iFirstWord);
	pGrp.m_MainGroup.m_iLastWord = SynVarToPrmClForLastWord(pGrp.m_MainGroup.m_iLastWord);

	pGrp.m_MainWordNo = SynVarToPrmClForLastWord(pGrp.m_MainWordNo);

}

void CClause::TranslateFormatCallerGroups(CMorphVariant& V)
{
    for (size_t i=0; i < V.m_vectorGroups.GetGroups().size(); i++) 
        TranslateCoordSynVarForGroup(*V.m_vectorGroups.GetGroupPtr(i));

    for ( size_t i = 0; i < V.m_vectorGroups.m_Relations.size(); i++ )
    {
        V.m_vectorGroups.m_Relations[i].m_iFirstWord = SynVarToPrmClForFirstWord(V.m_vectorGroups.m_Relations[i].m_iFirstWord);
        V.m_vectorGroups.m_Relations[i].m_iLastWord = SynVarToPrmClForLastWord( V.m_vectorGroups.m_Relations[i].m_iLastWord);
        V.m_vectorGroups.m_Relations[i].AssertValid();
    }

};


static void TranslateCoordClauseForGroup(const CClause& C, CGroup& pGrp) 
{
	pGrp.m_iFirstWord = C.UnitNoByWordNo(pGrp.m_iFirstWord);
	pGrp.m_iLastWord = C.UnitNoByWordNo(pGrp.m_iLastWord);
	pGrp.m_MainGroup.m_iFirstWord = C.UnitNoByWordNo(pGrp.m_MainGroup.m_iFirstWord);
	pGrp.m_MainGroup.m_iLastWord = C.UnitNoByWordNo(pGrp.m_MainGroup.m_iLastWord);
	pGrp.m_MainWordNo = C.UnitNoByWordNo(pGrp.m_MainWordNo);
}

void CClause::InterpretGroupBounds(CGroup& pGrp)
{
	if (m_SynVariants.empty()) return;
	
	const CMorphVariant& V =  *(m_SynVariants.begin());

	pGrp.m_iFirstWord = V.GetClauseFirstWord(pGrp.m_iFirstWord);
	assert (pGrp.m_iFirstWord != -1);
	pGrp.m_iLastWord = V.GetClauseLastWord(pGrp.m_iLastWord);
	assert (pGrp.m_iLastWord != -1);
	pGrp.m_MainGroup.m_iFirstWord = V.GetClauseFirstWord(pGrp.m_MainGroup.m_iFirstWord);
	assert (pGrp.m_MainGroup.m_iFirstWord != -1);
	pGrp.m_MainGroup.m_iLastWord = V.GetClauseLastWord(pGrp.m_MainGroup.m_iLastWord);
	assert (pGrp.m_MainGroup.m_iLastWord != -1);
	pGrp.m_MainWordNo = V.GetClauseLastWord(pGrp.m_MainWordNo);
	assert (pGrp.m_MainWordNo != -1);
}

const CAncodePattern* CClause::GetRootAncodePattern(int iType) const
{
	assert( (iType >= 0) && (iType < m_vectorTypes.size()) );
	const SClauseType& type = m_vectorTypes[iType];
	if( type.m_Root.IsEmpty() )
		return 0;
	
	const CMorphVariant* t= GetSynVariantByClauseType(type);
	
	if ( t == 0)  //  if no morph variant in the clause
		return 0;

	return &t->m_SynUnits[UnitNoByWordNo(type.m_Root.m_WordNo)];	
}

int CClause::UnitNoByWordNo(int iWord) const
{
	if( m_SynVariants.empty())
		return iWord - m_iFirstWord;

	return m_SynVariants.begin()->UnitNoByWordNo(iWord);
}




void CClause::FillWithAlreadyBuiltGroups(CFormatCaller& FormatCaller, CSVI pVar) const
{
	const CMorphVariant& synVar = *pVar;

	assert (FormatCaller.GetGroups().empty());

	for(int i = 0 ; i < synVar.m_vectorGroups.GetGroups().size() ; i++ )
	{
		CGroup G = synVar.m_vectorGroups.GetGroups()[i];
		TranslateCoordClauseForGroup(*this, G);
		FormatCaller.create_group( G );
	}

	for ( int i = 0; i < synVar.m_vectorGroups.m_Relations.size(); i++ )
	{
        CRelation R = synVar.m_vectorGroups.m_Relations[i];
		R.m_iFirstWord = UnitNoByWordNo(R.m_iFirstWord);
		R.m_iLastWord = UnitNoByWordNo(R.m_iLastWord);
        R.AssertValid();
        FormatCaller.m_Relations.push_back(R);
	}


}


void CClause::BuildVectorOfWords(CFormatCaller& FormatCaller, CSVI pVar) const
{
	const CMorphVariant& synVar = *pVar;

	FormatCaller.sent.clear();
	int iWordNum = m_iFirstWord;
	int i = 0;
	for(; i < synVar.m_SynUnits.size(); i++)
	{
		const CSynUnit& SynVarUnit = synVar.m_SynUnits[i];
		CSynPlmLine plmWord;
		if( SynVarUnit.m_Type == EWord)
		{
			plmWord.m_UnitType = EWord;
			assert( SynVarUnit.m_iHomonymNum < GetWords()[iWordNum].GetHomonymsCount() );
			GetWords()[iWordNum].InitializePlmLine(plmWord, SynVarUnit.m_iHomonymNum);
			plmWord.SetMorphByAncodePattern(SynVarUnit);
			plmWord.m_FoundPrepDependCases = SynVarUnit.m_FoundPrepDependCases; 
			iWordNum++;
		}
		else
		{
			plmWord.m_UnitType = EClause;
			plmWord.m_Clause = SynVarUnit.m_SentPeriod;
			int ii = m_pSent->FindClauseIndexByPeriod(SynVarUnit.m_SentPeriod);
			assert(ii != -1);
			const CClause& clause = m_pSent->GetClause(ii);
			if (clause.IsRelative())
			{
				plmWord.SetFlag(fl_relative_clause);
				const CSynHomonym& HomWhose = GetWords()[clause.m_RelativeWord.m_WordNo].GetSynHomonym(clause.m_RelativeWord.m_HomonymNo);
				plmWord.SetMorphByAncodePattern(HomWhose); 
			}
			else
			{
				if (SynVarUnit.m_iClauseTypeNum != -1)
				{
					const CAncodePattern* P = clause.GetRootAncodePattern(SynVarUnit.m_iClauseTypeNum);
					if (P)
						plmWord.SetMorphByAncodePattern(*P);
				};
			}
			if (SynVarUnit.m_iClauseTypeNum != -1)
				plmWord.m_ClauseType = clause.m_vectorTypes[SynVarUnit.m_iClauseTypeNum].m_Type;

			iWordNum += SynVarUnit.m_SentPeriod.size();
		}

		FormatCaller.AddWord(plmWord);	
	}
	
	

	/*
	for(i = 0 ; i < FormatCaller.sent.size() ; i++ )
	{
		if( FormatCaller.sent[i].m_UnitType == EClause )
			continue;

		assert(i <  synVar.m_SynUnits.size() );

		FormatCaller.SetGrammems(i,synVar.m_SynUnits[i].m_iGrammems, synVar.m_SynUnits[i].m_GramCodes.c_str());
	}*/


}


void CClause::GetBuildingUnits(vector<CBuildingUnit>& BuildingUnits)
{
	BuildingUnits.clear();
	for (int WordNo = m_iFirstWord;  WordNo <= m_iLastWord; WordNo++)
	{
		CBuildingUnit U;
		U.m_MaxUsedHomonymNo = 0;
		U.m_ChildClauseNo = m_pSent->GetMaxChildByFirstWord(*this, WordNo);
		U.m_WordNo = WordNo;
		U.m_BestHomonyms = GetMaxQWORD();
		

		if (U.m_ChildClauseNo != -1) 
		{
			const CClause& Child = m_pSent->m_Clauses[U.m_ChildClauseNo];
			U.m_HomonymsCount =  Child.m_vectorTypes.size();
			// если была вложена пустыха или клауза, у который есть синвариант без вершины,
			// тогда  в качестве варианта нужно добавить омоним пустой вершины клаузы
			if (		(U.m_HomonymsCount == 0) 
					||	!Child.HasUnambiguousStrongRootWithoutWeakHomonyms()
				)
				U.m_HomonymsCount++;

			WordNo = Child.m_iLastWord;
		}
		else
		{
			U.m_HomonymsCount =  GetWords()[WordNo].m_Homonyms.size();
		};
		assert (U.m_HomonymsCount > 0);
		assert (U.m_HomonymsCount <= 64);

		BuildingUnits.push_back(U);
	};
};

int AddUnitAsClause (vector<CBuildingUnit>::const_iterator pUnit, int CurrentHomonymNo, const CClause& C, CMorphVariant& synVariant)
{
	const CClause& Child = C.m_pSent->m_Clauses[pUnit->m_ChildClauseNo];
    CSynUnit SClauseHomonym(C.GetOpt()->GetGramTab());
	SClauseHomonym.m_Type = EClause;
	SClauseHomonym.m_iClauseTypeNum = (CurrentHomonymNo >= Child.m_vectorTypes.size())? -1 : CurrentHomonymNo;
	SClauseHomonym.m_SentPeriod = (CPeriod)Child;
	synVariant.m_SynUnits.push_back(SClauseHomonym);
	return Child.m_iLastWord  + 1;
};

int AddUnitAsWord (vector<CBuildingUnit>::const_iterator pUnit, int CurrentHomonymNo, const CClause& C, CMorphVariant& synVariant)
{
	synVariant.PushHomonymNum(CurrentHomonymNo); 
	const CSynWord& W = C.GetWords()[pUnit->m_WordNo]; 
	synVariant.m_SynUnits.back().CopyAncodePattern( W.m_Homonyms[CurrentHomonymNo] );
	synVariant.m_SynUnits.back().m_SentPeriod = CPeriod(pUnit->m_WordNo);
	return pUnit->m_WordNo+1;
};

// максимальное количество вариантов в одной  клаузе (все остальные игнорируются)
const int MorphVarMaxCount = 1600;

void CClause::BuildSynVariantsRecursive(vector<CBuildingUnit>::iterator pUnit, CMorphVariant& synVariant)
{
	int StartWordNo = pUnit->m_WordNo;
	int CountOfTypes =  pUnit->m_HomonymsCount;

	assert (CountOfTypes > 0);		
	for (int i = 0; i < CountOfTypes; i++)
	{
		if (!(pUnit->m_BestHomonyms & _QM(i))) continue;

		int NextWordNo;
		if (pUnit->m_ChildClauseNo != -1) 
			NextWordNo = AddUnitAsClause(pUnit, i, *this, synVariant);
		else
			NextWordNo = AddUnitAsWord(pUnit, i, *this, synVariant);

		if (i > pUnit->m_MaxUsedHomonymNo) 
			pUnit->m_MaxUsedHomonymNo = i;

		if (NextWordNo > m_iLastWord)
		{
			if (IsGoodVariant(synVariant)) 
				if (!IsTwoPotentialPredikatesInOneClause(synVariant)) 
						m_SynVariants.push_back(synVariant);
		}
		else
			BuildSynVariantsRecursive(pUnit+1,  synVariant);


		if (m_SynVariants.size() >= MorphVarMaxCount) return;

		synVariant.PopHomonymNum();


	}
}



//	Нам нужно, чтобы  все омонимы были представлены во множестве синвариантов, иначе при удалении омонимов
//	могут быть удалены все синваоринты клаузы.
//Функция AddSynVariantsWithUnusedHomonyms запукается  только тогда, когда число вариантов превысило допутимый предел.
	

void AddSynVariantsWithUnusedHomonyms(CClause& C, vector<CBuildingUnit>& BuildingUnits) 
{
	CMorphVariant synVariant(C.GetOpt()->GetGramTab());

	bool FoundUnusedHomonym;
	do
	{
		FoundUnusedHomonym = false;
		for (int UnitNo=0; UnitNo < BuildingUnits.size();UnitNo++)
		{
			int HomonymNo = 0;
			if (BuildingUnits[UnitNo].m_MaxUsedHomonymNo+1 < BuildingUnits[UnitNo].m_HomonymsCount)
			{
				BuildingUnits[UnitNo].m_MaxUsedHomonymNo++;
				HomonymNo = BuildingUnits[UnitNo].m_MaxUsedHomonymNo;
				FoundUnusedHomonym = true;
			};

			if (BuildingUnits[UnitNo].m_ChildClauseNo != -1) 
				AddUnitAsClause(BuildingUnits.begin()+UnitNo, HomonymNo, C, synVariant);
			else
				AddUnitAsWord(BuildingUnits.begin()+UnitNo, HomonymNo, C, synVariant);

		};
		if (FoundUnusedHomonym)
		{
            if (C.IsGoodVariant(synVariant)) 
			    C.m_SynVariants.push_back(synVariant);
			synVariant.Reset();

		};
	}
	while (FoundUnusedHomonym);	
};


void CClause::BuildSynVariants()
{
	m_SynVariants.clear();
	
	vector<CBuildingUnit> BuildingUnits;
	GetBuildingUnits(BuildingUnits);
	
	CMorphVariant synVariant(GetOpt()->GetGramTab());
    size_t EstimCount = 1;
    for (size_t i =0; i < BuildingUnits.size(); i++)
    {
        EstimCount *= BuildingUnits[i].m_HomonymsCount;
        if (EstimCount > MorphVarMaxCount)
            break;
    }
    if (EstimCount > MorphVarMaxCount)
        AddSynVariantsWithUnusedHomonyms(*this, BuildingUnits );
    else
    	BuildSynVariantsRecursive(BuildingUnits.begin(), synVariant);
	/*
    BuildSynVariantsRecursive(BuildingUnits.begin(), synVariant);
   	if (m_SynVariants.size() >= 200)
		AddSynVariantsWithUnusedHomonyms(*this, BuildingUnits );
*/
		

	for(SVI  pVar  = m_SynVariants.begin() ; pVar != m_SynVariants.end(); pVar++ )
	{
		CMorphVariant& synVariant = *pVar;
		m_pSent->ChooseClauseType(m_vectorTypes, synVariant);
	};

	rml_TRACE("BuildSynVariants found %i variants for %s\n",m_SynVariants.size(), GetTraceStr().c_str() );
}


void	CClause::BuildSubjAndPredMember(CMorphVariant& synVariant)
{
	int RootWordNo = (synVariant.m_ClauseTypeNo == -1) ? -1 : m_vectorTypes[synVariant.m_ClauseTypeNo].m_Root.m_WordNo;
	EClauseType Type = (synVariant.m_ClauseTypeNo == -1) ? UnknownSyntaxElement : m_vectorTypes[synVariant.m_ClauseTypeNo].m_Type;
	m_pSent->BuildSubjAndPredRelation(synVariant, UnitNoByWordNo(RootWordNo), Type);
};



void CreateGroupsForTermins(CClause& C, CFormatCaller& FormatCaller, CMorphVariant& synVariant)
{
  

	vector<int> SynVarHomonyms;
	for (int j = 0; j < synVariant.GetUnitsCount(); j++)
		SynVarHomonyms.push_back( synVariant.GetHomNum(j) );


    FormatCaller.BuildOborotGroups();

	for (size_t TerminNo=0; TerminNo< C.m_pSent->m_vectorTermins.size(); TerminNo++)
	{
		SFoundTermin T = C.m_pSent->m_vectorTermins[TerminNo];
		if (!T.is_part_of(C)) continue;
		T.m_iFirstWord = C.UnitNoByWordNo(T.m_iFirstWord);
		T.m_iLastWord = C.UnitNoByWordNo(T.m_iLastWord);

        // вставлено из-за примера "печатной информации,  фильмов, компьютерных банков данных, ничего. "
       /* bool bSubClause = false;
        for (size_t i=T.m_iFirstWord; i <= T.m_iLastWord; i++)
            if (FormatCaller.sent[i].m_UnitType == EClause)
            {
                bSubClause = true;
                break;
            }
        if (bSubClause) continue;*/


		if	(FormatCaller.create_groups_from_termin( T)) 
		{
			C.m_pSent->m_vectorTermins[TerminNo].m_bGroupsChecked = true;
			/*int WordNo = 0;
			for (int UnitNo = T.m_iFirstWord; UnitNo <= T.m_iLastWord; UnitNo++, WordNo++)
			{
				CSynWord& W = C.GetWords()[C.m_pSent->m_vectorTermins[TerminNo].m_iFirstWord+WordNo];
				//W.SetAllOtherHomsDel(SynVarHomonyms[UnitNo]); 
			};
            */
		}

	};
};




//вызывается, когда еще syn-варианты еще не построены(т.е. в первый раз)
bool CClause::BuildGroupsAndSynVariants(CFormatCaller& FormatCaller)
{
	try
	{		
		//строим декартово произведение омонимов и
		//заполняем очередным набором омонимов FormatCaller.sent
		BuildSynVariants();


		for(SVI  pVar  = m_SynVariants.begin() ; pVar != m_SynVariants.end(); pVar++ )
		{	
			CMorphVariant& synVariant = *pVar;
				
			FormatCaller.Reset();
			BuildVectorOfWords(FormatCaller, pVar);

			CreateGroupsForTermins(*this, FormatCaller, synVariant);

			//main syntax analise		
			FormatCaller.main_analyse();
			FormatCaller.BuildAutomaticSynrels();

			synVariant.m_vectorGroups = FormatCaller;
			BuildSubjAndPredMember(synVariant);
			
			AssignVariantWeight(synVariant);
			TranslateFormatCallerGroups(synVariant);

			AssignSynVariantsGrammems(synVariant, FormatCaller);
			AssignOborotMarksToDisruptConj(FormatCaller, synVariant);
		}

		for(int k = 0 ; k <	m_pSent->m_vectorTermins.size(); k++ )
			if (m_pSent->m_vectorTermins[k].is_part_of(*this) )
				if( !m_pSent->m_vectorTermins[k].m_bGroupsChecked )
				{
					m_pSent->m_vectorTermins.erase(m_pSent->m_vectorTermins.begin() + k);
					k--;
				};



		m_SynVariants.sort( GreaterByWeight );
		
		//nim 
		DeleteClauseTypeWhichAreNotUsedInAnyOfSynVars();

		return true;
	}
	catch(...)
	{
		m_pSent->OutputErrorString("Failed BuildGroupsAndSynVariants");
		return false;
	}
}



/*
 сравнение CMorphVariant-ов по номерам омонимов
*/
inline bool LessByHomonyms (const CMorphVariant& variant1, const CMorphVariant& variant2) 
{
	assert(variant1.m_SynUnits.size() == variant2.m_SynUnits.size());

	for(int i = 0 ; i < variant1.m_SynUnits.size() ; i++ )
	{
		if(variant1.m_SynUnits[i].m_iHomonymNum < variant2.m_SynUnits[i].m_iHomonymNum)
			return true;
		if(variant1.m_SynUnits[i].m_iHomonymNum > variant2.m_SynUnits[i].m_iHomonymNum)
			return false;
	}

	return false;
}



bool CClause::BuildGroupsForExistingSynVariants(CFormatCaller& FormatCaller, bool bRebuildAllGroups)
{
	try
	{
		
		
		m_SynVariants.sort( LessByHomonyms );
		
		for(SVI  pVar  = m_SynVariants.begin() ; pVar != m_SynVariants.end(); pVar++ )
		{
			CMorphVariant& synVariant = *pVar;

			//пересчитать границы групп 
			for (size_t i=0; i < synVariant.m_vectorGroups.GetGroups().size(); i++)
				InterpretGroupBounds( *synVariant.m_vectorGroups.GetGroupPtr(i) );


			if( !IsGoodVariant(synVariant) )				
					continue;

			
			BuildVectorOfWords(FormatCaller, pVar);

			if (!bRebuildAllGroups)
				FillWithAlreadyBuiltGroups(FormatCaller, pVar);
			else
				CreateGroupsForTermins(*this, FormatCaller, synVariant);

				
			FormatCaller.main_analyse();

			FormatCaller.BuildAutomaticSynrels();

			synVariant.m_vectorGroups = FormatCaller;
			BuildSubjAndPredMember(synVariant);

			AssignVariantWeight(synVariant);
			TranslateFormatCallerGroups(synVariant);
		
			AssignSynVariantsGrammems(synVariant, FormatCaller);
			AssignOborotMarksToDisruptConj(FormatCaller, synVariant);
			FormatCaller.Reset();
			

		}
		m_SynVariants.sort( GreaterByWeight );
	}

	catch(...)
	{
		m_pSent->OutputErrorString("Failed BuildGroupsForExistingSynVariants");
		return false;
	}

	return true;
}

bool CClause::BuildGroups(CFormatCaller& FormatCaller, bool bRebuildAllGroups)
{
	bool bRes;

	if( m_SynVariants.empty() )
		bRes = BuildGroupsAndSynVariants(FormatCaller);
	else
		bRes = BuildGroupsForExistingSynVariants(FormatCaller, bRebuildAllGroups);

	m_SynVariants.sort( GreaterByWeight );
	m_pSent->AfterBuildGroupsTrigger (*this);
	

	return bRes;
}




void CClause::AssignVariantWeight(CMorphVariant& synVariant)
{
	try
	{
		int iWeight = 0;
		
		//	adding weight by each maximal group 
		int i = synVariant.m_vectorGroups.GetGroups().size() - 1;
		bool bSubjIsInsideGroup = false;
		bool bPredicateIsInsideGroup = false;
		while(  i >= 0   )
		{
			const  CGroup& group = synVariant.m_vectorGroups.GetGroups()[i];
			CPeriod PeriodInSentenceCoords = synVariant.GetSentenceCoordinates(group);

			//  if the last or the first word in the group is indeclinable, 
			bool bStartsOrEndsWithIndeclinable = false;
			{
				const CSynUnit& UnitStart = synVariant.m_SynUnits[group.m_iFirstWord];
				if	(		(UnitStart.m_Type == EWord)
						&&	(GetOpt()->m_IndeclinableMask & GetWords()[PeriodInSentenceCoords.m_iFirstWord].m_Homonyms[UnitStart.m_iHomonymNum].m_iGrammems)
					)
					bStartsOrEndsWithIndeclinable = true;

				const CSynUnit& UnitEnd = synVariant.m_SynUnits[group.m_iLastWord];
				if	(		(UnitEnd.m_Type == EWord)
						&&	(GetOpt()->m_IndeclinableMask & GetWords()[PeriodInSentenceCoords.m_iLastWord].m_Homonyms[UnitEnd.m_iHomonymNum].m_iGrammems)
					)
					bStartsOrEndsWithIndeclinable = true;
			};

			if (		GetOpt()->IsGroupWithoutWeight(group.m_GroupType, group.m_Cause )
					||	(		(group.size() == 2)
							&&	bStartsOrEndsWithIndeclinable
						)
				)
			{ 
				i--; // go to the subgroup or to the next group
				continue;
			};


			// adding weight (size of the group in words) Note, that the group can contain a clause
			iWeight += PeriodInSentenceCoords.size();		

			//  if the last or the first word in the group is indeclinable, 
			if	(bStartsOrEndsWithIndeclinable)
					iWeight--;

			if	(		!synVariant.m_Subjects.empty()
					||	CPeriod(synVariant.GetFirstSubject()).is_part_of(group)
				)
				bSubjIsInsideGroup = true;

			if (CPeriod(synVariant.m_iPredk).is_part_of(group))
				bPredicateIsInsideGroup = true;


		
			
			// go to the subclauses and add weight by each subclause
			for (long k=group.m_iFirstWord; k <= group.m_iLastWord; k++)
				if( synVariant.m_SynUnits[k].m_Type == EClause )
				{
					CPeriod p  = synVariant.m_SynUnits[k].m_SentPeriod;
					CClause* clause = m_pSent->FindClauseByPeriod(p);
					assert(clause != NULL);

					vector<CMorphVariant*> clause_vars = clause->GetSynVariantIndexesByTypeNum(synVariant.m_SynUnits[k].m_iClauseTypeNum);				

					for (long j=0; j < clause_vars.size(); j++)
						if( clause_vars[j]->m_iWeight != -1) 
							clause_vars[j]->m_iWeight += iWeight;				
				};
			
			
			// go to the next groups
			for (i--; i >=0 && synVariant.m_vectorGroups.GetGroups()[i].is_part_of(group);  i--);
		}

		
		

		if (!bSubjIsInsideGroup)
			if (synVariant.m_bGoodSubject)
				iWeight += 1;

		if (!bPredicateIsInsideGroup)
			if (synVariant.m_bGoodSubject)
				iWeight += 1;
		
		
		synVariant.m_iWeight = iWeight;
	}
	catch(...)
	{
		return;
	}
}



const CGroup* CClause::GetLastFirmHost(int iWord, CSVI pSynVar) const
{
	const CMorphVariant& pVar = *pSynVar;
	for(int i = pVar.m_vectorGroups.GetGroups().size() - 1 ; i >= 0 ; i--)
	{		
		const CGroup& group = pVar.m_vectorGroups.GetGroups()[i];
		if(		(group.m_iFirstWord <= iWord) 
			&&	(group.m_iLastWord >= iWord) 
			&&	GetOpt()->is_firm_group(group.m_GroupType)
		  )
		return &group;
	}

	return NULL;		

}

const CGroup* CClause::GetLastHost(CPeriod P, CSVI pSynVar) const 
{
	const CMorphVariant& pVar = *pSynVar;
	// идем с конца, поэтому первая объемлющая группа будет  максимальной
	
	for(int i = pVar.m_vectorGroups.GetGroups().size() - 1 ; i >= 0 ; i--)
	{		
		const CGroup& group = pVar.m_vectorGroups.GetGroups()[i];
		if( P.is_part_of(group)  )
				return &group;
	}

	return NULL;		
}


//	функция AssignSynVariantsGrammems копирует морфологические характеристики слов из FormatCaller в синвариант.
//	Предполагается, что  в FormatCaller были потстроены новын группы, которые изменили 
//	морф. характеристики слов.

void CClause::AssignSynVariantsGrammems(CMorphVariant&  synVariant, const CFormatCaller& FormatCaller)
{
	assert(synVariant.m_SynUnits.size() == FormatCaller.sent.size());
	const CAgramtab& piGramTab = * (GetOpt()->GetGramTab());

	
	for(int UnitNo = 0; UnitNo < synVariant.m_SynUnits.size() ; UnitNo++ )
	{	
		const CSynPlmLine& W = FormatCaller.sent[UnitNo];
		
		if( W.m_UnitType != EWord )
			continue;

		if( W.has_unk_gramcodes() )
			continue;

		if	(		W.HasFlag(fl_in_oborot)
				&&	!(W.HasFlag(fl_oborot1) && W.HasFlag(fl_oborot2))
			)
			continue;

        synVariant.m_SynUnits[UnitNo].ModifyGrammems ( W.GetGrammems(),W.GetPoses());
		
	}


	
	//	берем все простые  предлоги омонима, на которых была построена предложная 
	//	группа. Если предложная группа была  построена, тогда m_FoundPrepDependCases
	//	содержит  падежи, которые  были найдены у зависимого от предлого слова.
	
	for(int UnitNo = 0 ; UnitNo < FormatCaller.sent.size() ; UnitNo++ )
	{
		synVariant.m_SynUnits[UnitNo].m_FoundPrepDependCases = FormatCaller.sent[UnitNo].m_FoundPrepDependCases; 

		if (		FormatCaller.sent[UnitNo].get_simple_preps() 
				&&	!FormatCaller.sent[UnitNo].get_simple_preps()->empty()
			) 
		{
			synVariant.m_SynUnits[UnitNo].m_SimplePrepNos.clear();

			const vector<int>& preps = *(FormatCaller.sent[UnitNo].get_simple_preps());
			
			for(int k = 0 ; k < preps.size() ; k++ ) 
				if (FormatCaller.sent[UnitNo].m_FoundPrepDependCases == 0)
					synVariant.m_SynUnits[UnitNo].m_SimplePrepNos.push_back(preps[k]);			
				else
					if( GetOpt()->m_pOborDic->m_Entries[preps[k]].m_DependCases & FormatCaller.sent[UnitNo].m_FoundPrepDependCases )
						synVariant.m_SynUnits[UnitNo].m_SimplePrepNos.push_back(preps[k]);			
		}
	}
}




static const CGroup* IsInThisGroupInner(const CClause& C, int iWord, const CMorphVariant& pVar, EGroupType type) 
{
	for(int i = 0 ;  i < pVar.m_vectorGroups.GetGroups().size() ; i++)
	{
		const CGroup& pGroup = pVar.m_vectorGroups.GetGroups()[i];
		if( (pGroup.m_iFirstWord <= iWord) && (pGroup.m_iLastWord >= iWord) )
			if(pGroup.m_GroupType == type)
				return &pGroup;
	}
	return NULL;			
}  

const CGroup* CClause::IsInThisGroup(int iWord, EGroupType type, CSVI& Var) const
{
	for(Var =  m_SynVariants.begin() ; Var != m_SynVariants.end(); Var++ )
	{
		const CGroup* pGroup = IsInThisGroupInner(*this, iWord,(*Var),type);

		if( pGroup )		
			return pGroup;
	}

	return NULL;
}

const CGroup* CClause::GetMaxLastHost(int iWord) const
{
	for(CSVI i =  m_SynVariants.begin() ; i != m_SynVariants.end(); i++ )
	{
		const CGroup* G = GetLastHost(iWord, i);
		if (G != NULL ) 
			return G;
	};

	return NULL;
}

const CGroup* CClause::GetMaxLastHost(int iWord, int iHomonym) const
{
	int UnitNo = UnitNoByWordNo(iWord);
	for(CSVI i =  m_SynVariants.begin() ; i != m_SynVariants.end(); i++ )
	{
		if ( i->m_SynUnits[UnitNo].m_iHomonymNum !=  iHomonym) continue;

		const CGroup* G = GetLastHost(iWord, i);
		if (G != NULL ) 
			return G;
	};

	return NULL;

};




void CClause::DeleteMarkedHomonyms(int iW)
{
	CSynWord& word = GetWords()[iW];
	for(int iH = word.GetHomonymsCount() - 1 ; iH >= 0 ; iH-- )
	{
		if	(	word.m_Homonyms[iH].m_bDelete )
			DeleteHomonym(iW, iH);
	}
}

SVI CClause::EraseMorphVariant(SVI pSynVar)
{
	//for(int j = 0 ; j < pSynVar->m_vectorGroupsIds.size() ; j++ )
	//	DeleteGroupInClauseGroupMap( pSynVar->m_vectorGroupsIds[j]);

	return m_SynVariants.erase( pSynVar );
};


void CClause::DeleteHomonym(int iW,int  iH)
{
	CSynWord& word  = GetWords()[iW];
	
	assert ( m_pSent->m_Words[iW].m_Homonyms.size() > 1);
	
	rml_TRACE("Delete homonym \"%s\" from Word \"%s\"(Only %i homonyms left)(InOborot=%s)\n", 
		m_pSent->m_Words[iW].m_Homonyms[iH].m_strLemma.c_str(), 
		m_pSent->m_Words[iW].m_strWord.c_str(), 
		m_pSent->m_Words[iW].m_Homonyms.size(),
		m_pSent->m_Words[iW].m_Homonyms[iH].m_bInOb ? "true" : "false"
		);

	int SynVarWordNo = UnitNoByWordNo(iW);

    
	//	уничтожаем синварианты и группы с этим омонимом 
	SVI pSynVar = m_SynVariants.begin();

	while(pSynVar !=  m_SynVariants.end())
	{
		const CMorphVariant& synVar = *pSynVar;
		if( synVar.m_SynUnits[SynVarWordNo].m_iHomonymNum ==  iH ) 
			pSynVar = EraseMorphVariant(pSynVar );
		else
			pSynVar++;
	}

    
	
	//	удаляем омоним у слова  
	word.EraseHomonym( iH );


	//	вершина клаузы могла указывать на этот омоним, такие вершины нао удалить 
	for(int TypeNo = 0 ; TypeNo < m_vectorTypes.size() ; TypeNo++ )
		if( m_vectorTypes[TypeNo].m_Root.m_WordNo == iW) 
			if(m_vectorTypes[TypeNo].m_Root.m_HomonymNo == iH) 
			{
				DeleteClauseType( TypeNo );
				TypeNo--;
			}
			else
				m_vectorTypes[TypeNo].m_Root.UpdateWhenDeleteHomonym(iW,iH);
					
	m_RelativeWord.UpdateWhenDeleteHomonym(iW, iH);
	if (m_RelativeWord.m_HomonymNo == -1)
		m_RelativeWord.m_WordNo = -1;

		
	// уменьшить номер омонимов во всех морфологических вариантах
	// и заново определить СSynVariant::m_ClauseTypeNo, поскольку
	// вектор типов был изменен
	for(pSynVar = m_SynVariants.begin(); pSynVar !=  m_SynVariants.end() ; pSynVar++)
	{
		CMorphVariant& synVar = *pSynVar;
		assert (synVar.m_SynUnits[SynVarWordNo].m_iHomonymNum != iH);

		if( synVar.m_SynUnits[SynVarWordNo].m_iHomonymNum > iH )
			synVar.m_SynUnits[SynVarWordNo].m_iHomonymNum--;

		m_pSent->ChooseClauseType(m_vectorTypes, synVar);
	}

	word.UpdateConjInfo();

};



bool CClause::IsGoodVariant(const CMorphVariant& synVariant) const 
{
	bool bOb1 = false, bInOb = false;

	int i = 0 ;
	for(; i < synVariant.m_SynUnits.size() ; i++ )
	{
		int iW = synVariant.GetUnitLastWord(i);
		int iHom = synVariant.m_SynUnits[i].m_iHomonymNum;
		if( synVariant.m_SynUnits[i].m_Type == EClause )
			continue;
		const CSynHomonym& hom = GetWords()[iW].GetSynHomonym(iHom);


		if( hom.IsOb1() && hom.IsOb2())
		{
			bOb1 = false;
			bInOb = false;
			continue;
		}
		if( hom.IsOb1() )
			bOb1 = true;

		if( hom.IsIsOb() && !bOb1 )
			break;

		if( !hom.IsIsOb() && bOb1 )
			break;

		if( hom.IsIsOb() )
			bInOb = true;

		if( hom.IsOb2() && !bOb1 )
			break;

		if( hom.IsOb2() )
		{
			bOb1 = false;
			bInOb = false;
		}
	}
	return i == synVariant.m_SynUnits.size();
}


EUnitType CClause::GetUnitType(int iUnit) const
{
	if( m_SynVariants.empty())
		return EWord;
	else
		return m_SynVariants.begin()->m_SynUnits[iUnit].m_Type;
}

int CClause::FillRelNodeByClauseUnit(const CMorphVariant& V, int iUnit,  CClauseRelation& rel)
{
	const CSynUnit& Unit = V.m_SynUnits[iUnit];
	assert( Unit.m_Type == EClause );
	const CClause* clause = m_pSent->FindClauseByPeriod(Unit.m_SentPeriod);
	assert(clause);
	rel.m_iClauseNo = m_pSent->FindClauseIndexByPeriod(Unit.m_SentPeriod);
	rel.m_pClauseVar = &V;

	if ( clause->m_vectorConjs.size() > 0 )
		return clause->m_vectorConjs[0].m_iFirstWord;

	if (Unit.m_iClauseTypeNum == -1)
		return clause->m_iFirstWord;
																	
    if (clause->m_vectorTypes[Unit.m_iClauseTypeNum].m_Root.m_WordNo != -1) // COPUL_T
    	return clause->m_vectorTypes[Unit.m_iClauseTypeNum].m_Root.m_WordNo;
    else
        return clause->m_iFirstWord;

}


void CClause::RecalculateRelationsCoordinates(int iClause)
{
	for(SVI pSynVar = m_SynVariants.begin() ; pSynVar != m_SynVariants.end(); pSynVar++ )
	{
		CMorphVariant& V = *pSynVar;
        for(int i = 0 ; i < V.m_vectorGroups.m_Relations.size() ; i++ )
		{
			CRelation& rel = V.m_vectorGroups.m_Relations[i];

			if( GetUnitType(UnitNoByWordNo(rel.m_iFirstWord)) == EClause ) 
			{
				rel.m_iFirstWord = FillRelNodeByClauseUnit(V, UnitNoByWordNo(rel.m_iFirstWord), rel.m_SourceClause );
				rel.m_SourceType = EClause;
			}
			else
				rel.m_SourceClause.m_iClauseNo = iClause;

			if( GetUnitType(UnitNoByWordNo(rel.m_iLastWord)) == EClause )
			{
				rel.m_iLastWord = FillRelNodeByClauseUnit(V, UnitNoByWordNo(rel.m_iLastWord), rel.m_TargetClause );
				rel.m_TargetType = EClause;					 
			}
			else
				rel.m_TargetClause.m_iClauseNo = iClause;
			rel.AssertValid();
			if ( -1 == rel.m_iLastWord || -1 == rel.m_iFirstWord )
				m_pSent->OutputErrorString("Syntactic relations coordinates for Semantics are wrong!");
		}
	}
}





void CClause::AssignOborotMarksToDisruptConj(const CFormatCaller& FormatCaller, CMorphVariant&  synVariant)
{
	// удаляем интерпретацию оборота у омонимов, которые вошли в разрывный  союз 
	int i = 0;
	for ( ; i < FormatCaller.sent.size(); i++ )
		if (FormatCaller.sent[i].m_DisruptOborotId != UnknownSyntaxElement)
		{
			int iW = SynVarToPrmClForLastWord( i);
			if( synVariant.GetUnitType(i) == EWord )
				m_pSent->DeleteOborotThatContains(iW);
		};

	// простовляем новую интерпретацию оборота у омонимов, которые вошли в разрывный  союз 
	for (i = 0; i < FormatCaller.sent.size(); i++ )
		if (FormatCaller.sent[i].m_DisruptOborotId != UnknownSyntaxElement)
		{
			int iW = SynVarToPrmClForLastWord( i);
			if( synVariant.GetUnitType(i) == EWord )
			{
				int iHom = synVariant.GetHomNum(i);
				GetWords()[iW].GetSynHomonym(iHom).m_bOborot1 = true;
				GetWords()[iW].GetSynHomonym(iHom).m_bOborot2 = true; 
				GetWords()[iW].GetSynHomonym(iHom).m_bInOb = true;
				GetWords()[iW].GetSynHomonym(iHom).m_OborotNo =  FormatCaller.sent[i].m_DisruptOborotId;
			}
		}
}




//если в synVariant фрагмента (клаузы) существует два предиката
bool CClause::IsTwoPotentialPredikatesInOneClause(const CMorphVariant& synVar) const
{
	if (!m_pSent->m_bShouldUseTwoPotentialRule) return false;
	return m_pSent->GetCountOfStrongRoots(*this, synVar)>1;
}

void CClause::RecalculateIndicesAfterInsertingWord(int InsertWordNo)
{
	if (m_iFirstWord >= InsertWordNo)
		m_iFirstWord++;

	if (m_iLastWord >= InsertWordNo)
		m_iLastWord++;

	int i = 0;
	for (; i < m_vectorConjs.size(); i++)
	{
		if (m_vectorConjs[i].m_iFirstWord >= InsertWordNo)
			m_vectorConjs[i].m_iFirstWord++;
		if (m_vectorConjs[i].m_iLastWord >= InsertWordNo)
			m_vectorConjs[i].m_iLastWord++;
	}

	for (i = 0; i < m_vectorTypes.size(); i++)
		m_vectorTypes[i].m_Root.UpdateWhenInsertWord(InsertWordNo);

	if (m_AntecedentWordNo != -1)
		if (m_AntecedentWordNo >= InsertWordNo)
			m_AntecedentWordNo++;

	m_RelativeWord.UpdateWhenInsertWord(InsertWordNo);
			

}


bool CClause::IsInTermin(int iWrdNum)
{
	for (int i = 0; i < m_pSent->m_vectorTermins.size(); i++)
		if (m_pSent->m_vectorTermins[i].m_iFirstWord <= iWrdNum &&
			m_pSent->m_vectorTermins[i].m_iLastWord > iWrdNum)
			return true;

	return false; 
}

void CClause::AddVariantWeightWithHomOneToFifty()
{
	for (SVI i = m_SynVariants.begin(); i != m_SynVariants.end(); i++)
		for (SVI j = m_SynVariants.begin(); j != m_SynVariants.end(); j++)
		{
			int bIdentic, iHomNumInVar;
			bIdentic = iHomNumInVar = 0;	

			for (int k = 0; k < i->GetUnitsCount(); k++)
				if ( i->GetHomNum(k) != 
					 j->GetHomNum(k) )
				{
					iHomNumInVar = k;
					bIdentic++;
				}
	
			if (bIdentic != 1) continue;

			const CSynUnit& sHomVarF = i->m_SynUnits[iHomNumInVar];
			const CSynUnit& sHomVarS = j->m_SynUnits[iHomNumInVar];

			if (sHomVarF.m_Type == EClause && 
				sHomVarF.m_iHomonymNum > -1 &&
				sHomVarS.m_iHomonymNum > -1) 
					continue;

			int iWrdNum = SynVarToPrmClForLastWord(iHomNumInVar);

			const CSynHomonym& HomF = GetWords()[iWrdNum].GetSynHomonym(sHomVarF.m_iHomonymNum);
			const CSynHomonym& HomS = GetWords()[iWrdNum].GetSynHomonym(sHomVarS.m_iHomonymNum);
		
			if (HomF.m_lFreqHom > 0 && HomS.m_lFreqHom > 0)
			{
				if (HomF.m_lFreqHom/HomS.m_lFreqHom >= 50) 
				 if (i->m_iWeight <  j->m_iWeight)
					i->m_iWeight = j->m_iWeight; 
			}
				
		}
}

void CClause::DeleteClauseTypeInHostClause(CClause* FirstHost,  int TypeNo)
{
	if (FirstHost->m_SynVariants.empty()) return;
	int UnitNo = FirstHost->UnitNoByWordNo(m_iFirstWord);
	assert (UnitNo != -1);
	if (UnitNo == -1) return;
	int VarsCount = FirstHost->m_SynVariants.size();
	for(SVI pSynVar = FirstHost->m_SynVariants.begin(); pSynVar != FirstHost->m_SynVariants.end(); )
	{
		bool bDeleteVariant = false;
		CSynUnit& C = pSynVar->m_SynUnits[UnitNo];

		assert ( (C.m_Type == EClause)  &&  (C.m_SentPeriod == *this) );

		if (C.m_iClauseTypeNum == TypeNo)
		{
			pSynVar = FirstHost->EraseMorphVariant(pSynVar );
			continue;
		}
		else
			if (C.m_iClauseTypeNum > TypeNo)
				C.m_iClauseTypeNum--;
		pSynVar++;
	};
	rml_TRACE ("DeleteClauseTypeInHostClause left only %i from %i synvariants in %s\n", FirstHost->m_SynVariants.size(), VarsCount, FirstHost->GetTraceStr().c_str());

};

void CClause::DeleteClauseType(int ClauseTypeNo)
{
	m_vectorTypes.erase(m_vectorTypes.begin() + ClauseTypeNo);
	// во всех синвариантах нужно уменьшить номера типов, которые стояли после ClauseTypeNo
	for (SVI k=m_SynVariants.begin(); k != m_SynVariants.end(); k++)
		if (k->m_ClauseTypeNo > ClauseTypeNo)
			k->m_ClauseTypeNo--;
		else
			if (k->m_ClauseTypeNo == ClauseTypeNo)
				k->m_ClauseTypeNo = -1;

	int FirstHostNum = m_pSent->GetMininalParentByClauseNo(m_SelfClauseNo);

	if (FirstHostNum != -1)
	{
		DeleteClauseTypeInHostClause (&m_pSent->m_Clauses[FirstHostNum], ClauseTypeNo);
	}
}

void CClause::ChangeAllClauseTypesToOneType(SClauseType Type)
{
	m_vectorTypes.clear();

	if (Type.m_Type != UnknownSyntaxElement)
	{
		m_vectorTypes.push_back(Type);
	};
	
	for (SVI k=m_SynVariants.begin(); k != m_SynVariants.end(); k++)
			if (Type.m_Type == UnknownSyntaxElement)
				k->m_ClauseTypeNo = -1;
			else
				k->m_ClauseTypeNo = 0;

	int FirstHostNum = m_pSent->GetMininalParentByClauseNo(m_SelfClauseNo);

	if (FirstHostNum != -1)
		m_pSent->m_Clauses[FirstHostNum].BuildSynVariants();
		

}

void CClause::DeleteClauseTypeWhichAreNotUsedInAnyOfSynVars()
{
	// если не построилось ни одного варианта надо выйти без разговора
	// Ноль вариантов - это уже ошибка.
	if ( m_SynVariants.empty() ) return;

	for (int i = 0; i < m_vectorTypes.size(); i++)
	{
		CSVI j = m_SynVariants.begin();
		for (; j != m_SynVariants.end(); j++)
			if (i == j->m_ClauseTypeNo)
				break;

		if ( j == m_SynVariants.end() )
		{
			DeleteClauseType(i);
			i--;
		}
	}
}




bool	CClause::HasUnambiguousStrongRootWithoutWeakHomonyms() const
{
	try {
		for (int i = 0; i < m_vectorTypes.size(); i++)
		{
			// Пустыхи и копулы пропускаем
			if (m_vectorTypes[i].m_Root.IsEmpty()) continue;;
			const CSynWord& word  = GetWords()[m_vectorTypes[i].m_Root.m_WordNo];

			int j = 0;

			for (; j < word.GetHomonymsCount(); j++)
				if (! GetOpt()->GetGramTab()->IsStrongClauseRoot(word.GetSynHomonym(j).m_iPoses ))
					break;

			if (j == word.GetHomonymsCount())	return true;
		};
	}
	catch (...)
	{
		m_pSent->OutputErrorString("HasUnambiguousStrongRootWithoutWeakHomonyms has an exception");

	};

	return false;
};


int  CClause::GetMainWordOfTheClauseForThisVariant(const CMorphVariant& M, const CClauseRelation& R ) const
{
	int MainWord = -1;

	EClauseType t =  (M.m_ClauseTypeNo == -1) ? UnknownSyntaxElement : m_vectorTypes[M.m_ClauseTypeNo].m_Type;
	if	(		R.m_ClauseTypes.empty()
			||	( find(R.m_ClauseTypes.begin(), R.m_ClauseTypes.end(), t ) != R.m_ClauseTypes.end())
		)
		MainWord = (M.m_ClauseTypeNo == -1) ? -1 : m_vectorTypes[M.m_ClauseTypeNo].m_Root.m_WordNo ;


	if (MainWord == -1 )
		if ( (GetOpt()->m_Language == morphRussian) && HasType( GetOpt()->m_RusParenthesis ))
			// find the first word which starts an oborot
			for ( int i = m_iFirstWord; i <= m_iLastWord; i++ )
				if ( GetWords()[i].HasOborot1() ) 
				{
					MainWord = i;
				}

	return MainWord;
};


int CClause::GetSynVariantsCount() const
{ 
	return m_SynVariants.size(); 
}

bool	CClause::IsValidCSVI(CSVI it) const
{
	return m_SynVariants.end() !=  it;
}

const CSVI CClause::GetSynVariantByNo(long lVarNum) const
{
	if( ( lVarNum < 0 ) && (lVarNum >= m_SynVariants.size()) )
		return m_SynVariants.end();

	CSVI pSynVar = m_SynVariants.begin();

	for (int i = 0; i < lVarNum; i++)
		pSynVar++;

	return  pSynVar;
}
