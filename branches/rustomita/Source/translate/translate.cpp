#include "../common/string_socket.h"
#include "../common/util_classes.h"
#include "../common/cgic.h"
#include <time.h>




void print_error(string err)
{
	if (err.empty()) return;
	cgiHeaderContentType("text/html");
	if (err[err.length()-1] != '\n')
		err+="\n";

	printf ( "%s", err.c_str() );
}



static void log(string t)
{
	string FileName = "translate_exe.log";
	try {
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		FileName = log_path + FileName;
		FILE * fp = fopen(FileName.c_str(), "a");
		tm today = RmlGetCurrentTime();
		char tmpbuf[128];
		strftime( tmpbuf, 128,"%H:%M, %d %B %Y > ", &today );
		fprintf (fp, "%s %s", tmpbuf, t.c_str() );
		fclose(fp);
	}
	catch (...) {
	};
	
}



void print_result_translation(const string &rus, const string &eng, const string &po, const char* TemplateFile)
{
	string path = string(cgiPathTranslated) + TemplateFile;
	FILE* fp = fopen (path.c_str(), "r");
	if(!fp)
	{
		print_error(string("Cannot open template file: "+path).c_str() );
		return ;
	}


	cgiHeaderContentType("text/html");
	string po_string = "VALUE=\""+po+'"';
	
	char buffer[3000];
	while( fgets(buffer, 3000, fp) )
	{
		StringTokenizer tok(buffer, "\t\r\n ");
		while (tok())
		{
			string s = tok.val();
			if(s == "rus_text>") 
			{
					printf (">%s", rus.c_str());
			}
			else 
			if(s == "eng_text>") 
				{
					printf (">%s",  eng.c_str());;
				}
			else 
				{	
					 printf (" %s",s.c_str());
				}

			if(s == po_string) 
				printf (" selected ");
		};
		printf ("\n");;
	}
}




void print_result_graph(const string &rus, string Graph, const string &po, const char* TemplateFile)
{
	log("print_result_graph");
	string path = string(cgiPathTranslated) + TemplateFile;
	
	FILE* fp = fopen (path.c_str(), "r");
	if(!fp)
	{
		print_error(string("Error in template file: "+path).c_str());
		return ;
	}

	log("used template file: "+path);

	long i =0;
	for (; i < Graph.length(); i++)
		if (Graph[i] == '\n')
			Graph.replace(i,1, "$$$");

   

	cgiHeaderContentType("text/html");
	string po_string = "value=\""+po+'"';
	string s;
	
	string Result;
	char buffer[3000];
	while( fgets(buffer, 3000, fp) )
	{
		Result += buffer;

	};
	i = Result.find ("rus_text>");
	if (i != -1)
	{
	  Result.replace(i, 9, ">"+rus);
	}
	i = Result.find ("$rus_graph");
	if (i != -1)
	{
	  Result.replace(i, 10, Graph);
	}

	printf ("%s",Result.c_str());
}

enum ActionEnum {
	TranslationAction,
	GraphAction,
};



SOCKET TransDaemonSocket;




bool GetDaemon (SOCKET& T) 
{

	string Path;
	try
	{
		 Path = GetIniFilePath() + "/translate.cfg";
	}
	catch (...)
	{
		return false;
	};

	vector<CHost> Hosts;
	LoadHosts(Path, Hosts);
	if (Hosts.size() != 1)
	{
		log ( Format("  in %s should be only on host \n", Path.c_str() ) );
		return false;
	};

	string strError;
	T = Hosts[0].CreateAndConnectSocket(strError);
	if (T == -1)
	{
		log ( Format("%s\n", strError.c_str() ) );	
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
	//print_error("good");
	//exit(1);

	//_sleep(20000);
	char russian[255], po[20], TemplateFile[256], action_str[250];
	bool bTest;
	ActionEnum Action;

	cgiFormStringNoNewlines("action", action_str, 256);
	if (strlen(action_str) == 0)
	{
		print_error(" no action specified");
		return -1;
	}
	
	if ( !strcmp(action_str, "test") )
	{
		bTest = true;
		strcpy(russian, "мама");
		strcpy(po,"общ");
		Action = TranslationAction;
	}
	else
	{
		bTest = false;
		cgiFormStringNoNewlines("russian", russian, 150);
		cgiFormStringNoNewlines("PO", po, 20);
		cgiFormStringNoNewlines("TemplateFile", TemplateFile, 256);

		if (strlen (TemplateFile) == 0)
		{
			print_error("Cannot read template file from html");
			return 1;

		};

		if (!strcmp(action_str,"transl"))
			Action = TranslationAction;
		else if (!strcmp(action_str,"graph"))
			Action = GraphAction;
		else
		{
			print_error("unknown action");
			return 1;
		};
	};
	

	if (! SocketInitialize(false) )
	{
		log("Service could invoke AfxSocketInit()");
		return -1;
	};
	
	if (!GetDaemon(TransDaemonSocket))
	{
		SocketDeinitialize();	
		log("Cannot connect to translate daemon");
		print_error("Cannot connect to translate daemon");
		return -1;
	};

	//========= Send ================
	try 
	{
		string S = Format ("%s#%s#%s", (Action == GraphAction)?"graph":"trans", po, russian);

		log (Format ("Send to daemon %s\n",S.c_str()));
		string ErrorStr;
		if (!SendString(TransDaemonSocket, S.c_str(), S.length(), ErrorStr))
		{
			log ("Cannot send string\n");
			SocketDeinitialize();
			return false;
		};
		
	}
	catch (...)
	{
		log ("cannot send string to daemon\n");
		print_error("cannot send string to daemon! ");
		SocketDeinitialize();
		return -1;
	}


	//========= Recieve ================
	string Result;
	try
	{
		
		NetworkErrorsEnum Res = RecieveString(TransDaemonSocket, Result, 25);
		SocketDeinitialize();
		if (Res != neSuccess)
		{
			char error_str[255];
			sprintf(error_str, "Error: %s\n", GetNetworkErrorString((NetworkErrorsEnum)Res).c_str());
			print_error(error_str);
			log (error_str);
			return -1;
		};
	}
	catch (...)
	{
		log ("cannot recieve string from daemon\n");
		print_error("cannot recieve string from daemon! ");
		SocketDeinitialize();
		return -1;
	}

	//========= Print results ================

	try 
	{
		log (Format ("Recieve from daemon %s\n",Result.c_str()));
		if (Result == "failed")
		{
			string err = Format("Daemon returned \"%s\"", Result.c_str()); 
			print_error( err );
			log (err);
			return -1;
		};

		if (bTest)
			print_error( Result );
		else
		if (Action == GraphAction)
			print_result_graph(russian, Result, po, TemplateFile);
		else
			print_result_translation(russian, Result, po, TemplateFile);

	}
	catch (...)
	{
		log ("Cannot print the result to the output file !\n");
		print_error("Cannot print result to the output file ! ");
		return -1;
	}
	

	return 0;
}


