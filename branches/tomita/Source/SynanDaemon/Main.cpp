
#include "../common/utilit.h"
#include "../common/string_socket.cpp"
#include <syslog.h>
#include <sys/wait.h>

void PrintUsage()
{
	printf ("A daemon for working Dialing-DWDS Shallow Syntax\n");
	printf ("Usage: ConcordDaemon [start|stop|start_cli]  [--no-query-dump]\n");	
	printf ("Where start - starts daemon\n");	
	printf ("      stop - stops  daemon\n");	
	printf ("      start_cli - starts the program not as a daemon bat as a simple console program\n");	
	exit(1);
};



extern bool LoadSynan();
extern bool UnloadSynan();
extern void synan_daemon_log(const string&);



void UnloadData()
{
	UnloadSynan();
	SocketDeinitialize();
	closelog();
};


void  termination_handler(int signum)
{
	UnloadData();
	syslog(LOG_ALERT, "Exiting from concordance ");
	exit(1);
};


bool GetLockName(string& Result)
{
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



int	main(int argc, char	**argv)
{
	bool bAllQueriesDump = true;
	if (argc < 2) 
		PrintUsage();
	else
		if (argc ==	2)
		{
			if (	 !strcmp (argv[0], "-h")
				|| !strcmp (argv[0], "-help")
				|| !strcmp (argv[0], "/h")
				|| !strcmp (argv[0], "/help")
				)
				PrintUsage();
		}
		else
		{
			if (!strcmp(argv [2], "--no-query-dump"))
			{
				bAllQueriesDump	= false;
				fprintf(stderr, "no dump of all queries \n");
			}
			else
				PrintUsage();
		};


	string LockFileName;
	if (!GetLockName(LockFileName))
	{
		return 1;
	};

	string Action =	argv[1];
	if (		(Action	!= "start")
		&&	(Action	!= "start_cli")
		&&	(Action	!= "stop")
		)
		PrintUsage();

	if (Action == "stop")
	{					
		if (access(LockFileName.c_str(), 04) !=	0)
		{
			printf("Concordance	was	not	started!\n");
			return 1;
		};
		FILE* fp = fopen (LockFileName.c_str(),	"r");
		if (!fp)
		{
			printf("Cannot open	file %s!\n", LockFileName.c_str());
			return 1;
		};
		pid_t pid;
		fscanf(fp, "%i", &pid);
		fclose (fp);

		if (kill (pid, SIGTERM)	== 0)
		{
			printf ("Synan Daemon was stopped\n");
			remove (LockFileName.c_str());
		}
		else
			printf ("Cannot	send a SIGTERM to the process\n");
		return 1;
	};



	synan_daemon_log	("Entering Synan Daemon");
	if (Action == "start_cli")
	{
		fprintf	(stderr, "removing %s\n",LockFileName.c_str());
		remove(LockFileName.c_str());
	};
	if (access(LockFileName.c_str(), 04) ==	0)
	{
		printf("Daemon already	started!\n");
		return 1;
	};

	if (Action == "start")
	{		
		//	working	as a daemon
		int	fd;	int	i;
		struct rlimit flim;
		if (getppid()!=1)
		{
			signal(SIGTTOU,SIG_IGN);
			signal(SIGTTIN,SIG_IGN);
			signal(SIGTSTP,SIG_IGN);
			if(fork()!=0)
			{
				exit(0);
			};
			setsid();
		}

		getrlimit(RLIMIT_NOFILE, &flim);

		for(fd=0;fd<flim.rlim_max;fd++)
			close(fd);
		chdir("/");

		openlog("Synan Server",	LOG_PID| LOG_CONS, LOG_DAEMON );

	}

	try{

		syslog(LOG_ALERT, "Open	a lock file	%s\n", LockFileName.c_str());
		FILE* fp = fopen (LockFileName.c_str(),	"w");
		if (!fp)
		{
			syslog(LOG_ALERT, "Cannot open file	%s!\n",	LockFileName.c_str());
			return 1;
		};
		fprintf(fp,	"%i", getpid());
		fclose (fp);

		synan_daemon_log ("SocketInitialize");
    	SocketInitialize(false);

		if (!LoadSynan ())
		{
			synan_daemon_log ("Cannot load Syntax");			
			SocketDeinitialize();
			return 1;

		};
	


		if (signal (SIGTERM, termination_handler) == SIG_IGN)
			signal (SIGTERM, SIG_IGN);

		if (Action == "start_cli")
			fprintf	(stderr, "\nWaiting	for	accept ... \n");

		while (1)
		{
			sleep(40);
		}
	}
	catch(...)
	{
		syslog(LOG_ALERT, "An exception	occured! ");
		UnloadData();
		SocketDeinitialize();
		return -1;
	};

	SocketDeinitialize();	

	return 1;
}




