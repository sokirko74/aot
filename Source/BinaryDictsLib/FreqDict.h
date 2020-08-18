#pragma once

#include "ParadigmPairFreq.h"       


class CFreqDict
{
	std::vector<CParadigmPairAndFreq> word_vec;
public:
	CFreqDict();
	bool Load(std::string  path);
	WORD GetFreq(DWORD EngParadigmId, DWORD RusParadigmId) const;
	
};
