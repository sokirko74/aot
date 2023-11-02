// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once

#include "SynPlmLine.h"

#include <unordered_set>


inline bool HasInSet(const std::unordered_set<std::string>& vct, const char* item)
{
	if (item == nullptr) return false;
	return vct.find(item) != vct.end();
}

int FindInList(const char* arrWords, int iLen, const char* word_upper);

class CGroups;

struct SFoundTermin : public CPeriod 
{
	int					m_TerminId;
	const CGroups*		m_pModel;
	EThesType			m_ThesType;
	bool				m_bGroupsChecked;

	SFoundTermin(const CGroups* pModel, EThesType eThesType, int iTerminId, int iW1, int iW2) 
		: CPeriod(iW1, iW2)
	{
		m_pModel = pModel;
		m_ThesType = eThesType;
		m_TerminId = iTerminId;
		m_bGroupsChecked = false;

	}

	bool operator < (const SFoundTermin& X) const
	{
		return m_iLastWord < X.m_iLastWord;
	}

};

typedef EGroupType ERelType;

struct CMorphVariant;

struct CClauseRelation 
{
		int						m_iClauseNo;
		const CMorphVariant*	m_pClauseVar;
		CPeriod					m_ClausePeriod;//используется только для межклаузных связей
		std::vector<EClauseType>		m_ClauseTypes; //используется только для межклаузных связей
		CClauseRelation();
};

struct CRelation : public CPeriod 
{
		uint64_t		m_iGrammems;
		std::string		m_GramCodes;
		EUnitType	m_SourceType;
		EUnitType	m_TargetType;
		ERelType	type; 

		CClauseRelation			m_SourceClause;
		CClauseRelation			m_TargetClause;

		CRelation ();
		CRelation (EGroupType _type, int iFirst, int iLast );
        void AssertValid();
};

struct CPeriodWithMainWord : public CPeriod
{
	/*
		the main word of the group 
	*/
	short				m_MainWordNo;

	CPeriodWithMainWord() : CPeriod()
	{
		m_MainWordNo = -1;
	};

};

class CTypedPeriod : public CPeriodWithMainWord
{
public:
	// type of the group
	EGroupType	m_GroupType; 

};

typedef std::vector<int> CIntVector;


class CGroup : public CTypedPeriod
{
	// grammems of the group, they can be  differ from the grammems of the main word 
	uint64_t			grammems;

public:


	// the main dubgroup
	CPeriod			m_MainGroup;
	
	// an explanation of this group
	const char*		m_Cause;

	// the rule number 
	char			m_RuleNo;




	// thesaurus interpretation of this group:
	// ID thesaurus  entry 
	int				m_iTerminID;
	// the type of the thesaurus
	int				m_iThesType;

	// Only for Russian, true, if the numeral value of the group  is more than 999
	bool			m_bNumeralMoreThanThousand;
    bool            m_bRussianNounNumeralGroup;

	bool			m_bCommaDelimited;

    bool            m_bAlreadyBuiltRelations;    

	//  used only for GLR Grammar
	std::string			m_GramCodes;
	CPeriod			m_OtherGroup;
    
	CGroup();
 	CGroup(size_t _first, size_t _last, const char* _cause);
	void CopyGroup(const CGroup& src_group);
	void Reset();

	
	
	//CGroup& operator =(const CGroup& src_group);
	

	uint64_t GetGrammems() const;
	void SetGrammems(uint64_t Grammems);
	void SetGrammems(uint64_t Grammems, const char * GramCodes);
	void SetGrammems(CSynPlmLine W);
};




//  this class contains only one slot   m_Groups, which contains groups ordered by GladkijPeriodLess

class CGroupsVector 
{
protected:
	std::vector<CGroup>		m_Groups;

public:
	// syntactic relation
	std::vector<CRelation>	m_Relations;
	
	const std::vector<CGroup>&	GetGroups() const {return m_Groups; };
	CGroup*					GetGroupPtr(int i)  {return &(m_Groups[i]); };
	void					Clear();

	int					get_main_word(size_t WordNo) const;
	int					get_next_main_word(size_t WordNo) const;
	int					get_main_word_in_group(const CPeriodWithMainWord& G) const;
	int					create_group (const CGroup& G);
	void				del_group (size_t GroupNo);
	void				del_group_by_period (CPeriod& P);
	int					get_minimal_group(size_t WordNo) const;
	int					get_maximal_group_no(size_t WordNo)  const;
	int					find_group		(const CPeriod&  P)  const; 
	bool				is_free (size_t WordNo)	{ return get_minimal_group(WordNo) == -1;};
	int					get_maximal_subgroup (const CPeriod& group, size_t WordNo)  const;
	
};



class CSyntaxOpt;

class CGroups  : public CGroupsVector
{
protected:
	const CSyntaxOpt*	m_pSyntaxOptions;
	std::vector<CGroup>		m_GroupsToRebuild;
	std::vector<CGroup>		m_AtomicGroups;
	

public:

	CPlmVector      sent;

	CGroups();
	CGroups(const CSyntaxOpt* Opt);
	CGroups(const CGroups& G);
	CGroups& operator=(const CGroups& G);

	const CSyntaxOpt* GetOpt() const { return  m_pSyntaxOptions;};



	// common const parameters
	const CGroup&		get_maximal_subgroup (size_t GroupNo, size_t WordNo)  const; 		   
	const CGroup&		get_maximal_subgroup (const CPeriod& group, size_t WordNo)  const; 		   
	size_t				get_maximal_group_size(size_t WordNo)  const;

    const CGroup&		get_maximal_group(size_t WordNo)  const;

    void				get_full_sentence(std::string& str_sent) const;
	bool				is_noun_group (const CGroup& G) const;

	
	bool				is_only_comma_delimited(const CGroup& G) const;

    // changers
	void	change_words_in_group_grammems(const CPeriod& group, uint64_t grammems, uint64_t breaks);
	bool    change_words_in_group_gramcodes(const CPeriod& group, const std::string& gramcodes, GrammemCompare CompareFunc);
	void	change_words_in_group_grammems(const CPeriod& group, uint64_t grammems);
	void	change_group_grammems(CGroup& group, uint64_t grammems, uint64_t breaks);
	void	BuildSimilarSynRels(CGroup& G,int iGroupNum);
	void	create_syn_rel(CGroup& new_group, int iSource, int iTarget, EGroupType type);
	void	AddWord(const CSynPlmLine& C);
	void	Reset();
	void	SetGrammems(int WordNo, uint64_t Grammems, const char* Gramcodes);
	void	SetMainWordNo(CGroup& G);
	void	BuildDefaultSynrels(size_t GroupNo);
	void	BuildAutomaticSynrels();
	void	ResizeAtomicDummy(size_t Number);
};
