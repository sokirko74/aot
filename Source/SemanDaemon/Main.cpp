#include "translate.h"

#include "../common/utilit.h"
#include "../common/argparse.h"


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
	parser.AddArgument("--logmode", "log mode (quiet, normal or debug)", true);
	parser.Parse(argc, argv);
}

int	main(int argc, const char **argv) {
	ArgumentParser args;
	initArgParser(argc, argv, args);
	DaemonLogModeEnum logMode = dlmNormal;
	if (args.Exists("logmode")) {
		logMode = ParseDaemonLogMode(args.Retrieve("logmode"));
	}
	TSemanHttpServer Server;
	try {
		DealWithLockFile("SemanDaemon.lck");
		const char* portKey = "Software\\Dialing\\Seman\\HttpPort";

		if (!CanGetRegistryString(portKey)) {
			throw CExpc(Format("  Cannot find the registry key %s\n", portKey));
		};

		int port = atoi(GetRegistryString(portKey).c_str());

		Server.Initialize(port, logMode, "seman_dmn.log");
	
		Server.Load();
		Server.Start();
	}
	catch (CExpc c)
	{
		std::cerr << c.m_strCause << "\n";
		return 1;
	}
	catch(...)
	{
		std::cerr << "General exception\n";
		return 1;
	};
	return 0;
}





