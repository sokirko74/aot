#pragma once

struct CParadigmPairAndFreq
{
	uint32_t	m_EngParadigmId;
	uint32_t	m_RusParadigmId;
	uint16_t	m_Freq;

	CParadigmPairAndFreq() {};
	CParadigmPairAndFreq(uint32_t EngParadigmId, uint32_t RusParadigmId, uint16_t Freq)
		:m_EngParadigmId(EngParadigmId), m_RusParadigmId(RusParadigmId), m_Freq(Freq){}
	bool operator < (const CParadigmPairAndFreq &p) const{
		if(m_EngParadigmId < p.m_EngParadigmId) return true;
		if   (    (m_EngParadigmId == p.m_EngParadigmId)
			   && (m_RusParadigmId < p.m_RusParadigmId)
			 ) return true;
		return false;
	}
};
