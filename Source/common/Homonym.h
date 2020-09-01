// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once 

enum EGraPairType {Date, WebAddr, Oborot, Keyb,  GermanDividedCompound, UnknownPairType };



class CFormInfo;

#include "AncodePattern.h"

class CHomonym : public CAncodePattern
{
    
public:
	// morphology
	std::string	m_strLemma;

	// corpus frequence information
	long m_lFreqHom; 

	// Oborots
	int  m_OborotNo;
	bool m_bOborot1; 
	bool m_bOborot2; 
	bool m_bInOb;
	
	
	long m_lPradigmID;
	
	bool m_bSmallNumber;
	size_t  m_iCmpnLen;
	bool m_bCmplLem;	
		
	bool m_bRussianOdin;
	

	// a special slot to mark all homonyms which should be deleted by 
	// function CSynWord::DeleteMarkedHomonymsWithClauses
	bool m_bDelete;

	
	CHomonym(const	CAgramtab* pGramTab);
	
	
	bool operator<(const CHomonym& hom) const
	{
		return m_strLemma < hom.m_strLemma;	
	}

	bool	HasSetOfGrammemsExact(QWORD Grammems) const;
	bool	IsOb1() const;
	bool	IsOb2() const;
	bool	IsIsOb() const;
	bool	IsLemma(const std::string& lemma) const;
	bool	IsMorphNoun() const;
    bool	IsSynNoun() const;
	bool	IsLeftNounModifier() const;
	void	DeleteOborotMarks();

	bool	ProcessLemmaAndGrammems(const char* strLemma);
	void	SetLemma(std::string Lemma);
	std::string	GetGrammemsStr() const;
	
    void    SetMorphUnknown();
    void    SetHomonym(const CFormInfo* F);
    
	
	
};
