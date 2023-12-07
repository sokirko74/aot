#include "LemWord.h"
#include "Homonym.h"
#include "morph_dict/common/util_classes.h"
#include "morph_dict/agramtab/agramtab.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"

CHomonym::CHomonym(MorphLanguageEnum l)
{
	SetLanguage(l);
	m_iCmpnLen = 0;
	m_bCmplLem = false;
	m_lPradigmID = -1;
	m_bDelete = false;
	m_bRussianOdin = false;

	m_bOborot1 = false;
	m_bOborot2 = false;
	m_bInOb = false;
	m_OborotNo = -1;

	m_lFreqHom = 0;
}


void	CHomonym::SetLemma(std::string Lemma)
{
	MakeUpperUtf8(Lemma);
	m_strLemma = Lemma;
};

const std::string& CHomonym::GetLemma() const {
	return m_strLemma;
}


bool	CHomonym::HasSetOfGrammemsExact(uint64_t Grammems) const
{
	for (int i = 0; i < GetGramCodes().length(); i += 2)
	{
		uint64_t g;
		if (!GetGramTab()->GetGrammems(GetGramCodes().c_str()+i, g))
		{
				assert (false);
		};
		if ((g & Grammems) ==  Grammems)
			return true;

	};

	return false;;
};



bool CHomonym::IsOb1() const
{ 
	return m_bOborot1;	
}

bool CHomonym::IsOb2() const
{ 
	return m_bOborot2;	
}

bool CHomonym::IsIsOb() const
{
	return m_bInOb;	
}


bool CHomonym::IsLemma(const std::string& lemma) const 
{
	return m_strLemma == lemma;
};

bool CHomonym::IsSynNoun() const
{
	return GetGramTab()->IsSynNoun(m_iPoses, m_strLemma );
};

bool  CHomonym::IsMorphNoun() const
{
	return GetGramTab()->IsMorphNoun(m_iPoses);
};

bool	CHomonym::IsLeftNounModifier() const
{
	return GetGramTab()->is_left_noun_modifier(m_iPoses, m_iGrammems);
};


std::string	CHomonym::GetGrammemsStr() const
{
	return GetGrammemsByAncodes();
};

void CHomonym::DeleteOborotMarks()
{
		m_bOborot1 = false;
		m_bOborot2 = false;
		m_bInOb = false;
		m_OborotNo = -1;
};

void  CHomonym::SetPredictedWord(std::string gram_codes, std::string common_gram_codes)
{
    CAncodePattern::SetPredictedWord(gram_codes, common_gram_codes);
    m_lPradigmID = -1;
}

bool CHomonym::operator < (const CHomonym& hom) const
{
	return m_strLemma < hom.m_strLemma;
}

void  CHomonym::CopyFromFormInfo(const CFormInfo* F) {
	SetGramCodes(F->GetSrcAncode());
	m_CommonGramCode = F->GetCommonAncode();
	m_lPradigmID = F->GetParadigmId();
	if (F->GetLemSign() == '+') {
		m_SearchStatus = DictionaryWord;
	}
	else if (F->GetLemSign() == '-') {
		m_SearchStatus = PredictedWord;
	}
	else {
		throw CExpc("Bad lem sign %c", F->GetLemSign());
	}
	InitAncodePattern();

}

void  CHomonym::SetHomonym(const CFormInfo* F)
{
	CopyFromFormInfo(F);
    m_strLemma = convert_to_utf8(F->GetWordForm(0), GetGramTab()->m_Language);
	m_iCmpnLen = strcspn(m_strLemma.c_str(), "-");
	m_bCmplLem = ((BYTE)m_iCmpnLen != m_strLemma.length());
	m_lFreqHom = F->GetHomonymWeight();
	if (0 == m_lFreqHom)
		m_lFreqHom = 1;
}

std::string CHomonym::GetDebugString() const {
	assert(!GetLemma().empty());
	assert(!GetGramCodes().empty());
	assert(!m_CommonGramCode.empty());
	std::string r;
	r += " " + Format("%c", GetLemSign());
	r += " " + GetLemma();
	r += " " + GetGramTab()->GetTabStringByGramCode(m_CommonGramCode.c_str());
	for (int i = 0; i < GetGramCodes().length(); i += 2)
		r += " " + GetGramTab()->GetTabStringByGramCode(GetGramCodes().c_str() + i);
	return r;
}


