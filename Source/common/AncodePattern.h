#ifndef ancode_pattern_h
#define ancode_pattern_h

class CAgramtab;
class CSyntaxOpt;
#include "utilit.h"

class CAncodePattern 
{
    const	CAgramtab* m_pGramTab;
     void    ResetFlags();

public:
	QWORD			m_TypeGrammems;
	QWORD			m_iGrammems;
	char			m_LemSign;
	string			m_CommonGramCode;
	string			m_GramCodes;
	size_t			m_iPoses;
    bool            m_bUnkGramcodes;
			
	// all single preposition interpretations
	vector<int>		m_SimplePrepNos;

	CAncodePattern(const	CAgramtab* pGramTab);
    const	CAgramtab* GetGramTab() const;
	void	CopyAncodePattern(const CAncodePattern& X);
	bool	HasGrammem(BYTE gram) const;
	bool	HasPos(BYTE pos) const;
	string	GetGrammemsByAncodes() const;
	bool	ModifyGrammems(QWORD Grammems, size_t Poses=0xffffffff);
	bool	InitAncodePattern();
    void    SetMorphUnknown();
    bool    DeleteAncodesByGrammemIfCan(BYTE Grammem);
    string	GetPartOfSpeechStr() const;

};


#endif
