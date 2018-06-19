// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef rus_sentence_h
#define rus_sentence_h




#include "StdSynan.h"
#include "../SynCommonLib/Sentence.h"
#include "RusWord.h"
#include "RusClause.h"



/*
 ���-�� ����� ��� ������ ������ ���������� ��.�. ������� � �����.��. "����" ��� "�����" 
*/
struct SAnalyticalFormVariant
{
	enum eAnalyticType { Infinitive, Predikative, Short_Form, Comp_Adj }; 
	//����� �����
	int iWordNum;
	//���-�� ������� ��� �����
	int iHomCount;
	//����� ���� �������, ����������� ������������� ����������
	eAnalyticType ePos;
	//����� �������
	string s_Lemma;
	//��� ������� ����� ���������;
	bool bAllHomPredik;
	SAnalyticalFormVariant(int iWrd, int iCount, eAnalyticType iPos, string s_lem, bool bAllPredik)
	{
		iWordNum = iWrd;
		iHomCount = iCount;
		ePos = iPos;
		s_Lemma = s_lem;
		bAllHomPredik = bAllPredik;
	};
	//�������� ������� : ������ ������������� �������� ���, ������� �����
	//������ ����� ��������, ��������� � ������� ���, ������� �������� �����. ����. ����.
	bool operator<(const SAnalyticalFormVariant& X) const
	{
		if (bAllHomPredik)
			return true;

		if (X.bAllHomPredik)
			return false;

		if ( s_Lemma == "�������" ||
			 s_Lemma == "�������" ||
			 s_Lemma == "��������" ||
			 s_Lemma == "�������" ||
			 s_Lemma == "����������" )
			 return true;

		if ( X.s_Lemma == "�������" ||
			 X.s_Lemma == "�������" ||
			 X.s_Lemma == "��������" ||
			 X.s_Lemma == "�������" ||
			 X.s_Lemma == "����������" )
			 return false;


		if ( iHomCount < X.iHomCount && Comp_Adj != ePos && Infinitive != ePos )
			return true;

		if ( Comp_Adj != ePos && Infinitive != ePos &&
			 ( Comp_Adj == X.ePos || Infinitive == X.ePos ) )
			return true;

		if ( Infinitive == ePos && Comp_Adj == X.ePos)
			return true;

		return false;

	};
};


typedef vector<SAnalyticalFormVariant> CAnalyticalFormVars; 








class CRusSentence   : public CSentence
{
	bool IsAdjDeclination (const CSynHomonym& H) const;
    vector<CPeriod> FindParenthesisOborotsWithoutComma() const;
    bool  CheckBorders (int start, int end) const;
    bool  CanBeInsideParenthesisOborot(const CSynWord& W) const;
public:
	int		m_KOTORYI_INDEX;
	int		m_CHEI_INDEX;
	const CRusSyntaxOpt* GetOpt() const 
	{
		return (const CRusSyntaxOpt*)m_pSyntaxOptions;
	};

	CRusSentence(const CSyntaxOpt* m_pSyntaxOptions);
	~CRusSentence();
	void ReadNextFromPlmLinesLanguageSpecific();
    CFormatCaller* GetNewFormatCaller() const override ;

	void CheckGroupsForAnalyticalVerbForms(CAnalyticalFormVars& vectorAnalyticalVariants, CPeriod ClausePeriod);
	void CutPrefixEksAndVize();
	void DeleteHomOneToThousand();
	void BuildDash(int iClauseNum, int iWrdBefore, bool bCopul = false);

	bool RunSyntaxInClauses(ESynRulesSet type);
	
	void DetermineParenthesis();
	void DisruptPronounPredik();
	void ChangeSubjAndItsGroupGrammems();

	
	void AddWeightForSynVariantsInClauses();
	
	
	


	//   Base Clause Rules
	void InitClauseVectorRules();
	bool ClauseHasSubjPredik(const CClause& Clause) const;
	bool TryToFindCommonGroupsForUnitedSynVariants(const CClause& ClFirst, const CClause& ClSecond, vector<int>& Types, bool bFindSubjPredikate);
	bool RuleForDashClause(int iClauseNum);
	bool RuleForEmptyClauses(int iClauseNum);
	bool RuleForKTO(int iClauseNum);
	
	bool RuleForSubClause(int iClauseNum);
	bool RuleForSubClauseWithCoordiantion(int iClauseNum);
	
	bool RuleForAdverbPart(int iClauseNum);
	bool RuleForFragNounGroup(int iClauseNum);
	
	bool RuleForClauseDisruptWithSubordinateClauses(int iClauseNum);
	bool RuleForDisruptClausesBySubject(int iClauseNum);
	bool RuleForCommaDashClause(int iClauseNum);
	bool RuleForParenthesis(int iClauseNum);

	bool HasStrictBorderLeft(int iClauseNum);
	
	bool HasStrictestLeftClauseBorder(int iClauseNum) const;
	bool IsGoodEmptyClause(const CClause* pAbstClause) const;
	bool CanBeMainClauseOfTheLeftGoodEmptyClause(const CClause* pAbstClause1, bool bStrictPrevClauseBorder) const;
	
	
	bool	FindNounFromTheRightOfParticiple(const CClause* pClause,  const CSynHomonym& Participle) const;
	int		FindNounFromTheLeftOfParticiple(const CClause* pClause, const CSynHomonym& Participle) const;
	bool	CheckCoordinationOfNounAndParticiple(const CSynHomonym& Participle, const CClause* pClause, size_t NounWordNo, size_t NounHomonymNo) const;
	bool	RuleForParticiples(int iClauseNum);
	bool	RuleForSimilarNounGroupClauses(int iClauseNum, int MinCountOfSimilarMembers);
	bool	RuleForUnitingClausesBySubject(int iClauseNum, bool bIgnoreWeakTypes);
	bool	RuleForUnitingClausesBySubjectWeak(int iClauseNum);
	bool	RuleForUnitingClausesBySubjectStrong(int iClauseNum);
	
	bool	RuleForSimilarNounGroupClauses2(int iClauseNum);
	bool	RuleForSimilarNounGroupClauses3(int iClauseNum);
	bool	RuleForDisruptConjUnion(int iClauseNum);
	bool	CanBeDisruptConjClause (const CClause& pClauseLeft, const CClause& pClauseRight) const;
	void	BuildClauseRelations();
	bool	CheckCoordinarionBetweenClauseRoots(GrammemCompare CompareFunc, int iClauseNum1, int iClauseNum2 ) const;

	void	TryBuildVerbLemmaWithKa();
	void	TryToAddComparativeTypeToClause();
	void	TryToRebuildDashInClause();

	bool	BuildClauses();
	void	BuildAnalyticalVerbForms();
	bool	CheckAnalyticalVerbForm(int iVWrd, int iSWrd);
	bool	IsAnalyticalVerbForm(int iVerbWrd, int iSWrd, int& VerbHomNo, CIntVector& AnalyticHom);

	bool	SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd); 
	EClauseType GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const;

	const CRusGramTab* GetRusGramTab() const 
	{
		return (CRusGramTab*)GetOpt()->GetGramTab();
	};
	
	void	AfterBuildGroupsTrigger(CClause& C);
	void	BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType); 
	bool	find_subj_and_predic_noun_with_dash(CMorphVariant& synVariant );
	bool	find_subj(CMorphVariant& synVariant,int predk);
    bool	can_be_subject(const CMorphVariant& synVariant, int SubjWordNo, int RootWordNo, int exactitude,  bool bCanBeWhatAsSubj) const;
	bool	GleicheSubjPredForNumeralAndSimilar(const CMorphVariant& synVariant, int WordNo, int iGroup) const;
	bool	gleiche_subj_pred_for_numerals_as_nouns(const CMorphVariant& synVariant,int i_predk, int i_subj) const;
	bool	check_verb_subj_coordination(const CMorphVariant& synVariant, int predk, int SubjGroupFirstWordNo, int& SubjWordNo) const;
	bool	check_adj_subj_coordination(const CMorphVariant& synVariant,int predk, int SubjGroupFirstWordNo, int& SubjWordNo) const;
	bool	IsGoodSubject(const CMorphVariant& synVariant, const string& LastSentenceChar) const;

	int		GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const;
	bool	WordSchemeEqForThesaurus(const CSynHomonym& Homonym, const CSynPlmLine& word_scheme) const;
	int		GetRuleByGroupTypeForThesaurus(int GroupType) const;
	void	InitHomonymMorphInfo (CSynHomonym& H);
	void	InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord);
	bool	HasNounInNom(const CSynWord& _W  ) const;
	bool	HasAnalyticalBe(const CSynWord& _W)  const;
	bool	AllHomonymsArePredicates(const CSynWord& W) const;


	void	SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord);
	void	SolveAmbiguityUsingRuleForShortAdj();
	bool	IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int& HomonymNo) const;

	bool	IsInitialClauseType(EClauseType ClauseType) const;

	void	DeleteSomeTypesInRelativeClauses();
	bool	CanBeRelativeAntecedent(const CSynHomonym& H) const;
	bool	IsProfession(const CSynHomonym& H) const;

	void	ProcessFio1Fio2();

	void	ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V);
	bool	CheckLastPredicatePosition(size_t ClauseLastWordNo, long RootWordNo) const { return true;};	
    void    CloneHomonymsForOborots() override;	
};






#endif 
