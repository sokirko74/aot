#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"

#include "morph_dict/common/argparse.h"




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
	CDictionary Dict;
	try {
		if (action == "from_txt")
		{
			Dict.LoadDictScheme(args.Retrieve("output-ross-folder"));
			Dict.ImportFromTextFile(args.Retrieve("input-file"));
			Dict.Save();
		}
		else if (action == "to_txt")
		{
			Dict.LoadAndExportDict(args.Retrieve("input-ross-folder"), args.Retrieve("output-file"));
		}
		else {
			LOGE << "bad action " << action;
			return -1;
		}
	}
	catch (CExpc c)
	{
		LOGE << c.what() << std::endl;
	}
	catch (article_parse_error c)
	{
		auto a = c.what();
		LOGE << a << std::endl;
	}
	catch (const std::exception& ex)
	{
		auto a = ex.what();
		LOGE << a << std::endl;
	}



}

