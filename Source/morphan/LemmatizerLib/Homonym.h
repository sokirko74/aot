#pragma once 

#include "morph_dict/lemmatizer_base_lib/AncodePattern.h"
#include "synan/SimpleGrammarLib/InputSymbol.h"


enum EGraPairType {Date, WebAddr, Oborot, Keyb,  GermanDividedCompound, UnknownPairType };

class CFormInfo;


class CHomonym : public CAncodePattern
{
	std::string	m_strLemma;

public:

	// corpus frequence information
	long m_lFreqHom; 

	// Oborots
	int  m_OborotNo;
	bool m_bOborot1; 
	bool m_bOborot2; 
	bool m_bInOb;
	
	
	uint32_t m_lPradigmID;
	
	bool m_bSmallNumber;
	size_t  m_iCmpnLen;
	bool m_bCmplLem;	
		
	bool m_bRussianOdin;
	
	std::string m_OtherWordDescriptors;

	// a special slot to mark all homonyms which should be deleted by 
	// function CSynWord::DeleteMarkedHomonymsWithClauses
	bool m_bDelete;

	//  all possible interpretation in the format grammar (SimpleGrammar, GLR)
	std::set<CInputSymbol> m_AutomatSymbolInterpetation;

	
	CHomonym(MorphLanguageEnum l);
	
	
	bool operator<(const CHomonym& hom) const;
	bool	HasSetOfGrammemsExact(uint64_t Grammems) const;
	bool	IsOb1() const;
	bool	IsOb2() const;
	bool	IsIsOb() const;
	bool	IsLemma(const std::string& lemma) const;
	bool	IsMorphNoun() const;
    bool	IsSynNoun() const;
	bool	IsLeftNounModifier() const;
	void	DeleteOborotMarks();

	void	SetLemma(std::string Lemma);
	const std::string& GetLemma() const;
	std::string	GetGrammemsStr() const;
	
    void    SetPredictedWord(std::string gram_codes, std::string common_gram_codes="??");
    void    SetHomonym(const CFormInfo* F);
	void	CopyFromFormInfo(const CFormInfo* F);
    
	std::string GetDebugString() const;
	
};
