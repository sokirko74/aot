#include "syn_realization.h"

void CSynRealization::SetEmpty()
{
	m_Preps.clear();
	m_Conj.m_UnitNo = ErrUnitNo;
	m_Grammems = 0;
	m_Other = "";
	m_Cortege = TCortege();
};

// проверяет, приписан ли узлу предлог PrepNo
bool CSynRealization::HasThePrep(uint16_t UnitNo) const
{
	for (long i = 0; i < m_Preps.size(); i++)
		if (m_Preps[i].m_UnitNo == UnitNo)
			return true;

	return false;
};
