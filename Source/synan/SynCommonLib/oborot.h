// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef oborot_h
#define oborot_h

#include "stdafx.h"
#include "assert.h"



struct CCoordConjType 
{
	// conjunction
	std::string Word;
	// true, if before the conjunction should be a comma 
	bool should_be_after_comma;

	bool operator == (const char* wordUpper) const 
	{
		return Word == wordUpper;
	};
};



enum EConjType {sub_conj, comp_conj, disrupt_conj,  no_type};


struct  COborotForSyntax 
{
	std::string		m_GrammarFeature;
	// dictionary entry 
    std::string		m_OborotEntryStr;

	// part of speech from GF 
	int			m_Poses;

	// true if oborot is a parenthesis word or construction
	//  for example: "To tell the truth", "Generally"...
	bool		m_bParenthesis; 

	//  true if oborot is a clause conjunction which can subordinate an infinitive
	bool		m_bConjWithInfinitiveSubord;

	// the type of conjunction  assert (m_PartOfSpeech == CONJ)
	EConjType	m_ConjType;

	// all cases from the entry field GF2 
	uint64_t m_DependCases;

	// a union of all possible COborotForSyntax::m_DependCases with the same Content field 
	uint64_t m_AllPossibleDependCases;


    COborotForSyntax ();
	void Init();

	void AddPartOfSpeech(BYTE POS);
	bool HasPartOfSpeech(BYTE POS) const;
    bool HasOnlyPartOfSpeech(BYTE POS) const;
	
};



// SDoubleConj is structure to hold a disruptive conjunction, which were read from the
//  oborot dictionary
struct SDoubleConj
{
	// a reference to COborDic::m_Entries 
	int				 m_iOborNum;

	// the first part of the conjunction 
	std::vector<std::string> m_FirstPart;

	// the second  part of the conjunction 
	std::vector<std::string> m_SecondPart;
	
	// true, if the second part  is equal to the first, for example
	// "или ... или" (Russian)
	// "teils ... teils" (German)
	bool			m_bRepeating;

	bool operator < (const SDoubleConj& X) const
	{
		assert (m_FirstPart.size() > 0);
		assert (X.m_FirstPart.size() > 0);
		return m_FirstPart[0].compare(X.m_FirstPart[0]) < 0;
	};
};



typedef std::map<std::string, std::vector<int>	> CSimplePrepToArticles;

class COborDic;
class CSyntaxOpt;

typedef void(COborDic::*WriteOborFuncType)(std::string s, int OborotNo); 



class COborDic 
{
public:
	//  all entries of the dictionary
	std::vector<COborotForSyntax>	m_Entries;

	// a specially  processed subset of  m_Entries, which contains 
	// disruptive conjunctions
	std::vector<SDoubleConj>			m_DisruptConj; 


	COborDic(const CSyntaxOpt*);

	const CSyntaxOpt*	GetOpt() const ;

	//  building members
	bool	ReadOborots(const CDictionary* piOborDic);
	
	//  using members
	const std::vector<CCoordConjType>&	GetCoordConjs() const {return m_SimpleCoordConj;};
	const StringVector&			GetSubConjs() const {return m_SimpleSubConj;};
	int								FindSubConj(const std::string& word_upper) const;

	int						FindSimplePrep(std::string strPrep) const;
	std::vector<int>				FindAllArticlesForSimplePrep(std::string strPrep) const;
	
	void					WriteSimplePrep(std::string s, int OborotNo); 
	void					TokenizeDoubleConj(std::string s, int OborotNo);
	void					WriteSimpleSubConj(std::string s, int OborotNo);
	
	
protected:
	//  syntax options 
	const CSyntaxOpt*			m_pSyntaxOptions;

	//  simple lists of conjunctions
	StringVector				m_SimpleSubConj;
	std::vector<CCoordConjType>		m_SimpleCoordConj;

	
	// a  from a simple preposition to all dictionary entries for  this  preposition
	CSimplePrepToArticles		m_mapSimplePrep;  



	virtual bool	ReadOborDic (const CDictionary* piOborDic) = 0;


	void	BuildOborots(int iUnit, const CDictionary* piOborDic, WriteOborFuncType WriteOborFunc);
	void	BuildOborot(std::string s, int OborotNo, WriteOborFuncType WriteOborFunc);
	void	MergeCasesOfSimililarSimplePreps();

	
	
};


#endif
