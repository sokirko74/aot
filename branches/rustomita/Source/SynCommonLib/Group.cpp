// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma warning(disable:4786)
#include "stdafx.h"

#include "Group.h"
#include "SyntaxInit.h"
#include "MorphVariant.h"

// ================================================
// ================  CRelation ====================
// ================================================
CClauseRelation::CClauseRelation()
{
	m_iClauseNo = -1;
	m_pClauseVar = 0;
};

CRelation::CRelation ()
{
	m_SourceType = EWord;
	m_TargetType = EWord;
	m_iGrammems = 0;
}

CRelation::CRelation (EGroupType _type, int iFirst, int iLast )
{
	m_iGrammems = 0;
	m_SourceType = EWord;
	m_TargetType = EWord;
	type = _type;
	m_iFirstWord = iFirst;
	m_iLastWord = iLast;		
};


void CRelation::AssertValid()
{
    assert (m_iFirstWord >= 0);
    assert (m_iLastWord >= 0 );
}


// ================================================
// ================  CGroup ====================
// ================================================

CGroup::CGroup() : CTypedPeriod()
{
	Reset();
};

CGroup::CGroup(size_t _first, size_t _last, const char* _cause)
{
	Reset ();
	m_iFirstWord = _first;
	m_iLastWord  = _last;
	m_Cause = _cause;
}


void CGroup::Reset()
{
	m_Cause = NULL;
	m_MainGroup.m_iFirstWord = 0;
	m_MainGroup.m_iLastWord = 0;
	m_GroupType = UnknownSyntaxElement;
	m_iFirstWord =  0;
	m_iLastWord = 0;
	grammems = 0;
	m_RuleNo = -1;
	m_iTerminID = -1;
	m_iThesType = NoneThes;
	m_bNumeralMoreThanThousand = false;
	m_MainWordNo = -1;
	m_bCommaDelimited = false;
    m_bAlreadyBuiltRelations =  false;
};



void CGroup::SetGrammems(QWORD Grammems)
{
	grammems = Grammems;
}

QWORD CGroup::GetGrammems() const
{
	return grammems;
}




/*CGroup& CGroup:: operator =(const CGroup& src_group)
{
	CopyGroup(src_group);
	return *this;
}*/

//==============================================================


void	CGroupsVector :: Clear()
{
	m_Groups.clear();
};

//  return maximal group for the sequence 
//  which is ordered by GladkijPeriodLess
int CGroupsVector :: get_maximal_group_no(size_t WordNo)  const 
{
	vector<CGroup>::const_iterator begin = GetGroups().begin();
	vector<CGroup>::const_iterator end = GetGroups().end();
	if (begin == end) return -1;

	CPeriod P(WordNo);

	do 
	{
		end--;
		if(P.is_part_of(*end))
			return end - begin;
	} while(end != begin);


	return -1;
};

// 1.Если нет группы, в которую входит WordNo, то функция возращает WordNo.
// 2.Иначе функция возращает номер первого слова самой минимальной из 
//   главных групп.
  
int CGroupsVector :: get_main_word(size_t WordNo)  const 
{
   int i = get_maximal_group_no (WordNo);
   if (i != -1)  
	   return GetGroups()[i].m_MainWordNo;
   else
		return  WordNo;
};


int CGroupsVector :: get_next_main_word(size_t WordNo)  const 
{
	int i  = get_maximal_group_no (WordNo);
	if (i == -1)
		WordNo++;
	else 
		WordNo =  GetGroups()[i].m_iLastWord + 1;

	return get_main_word(WordNo);
};

int CGroupsVector :: get_main_word_in_group(const CPeriodWithMainWord& G) const
{
	assert (G.m_MainWordNo != -1);
	return G.m_MainWordNo;
};


// Все подгруппы группы G должны стоять слева от G,
// но не все группы, стоящие слева, являются ее подгруппами.
// Все подгруппы, которые стоят слева, либо являются ее подгруппами,
// либо стоят левее по предложению. Таким образом, на множестве
// groups задан строгий порядок. 
 
int CGroupsVector :: create_group (const CGroup& G)
{
	
	assert (G.m_MainWordNo <= G.m_iLastWord);
	assert (G.m_MainWordNo >= G.m_iFirstWord);

	int i = 0;

	for (; (i<GetGroups().size()) && GladkijPeriodLess(GetGroups()[i],G); i++);

	m_Groups.insert (m_Groups.begin() + i,G);

	return i;
};


int CGroupsVector :: find_group (const CPeriod&  P)  const 
{
	for (size_t i =  0; i < GetGroups().size(); i++)
		if (P == (CPeriod)GetGroups()[i])
			return i;

    return -1;
};

void CGroupsVector :: del_group (size_t GroupNo)
{
    for (int i=m_Relations.size()-1; i>=0; i--)
        if ( m_Relations[i].is_part_of(m_Groups[GroupNo]) )
            m_Relations.erase(m_Relations.begin() + i);
    m_Groups.erase (m_Groups.begin()+GroupNo);
};

void CGroupsVector :: del_group_by_period (CPeriod& P)
{
	int iGr = find_group(P);
	if( iGr != -1 )
	{
		del_group (iGr); 
	}
};

int CGroupsVector :: get_minimal_group (size_t WordNo) const 
{
	for (size_t i =  0; i < GetGroups().size(); i++)
		if  ((GetGroups()[i].m_iFirstWord <= WordNo) &&  (WordNo <= GetGroups()[i].m_iLastWord))
			return i;

	return -1;
};


/*
	return the maximal subgroup of group which contains WordNo
*/
int CGroupsVector::get_maximal_subgroup (const CPeriod& group, size_t WordNo)  const
{
	int i = find_group(group);

	if( i == -1 )
		i = GetGroups().size() - 1;
	else
		i = i - 1;

	for( ; i >= 0 ; i--)
	{		
		if( GetGroups()[i].is_part_of(group) &&  CPeriod(WordNo, WordNo).is_part_of(GetGroups()[i]))		
			return i;		
	}

    return -1;
} 		   


//==============================================================

CGroups::CGroups() 
{	
	m_pSyntaxOptions = 0;
	assert(false);
};

CGroups::CGroups(const CSyntaxOpt* Opt) 
{	
	m_pSyntaxOptions = Opt;
};

CGroups& CGroups::operator = (const CGroups& G)
{
	sent = G.sent;
	m_AtomicGroups = G.m_AtomicGroups;
	m_Groups = G.GetGroups();
	m_pSyntaxOptions = G.GetOpt();
	return *this;
};

CGroups :: CGroups(const CGroups& G)
{
	*this = G;
};


void   CGroups :: get_full_sentence(string& str_sent) const 
{
	for(int i = 0 ; i < sent.size(); i++)
		if (sent[i].get_word())
		str_sent += sent[i].get_word()+string(" ");
}


bool CGroups :: is_noun_group (const CGroup& G) const
{
    return sent[G.m_MainWordNo].is_morph_noun();
}

bool CGroups :: has_sub_clauses(size_t GroupNo)  const 
{
	for (int i = GetGroups()[GroupNo].m_iFirstWord; i <= GetGroups()[GroupNo].m_iLastWord; i++)
		if  (	sent[i].m_UnitType == EClause)  
			return true;

	return false;
};

/*
	выдает самую большую группу, которая включает WordNo и которая не включает клауз
*/
int CGroups :: get_maximal_group_no_without_clause(size_t WordNo)  const 
{
	vector<CGroup>::const_iterator begin = GetGroups().begin();
	vector<CGroup>::const_iterator iterat = GetGroups().end();

	if (begin == iterat) return -1;

	CPeriod P(WordNo);

	for (iterat--; iterat >= begin; iterat--)
		if  (		P.is_part_of(*iterat) 
			&&	!has_sub_clauses(iterat - begin)
			)

			return iterat - begin;

	return -1;
};

const CGroup& CGroups :: get_maximal_group(size_t WordNo)  const 
{
	int i = get_maximal_group_no (WordNo);	
	if (i != -1)
		return GetGroups()[i];
	else
		return m_AtomicGroups[WordNo];
};


const CGroup* CGroups :: get_maximal_group_ptr(size_t WordNo)  const 
{
	int i = get_maximal_group_no (WordNo);	
	if (i != -1)
		return &(GetGroups()[i]);
	else
		return &(m_AtomicGroups[WordNo]);
};



void   CGroups::change_words_in_group_grammems(const CPeriod& group, QWORD grammems, QWORD breaks)
{
	for(int i = group.m_iFirstWord ; i <= group.m_iLastWord ; i++ )
	{
		sent[i].SetGrammems(sent[i].GetGrammems() & (grammems | ~breaks));
		m_AtomicGroups[i].SetGrammems(sent[i].GetGrammems());
	};
}

void   CGroups::change_group_grammems(CGroup& group, QWORD grammems, QWORD breaks)
{
	group.SetGrammems(group.GetGrammems() & (grammems | ~breaks));
}

/*
	return the maximal subgroup of group which contains WordNo
*/
const CGroup& CGroups::get_maximal_subgroup (const CPeriod& group, size_t WordNo)  const
{
	int i = CGroupsVector::get_maximal_subgroup(group, WordNo);

	if( i != -1 )
		return GetGroups()[i];
	else
		return m_AtomicGroups[WordNo];
} 		   



const CGroup& CGroups :: get_maximal_subgroup (size_t GroupNo, size_t WordNo)  const 
{
	assert  (m_AtomicGroups[WordNo].is_part_of (GetGroups()[GroupNo]));

	for (int i = (int)GroupNo-1; i >=0; i--)
		if (m_AtomicGroups[WordNo].is_part_of (GetGroups()[i]))
			return GetGroups()[i];

    return m_AtomicGroups[WordNo];
};



QWORD CGroups :: get_group_grammems(size_t WordNo) const
{
 int i = get_maximal_group_no_without_clause(WordNo);	
 if (i != -1)
	 return GetGroups()[i].GetGrammems();
 else
 	 return sent[WordNo].GetGrammems();
}

size_t CGroups :: get_maximal_group_size(size_t WordNo)  const 
{
	return get_maximal_group(WordNo).size();
};


void  CGroups::SetMainWordNo(CGroup& G)
{
    if (G.m_MainGroup == G)
          G.m_MainGroup = G.m_iFirstWord;		

	if (G.m_MainGroup.m_iFirstWord ==  G.m_MainGroup.m_iLastWord)
		G.m_MainWordNo = G.m_MainGroup.m_iFirstWord;		
	else
	{
		int MainGroupNo = find_group(G.m_MainGroup);
		assert (MainGroupNo != -1);
		G.m_MainWordNo = m_Groups[MainGroupNo].m_MainWordNo;		
	}
};









void CGroups :: BuildSimilarSynRels(CGroup& G, int iGroupNum)
{
	
	//	finding an independent conjunction, if no conjunction is found than the first comma
	
	int iComma = -1;
	size_t i = G.m_iFirstWord;
	for(  ; i <= G.m_iLastWord;  )
	{
		const CGroup& child = get_maximal_subgroup(G, i);
		int main_word_no = get_main_word_in_group(child);
		int conj_type =  sent[main_word_no].get_coord_conj_no();
		if(  conj_type != -1 )
			break;


		 if( iComma == -1 )
			if (sent[main_word_no].get_word() )
				if(    sent[main_word_no].get_word()[0] == ','  
					|| sent[main_word_no].get_word()[0] == '(' 
					|| sent[main_word_no].get_word()[0] == ')' 
					|| sent[main_word_no].get_word()[0] == '-'
				)
				iComma = main_word_no;

		i += child.size();		
	}

	// iSource is a word which subdues all maximal subgroups
	int iSource = -1;
	if( i == G.m_iLastWord + 1 )
		iSource = iComma;
	else
		iSource = i;

    if(iSource  == -1) return; // возможна пунктуационная ошибка входного текста
	

	// connect iSource to all maximal subgroups, except itself
	for(i = G.m_iFirstWord ; i <= G.m_iLastWord ; )
	{
		const CGroup& child = get_maximal_subgroup(G,i);
		int main_word_no = get_main_word_in_group(child);
		if (main_word_no != iSource)
			create_syn_rel(G, iSource, main_word_no, G.m_GroupType);
		i += child.size();		
	}

	// correcting all relations, which go inside this group, 
	// We are to do it, since the main word of a SIMILAR_GROUP is not a root in the dependency tree.
	// The root is normally a conjunction and the main word is th first word of the group.
	for(int j = 0 ; j < m_Relations.size() ; j++ )
	{
		CRelation& rel = m_Relations[j];
        if (rel.is_part_of(G)) continue;

		if(CPeriod(rel.m_iFirstWord).is_part_of(G) )
			rel.m_iFirstWord = iSource;

		if(CPeriod(rel.m_iLastWord).is_part_of(G) )
			rel.m_iLastWord = iSource;
        rel.AssertValid();
	}
};


bool CGroups::is_only_comma_delimited(const CGroup &G) const
{
	for ( int i = G.m_iFirstWord; i <= G.m_iLastWord; i++ )
		if(sent[i].get_coord_conj_no() !=  -1)  
				return false;


	return true;
}


void CGroups::create_syn_rel(CGroup& new_group, int iSource, int iTarget, EGroupType type)
{
	CRelation new_rel;
    assert  (new_rel.m_iFirstWord < sent.size());
    assert  (new_rel.m_iLastWord < sent.size());
	new_rel.m_iFirstWord = iSource;
	new_rel.m_iLastWord = iTarget;
	new_rel.type = type;
	new_rel.m_iGrammems = new_group.GetGrammems();
    new_rel.AssertValid();
	m_Relations.push_back(new_rel);
    new_group.m_bAlreadyBuiltRelations = true;
    
}



void CGroups::AddWord(const CSynPlmLine& C)
{
	size_t i = sent.size();

	sent.push_back(C);
	CGroup P (i, i,"Atomic group");
	P.SetGrammems(sent[i].GetGrammems());
	P.m_MainWordNo = i;
	m_AtomicGroups.push_back (P); 
	assert (m_AtomicGroups.size() == sent.size());
};

void CGroups::ResizeAtomicDummy(size_t Number)
{
	Reset();
	for (size_t i=0; i < Number; i++)
		AddWord(CSynPlmLine());
};

void CGroups::Reset()
{
	sent.clear();
	m_Groups.clear();
	m_AtomicGroups.clear();
	m_GroupsToRebuild.clear();
    m_Relations.clear();
}


void CGroups::SetGrammems(int WordNo, QWORD Grammems, const char* Gramcodes)
{
	sent[WordNo].SetGrammems(Grammems);
	sent[WordNo].m_gramcodes = Gramcodes;
	m_AtomicGroups[WordNo].SetGrammems(Grammems);		
}

void  CGroups::BuildDefaultSynrels(size_t GroupNo) 
{
	CGroup& G = m_Groups[GroupNo];
	size_t WordNo=G.m_iFirstWord;
	size_t MainWordNo = get_main_word_in_group(G);
	for (; WordNo <= G.m_iLastWord; )
	{
		const CGroup& Child = get_maximal_subgroup(GroupNo, WordNo);
		WordNo = Child.m_iLastWord+1;
		if (!(Child == G.m_MainGroup))
			create_syn_rel(G,MainWordNo,get_main_word_in_group(Child),G.m_GroupType);
	};
};


void  CGroups::BuildAutomaticSynrels()
{

	for (size_t GroupNo = 0; GroupNo<m_Groups.size(); GroupNo++)
	{
		CGroup& G = m_Groups[GroupNo];
        if (G.m_bAlreadyBuiltRelations) continue;
        

		if (GetOpt()->IsSimilarGroup(G.m_GroupType)) continue;

		if (G.m_GroupType == GetOpt()->m_GenNounGroupType)
		{
			int w1 = get_main_word_in_group(G);
			int w2 = get_main_word_in_group(get_maximal_subgroup(GroupNo, G.m_iLastWord));
			create_syn_rel(G, w1, w2, GetOpt()->m_GenNounGroupType);				
		}
		else
		if (G.m_GroupType == GetOpt()->m_PrepNounGroupType )
		{
			create_syn_rel(G, get_main_word_in_group(G), get_main_word_in_group(get_maximal_subgroup(GroupNo, G.m_iLastWord)), GetOpt()->m_PrepNounGroupType);
		}
		else
		if (G.m_GroupType == GetOpt()->m_DirObjGroupType )
		{
			create_syn_rel(G, get_main_word_in_group(G), get_main_word_in_group(get_maximal_subgroup(GroupNo, G.m_iLastWord)), GetOpt()->m_DirObjGroupType);
		}
		else
		if (G.m_GroupType == GetOpt()->m_NPGroupType)
		{
			int i1 = get_main_word_in_group(G);
			int i2 = get_main_word_in_group(get_maximal_subgroup(GroupNo, G.m_iLastWord));
			if( i1 != i2 )	
				create_syn_rel(G, i1, i2, G.m_GroupType);
			else
			{
				int i3 = get_main_word_in_group(get_maximal_subgroup(GroupNo, G.m_iFirstWord));
				assert(i3 != i1);
				create_syn_rel(G, i1, i3, G.m_GroupType);
			}
		}
		else
		if (G.m_GroupType == GetOpt()->m_NounAdjGroupType)
		{
			const CGroup& LastChild = get_maximal_subgroup(G, G.m_iLastWord);

			create_syn_rel(G, get_main_word_in_group(G), 
									get_main_word_in_group(LastChild), GetOpt()->m_NounAdjGroupType);
		}
		else
		if (G.m_GroupType == GetOpt()->m_OborotGroupType)
		{
			// no relations  for  oborots, since they should  be fixed groups
		}
		else
		{
				BuildDefaultSynrels(GroupNo);
		};
	}

	for (size_t GroupNo = 0; GroupNo<m_Groups.size(); GroupNo++)
	{
		CGroup& G = m_Groups[GroupNo];

        if (G.m_bAlreadyBuiltRelations) continue;

		if (GetOpt()->IsSimilarGroup(G.m_GroupType))
		    BuildSimilarSynRels(G, GroupNo);
	};

};






