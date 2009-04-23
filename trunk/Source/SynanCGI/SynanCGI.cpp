#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif 

#include "../common/cgic.h"

#include "../common/string_socket.h"
#include "../common/utilit.h"
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <fstream>



void print_error(const string &err)
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "%s\n", err.c_str());

}


static void log(string t)
{
	
	try
	{
		string FileName = "synan_cgi.log";
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
	}
	catch(...)
	{
		print_error("Cannot get log file name from Registry");
		return;
	};

	
	try
	{
		string FileName = "synan_cgi.log";
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		FileName = log_path + FileName;
		FILE* fp = fopen(FileName.c_str(), "a");
		fprintf (fp, "%s", t.c_str());
		fclose(fp);
	}
	catch (...)
	{
		string s = Format("Cannot write to log-file , error message = %s", t.c_str() );
		print_error(s);
	};
}



void print_synan_result(const string &InputText, string& syn_str, string LanguaStr, const char* TemplateFile)
{
	string path = string(cgiPathTranslated) +TemplateFile;

	ifstream file(path.c_str());
	if(!file.is_open())
	{
		print_error(string("Error in template file: "+path).c_str());
		return ;
	}
	for (long i =0; i < syn_str.length(); i++)
		if (syn_str[i] == '\n')
			syn_str.replace(i,1, "$$$");

   

	cgiHeaderContentType("text/html");	
	string s;
	
	string Result;
	while(true)
	{
		file >> s;
		if(!file) break;

		// processing language combo box
		if (s == "selected")
			continue;
		if ((s == "_Russian") && (LanguaStr == "_Russian"))
			s += " selected";

		if ((s == "_German") && (LanguaStr == "_German"))
			s += " selected";

		Result += " " + s;
		
		
	}
	int ind = Result.find ("InputText>");
	if (ind != -1)
	{
	  Result.replace(ind, 10, ">"+InputText);
	}
	ind = Result.find ("$syn_struct");
	if (ind != -1)
	{
	  Result.replace(ind, 11, syn_str);
	}
	

	cout << Result;
}



SOCKET SynanDaemonSocket;



const char* RegHosts =  "Software\\Dialing\\Synan\\Daemon";
bool GetDaemon (SOCKET& T) 
{
	string Path =  GetRegistryString(RegHosts);
	vector<CHost> Hosts;
	LoadHosts(Path, Hosts);
	if (Hosts.size() != 1)
	{
		log ( Format("  in %s should be one host (there are %i host(s)) \n", Path.c_str(), Hosts.size() ) );
		return false;
	};
	string strError;
	T = Hosts[0].CreateAndConnectSocket(strError);
	if (T == -1)
	{
		log ( Format("cgi-script failed to open socket %s: %s", Hosts[0].GetAddressStr().c_str(), strError.c_str()) );
		return false;

	};

	return true;
};


int cgiMain() 
{	
	string sError;
	if (!IsRmlRegistered(sError))
	{
		print_error(sError);
		return -1;
	}
	log ("enter cgiMain\n");
	//cgiReadEnvironment("capcgi.dat");

	//_sleep(20000);
	            	
	

	if (! SocketInitialize(false) )
	{
		log("Service could invoke AfxSocketInit()");
		return -1;
	};
	
	if (!GetDaemon(SynanDaemonSocket))
	{
		SocketDeinitialize();	
		log("Cannot connect to synan daemon");
		print_error("Cannot connect to synan daemon");
		return -1;
	};




	cout.sync_with_stdio();

	char TextStr[1024*10], Langua[40], TemplateFile[256];
	cgiFormStringNoNewlines("InputText", TextStr, 10240);
	cgiFormStringNoNewlines("Language", Langua, 40);
	cgiFormStringNoNewlines("TemplateFile", TemplateFile, 256);
	if (strlen (TemplateFile) == 0)
	{
		print_error("Cannot read template file from html");
		return 1;

	};



	try {
		log ("SynAnAction\n");
		string S = Format ("%s#%s", Langua, TextStr);
		string ErrorStr;
		if (!SendString(SynanDaemonSocket, S.c_str(), S.length(), ErrorStr))
		{
			SocketDeinitialize();
			return false;
		};
		
		NetworkErrorsEnum Res = RecieveString(SynanDaemonSocket, S, 60);
		CloseSocket(SynanDaemonSocket);
		if (Res != neSuccess)
		{
			char error_str[255];
			sprintf(error_str, "Error: %s", GetNetworkErrorString((NetworkErrorsEnum)Res).c_str());
			print_error(error_str);
			SocketDeinitialize();
			return -1;
		};

		
		if (S == "failed")
		{
			print_error(Format("Daemon returned \"%s\"", S.c_str()) );
			SocketDeinitialize();
			return -1;
		};

		print_synan_result(TextStr, S, Langua, TemplateFile);
		log ("SynAnAction was ok\n");
	}
	catch (...)
	{
		log ("SynAnAction failed\n");
		print_error("Cannot run Synan! ");
		return -1;
	}
	SocketDeinitialize();

	return 0;
}


