#include "morph_dict/common/utilit.h"
#include "morph_dict/common/util_classes.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"

void PrintUsage()
{
	std::cout << "StructDict Loader\n";
	std::cout << "This program converts text version of structural dictionary to binary\n";
	std::cout << "Usage: StructDictLoader <ToTxt|FromTxt> <Inputfile> <OutputCatalog>\n";
	std::cout << "Example: StructDictLoader ToTxt ross.txt c:/rml/dicts/ross \n";
	exit(-1);
};

void export_dict(std::string fileName, std::string folder) {
	CDictionary Dict;
	if (!Dict.Load(folder.c_str()) || !Dict.ReadUnitComments())
	{
		throw CExpc("Cannot load dictionary from %s", folder.c_str());
	};
	std::ofstream outf (fileName, std::ios::binary);
	if (!outf.is_open())
	{
		throw CExpc("Cannot write to %s", fileName.c_str());
	};

	CTempArticle A;
	A.m_pRoss = &Dict;

	for (uint16_t i = 0; i < Dict.m_Units.size(); i++)
	{
		A.ReadFromDictionary(i, true, true);
		outf << "============\n" << Dict.GetUnitTextHeader(i);
		outf << A.GetArticleStrUtf8(true);
	};
}

void import_dict(std::string fileName, std::string folder) {
	CDictionary Dict;
	if (!FileExists(fileName.c_str()))
	{
		throw CExpc("Cannot read %s", fileName.c_str());
	};

	Dict.LoadOnlyConstants(folder.c_str());

	Dict.m_bShouldSaveComments = true;

	std::string Messages;
	if (!Dict.ImportFromText(fileName, 1, Messages)) {
		throw CExpc(Messages);
	}
	Dict.Save();
}

/*
void build_domains_json(std::string fileName) {
	CDictionary Dict;
	auto folder = GetParentPath(fileName);

	if (!Dict.Load(folder.c_str()) || !Dict.ReadUnitComments())
	{
		throw CExpc("Cannot load dictionary from %s", folder.c_str());
	};

	auto js_path  = MakePath(folder, "domains.json");

	std::ofstream outf(js_path);
	if (!outf.is_open())
	{
		throw CExpc("Cannot write to %s", js_path.c_str());
	};
	auto domains = nlohmann::json::array();
	for (size_t i = 0; i < Dict.m_Domens.size(); ++i) {
		for (size_t k = 0; k < Dict.m_Domens[i].PartsSize; ++k) {
			auto dom_str = Dict.m_Domens[Dict.m_Domens[i].Parts[k]].GetDomStr();
			std::cout << "part " << dom_str << "\n";
			Dict.m_Domens[i].DomainParts.push_back(dom_str);
		}
		auto js = Dict.m_Domens[i].WriteToJson();
		domains.push_back(js);
	}
	outf << domains.dump(4);

}

*/
int main(int argc, char** argv)
{
	if (argc != 4) {
		PrintUsage();
	}

	std::string action = argv[1];

	try {
		if (action == "FromTxt")
		{
			import_dict(argv[2], argv[3]);
		}
		else if (action == "ToTxt")
		{
			export_dict(argv[2], argv[3]);
		}
		//else if (action == "domain_json") {
		//	build_domains_json(argv[2]);
		//}
		else {
			PrintUsage();
			return -1;
		}
	}
	catch (CExpc c)
	{
		std::cerr << convert_to_utf8(c.m_strCause, morphRussian) << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << convert_to_utf8(ex.what(), morphRussian) << std::endl;
	}



}
