#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"
#include "LemmatizedText.h"
#include <fstream>


CLemmatizedText::CLemmatizedText(CAgramtab* pGramTab, CLemmatizer* pLemmatizer) :
	m_pGramTab(pGramTab),
	m_pLemmatizer(pLemmatizer)
{
};


void CLemmatizedText::CreateFromTokemized(const CGraphmatFile* Gr)
{
	if (!m_pLemmatizer || !m_pGramTab) {
		throw CExpc("no morphology in CLemmatizedText");
	}

	m_LemWords.clear();

	bool bInFixedExpression = false;
	bool bHasSpaceAfter = true;
	short oborot_no = -1;
	for (const CGraLine& token: Gr->GetUnits())
	{
		//=====   do not lemmatize oborots with EXPR=Fixed!
		if (token.HasDes(OFixedOborot))
		{
			bInFixedExpression = true;
		};
		if (token.HasDes(OEXPR1)) {
			oborot_no = token.GetOborotNo();
		}

		CLemWord word(m_pGramTab);
		word.CreateFromToken(token);
		word.m_bHasSpaceBefore = bHasSpaceAfter;
		bHasSpaceAfter = word.m_bSpace || token.HasSingleSpaceAfter();

		if (bInFixedExpression)
		{
			if (token.HasDes(OEXPR2))
				bInFixedExpression = false;
		}
		else if (m_pLemmatizer->GetLanguage() == token.GetTokenLanguage())
		{
			std::string word_s8 = token.GetToken();
			std::vector<CFormInfo> paradigms;
			m_pLemmatizer->CreateParadigmCollection(false, word_s8, !token.HasDes(OLw), true, paradigms);
			for(auto& p: paradigms)
			{
				CHomonym* h = word.AddNewHomonym();
				h->SetHomonym(&p);
				word.InitLevelSpecific(token, oborot_no, h);
			}
		}

		if (word.GetHomonymsCount() == 0 && !word.m_bSpace) {
			CHomonym* h = word.AddNewHomonym();
			if (!token.HasDes(OPun)) {
				h->SetMorphUnknown();
			}
			h->SetLemma(word.m_strUpperWord);
			word.InitLevelSpecific(token, oborot_no, h);
		}

		m_LemWords.push_back(word);
		if (token.HasDes(OEXPR2)) {
			oborot_no = -1;
		}
	}
}




bool CLemmatizedText::SaveToFile(std::string filename) const
{
	try
	{
		std::ofstream outp(filename.c_str(), std::ios::binary);
		if (!outp.is_open()) return false;
		for (auto& w : m_LemWords) {
			for (size_t i = 0; i < w.GetHomonymsCount(); ++i) {
				outp << w.GetDebugString(w.GetHomonym(i), i == 0) << "\n";
			}
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}


