// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef synan_word_h
#define synan_word_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"
#include "SyntaxInit.h"
#include "Word.h"
#include "SynPlmLine.h"
#include "../SimpleGrammarLib/InputSymbol.h"
#include "../common/LemWord.h"
#pragma warning(disable:4786) 


class CSynHomonym : public CHomonym
{
public:
	const COborotForSyntax*	GetOborotPtr()  const
	{
		assert ( m_OborotNo != -1);
		return &GetOpt()->m_pOborDic->m_Entries[m_OborotNo];
	};
	
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

	//  all possible interpretation in the formats grammar
	set<CInputSymbol> m_AutomatSymbolInterpetation;

	const CSentence* m_pSent;	
	
	CSynHomonym(const CSentence* pSentence);
	const	CSyntaxOpt* GetOpt() const;
	
	bool	CompareWithPredefinedWords(const SDatItems& arr) const;
    
	
};
class CSynWord  : public CLemWord
{
public:
	//  a reference to a divided prefix (for German)
	int		m_TrennbarePraefixWordNo;

	// ========================  Morphology ========================
	vector<CSynHomonym> m_Homonyms;


	// is a simple coordinating conjunction
	bool m_bSimilarConj;

	// the result of the search of subordinating  conjunction list via GetOpt()->m_pOborDic->FindSubConj
	int  m_SubordinateConjNo;


	// =============          Thesaurus interpretation =================
	// the type of thesaurus of the domain collocation (applicable only for the first word of the collocation)
	EThesType m_ThesType;

	// is between m_bFirstWordInTermin и m_bLastWordInTermin (inside a domain collocation)
	bool m_bInTermin;

	// the first word of a domain collocation
	bool m_bFirstWordInTermin;

	// the last word of a domain collocation
	bool m_bLastWordInTermin;

	//  номер термина, который  начинается с этого слова
	int	 m_iTerminID;



	// ========================  Russian Language  ========================
	// является ли слово мылым числительным  ("два", "три")
	bool m_bSmallNumber;

	// слово является оборотом, у которого есть  GF=ВВОДН, но не выделено в тексте знаками препинания
	bool m_bBadParenthesis;


	int m_iReduplication;

	// the main verb of an auxiliary verb
	vector<int>	m_MainVerbs;

	

	// =============          Special slots =================

	// the holder of this Word
	CSentence* m_pSent;

	// m_iClauseNo is used for semantics interface
	int	 m_iClauseNo;
	

	// is created by syntax
	bool m_bArtificialCreated;


	set<CInputSymbol> m_AutomatSymbolInterpetationUnion;

	CSynWord(CSentence* pSent); 
	const	CSyntaxOpt* GetOpt() const; 
	
	
	void	Reset();
	
	
	
	
	size_t	GetHomonymsCount() const	{ return m_Homonyms.size(); }
	bool	InitializePlmLine(CSynPlmLine& pPlmWord, int HomonymNo) const;
	CSynHomonym CloneHomonymByAnotherHomonym(const CSynHomonym* pHomonym, QWORD iGrammems, BYTE iTagID) const;

	const CSynHomonym& GetSynHomonym(int i) const	{ return m_Homonyms[i]; }
	CSynHomonym& GetSynHomonym(int i) 	{ return m_Homonyms[i]; }
   	const CHomonym* GetHomonym(int i) const { return &m_Homonyms[i]; };
	CHomonym* GetHomonym(int i){ return &m_Homonyms[i]; };
    virtual CHomonym* AddNewHomonym() { m_Homonyms.push_back(CSynHomonym(m_pSent)); return &m_Homonyms.back(); };;


	void SetGoodHomonym(int i );
	void EraseHomonym(int iHom);

	void	CloneHomonymForOborot();
	void	TryBuildVerbLemmaWithKa();

	const COborotForSyntax*	GetOborotPtr() const;
	
	
	void	BuildTerminalSymbolsByWord();
	bool	IsEqualToGrammarItem(const CSynHomonym& L, const CGrammarItem& I);
	void	UpdateConjInfo();
    void    InitLevelSpecific(CHomonym* pHom);
};

typedef vector<CSynWord> CWordVector;







#endif 
