#include "../common/string_socket.h"
#include <assert.h>
#include "VisualGraph.h"
#include "translate.h"



RussianIntoEnglishTranslator *translater = 0;


void seman_daemon_log(const string&  t)
{
	string FileName = "tr_srv.log";
	try {
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		FileName = log_path + FileName;

		FILE * fp = fopen (FileName.c_str(), "a");
		struct tm today = RmlGetCurrentTime();
		char tmpbuf[255];
		strftime( tmpbuf, 255,"%H:%M, %d %B %Y > ", &today );
		fprintf (fp, "%s", tmpbuf); 
		fprintf (fp, "%s\n", t.c_str()); 
		fclose(fp);

	}
	catch (...) {
	};
}


RussianIntoEnglishTranslator::RussianIntoEnglishTranslator()
{
		
	try 
	{
		seman_daemon_log("Init Preseman\n");
		m_SemBuilder.m_RusStr.m_pData->Init();

		seman_daemon_log("Init Seman\n");
		m_SemBuilder.m_RusStr.m_pData->Initialize();

		seman_daemon_log("InitializeIndices\n");
		m_SemBuilder.m_RusStr.m_pData->InitializeIndices();

	} 
	catch (...)
	{
		seman_daemon_log(Format("Error in Constructor: %s\n",(const char*)m_SemBuilder.m_RusStr.m_pData->m_LastError.c_str()));
		throw;
	}

	
}


RussianIntoEnglishTranslator::~RussianIntoEnglishTranslator()
{
}




string& ConvertPath (string& s) {
	for (long  i=0;i < s.length(); i++)
		if (s[i] == '\\')
			s[i] = '/';
	return s;

};


int Count = 0;
string RussianIntoEnglishTranslator::translate(string russian, const string &po)
{
	string R = russian;
	Count++;
	if (Count  == 716)
	{
		int z = 0;
	}
	TrimLeft (R);
	russian = R.c_str();

	string Graph;

	try {
		seman_daemon_log(Format("  FindSituations %s\n", russian.c_str()));
		m_SemBuilder.FindSituations(russian.c_str(), 0, po.c_str(), 20000, -1, "", Graph);
		
	} 
	catch (...)
	{
		string s = m_SemBuilder.m_RusStr.m_pData->m_LastError;
		if (s.empty())
			s = "unknown error";

		seman_daemon_log(Format ("Error in FindSituations: %s\n", s.c_str()));
		return "Unexpected Error";
	}

	string res;


	try {
		for(;;)
		{
			
			seman_daemon_log("  TranslateToEnglish\n");
			if (!m_SemBuilder.TranslateToEnglish(Graph))
			{
				seman_daemon_log("Error in TranslateToEnglish\n");
				return "Unexpected Error";
			}



			seman_daemon_log("  BuildSentence\n");
			string TranslatedSent;
			if (!m_SemBuilder.BuildSentence(TranslatedSent))
			{
				seman_daemon_log("Error in Synthesis\n");
				return "Unexpected Error";
			}
			res += TranslatedSent;
			
			seman_daemon_log("  FindSituationsForNextSentence\n");
			if (!m_SemBuilder.FindSituationsForNextSentence()) break;
			
		}

		return res;

	} 
	catch (...)
	{
		seman_daemon_log("Error in TranslateToEnglish or Synthesis\n");
		return "Unexpected Error";
	}

}





string RussianIntoEnglishTranslator::GetJavaGraph( )
{
	//int retcode = Tcl_Eval(theInterp,"GetJavaGraph");
	//string s = theInterp->result;
	//return s;
	CVisualSemGraph Graph;
	Graph.InitFromSemantics(m_SemBuilder);
	Graph.SetGraphLayout();
	return Graph.GetResultStr();
	//InitFromSemantics(Graph, holder);

};


string RussianIntoEnglishTranslator::build_rus_graph(string russian, const string &po)
{
	try 
	{
		seman_daemon_log("Build Graph: " + russian);
		
		string res;
		//seman_daemon_log("FindSituations\n");
		string Graph;
		if (!m_SemBuilder.FindSituations(russian.c_str(), 0, po.c_str(), 20000, -1, "", Graph))
		{
			seman_daemon_log("Error in SemBuilder\n");
			return "Unexpected Error";
		}
		res = GetJavaGraph();
		return res;

	} catch (...)
	{
		seman_daemon_log("Error in Translator\n");
		return "Unexpected Error";
	}

};

//=================================================================
//==================== DAEOMON ===================================
//=================================================================


struct CTransHost : public CHost
{
	CTransHost(bool bDoubleMessage, ProtocolDensityEnum pdProtocolDensity) :
		CHost(bDoubleMessage, pdProtocolDensity)
	{
	};
	string	ProcessSocketString(const string& S, SOCKET rConnectedSocket);
	bool	InitHostData();
	bool	FreeHostData();
};

CTransHost* pGlobalSynHost = 0;


string	CTransHost::ProcessSocketString(const string& S, SOCKET rConnectedSocket)
{
	int i = S.find("#");
	if (i == string::npos) 
	{
		seman_daemon_log("Error of protocol\n");
		return "failed";
	};
	string Action  = S.substr(0, i);
	if (	Action != "graph" 
		&&	Action != "trans" 
		)
	{
		seman_daemon_log("bad action\n");
		return "failed";
	};
	string Text = S;
	Text.erase(0,i+1);
	i = Text.find("#");
	if (i == string::npos) 
	{
		seman_daemon_log("Error of protocol\n");
		return "failed";
	};
	string po = Text.substr(0, i);
	Text.erase(0,i+1);

	Trim(Text);
	if (Text.empty())
	{
		seman_daemon_log("Empty string recieved!\n");
		return "failed";
	};

	
	string Result;
	if (Action == "trans")
		Result = translater->translate(Text, po);
	else
		Result = translater->build_rus_graph(Text, po);

	return Result;
};

bool	CTransHost::InitHostData() 
{
	return true;
};


bool	CTransHost::FreeHostData() 
{
	return true;
};

extern void(*GlobalErrorMessage)(const string&);
bool LoadTranslator()
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
		seman_daemon_log ( Format("  in %s should be only one host \n", Path.c_str() ) );
		return false;
	};
	assert (pGlobalSynHost == 0);
	pGlobalSynHost = new  CTransHost(false, pdMediumDensity);
	

	pGlobalSynHost->CopyAddressParametersFrom(Hosts[0]);
	pGlobalSynHost->m_LogFunction = seman_daemon_log;
	GlobalErrorMessage = seman_daemon_log;
	//pGlobalSynHost->m_pdProtocolDensity = pdHeavyDensity;
	
	try {
		seman_daemon_log ( "Loading Translator\n");
		translater = new RussianIntoEnglishTranslator;
	}
	catch (...)
	{
		seman_daemon_log ( "  an exception occurred!\n");
		return false;
	};

	pGlobalSynHost->CreateListener();

	return true;

};

bool UnloadTranslator()
{
	seman_daemon_log ( "Unloading translator...\n");
	delete translater;
	delete pGlobalSynHost;
	return true;
};


