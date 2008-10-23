// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __PARADIGM_H_
#define __PARADIGM_H_

#include  "../common/utilit.h"
#include  "MorphAutomat.h"

class CLemmatizer;


class CFormInfo 
{
	bool					m_bPrefixesWereCut;
	bool					m_bFlexiaWasCut;
    string                  m_UserPrefix;
	CAutomAnnotationInner m_InnerAnnot;
	const CLemmaInfoAndLemma& GetLemmaInfo() const;
	const CFlexiaModel& GetFlexiaModel() const;
	bool  IsValid() const;

public:
	const CLemmatizer*			m_pParent;
	string						m_InputWordBase;
	bool						m_bFound;

	
	CFormInfo();
	void	Create(const CLemmatizer*	pParent, const CAutomAnnotationInner& A, const string& InputWordForm, bool bFound);
	void	Copy(const CFormInfo& from);
	void	AttachLemmatizer(const CLemmatizer* parent);
	bool	SetParadigmId(DWORD newVal);

	DWORD	GetParadigmId() const;
	DWORD	GetCount () const;
	
	string	GetSrcNorm() const; 
	// returns the weight of a word form 
	int		GetHomonymWeightWithForm(WORD pos) const; 
	// returns the weight of this word 
	int		GetWordWeight() const;

	string	GetAncode(WORD pos) const;
	string	GetWordForm (WORD pos) const;
	string	GetSrcAncode() const;
	int		GetHomonymWeight() const;
	BYTE	GetAccentedVowel(WORD pos) const;
	BYTE	GetSrcAccentedVowel() const;
	string	GetCommonAncode() const;
	char	GetLemSign() const; 
	size_t	GetLemmaPrefixLength() const; 
    string  FormatAsInPlmLine() const;
    void    SetUserPrefix(const string& UserPrefix);
    void    SetUserUnknown();
};


#endif //__PARADIGM_H_
