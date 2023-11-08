#include "morph_dict/common/utilit.h"
#include "morph_dict/common/util_classes.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"
#include "morph_dict/common/argparse.h"



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

	CTempArticle A(Dict.m_MaxNumDom);
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

void build_fields_json(std::string fileName) {
	CDictionary Dict;
	auto folder = GetParentPath(fileName);
	//MakePathAndCheck(folder, "config.txt", Dict.ConfigFile);
	//MakePathAndCheck(folder, "fields.bin", Dict.FieldsFile);


	Dict.ReadConfig();
	Dict.BuildFields(Dict.m_MaxNumDom);
	//Dict.WriteFieldsJson();

}

void initArgParser(int argc, const char** argv, ArgumentParser& parser) {
	parser.AddOption("--help");
	parser.AddArgument("--action", "action", true);
	parser.AddArgument("--input-file", "input file", true);
	parser.AddArgument("--output-file", "input file", true);
	parser.AddArgument("--output-ross-folder", "output file", true);
	parser.AddArgument("--input-ross-folder", "c:/*.txt", true);
	parser.AddArgument("--log-level", "log level", true);
	parser.Parse(argc, argv);
}

int main(int argc, const char** argv)
{
	ArgumentParser args;
	initArgParser(argc, argv, args);
	init_plog(args.GetLogLevel(), "struct_dict_load.log");
	std::string action = args.Retrieve("action");
	try {
		if (action == "from_txt")
		{
			auto inp = args.Retrieve("input-file");
			import_dict(inp, args.Retrieve("output-ross-folder"));
		}
		else if (action == "to_txt")
		{
			export_dict(args.Retrieve("output-file"), args.Retrieve("input-ross-folder"));
		}
		else {
			LOGE << "bad action " << action;
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

