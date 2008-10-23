// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef ger_sentence_h
#define ger_sentence_h




#include "GerSynan.h"
#include "../SynCommonLib/Sentence.h"
#include "GerWord.h"
#include "GerClause.h"
#include "GerSyntaxOpt.h"




class CGerSentence   : public CSentence
{
public:
	const CGerSyntaxOpt* GetOpt() const 
	{
		return (const CGerSyntaxOpt*)m_pSyntaxOptions;
	};

	CGerSentence(const CSyntaxOpt* m_pSyntaxOptions);
	~CGerSentence();
	void ReadNextFromPlmLinesLanguageSpecific();
	void DeleteHomOneToThousand();
	bool RunSyntaxInClauses(ESynRulesSet);
	
	
	void AddWeightForSynVariantsInClauses();
	

	//   Base Clause Rules
	bool	BuildClauses();
	const CGerGramTab* GetGerGramTab() const 
	{
		return (CGerGramTab*)GetOpt()->GetGramTab();
	};
	void	AfterBuildGroupsTrigger(CClause& C);
	void	BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType); 
	int		get_weight_of_subj_pred_relation(const CMorphVariant& synVariant, const int Subj, const int Pred, bool bEnclosed);
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

	//  clause  hierarchy
	void	InitClauseVectorRules();
	bool	RuleForSubClause(int iClauseNum);
	bool	CanBeRelativeAntecedent(const CSynHomonym& H) const;
	bool	RuleForPartzipialKonstruktion(int iClauseNum);
	bool	RuleForEmptyClauses(int iClauseNum);
	bool	RuleForUmZuKonstruktion(int iClauseNum);
	bool	RuleForModalVerb(int iClauseNum);
	bool	RuleForInfinitive(int iClauseNum);
	bool	RuleForDividedVerbForms(int iClauseNum);
	

	// analytical forms
	void	ParticipleAndVerbInOneForm();
	void	BuildTrennbarePraefixe();
	void	BuildAnalyticalForms();
	
	// postmorphology
	void	DeleteHomonymsUsingGrossSchriebung();
	void	SolveAmbiguityDeterminerAndPronoun();
	void	SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord);
	bool	SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd); 
	void	UnitOrdinalNumbersWithFullStop();

	

	void	ChooseClauseType(const  vector<SClauseType>& vectorTypes,CMorphVariant& V);
	bool	CheckLastPredicatePosition(size_t ClauseLastWordNo, long RootWordNo)  const;
	void	FindReflexivePronomen();

};




#endif 
