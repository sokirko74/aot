
#ifndef __PostMorphInterface_h
#define __PostMorphInterface_h
#include <vector>
#include <string>
using namespace std;
class CPlmLineCollection;

class CPostMorphInteface
{
protected:
    vector<string>  m_ResultLemWords;    
public:
    bool			m_bCanChangeInputText;
	bool			m_bHumanFriendlyOutput;
	virtual bool	ProcessData(const CPlmLineCollection *piInTextItems) = 0;
	virtual ~CPostMorphInteface() {};
    const std::string&    GetResultLemWord(size_t i) const { return m_ResultLemWords[i];};
    size_t    GetResultLemWordCount() const {return m_ResultLemWords.size(); }
    void SwapResults(vector<string>& v) {v.swap(m_ResultLemWords);};
};

#endif



