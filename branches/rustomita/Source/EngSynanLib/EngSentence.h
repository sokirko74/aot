
#ifndef eng_sentence_h
#define eng_sentence_h

#include "../SynCommonLib/Sentence.h"
#include "EngSyntaxOpt.h"


class CEngSentence :	public CSentence
{
public:
	CEngSentence(const CSyntaxOpt* m_pSyntaxOptions);
	~CEngSentence();

	const CEngSyntaxOpt* GetOpt() const 
	{
		return (const CEngSyntaxOpt*)m_pSyntaxOptions;
	};
	void ReadNextFromPlmLinesLanguageSpecific();
	void DeleteHomOneToThousand();
	bool RunSyntaxInClauses(ESynRulesSet);
	
	
	void AddWeightForSynVariantsInClauses();
	

	//   Base Clause Rules
	bool	BuildClauses();
	const CEngGramTab* GetEngGramTab() const 
	{
		return (CEngGramTab*)GetOpt()->GetGramTab();
	};
	void	AfterBuildGroupsTrigger(CClause& C);
	void	BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType); 
	int		GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const;
	bool	WordSchemeEqForThesaurus(const CSynHomonym& Homonym, const CSynPlmLine& word_scheme) const;
	int		GetRuleByGroupTypeForThesaurus(int GroupType) const;
	void	InitHomonymMorphInfo (CSynHomonym& H);
	void	InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord);
	EClauseType GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const;


	bool	IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int& HomonymNo) const;
	bool	AllHomonymsArePredicates(const CSynWord& W) const;
	bool	IsInitialClauseType(EClauseType ClauseType) const;
	bool	IsProfession(const CSynHomonym& H) const;


	void	SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord);

	bool	CanBeRelativeAntecedent(const CSynHomonym& H) const;
	bool	SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd); 

	
	void	ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V);
	bool	CheckLastPredicatePosition(size_t ClauseLastWordNo, long RootWordNo)  const { return true;};	

};

#endif
