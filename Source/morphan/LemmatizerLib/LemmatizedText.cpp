#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"
#include "LemmatizedText.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#include <fstream>


CLemmatizedText::CLemmatizedText(MorphLanguageEnum l)
{
	m_Language = l;
};


void CLemmatizedText::CreateFromTokemized(const CGraphmatFile* Gr)
{
	auto lemmatizer = GetMHolder(m_Language).m_pLemmatizer;

	
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

		CLemWord word(m_Language);
		word.CreateFromToken(token);
		word.m_bHasSpaceBefore = bHasSpaceAfter;
		bHasSpaceAfter = word.m_bSpace || token.HasSingleSpaceAfter();

		if (bInFixedExpression)
		{
			if (token.HasDes(OEXPR2))
				bInFixedExpression = false;
		}
		else if (m_Language == token.GetTokenLanguage())
		{
			std::string word_s8 = token.GetToken();
			std::vector<CFormInfo> paradigms;
			lemmatizer->CreateParadigmCollection(false, word_s8, !token.HasDes(OLw), true, paradigms);
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
				h->SetPredictedWord();
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

MorphLanguageEnum CLemmatizedText::GetDictLanguage() const {
	return m_Language;
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


