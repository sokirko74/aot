// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef clause_h
#define clause_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"
#include "Word.h"
#include "Group.h"
#include "MorphVariant.h"
#include "FormatCaller.h"

class CSentence;

#pragma warning(disable:4786) 

enum ConjFromWhatList {FROM_OBOR_DIC, FROM_SUB_CONJ};

struct SConjIndex : public CPeriod
{
	ConjFromWhatList	m_FromWhere;
	int					m_index;

	SConjIndex(ConjFromWhatList FromWhere, int index,int iWord1, int iWord2) 
		:CPeriod(iWord1,  iWord2)
	{
		m_FromWhere = FromWhere;
		m_index = index; //index from OBOR_DIC  or FROM_SUB_CONJ
	};
};





class CClauseCollection;

typedef std::pair<int, int> CIntPair;
//typedef std::map<int, CGroup> CGroupMap;



struct CBuildingUnit 
{
	int			m_WordNo;
	int			m_HomonymsCount;
	int			m_MaxUsedHomonymNo; 
	int			m_ChildClauseNo;
	//  this presupposes that one word cannot have more than 64 homonym
	QWORD		m_BestHomonyms;
};




class CClause : public CPeriod
{
public:
	// list of all conjunctions, which  the clause contains
	std::vector<SConjIndex> m_vectorConjs; 

	// pointer to relative word 
	CWordAndHomonym	m_RelativeWord;

	//  a pointer to the found antecedent for the relative word
	int				m_AntecedentWordNo;

	//types of clause
	std::vector<SClauseType> m_vectorTypes;

	//  the number of clause in CClauseCollection::m_Clauses
	size_t	m_SelfClauseNo;
	
	// count of punctuation marks in the clause
	int m_iPunctSignsCount;

	// all found groups in all morphological variants
    //CGroupMap  m_mapGroups;

	std::list<CMorphVariant>	m_SynVariants;

	std::set<CInputSymbol> m_AutomatSymbolInterpetationUnion;

	//the sentence of this  clause (the parent)
	CSentence* m_pSent;
	CWordVector&	GetWords(); 
	const	CWordVector& GetWords() const;
	const	CSyntaxOpt* GetOpt() const; 

	// Should we ignore clause in BuildSynVariants
	// it is done for a test run of BuildSynVariants, when the whole clause structure is changing 
	bool	m_bIgnoreWhileBuildingSynVariants;

	
	CClause(CSentence* pSent, int iFirstWord, int iLastWord);


	bool	IsRelative() const {return !m_RelativeWord.IsEmpty();};
	int		FindType(EClauseType Type) const;
    int     FindCommonType  (const CClause& C ) const;
	bool	HasType (EClauseType Type)  const;
	int		FindClauseSubordConj(const std::string& ConjStr)  const;
	bool	HasSubConj (const std::string& strConj = std::string()) const;
	bool	HasLeftStarter() const;

	// the clause has only one coordinating  conjunction
	bool HasOnlyOneSimConj() const;
	//  the clause has at least one coordinating  conjunction and no subordinate conj.
	bool HasAtLeastOneSimConj() const;
	//the clause has one coord. conj., from the Oborot dictionary
	bool HasOnlyOneSimConjFromOborDic() const; 



	void AddVariantWeightWithHomOneToFifty();
	bool IsInTermin(int iWrdNum);
	void RecalculateIndicesAfterInsertingWord(int iDash);
	void RecalculateRelationsCoordinates(int iClause);
	bool BuildGroups(CFormatCaller& FormatCaller, bool bRebuildAllGroups);
	bool BuildGroupsAndSynVariants(CFormatCaller& FormatCaller);
	bool BuildGroupsForExistingSynVariants(CFormatCaller& FormatCaller, bool bRebuildAllGroups);
	bool GetVariant(CFormatCaller& FormatCaller,bool bFirstTime,CMorphVariant& synVariant, bool& bHasBothoOb1andOb2);
	

	const CGroup* GetLastHost(CPeriod P, CSVI pSynVar) const;
	const CGroup* GetLastFirmHost(int iWord, CSVI pSynVar) const;
	const CGroup* GetMaxLastHost(int iWord) const;
	const CGroup* GetMaxLastHost(int iWord, int iHomonym) const;
	const CGroup* IsInThisGroup(int iWord, EGroupType type, CSVI& Dummy) const ;

	bool KillHomonymsCoverage( );
	
	void DeleteHomonym(int iW,int iH);
	void DeleteMarkedHomonyms(int iW);
	
	bool CanHaveEncloseClauseFromTheLeft() const;

	const CAncodePattern* GetRootAncodePattern(int iType) const;
	bool	HasUnambiguousStrongRootWithoutWeakHomonyms() const;


	bool IsGoodVariant(const CMorphVariant& synVariant) const ;

	int							GetSynVariantsCount() const;
	std::vector<CMorphVariant*>		GetSynVariantIndexesByTypeNum(int i_type);
	const CMorphVariant*		GetSynVariantByClauseType(const SClauseType& type) const;
	const CSVI					GetSynVariantByNo(long lVarNum) const;
	bool						IsValidCSVI(CSVI it) const;

	int PassSubClause(int iWord) const;
	int  UnitNoByWordNo(int iWord) const;
	EUnitType GetUnitType(int iUnit ) const;
	bool	IsTwoPotentialPredikatesInOneClause(const CMorphVariant& synVar) const;
	


	void DeleteClauseTypeWhichAreNotUsedInAnyOfSynVars();
	void AssignSynVariantsGrammems(CMorphVariant&  synVariant, const CFormatCaller& FormatCaller);
	void AssignOborotMarksToDisruptConj(const CFormatCaller& FormatCaller, CMorphVariant&  synVariant);
	bool AddNewGroup(CGroup& pNewGroup,CMorphVariant& synVariant, bool bForceToAdd = false);
	void FillWithAlreadyBuiltGroups(CFormatCaller& FormatCaller, CSVI pVar) const;
	void BuildVectorOfWords(CFormatCaller& FormatCaller, CSVI pVar) const;
	void AssignVariantWeight(CMorphVariant& synVariant);
	void DeleteGroupInClauseGroupMap(int key);	
	void InterpretGroupBounds(CGroup& pGrp);
	
	int FillRelNodeByClauseUnit(const CMorphVariant& V, int iUnit,  CClauseRelation& rel);
	void ChangeClauseTypeForParticiples();
	std::string  GetTraceStr() const;
	void	BuildSynVariants();
	void	BuildSubjAndPredMember(CMorphVariant& synVariant); 		
	void	DeleteClauseType(int ClauseTypeNo);
	void	ChangeAllClauseTypesToOneType(SClauseType Type);
	void	DeleteClauseTypeInHostClause(CClause* FirstHost,  int TypeNo);
	bool	AssignWordNoAndClauseVarToRelNode(int& iNode, std::vector<CSVI>& ClauseVars, const std::vector<EClauseType>& types);	
	SVI		EraseMorphVariant(SVI pSynVar);

	void	GetBuildingUnits(std::vector<CBuildingUnit>& BuildingUnits);
	void	BuildSynVariantsRecursive(std::vector<CBuildingUnit>::iterator pUnit, CMorphVariant& synVariant);
	int		GetMainWordOfTheClauseForThisVariant(const CMorphVariant& M, const CClauseRelation& R ) const;
	void	TranslateCoordSynVarForGroup(CGroup& pGrp) const;
	void	TranslateFormatCallerGroups(CMorphVariant& V);
    int     SynVarToPrmClForLastWord(int UnitNo) const;
    int     SynVarToPrmClForFirstWord(int UnitNo)  const;
};



class CClauseCollection
{
public:
	std::vector<CRelation>	m_ClausesRelations;	
	std::vector<CClause>		m_Clauses;
	
	CClauseCollection() {};
	virtual ~CClauseCollection() {};

	CClause& GetClause(int i)
	{ return m_Clauses[i]; };

	const CClause& GetClause(int i) const
	{ return m_Clauses[i]; };


	// вставляет новую клаузу,  сохраняя порядок на клаузах
	CClause& AddClause(CClause& clause);


	int GetClausesCount() const
	{ return m_Clauses.size(); };


	/*
	 Изменяет правую границу фрагмента.
	*/
	void ChangeRightBorderPr(int i, int iNewBorder);


	/*
	  находит ближашую клаузу слева, которая не вкладывается в 
	  клаузу ClauseNo
	*/
	int FindPreviousClauseIndex(int ClauseNo) const;

	/*
	  выдает минимальную клаузу, которая включает данную клаузу
	*/
	int GetMininalParentByClauseNo(int ClauseNo) const;

	/*
	  выдает минимальную клаузу, которая включает данное слово
	*/
	int GetMinClauseByWordNo(int WordNo) const;
	
	/*
	  проверяет, что клауза уже куда-т вложена
	*/
	bool IsEnclosedClause (int iClauseNum) const ;


/*
	  ищем  клаузу по началу и концу 
	*/
	int FindClauseIndexByPeriod(const CPeriod& period) const;
	const CClause* FindClauseByPeriod(const CPeriod&  period) const;
	CClause* FindClauseByPeriod(const CPeriod&  period);


	/*
	  обнуление клауз
	*/
	void Clear();

	/*
	 уничтожает 
	*/
	void DeleteClause(int iClauseNum);
	/*
	 функция выдает клаузы, которые включают  pLeftPeriod и у которых  конец совпадает с концом pLeftPeriod
	*/
	void GetClausesByLastWord(std::vector<int>& EncloseClauseVector, const CPeriod* pLeftPeriod) const;
	/*
	 функция выдает клаузы, которые включают  pRightPeriod и у которых  начало совпадает с началом  pRightPeriod
	*/
	void GetClausesByFirstWord(std::vector<int>& EncloseClauseVector, const CPeriod* pRightPeriod) const;

	/*
	 выдает максимальную клаузу, которая начинается с iFWrd
	*/
	int GetMaxClauseByFirstWord(int iFWrd) const;

	/*
	 выдает максимальную клаузу, которая заканчивается на iFWrd
	*/
	int GetMaxClauseByLastWord(int iFWrd) const ;

	int GetMaxChildByLastWord(CPeriod ParentClause, int LastWordNo) const;
	int GetMaxChildByFirstWord(CPeriod ParentClause, int FirstWordNo) const;

	/*
	  выдает примитивную клаузу с минимальным номером, у которой 
	  номер первого слова равен FirstWordNo
	*/
	int GetMinClauseByFirstWord(int FirstWordNo) const ;

	/*
	  выдает примитивную клаузу с минимальным номером, у которой 
	  номер последнего слова равен LastWordNo
	*/
	int GetMinClauseByLastWord(int LastWordNo) const ;

	// Инициализирует CClause::m_SelfClauseNo
	void Renumerate();

	//  сортирует клаузы
	void SortClauses();

	int PassSubClauses(CPeriod ParentClause, int WordNo) const;

};


#endif 
