#include "PostLemWord.h"
#include "../LemmatizerLib/Lemmatizers.h"


CPostLemWord::CPostLemWord(const CAgramtab* pGramTab):m_pGramTab(pGramTab)
{
    m_bFirstUpperAlpha = false; 
    m_bQuoteMark = false;

}
size_t	CPostLemWord::GetHomonymsCount() const
{
    return m_Homonyms.size();
}

const CHomonym* CPostLemWord::GetHomonym(int i) const
{
    return &m_Homonyms[i];
}

CHomonym* CPostLemWord::GetHomonym(int i)
{
    return &m_Homonyms[i];
}
void CPostLemWord::EraseHomonym(int iHom)
{
    m_Homonyms.erase(m_Homonyms.begin() + iHom);
}

CHomonym* CPostLemWord::AddNewHomonym()
{
    m_Homonyms.push_back (CHomonym(m_pGramTab));
    return &m_Homonyms.back();

}

void CPostLemWord::InitLevelSpecific(CHomonym* pHom)
{
    m_bFirstUpperAlpha =  (m_Register == UpUp) || (m_Register == UpLow);
    m_bQuoteMark = HasDes(OPun) && m_strWord.find("\"") != string::npos;
    pHom->InitAncodePattern();
}

void CPostLemWord::DeleteAllHomonyms()
{
    m_Homonyms.clear();
}
void CPostLemWord::SafeDeleteMarkedHomonyms()
{
    for(int i = (int)GetHomonymsCount()- 1 ; i >= 0 ; i-- )
	{
        if (GetHomonymsCount() == 1) return;
		if( GetHomonym(i)->m_bDelete )
			EraseHomonym(i);			
	}
}



bool CPostLemWord::LemmatizeForm(const string& s, const CLemmatizer* pLemmatizer )
{
    vector<CFormInfo> Paradigms;
    string Word = s;
	if (!pLemmatizer->CreateParadigmCollection(false, Word, false, false, Paradigms)) return false;
    if (Paradigms.empty()) return false;
    DeleteAllHomonyms();
    for (size_t i=0; i < Paradigms.size(); i++)
    {
        CHomonym H(m_pGramTab);
        H.SetHomonym(&Paradigms[i]);
        m_Homonyms.push_back(H);
    }
    return true;

};

