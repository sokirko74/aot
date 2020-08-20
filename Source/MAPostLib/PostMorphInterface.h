#pragma once

#include <vector>
#include <string>
#include "../common/utilit.h"

using namespace std;
class CPlmLineCollection;

class CPostMorphInteface
{
protected:
    vector<string>  m_ResultLemWords;  
    MorphLanguageEnum m_Language;
public:
    bool			m_bCanChangeInputText;
	bool			m_bHumanFriendlyOutput;
	virtual bool	ProcessData(const CPlmLineCollection *piInTextItems) = 0;
    CPostMorphInteface() : m_Language(morphUnknown) {};
	virtual ~CPostMorphInteface() {};
    std::string    GetResultPlmLineUtf8(size_t i) const { 
            return convert_to_utf8(m_ResultLemWords[i], m_Language);
    };
    size_t    GetResultPlmLineCount() const {return m_ResultLemWords.size(); }
    void SwapResults(vector<string>& v) {v.swap(m_ResultLemWords);};
};

