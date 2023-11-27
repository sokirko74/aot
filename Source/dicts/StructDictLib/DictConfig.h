#pragma once 

#include <string>
#include <vector>

struct TDictConfig {
	
	std::string DictName;
	
	int MaxMeanNum;

	std::string DictFolder;

	std::vector<std::string> DictUsers;
	
	void ReadConfig(std::string path);
};

