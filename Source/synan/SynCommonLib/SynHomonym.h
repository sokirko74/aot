#pragma once 

#include "morphan/LemmatizerLib/Homonym.h"
#include "LemmaList.h"

class CSentence;
class CSyntaxOpt;
struct COborotForSyntax;

class CSynHomonym : public CHomonym
{
	void ResetSelfMembers();
public:
	bool m_bAdvAdj;
	bool m_bCanSynDependOnAdj;
	bool m_bCanSynDependOnAdv;
	bool m_bCanSubdueInfinitive;
	bool m_bCanSubdueInstr;
	bool m_bNounHasAdjectiveDeclination;
	int m_CoordConjNo;
	bool m_bMonth;
	bool m_bPassive; // is used for German verbs only

	// a special slot which is now used only in CSentence::FindTermins to select all  homonyms which 
	// suit the found termin
	bool m_bGoodHomonym;

	// all German verbs which should have special Perfect order in  subclauses, for example
	//  Ich weiss, dass du die Gefahr hast kommen sehen
	bool	m_bPerfectAnomalie;

	// all German verbs which can subdue an infinitive without zu
	//  Ich bleibe stehen.
	bool m_bInfinitiveConstruction;

	// all German adjectives  which can subdue at least on NP
	//  "arm an+D"
	bool m_bAdjWithActiveValency;

	const CSentence* m_pSent;

	CSynHomonym(MorphLanguageEnum l);

	CSynHomonym(const CHomonym& h);

	void SetSentence(const CSentence*);

	const COborotForSyntax* GetOborotPtr()  const;

	const	CSyntaxOpt* GetOpt() const;

	bool	CompareWithPredefinedWords(const CLemmaList& arr) const;


};