
#ifndef __GermanPostMorph_h
#define __GermanPostMorph_h


#include "../MAPostLib/PostMorphInterface.h" 
#include "../SimpleGrammarLib/SimpleGrammar.h" 

class CGermanPostMorph : public CPostMorphInteface
{
	CWorkGrammar		m_PersonGrammar;
	const CAgramtab*	m_GerGramTab;

public:
	CGermanPostMorph();
	~CGermanPostMorph();

	bool	ProcessData(const CPlmLineCollection *piInTextItems, CPlmLineCollection& piOutTextItems);
	bool	Init(const CAgramtab* GerGramTab);
	
};

#endif
