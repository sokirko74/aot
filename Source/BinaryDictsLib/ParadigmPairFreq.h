#pragma once

struct CParadigmPairAndFreq
{
	uint32_t	m_EngParadigmId;
	uint32_t	m_RusParadigmId;
	WORD	m_Freq;

	CParadigmPairAndFreq() {};
	CParadigmPairAndFreq(uint32_t EngParadigmId, uint32_t RusParadigmId, WORD Freq)
		:m_EngParadigmId(EngParadigmId), m_RusParadigmId(RusParadigmId), m_Freq(Freq){}
	bool operator < (const CParadigmPairAndFreq &p) const{
		if(m_EngParadigmId < p.m_EngParadigmId) return true;
		if   (    (m_EngParadigmId == p.m_EngParadigmId)
			   && (m_RusParadigmId < p.m_RusParadigmId)
			 ) return true;
		return false;
	}
};
