#ifndef __PARADIGMPAIRFREQ_H_
#define __PARADIGMPAIRFREQ_H_

#include "../common/utilit.h"

struct CParadigmPairAndFreq
{
	DWORD	m_EngParadigmId;
	DWORD	m_RusParadigmId;
	WORD	m_Freq;

	CParadigmPairAndFreq() {};
	CParadigmPairAndFreq(DWORD EngParadigmId, DWORD RusParadigmId, WORD Freq)
		:m_EngParadigmId(EngParadigmId), m_RusParadigmId(RusParadigmId), m_Freq(Freq){}
	bool operator < (const CParadigmPairAndFreq &p) const{
		if(m_EngParadigmId < p.m_EngParadigmId) return true;
		if   (    (m_EngParadigmId == p.m_EngParadigmId)
			   && (m_RusParadigmId < p.m_RusParadigmId)
			 ) return true;
		return false;
	}
};




#endif
