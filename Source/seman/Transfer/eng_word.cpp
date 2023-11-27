#include "eng_word.h"


void CEngSemWord::SetTense(EngVerbTenseEnum	NewTense, std::string Maker)
{
	m_Tense = NewTense;
	m_TenseHistory.push_back(CTenseHistory(NewTense, Maker));
};

EngVerbTenseEnum CEngSemWord::GetTense() const
{
	return m_Tense;
};

void CEngSemWord::Init()
{
	m_bDoNotChangeForm = false;
	m_Poses = 0;
	m_bMorphologicalPassiveForm = false;
	m_bImperative = false;
	m_OneselfStr = "";
	SetTense(zero_tn, "Constructor");

};

CEngSemWord::CEngSemWord() : CSemWord()
{
	Init();
};

CEngSemWord::CEngSemWord(const CSemWord& X)
{
	*((CSemWord*)this) = X;
	Init();
};

// принадлежит ли данная часть речи набору частей речи, который приписан слову?
bool   CEngSemWord::HasPOS(part_of_speech_t POS) const
{
	return (m_Poses & (1 << POS)) > 0;
};

const std::string& CEngSemWord::GetWord() const {
	return m_WordStr;
}


// устанавливает  нужный регистр
void CEngSemWord::fix_case()
{
	if (m_WordStr.empty() || m_bDoNotChangeForm) return;
	if (m_CharCase == UpUp) {
		MakeUpperUtf8(m_WordStr);
	}
	else if ((m_CharCase == UpLow) || HasOneGrammem(eProper))
	{
		MakeTitleUtf8(m_WordStr);
	}
	else {
		MakeLowerUtf8(m_WordStr);
	}
}

void CEngSemWord::SetWord(const std::string& s, bool set_case) {
	m_WordStr = s;
	if (set_case) {
		fix_case();
	}
}

void CEngSemWord::AddPostfix(const std::string& suffix) {
	m_WordStr += suffix;
}

void CEngSemWord::AddPrefix(const std::string& prefix, bool set_case) {
	m_WordStr = prefix + m_WordStr;
	if (set_case) {
		fix_case();
	}

}