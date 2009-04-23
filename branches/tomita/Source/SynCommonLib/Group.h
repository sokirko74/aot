// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef group_h
 #define group_h


#include "SynPlmLine.h"




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
		CPeriod					m_ClausePeriod;//используетс€ только дл€ межклаузных св€зей
		vector<EClauseType>		m_ClauseTypes; //используетс€ только дл€ межклаузных св€зей
		CClauseRelation();
};

struct CRelation : public CPeriod 
{
		QWORD		m_iGrammems;
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

typedef vector<int> CIntVector;


class CGroup : public CTypedPeriod
{
	// grammems of the group, they can be  differ from the grammems of the main word 
	QWORD			grammems;

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

	bool			m_bCommaDelimited;

    bool            m_bAlreadyBuiltRelations;    

	//  used only for GLR Grammar
	string			m_GramCodes;
    
	CGroup();
 	CGroup(size_t _first, size_t _last, const char* _cause);
	void CopyGroup(const CGroup& src_group);
	void Reset();

	
	
	//CGroup& operator =(const CGroup& src_group);
	

	QWORD GetGrammems() const;
	void SetGrammems(QWORD Grammems);

};




//  this class contains only one slot   m_Groups, which contains groups ordered by GladkijPeriodLess

class CGroupsVector 
{
protected:
	vector<CGroup>		m_Groups;

public:
	// syntactic relation
	vector<CRelation>	m_Relations;
	
	const vector<CGroup>&	GetGroups() const {return m_Groups; };
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
	vector<CGroup>		m_GroupsToRebuild;
	vector<CGroup>		m_AtomicGroups;
	

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
	int					get_maximal_group_no_without_clause(size_t WordNo)  const;
	const CGroup&		get_maximal_group(size_t WordNo)  const;
	const CGroup*		get_maximal_group_ptr(size_t WordNo)  const;
	bool				has_sub_clauses(size_t GroupNo)  const;
	QWORD				get_group_grammems(size_t WordNo) const;
	void				get_full_sentence(string& str_sent) const;
	bool				is_noun_group (const CGroup& G) const;

	
	bool				is_only_comma_delimited(const CGroup& G) const;
	bool				is_only_comma_with_pronoun_p_in_group(const CGroup& G) const;

	// changers
	void	change_words_in_group_grammems(const CPeriod& group, QWORD grammems, QWORD breaks);
	void	change_group_grammems(CGroup& group, QWORD grammems, QWORD breaks);
	void	BuildSimilarSynRels(CGroup& G,int iGroupNum);
	void	create_syn_rel(CGroup& new_group, int iSource, int iTarget, EGroupType type);
	void	AddWord(const CSynPlmLine& C);
	void	Reset();
	void	SetGrammems(int WordNo, QWORD Grammems, const char* Gramcodes);
	void	SetMainWordNo(CGroup& G);
	void	BuildDefaultSynrels(size_t GroupNo);
	void	BuildAutomaticSynrels();
	void	ResizeAtomicDummy(size_t Number);
};

//extern int get_maximal_group_no(vector<CGroup>::const_iterator begin, vector<CGroup>::const_iterator end, size_t WordNo);



#endif
