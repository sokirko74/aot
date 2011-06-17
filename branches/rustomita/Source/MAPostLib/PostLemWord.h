#ifndef postlemword_h
 #define postlemword_h

#include "../common/LemWord.h"       

class CLemmatizer;
class CPostLemWord : public CLemWord
{
    vector<CHomonym> m_Homonyms;
    const CAgramtab* m_pGramTab;
public:
    bool	m_bFirstUpperAlpha;
    bool    m_bQuoteMark;

    CPostLemWord(const CAgramtab* pGramTab);
    size_t	GetHomonymsCount() const; 
	const CHomonym* GetHomonym(int i) const;
	CHomonym* GetHomonym(int i);
	void EraseHomonym(int iHom);;
    CHomonym* AddNewHomonym();;
    void InitLevelSpecific(CHomonym* pHom);
    void DeleteAllHomonyms();
    void SafeDeleteMarkedHomonyms();
    bool LemmatizeForm(const string& s, const CLemmatizer* pLemmatizer );
};


#endif
