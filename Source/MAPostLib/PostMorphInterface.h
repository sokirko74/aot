
#ifndef __PostMorphInterface_h
#define __PostMorphInterface_h

class CPlmLineCollection;

class CPostMorphInteface
{
public:
    bool			m_bCanChangeInputText;
	bool			m_bHumanFriendlyOutput;
	virtual bool	ProcessData(const CPlmLineCollection *piInTextItems, CPlmLineCollection& piOutTextItems) = 0;
	virtual ~CPostMorphInteface() {};
};

#endif



