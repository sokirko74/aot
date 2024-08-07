#include "rus_sem_word.h"

bool HasReflexiveSuffix(const std::string& s)
{
	return endswith(s, "СЯ") || endswith(s, "СЬ");
};

void CRusSemWord::Init()
{
	m_IsPunct = false;
	m_ILE = false;
	m_bRomanNumber = false;
	m_SynWordNo = -1;
	m_ArabicNumber = false;
};

CRusSemWord::CRusSemWord() : CSemWord()
{
	Init();
};
CRusSemWord::CRusSemWord(long WordNo, std::string Lemma) : CSemWord(WordNo, Lemma)
{
	Init();
};


bool CRusSemWord::IsReflexiveVerb() const
{
	return  (HasPOS(VERB)
		|| HasPOS(INFINITIVE)
		|| HasPOS(PARTICIPLE)
		|| HasPOS(ADVERB_PARTICIPLE)
		)
		&& HasReflexiveSuffix(GetWord());
};

part_of_speech_mask_t CRusSemWord::GetRusRichPoses() const
{
	part_of_speech_mask_t Poses = m_Poses;

	if (m_Lemma == "ЛИ")
		Poses &= ~(1 << CONJ);

	if ((m_Lemma == "ПЕРВОЕ")
		|| (m_Lemma == "ВТОРОЕ")
		|| (m_Lemma == "ТРЕТЬЕ")
		|| (m_Lemma == "ЧЕТВЕРТОЕ")
		)
		Poses |= (1 << NUMERAL_P);


	return Poses;
};

bool   CRusSemWord::HasPOS(part_of_speech_t POS) const
{
	return  (GetRusRichPoses() & 1 << POS) > 0;
};

bool CRusSemWord::IsEqualMorph(const CRusSemWord& W) 	const
{
	return    (m_Lemma == W.m_Lemma)
		&& (GetWord() == W.GetWord())
		&& (GetAllGrammems() == W.GetAllGrammems())
		&& (m_Poses == W.m_Poses);
};


void CRusSemWord::InitWordFeatures(const CSentence* piSent, long WordNo) {
    const CSynWord& W = piSent->m_Words[WordNo];
    SetWord(W.m_strWord);
    m_GraphDescrs = " " + W.BuildGraphemDescr() + " ";
    m_IsPunct = W.HasDes(OPun);
    m_ILE = W.HasDes(OLLE);
    m_ArabicNumber = W.HasDes(ODigits);
    m_bRomanNumber = W.HasDes(ORoman);
    if (m_ILE) SetFormGrammems(rAllCases);
    m_CharCase = W.m_Register;

    if ((WordNo == 0) && (m_CharCase == UpLow))
        if (!HasOneGrammem(rToponym)
            && !HasOneGrammem(rName)
            && !HasOneGrammem(rPatronymic)
            )
            m_CharCase = LowLow;

    m_PostPuncts = "";
    for (long i = WordNo + 1; i < piSent->m_Words.size(); i++) {
        std::string w = piSent->m_Words[i].m_strWord;
        if (!w.empty() && std::iswpunct(w[0]))
            m_PostPuncts += w;
        else
            break;
    };

};

const std::string& CRusSemWord::GetWord() const {
    return m_WordUpper;
}

void CRusSemWord::SetWord(const std::string& s)  {
    m_WordUpper = s;
    MakeUpperUtf8(m_WordUpper);
}

// является ли данное слово кавычкой
bool CRusSemWord::IsQuoteMark() const
{
    return GetWord() == "\"";
};
