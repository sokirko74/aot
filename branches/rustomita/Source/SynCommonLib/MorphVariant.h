// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef SynVariant_h
#define SynVariant_h

#include "stdafx.h"
#include "Group.h"

#include "../SimpleGrammarLib/InputSymbol.h"
#include "../common/AncodePattern.h"

typedef  vector<int>  CIntVector;
typedef	 pair<int, int> CIntPair;


class CSentence;
struct CWordAndHomonym
{
	int	m_WordNo;
	int	m_HomonymNo;

	CWordAndHomonym();
	void SetEmpty ();
	bool IsEmpty () const;
	bool operator == (const  CWordAndHomonym& X) const;
	void UpdateWhenDeleteHomonym(int WordNo, int DeleteHomonymNo);
	void UpdateWhenInsertWord(int InsertWordNo);
	bool IsValid(const CPeriod& Period, const CSentence* Parent) const;
};




struct SClauseType
{
	EClauseType			m_Type;
	CWordAndHomonym		m_Root;

set<CInputSymbol>	m_AutomatSymbolInterpetation;

	SClauseType(EClauseType Type, int iNodeIndex, int iHomNum)
	{
		m_Type = Type;
		m_Root.m_WordNo = iNodeIndex;
		m_Root.m_HomonymNo = iHomNum;
	};
	SClauseType()
	{
		m_Type = UnknownSyntaxElement;
	}

	bool operator== (const SClauseType& type) const 
	{
		return	(type.m_Type == m_Type) &&
				(type.m_Root == m_Root);
	}
	
};




struct CSynUnit : public CAncodePattern
{
	// clause or word 
	EUnitType				m_Type;	

	// the coordinates in the sentence
	CPeriod					m_SentPeriod;

	// all cases, which can be used between a preposition and a subordinated NP (used only for Russian1)
	QWORD					m_FoundPrepDependCases;

	// the index of a clause type if m_Type==EClause
	int						m_iClauseTypeNum;

	// the index of a homonym  if m_Type==EWord
	int						m_iHomonymNum;
	
	// is used only for German verbs 
	bool					m_bReflexive; 

    CSynUnit(const CAgramtab* pGramTab);


};


struct  CMorphVariant
{
    const CAgramtab*                m_pGramTab;
	// all groups  which are built in this  morphological variant
	// sorted by  GladkijPeriodLess
	// there should be no  dublicates and no non-projectivities 
	CGroupsVector					m_vectorGroups;

	// vector of units which  constitute this morph. variant
	vector<CSynUnit>				m_SynUnits; 	

	// the clause type, which is found in this CMorphVariant
	int								m_ClauseTypeNo;
	
	// possible subjects of the clause ( measured in Units)
	vector<int>						m_Subjects;

	bool							m_bGoodSubject;

	/* Russian notes:
		Вершина клаузы может не совпадать со сказуемым.
	   Во фразе "я - комсомолец"  вершина будет равны -1, а m_iPredk
	   будет показывать на "комсомолец". Слот m_iPredk выставляет правилом поиска подлежащего 
	   и сказуемого, а  вершина - фрагментацией.
	*/


	// predicate of the clause ( measured in Units)
	// for German it is always the root word of the clause (for Russian it is not)
	int		m_iPredk;	
	// the weight of the variant
	int		m_iWeight;

				

	CMorphVariant(const CAgramtab* pGramTab);

	bool	HasSubj() const;
	bool	HasPredk() const;
	int		GetFirstSubject() const;

	void	PushHomonymNum(int i);
	void	PopHomonymNum();
	void	Reset();
	void	ResetSubj();
	

	/*
	 comparing CMorphVariant by  m_SynUnits
	*/
	bool EqualTo(const CMorphVariant& variant) const;

	
	/*
	  the procedure return a period in sentence coordinates by a period in morph. variant coordinates (units)
	*/
    CPeriod GetSentenceCoordinates(const CPeriod& PeriodInUnits) const;
	/*
	  the procedure returns the Unit Number by Word Number
	  If iWord is out of bounds, then it returns -1
	*/
	int	UnitNoByWordNo(int iWord) const;

	
	size_t GetUnitsCount() const;
	EUnitType GetUnitType(int iUnit) const;
	const vector<int>& GetSimplePrepNos(int i) const;
	int GetHomNum(int iMemb) const;

	//  returns true id iWrd is in subclause 
	bool IsInSubclause(int iWrd) const;

	//	returns the last Word Number of unit iUnit
	int GetUnitLastWord(int iUnit) const;

	//	returns the first Word Number of unit iUnit
	int GetUnitFirstWord(int iUnit) const;


	/*
		if  iWord is in a unit which is subclause , then it returns the last word of this  clause
		otherwise it returns  iWord.
	*/

	int GetClauseLastWord(int iWord) const;
	/*
		if  iWord is in a unit which is subclause, then it returns the first word of this  clause
		otherwise it returns  iWord.
	*/

	int GetClauseFirstWord(int iWord) const;

	CPeriod GetSentPeriod() const;
};


typedef list<CMorphVariant>::const_iterator	CSVI;
typedef list<CMorphVariant>::iterator			SVI;



#endif
