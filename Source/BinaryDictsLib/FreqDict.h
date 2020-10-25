#pragma once

#include "ParadigmPairFreq.h"       


class CFreqDict
{
	std::vector<CParadigmPairAndFreq> word_vec;
public:
	CFreqDict();
	bool Load(std::string  path);
	uint16_t GetFreq(uint32_t EngParadigmId, uint32_t RusParadigmId) const;
	
};
