// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#pragma  once

#include  "../common/utilit.h"
#include  "MorphAutomat.h"
#include "MorphDict.h"

class CLemmatizer;


class CFormInfo 
{
	bool					m_bPrefixesWereCut;
	bool					m_bFlexiaWasCut;
    std::string                  m_UserPrefix;
	CAutomAnnotationInner m_InnerAnnot;
	const CLemmaInfoAndLemma& GetLemmaInfo() const;
	const CFlexiaModel& GetFlexiaModel() const;
	bool  IsValid() const;

public:
	const CLemmatizer*			m_pParent;
	std::string						m_InputWordBase;
	bool						m_bFound;

	
	CFormInfo();
	void	Create(const CLemmatizer*	pParent, const CAutomAnnotationInner& A, const std::string& InputWordForm, bool bFound);
	void	Copy(const CFormInfo& from);
	void	AttachLemmatizer(const CLemmatizer* parent);
	bool	SetParadigmId(uint32_t newVal);

	uint32_t	GetParadigmId() const;
	uint32_t	GetCount () const;
	
	std::string	GetSrcNorm() const; 
	// returns the weight of a word form 
	int		GetHomonymWeightWithForm(uint16_t pos) const; 
	// returns the weight of this word 
	int		GetWordWeight() const;

	std::string	GetAncode(uint16_t pos) const;
	std::string	GetWordForm (uint16_t pos) const;
	std::string	GetSrcAncode() const;
	int		GetHomonymWeight() const;
	BYTE	GetAccentedVowel(uint16_t pos) const;
	BYTE	GetSrcAccentedVowel() const;
	std::string	GetCommonAncode() const;
	char	GetLemSign() const; 
	size_t	GetLemmaPrefixLength() const; 
    std::string  FormatAsInPlmLine() const;
    void    SetUserPrefix(const std::string& UserPrefix);
    void    SetUserUnknown();
};

