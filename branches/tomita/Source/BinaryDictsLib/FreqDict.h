// FreqDict.h : Declaration of the CFreqDict

#ifndef __FREQDICT_H_
#define __FREQDICT_H_
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <cassert>

#include "ParadigmPairFreq.h"       


class CFreqDict
{
	std::vector<CParadigmPairAndFreq> word_vec;
public:
	CFreqDict();
	bool Load(string  path);
	WORD GetFreq(DWORD EngParadigmId, DWORD RusParadigmId) const;
	
};

#endif //__FREQDICT_H_
