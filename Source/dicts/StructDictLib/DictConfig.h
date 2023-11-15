#pragma once 

#include <string>

struct TDictConfig {
	
	std::string DictName;
	
	int MaxMeanNum;

	std::string DictFolder;

	
	void ReadConfig(std::string path);
};

