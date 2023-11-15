#include "DictConfig.h"
#include "../../common/json.h"
#include "fstream"
#include <assert.h>



void TDictConfig::ReadConfig(std::string folder) {
	std::ifstream inp;
	auto path = MakePath(folder, "config.txt");
	inp.open(path);
	if (!inp.good()) {
		throw CExpc("cannot open file %s", path.c_str());
	}
	auto config = nlohmann::json::parse(inp);

	MaxMeanNum = config.value("MaxMeanNum", 7);
	assert(1 <= MaxMeanNum < 15);
	DictName = config.at("DictName");
	DictFolder = folder;
	

}