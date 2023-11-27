#include "sem_word.h"

void CSemWord::Init()
{
	m_CharCase = LowLow;
	m_FormGrammems = 0;
	m_TypeGrammems = 0;
	m_WordNo = -1;
	m_ParadigmId = -1;
	m_WordWeight = 0;
	m_AdditParadigmId = -1;
	m_bAdverbFromAdjective = false;
	m_ArabicNumber = false;
};

CSemWord::CSemWord() { Init(); }

CSemWord::CSemWord(long WordNo, std::string Lemma)
{
	Init();
	m_WordNo = WordNo;
	m_Lemma = Lemma;
};

// принадлежит ли граммема слову 
bool CSemWord::HasOneGrammem(grammem_t grammem) const
{
	return		((m_FormGrammems & _QM(grammem)) != 0)
		|| ((m_TypeGrammems & _QM(grammem)) != 0);
}

bool CSemWord:: operator == (const long& X) const
{
	return (m_WordNo == X);
};


bool CSemWord::IsRusSubstPronounP() const
{
	if (m_Poses != (1 << PRONOUN_P)) return false;

	return     (m_Lemma == "КАЖДЫЙ")
		|| (m_Lemma == "ЛЮБОЙ")
		|| (m_Lemma == "ТОТ")
		|| (m_Lemma == "КОТОРЫЙ")
		|| (m_Lemma == "ДРУГОЙ")
		|| (m_Lemma == "ОДИН");
};

grammems_mask_t CSemWord::GetAllGrammems() const
{
	return m_FormGrammems | m_TypeGrammems;
}

grammems_mask_t		CSemWord::GetFormGrammems() const
{
	return m_FormGrammems;
}

void		CSemWord::SetFormGrammems(grammems_mask_t g)
{
	m_FormGrammems = g;
}

grammems_mask_t		CSemWord::GetTypeGrammems() const
{
	return m_TypeGrammems;
}

void		CSemWord::SetTypeGrammems(grammems_mask_t g)
{
	m_TypeGrammems = g;
}

void		CSemWord::AddFormGrammem(grammem_t g)
{
	m_FormGrammems |= _QM(g);
};

