// FreqDict.cpp : Implementation of CFreqDict
#include "stdafx.h"
#include "../common/utilit.h"
#include "FreqDict.h"
#include "../common/bserialize.h"
/////////////////////////////////////////////////////////////////////////////
// CFreqDict

inline size_t get_size_in_bytes (const CParadigmPairAndFreq& t)
{
	return get_size_in_bytes(t.m_EngParadigmId) + get_size_in_bytes(t.m_RusParadigmId) + get_size_in_bytes(t.m_Freq);

};

inline size_t save_to_bytes(const CParadigmPairAndFreq& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_EngParadigmId, buf);
	buf += save_to_bytes(i.m_RusParadigmId, buf);
	buf += save_to_bytes(i.m_Freq, buf);
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(CParadigmPairAndFreq& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_EngParadigmId, buf);
	buf += restore_from_bytes(i.m_RusParadigmId, buf);
	buf += restore_from_bytes(i.m_Freq, buf);
	return get_size_in_bytes(i);
}




CFreqDict::CFreqDict()
{
}

bool CFreqDict::Load(string _path)
{
	// TODO: Add your implementation code here
	ReadVector(_path, word_vec);
	if (word_vec.empty())
		return false;

	std::sort(word_vec.begin(), word_vec.end());

	return true;
}


// выдает частоту перевода русской леммы RusParadigmId в английскую лемму EngParadigmId
WORD CFreqDict::GetFreq(DWORD EngParadigmId, DWORD RusParadigmId) const
{
	// TODO: Add your implementation code here
	typedef std::vector<CParadigmPairAndFreq>::const_iterator Iter;
	std::pair<Iter, Iter> range = std::equal_range(word_vec.begin(), word_vec.end(), CParadigmPairAndFreq(EngParadigmId, RusParadigmId, 0));
	WORD freq = 0;
	for(Iter it = range.first; it != range.second; ++it){
		freq += range.first->m_Freq;
	}
	return freq;
}

