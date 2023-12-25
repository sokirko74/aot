#include "translate.h"

#include "morph_dict/common/utilit.h"
#include "morph_dict/common/argparse.h"


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddArgument("--log-level", "log level", true);

    parser.Parse(argc, argv);
}

int	main(int argc, const char **argv) {
	ArgumentParser args;
	initArgParser(argc, argv, args);
    auto log_path = GetLogPath("seman_dmn.log");
    init_plog(args.GetLogLevel(), log_path);
	TSemanHttpServer Server;
	try {
		DealWithLockFile("SemanDaemon.lck");
		const char* portKey = "Software\\Dialing\\Seman\\HttpPort";

		if (!CanGetRegistryString(portKey)) {
			throw CExpc(Format("  Cannot find the registry key %s\n", portKey));
		};
		int port = atoi(GetRegistryString(portKey).c_str());
		Server.Initialize(port);
	
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





