#pragma once

#include "ParadigmPairFreq.h"       


class CFreqDict
{
	std::vector<CParadigmPairAndFreq> word_vec;
public:
	CFreqDict();
	bool Load(std::string  path);
	WORD GetFreq(uint32_t EngParadigmId, uint32_t RusParadigmId) const;
	
};
