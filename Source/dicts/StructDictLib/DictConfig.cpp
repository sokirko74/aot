#include "DictConfig.h"
#include "../../common/rapidjson.h"
#include "../../common/util_classes.h"
#include "fstream"
#include <assert.h>



void TDictConfig::ReadConfig(std::string folder) {
	auto path = MakePath(folder, "config.txt");
	std::ifstream inp;
	inp.open(path);
	if (!inp.good()) {
		throw CExpc("cannot open file %s", path.c_str());
	}
	rapidjson::Document doc;
	rapidjson::IStreamWrapper isw(inp);
	doc.ParseStream(isw);
	if (doc.HasMember("MaxMeanNum"))
		MaxMeanNum = doc["MaxMeanNum"].GetInt();
	else
		MaxMeanNum = 7;
	assert(1 <= MaxMeanNum < 15);
	DictName = doc["DictName"].GetString();
	DictFolder = folder;
	

}