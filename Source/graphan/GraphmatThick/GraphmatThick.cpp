#include "morph_dict/common/util_classes.h"
#include "../GraphanLib/GraphmatFile.h"
#include "morph_dict/common/argparse.h"

void initArgParser(int argc, const char** argv, ArgumentParser& parser) {
	parser.AddOption("--help");
	parser.AddArgument("--language", "language");
	parser.AddArgument("--input-file", "specify file for input text in utf-8");
	parser.AddArgument("--output-file", "specify file for tokens table");
	parser.Parse(argc, argv);
}


int main(int argc, const char* argv[])
{
	ArgumentParser args;
	initArgParser(argc, argv, args);
	std::string strErr;
	if (!IsRmlRegistered(strErr))
	{
		std::cerr << strErr << "\n";
		exit(1);
	};

	try {			
		CGraphmatFile Graphan;
		Graphan.m_Language = args.GetLanguage();
		if (!Graphan.LoadDicts())
		{
			std::cerr << "Cannot load dictionaries:" << Graphan.GetLastError() << "\n";
			return 1;
		};
		auto inputFile = args.Retrieve("input-file");
		std::cout << "Loading file " << inputFile << "\n";
		Graphan.m_GraOutputFile = args.Retrieve("output-file");
		Graphan.LoadFileToGraphan(inputFile);
		std::cout  << "Found Units:" <<  Graphan.GetUnits().size() << "\n";
		return 0;
	}
	catch (CExpc c)
	{
		std::cerr << "An exception occurred:" << c.what() << "\n";
	}
	catch (...)
	{
		std::cerr << "general exception\n";
	};
	
	return 1;
}

