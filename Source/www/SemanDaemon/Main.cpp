#include "translate.h"

#include "morph_dict/common/utilit.h"
#include "morph_dict/common/argparse.h"



void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddArgument("--host", "0.0.0.0 or other ip");
    parser.AddArgument("--port", "17019");
    parser.AddArgument("--log-level", "log level", true);
    parser.Parse(argc, argv);
}

int	main(int argc, const char **argv) {
	ArgumentParser args;
	initArgParser(argc, argv, args);
    auto log_path = GetLogPath("seman_dmn.log");
    init_plog(args.GetLogLevel(), log_path, false);
	TSemanHttpServer Server;
	try {
		DealWithLockFile("SemanDaemon.lck");
        GlobalLoadMorphHolder(morphRussian);
        GlobalLoadMorphHolder(morphEnglish);
        Server.Initialize(args.Retrieve("host"), atoi(args.Retrieve("port").c_str()));
		Server.Load();
		Server.Start();
	}
	catch (std::exception& c)
	{
		LOGE << c.what();
		return 1;
	}
	catch(...)
	{
		LOGE << "General exception";
		return 1;
	};
	return 0;
}





