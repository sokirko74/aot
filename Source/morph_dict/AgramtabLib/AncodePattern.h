#ifndef ancode_pattern_h
#define ancode_pattern_h

class CAgramtab;
#include "morph_dict/common/utilit.h"

class CAncodePattern 
{
    const	CAgramtab* m_pGramTab;
    void    ResetFlags();
	std::string			m_GramCodes;

public:
	uint64_t			m_TypeGrammems;
	uint64_t			m_iGrammems;
	char			m_LemSign;
	std::string			m_CommonGramCode;
	part_of_speech_mask_t    m_iPoses;
    bool            m_bUnkGramcodes;
			
	// all single preposition interpretations
	std::vector<int>		m_SimplePrepNos;

	CAncodePattern(const	CAgramtab* pGramTab);
    const	CAgramtab* GetGramTab() const;
	void	CopyAncodePattern(const CAncodePattern& X);
	bool	HasGrammem(BYTE gram) const;
	bool	HasPos(BYTE pos) const;
	std::string	GetGrammemsByAncodes() const;
	bool	ModifyGrammems(uint64_t Grammems, part_of_speech_mask_t Poses=0xffffffff);
	bool	InitAncodePattern();
    void    SetMorphUnknown();
    bool    DeleteAncodesByGrammemIfCan(BYTE Grammem);
    std::string	GetPartOfSpeechStr() const;
    const std::string& GetGramCodes() const;
    void  SetGramCodes(const std::string&);
    void  SetGramCodes(const char*);

};


#endif
