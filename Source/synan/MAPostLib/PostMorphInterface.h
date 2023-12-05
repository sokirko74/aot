#pragma once

#include <vector>
#include <string>
#include "morph_dict/common/utilit.h"
#include "morphan/LemmatizerLib/LemmatizedText.h"

using namespace std;


class CPostMorphInteface
{
public:
    MorphLanguageEnum m_Language;
    bool			m_bCanChangeInputText;
	bool			m_bHumanFriendlyOutput;

	virtual void ProcessData(CLemmatizedText& text) = 0;
    
    CPostMorphInteface() : m_Language(morphUnknown) {};
	
    virtual ~CPostMorphInteface() {};
   
};

