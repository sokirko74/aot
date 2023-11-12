#pragma once 

#include <string>

struct TDictConfig {
	
	std::string DictName;
	
	int MaxMeanNum;

	
	void ReadConfig(std::string path);
};

