// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef oborot_h
#define oborot_h

#include "stdafx.h"
#include "assert.h"



struct CCoordConjType 
{
	// conjunction
	char word[20];
	// true, if before the conjunction should be a comma 
	bool should_be_after_comma;
	bool operator == (const char* WordUpper) const 
	{
		return !strcmp(WordUpper, word);
	};
};



enum EConjType {sub_conj, comp_conj, disrupt_conj,  no_type};


struct  COborotForSyntax 
{
	string		m_GrammarFeature;
	// dictionary entry 
    string		m_OborotEntryStr;

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
	QWORD m_DependCases;

	// a union of all possible COborotForSyntax::m_DependCases with the same Content field 
	QWORD m_AllPossibleDependCases;


    COborotForSyntax ();
	void Init();

	void AddPartOfSpeech(BYTE POS);
	bool HasPartOfSpeech(BYTE POS) const;
    bool HasOnlyPartOfSpeech(BYTE POS) const;
	
};



// SDoubleConj is structure to hold a disruptive conjunction, which were read from the
//  oborot dictionary
const int MaxConjSize = 30;
struct SDoubleConj
{
	
	struct string30 {
		char m_item[MaxConjSize];
		bool operator == (const string30& X) const
		{
			return strcmp(m_item, X.m_item) == 0;
		};
	};

	// a reference to COborDic::m_Entries 
	int				 m_iOborNum;

	// the first part of the conjunction 
	vector<string30> m_FirstPart;

	// the second  part of the conjunction 
	vector<string30> m_SecondPart;
	
	// true, if the second part  is equal to the first, for example
	// "или ... или" (Russian)
	// "teils ... teils" (German)
	bool			m_bRepeating;

	bool operator < (const SDoubleConj& X) const
	{
		assert (m_FirstPart.size() > 0);
		assert (X.m_FirstPart.size() > 0);
		return strcmp(m_FirstPart[0].m_item, X.m_FirstPart[0].m_item) < 0;
	};
};



typedef map<string, vector<int>	> CSimplePrepToArticles;

class COborDic;
class CSyntaxOpt;

typedef void(COborDic::*WriteOborFuncType)(string s, int OborotNo); 



class COborDic 
{
public:
	//  all entries of the dictionary
	vector<COborotForSyntax>	m_Entries;

	// a specially  processed subset of  m_Entries, which contains 
	// disruptive conjunctions
	vector<SDoubleConj>			m_DisruptConj; 


	COborDic(const CSyntaxOpt*);

	const CSyntaxOpt*	GetOpt() const ;

	//  building members
	bool	ReadOborots(const CDictionary* piOborDic);
	
	//  using members
	const vector<CCoordConjType>&	GetCoordConjs() const {return m_SimpleCoordConj;};
	const StringVector&			GetSubConjs() const {return m_SimpleSubConj;};
	int								FindSubConj(const char* word_upper) const;

	int						FindSimplePrep(string strPrep);
	vector<int>				FindAllArticlesForSimplePrep(string strPrep);
	
	void					WriteSimplePrep(string s, int OborotNo); 
	void					TokenizeDoubleConj(string s, int OborotNo);
	void					WriteSimpleSubConj(string s, int OborotNo);
	
	
protected:
	//  syntax options 
	const CSyntaxOpt*			m_pSyntaxOptions;

	//  simple lists of conjunctions
	StringVector				m_SimpleSubConj;
	vector<CCoordConjType>		m_SimpleCoordConj;

	
	// a  from a simple preposition to all dictionary entries for  this  preposition
	CSimplePrepToArticles		m_mapSimplePrep;  



	virtual bool	ReadOborDic (const CDictionary* piOborDic) = 0;


	void	BuildOborots(int iUnit, const CDictionary* piOborDic, WriteOborFuncType WriteOborFunc);
	void	BuildOborot(string s, int OborotNo, WriteOborFuncType WriteOborFunc);
	void	MergeCasesOfSimililarSimplePreps();

	
	
};


#endif
