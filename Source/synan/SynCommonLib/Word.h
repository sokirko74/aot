// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once 

#include "SyntaxInit.h"
#include "Word.h"
#include "SynPlmLine.h"
#include "synan/SimpleGrammarLib/InputSymbol.h"
#include "morphan/LemmatizerLib/LemWord.h"
#include "SynHomonym.h"

#pragma warning(disable:4786) 


class CSynWord  : public CLemWord
{
	void ResetSelfMembers();
public:
	//  a reference to a divided prefix (for German)
	int		m_TrennbarePraefixWordNo;

	// ========================  Morphology ========================
	std::vector<CSynHomonym> m_Homonyms;


	// is a simple coordinating conjunction
	bool m_bSimilarConj;

	// the result of the search of subordinating  conjunction list via GetOpt()->GetOborDic()->FindSubConj
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
	std::vector<int>	m_MainVerbs;

	

	// =============          Special slots =================

	// the holder of this Word
	CSentence* m_pSent;

	// m_iClauseNo is used for semantics interface
	int	 m_iClauseNo;
	

	// is created by syntax
	bool m_bArtificialCreated;

	CSynWord(MorphLanguageEnum l);

	CSynWord(const CLemWord &);

	void SetSentence(CSentence* s);

	const	CSyntaxOpt* GetOpt() const; 
		
	void	Reset();
		
	size_t	GetHomonymsCount() const	{ return m_Homonyms.size(); }
	
	bool	InitializePlmLine(CSynPlmLine& pPlmWord, int HomonymNo) const;
	
	CSynHomonym CloneHomonymByAnotherHomonym(const CSynHomonym* pHomonym, uint64_t iGrammems, BYTE iTagID) const;

	const CSynHomonym& GetSynHomonym(int i) const;

	CSynHomonym& GetSynHomonym(int i);

	const CHomonym* GetHomonym(int i) const override;
	
	CHomonym* GetHomonym(int i) override;
	
	CHomonym* AddNewHomonym() override;


	void SetGoodHomonym(int i );
	void EraseHomonym(int iHom);

	void	CloneHomonymForOborot(int HNum = -1);
	void	TryBuildVerbLemmaWithKa();

	const COborotForSyntax*	GetOborotPtr() const;
	
	
	void	UpdateConjInfo();
    void    InitLevelSpecific(CSynHomonym& h);
};

typedef std::vector<CSynWord> CWordVector;
