#include "SynanDmn.h"

#include "../common/utilit.h"
#include "../common/argparse.h"

void UnloadData() {
	UnloadSynan();
};


void  termination_handler(int signum) {
	synan_daemon_log("termination_handler synan daemon");
	UnloadData();
	exit(1);
};

bool GetLockName(string& Result) {
	try {
		Result = GetRmlVariable() +"/SynanDaemon.lck";
		return true;
	}
	catch (CExpc c)
	{
	    fprintf(stderr, "%s\n", c.m_strCause.c_str());
		return false;
	}
};

void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
	parser.AddArgument("--logmode", "log mode (quiet, normal or debug)", true);
	parser.AddOption("--quiet");
	parser.Parse(argc, argv);
}


int	main(int argc, const char	**argv) {
	ArgumentParser args;
	initArgParser(argc, argv, args);
	DaemonLogModeEnum logMode = dlmNormal;
	if (args.Exists("logmode")) {
		logMode = ParseDaemonLogMode(args.Retrieve("logmode"));
	}

	string LockFileName;
	if (!GetLockName(LockFileName))	{
		return 1;
	};
	synan_daemon_log ("Entering Synan Daemon");
	
	if (FileExists(LockFileName.c_str())) {
		std::cerr << "removing " << LockFileName << "\n";
		remove(LockFileName.c_str());
	}

	try {
		FILE* fp = fopen (LockFileName.c_str(),	"w");
		if (!fp) {
			std::cerr << "Cannot open file	" << LockFileName << "\n";
			return 1;
		};
		fprintf(fp,	"%i", GetPID());
		fclose (fp);

		synan_daemon_log ("SocketInitialize");
		InitSockets();

		if (!LoadSynan ()) {
			synan_daemon_log ("Cannot load Syntax");			
			return 1;
		};

		SetSigTermHandler(termination_handler);
		
		const char* portKey = "Software\\Dialing\\Synan\\HttpPort";

		if (!CanGetRegistryString(portKey)) {
			synan_daemon_log(Format("  Cannot find the registry key %s\n", portKey));
			return 1;
		};

		int port = atoi(GetRegistryString(portKey).c_str());
		synan_daemon_log(Format("  try using port %i  to start synan daemon \n", port));

		TSynanHttpServer Server(port, synan_daemon_log, logMode);
		std::cerr << "Start listen socket\n";
		if (!Server.Start()) {
			return 1;
		}
	}
	catch(...)
	{
		std::cerr << "General exception\n";
		return 1;
	};

	UnloadData();
	return 0;
}




