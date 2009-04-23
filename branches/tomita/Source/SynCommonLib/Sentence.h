// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef sentence_h
#define sentence_h

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Word.h"

#include "Clause.h"
#include "oborot.h"


enum ESynRulesSet{ SimpleSimilarRules, RulesBeforeSimClauses, RulesAfterSimClauses, AllRules, GroupRulesForClause};

typedef pair<int, int> CIntPair;
typedef vector<CIntPair> CIntPairVector;

struct SGraPair
{
	int m_iW1;
	int m_iW2;
	int m_iOborotNum;
	EGraPairType m_type;

	SGraPair()
	{
		Reset();
	};

	SGraPair(int iW1,int  iW2,EGraPairType type )
	{
		m_iW1 = iW1; 
		m_iW2 = iW2; 
		m_type = type;
		m_iOborotNum = -1;
	};

	SGraPair(int iW1,int  iW2,EGraPairType type, int iOborotNum )
	{
		m_iW1 = iW1; 
		m_iW2 = iW2; 
		m_type = type;
		m_iOborotNum = iOborotNum;
	};


	void Reset()
	{
		m_iOborotNum = -1;
		m_iW1 = -1;
		m_iW2 = -1;
		m_type = UnknownPairType;	
	};

};








/*
  с помощью  этих констант определяется, от какой клаузы из двух клауз будут 
  взяты параметры для объединенной клаузы: от левой или от правой 
  Это тип используется только при работе  с функцией UniteClauses 
*/
enum ParametersSourceEnum {
	LeftClauseParams,
    RightClauseParams
};

typedef bool (CSentence::*FirstGrRoule)(int CluauseNo);

struct CClauseRule {
	FirstGrRoule	m_Rule;
	string			m_Description;  
	CClauseRule (FirstGrRoule Rule, 	string	Description )
	{
		m_Description = Description;
		m_Rule =  Rule;
	};

};




class CSentence   : public CClauseCollection
{
protected:
	CGroups		m_GroupsUnion;
	// working with  thesaurus 
	virtual bool	WordSchemeEqForThesaurus(const CSynHomonym& Homonym, const CSynPlmLine& word_scheme) const = 0;
	bool	EqToTermin(const CSynWord& Word,   const string& termin, const CSynPlmLine& word_scheme, int iHom) const;
	void	FindTermins();
	void	FindAllTermins();

public:


	
	CWordVector m_Words;
	CWordVector& GetWords() { return m_Words;};
	const CWordVector& GetWords() const { return m_Words;};

	vector<int> m_vectorPrClauseNo;
	bool	m_bShowAllGroups;
	bool	m_bPanicMode;
	vector<CClauseRule> m_vectorPrimitiveRules;
	vector<CClauseRule> m_vectorMainEncloseRules;
	vector<CClauseRule> m_vectorDisruptRules;
    // используется только в фукции CheckPairDelimiters
	bool m_bFirstInPairFound;	
	// вектор всех найденных терминов
	vector<SFoundTermin>	m_vectorTermins;	
	// вектор всех графематических вилок (типа EXPR1-EXPR2)
	vector<SGraPair>		m_vectorGraPairs;


	//  list of models which were built upon grammars
	// should  be a list since we use a pointer to its member
	list<CGroups>			m_GrammarModels;
	// вектор всех найденных терминов
	vector<SFoundTermin>	m_GrammarItems;	

	bool m_bShouldUseTwoPotentialRule;
	
	// ссылки на словари и опции
	const CSyntaxOpt* m_pSyntaxOptions;

	CSentence(const CSyntaxOpt* m_pSyntaxOptions);
	virtual ~CSentence() {};

	// language specific 
	virtual void	ReadNextFromPlmLinesLanguageSpecific () = 0;
	virtual bool	BuildClauses() = 0;
	
	virtual void	AfterBuildGroupsTrigger(CClause& C)   = 0;
	virtual void	BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType)   = 0;  
	virtual int		GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const  = 0;
	virtual void	InitHomonymMorphInfo (CSynHomonym& H) = 0;
	virtual void	InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord) = 0;
	virtual void	SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord) = 0;
	virtual bool	IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int& HomonymNo) const = 0;
	virtual bool	AllHomonymsArePredicates(const CSynWord& W) const = 0;
	virtual EClauseType GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const = 0;
	virtual bool	CanBeRelativeAntecedent(const CSynHomonym& H) const = 0;	
	virtual bool	IsProfession(const CSynHomonym& H) const = 0;
	virtual bool	RunSyntaxInClauses(ESynRulesSet) = 0;	
	virtual void	ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V) = 0;
	virtual bool	CheckLastPredicatePosition(size_t ClauseLastWordNo, long RootWordNo)  const = 0;	
	


	const CSyntaxOpt* GetOpt() const 
	{
		return m_pSyntaxOptions;
	};

	bool ReadNextFromPlmLines(const CPlmLineCollection* piPLMLinePtr, size_t& LineNo);

	
	void Reset();
	bool KillHomonymsInAllSentence();
	

	bool DeleteMarkedHomonymsWithClauses(int iWord);
	/*
	 вызывает DeleteMarkedHomonymsWithClauses для всех слов периода
	*/
	bool DeleteMarkedHomonymsWithClauses(const CPeriod& P);
    void DontDeleteExpr2ifExpr1Exists(const CPeriod& P);

	void	CalculatePrimitiveClausesCount();
	bool	GetHomonymByClauseVariant(long iWord, CSVI pSynVar, CSynHomonym& H)  const;
	bool	GetHomonymByClauseVariantInterface(long iWord, long iVar, long iClause, CSynHomonym& H)  const;
	long	GetPrimitiveClauseNoByClauseNo(long lClauseNo) const;

	
	void AssignClauseNoToWords();
	void InitWordsInTermins();
	
	
	void BuildWordsSegments(CIntPairVector& WordsSegments);
	void FindGraPairs();
	
	
	bool FindTerminsHelper(CWordVector::iterator words, CWordVector::const_iterator end_of_words, CTermin::const_iterator termin, CTermin::const_iterator end_of_termin, CPlmVector::const_iterator word_scheme,  CPlmVector::const_iterator word_scheme_end, bool& bError );
	void RecalculateRelationsCoordinates();
	bool IsInNonAtomicOborPairs(int iW) const;
	void MarkWordAsDeleted(int iDelWrd);
	string	GetSentenceBeginStr() const;
	void	OutputErrorString(string Message) const;
	bool ProcessGrammarModels();


	
	void	TraceClauses() const;
	bool	IsValid() const;	
	
	void	DeleteHomonymsIfTooManyPredictedWords();
	void	SetIgnoreWhileBuildingSynVariantsToAllClauses (bool Value);  
	void	AscribeSimplePrepositionInterpretations(BYTE PREP_POS);
	void	DeleteOborotThatContains(int WordNo);
	bool	IsPanicSentence();
	void	RecalculateIndicesAfterInsertingWord(int InsertWordNo);
	bool	IsBetweenGraphematicalPairDescriptors(int WordNo) const;
	void	CloneHomonymsForOborots();

	bool	RunClauseRule(const CClauseRule*it, int iClauseNum);
	void	OneRunOfClauseRules(const vector<CClauseRule>& ListOfRules);
	int	    GetCoordConjNo(const char* WordUpper) const;

	int		FindWordWithOneHomonym(int iFirstWord, int iLastWord, BYTE pos) const;
	CFormatCaller* GetNewFormatCaller() const;

	

	//  build initial clauses
	virtual bool	SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd) = 0; 
	virtual bool	IsInitialClauseType(EClauseType ClauseType) const = 0;
	bool	BuildInitialClauses();
	int		CheckIfBracketIsAClauseBorder(int WordNo, int& iStartSearch);
	int		CanLinkSimpleSimilar(int i);
	int		IsClauseBorder(int WordNo, int& iStartSearch,int& iPunctsCount, int iFWrd);
	void	InitClauseType(CClause& clause);
	void	InitConjunctions(CClause* pClause);

	//   Clause hierarchy
	bool		IsWordClause(const CClause& rClause, int iWrd) const;
	bool		IsStrictLeftClauseBorder(CPeriod P) const;
	bool		EncloseClauseAsWord(int iWhat, int iWhere);
	CClause&	UniteClauses(int iClauseL, int iClauseR, ParametersSourceEnum whose_chars_to_copy);
	bool		CreateEnclosedNotStrongClause(int MainClauseNo, int pFWrd, int pLWrd, SClauseType ClType);
	bool		RuleForRelativeClause(int iClauseNum);
	bool		RuleForUnitingEmptyClauseWithStrongLeftBorder(int iClauseNum);


	bool		ModifyGrammems(CAncodePattern& P, QWORD Grammems);

	void		BuildGLRGroupsInClause(CClause& pClause);
	void		BuildGLRGroupsInSentence();
	void		BuildTerminalSymbolsByClause(size_t ClauseNo);
	void		ApplyGLR_Parsing (CGLRParser& Parser, const vector<CBuildingUnit>& BuildingUnits);
	void		GetBestChunks(CGLRParser& Parser, vector< COccurrence>& Occurrences) const;
	bool		BuildMorphVariantsByTomita(const CGLRParser& Parser, const vector< COccurrence>& Occurrences, CClause& pClause, vector<CBuildingUnit>& BuildingUnits);
	CGroup		ConvertFromTomitaNodeToCGroup(const CGLRParser& Parser, const CClause& pClause, int SymbolNo ) const;
	void		BuildGroupByGLR(const CGLRParser& Parser, const COccurrence& C, const CClause& pClause, CGroups& Groups ) const;
	void		ProjectUsedHomonyms(const CGLRParser& Parser, const COccurrence& C, const size_t OccurNo, vector<CBuildingUnit>& BuildingUnits, vector<set< pair<int, int> > >& UsedOccurrences ) const;

	bool		CheckSubclausePredicatePositionForAllRoots(const CClause& C);

	int			FindAuxVerbByLemma(int MainWordNo, const string& Lemma) const;
	int			FindFirstAuxVerb(int MainWordNo) const;

	size_t			GetPrimitiveClausesCount() const;
	const CClause*	GetPrimitiveClause(int ClauseNo) const;
    void AddSynWord( CSynWord& Word);
    bool        CheckTerminIntersection(int iFWrd, int iLWrd) const;
	

};


extern  bool GetHomonymByClauseVariantWithModifiedProperties (int iUnit, const CSynWord& word, const CMorphVariant& syn_var, CSynHomonym& H);


#endif 
