#include "stdafx.h"

#define SOCKET int
#include "../common/string_socket.h"
#include "../common/BigramsReader.h"
#include "JVisualSynAnParamBuilder.h"

struct CSynHost : public CHost
{
	CSynHost(bool bDoubleMessage, ProtocolDensityEnum pdProtocolDensity) :
					CHost(bDoubleMessage, pdProtocolDensity) 
	{};
	string	ProcessSocketString(const string& S, SOCKET rConnectedSocket);
	string	ProcessBigramsSocketString(const string& S, SOCKET rConnectedSocket);
};




CSynHost* pGlobalSynHost = 0;
CSyntaxHolder RussianSyntaxHolder;
CSyntaxHolder GermanSyntaxHolder;



void synan_daemon_log(const string&  t)
{
	
	try {
		
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		struct tm today = RmlGetCurrentTime();
		char tmpbuf[255];
		string FileName = log_path + "synan_dmn.log";

		FILE * fp = fopen (FileName.c_str(), "a");
		strftime( tmpbuf, 255,"%d%B%Y %H:%M:%S", &today );
		fprintf (fp, "%s > %s\n", tmpbuf, t.c_str());
		fclose(fp);

	}
	catch (...) {
	};
}










// the input string should be of the following format:
// bigrams#Word
const string bigrams_command = "bigrams#";
string	CSynHost::ProcessBigramsSocketString(const string& S, SOCKET rConnectedSocket) 
{
	if (S.substr(0,bigrams_command.length()) != bigrams_command)
		return "failed";
	string Input = S;
	Input.erase(0, bigrams_command.length());
	if (Input.length() > 500) return "word is too long";
	char w[500]; 
	int MinBigramsFreq, iDirectBigrams;
	if (sscanf (Input.c_str(), "%[^#]#%i %i",  w, &MinBigramsFreq, &iDirectBigrams) != 3)
		return "Wrong format";
	string Word = w;
	return GetConnectedWords(Word, MinBigramsFreq, iDirectBigrams==1);
}


// the input string should be of the following format:
// _Langua#Text,
// Langua can be Russia, English or German
// Text is the iput text
string	CSynHost::ProcessSocketString(const string& S, SOCKET rConnectedSocket) 
{
	
	if (S.substr(0,bigrams_command.length()) == bigrams_command)
		return ProcessBigramsSocketString(S, rConnectedSocket);

	int i = S.find("#");
	if (i == string::npos) 
	{
		synan_daemon_log("Error of protocol\n");
		return "failed";
	};
	MorphLanguageEnum Langua;

	if (S[0] != '_')
	{
		synan_daemon_log("Error of protocol\n");
		return "failed";
	};

	if (		!GetLanguageByString(S.substr(1,i-1), Langua) 
			||	Langua == morphEnglish
		)
	{
		synan_daemon_log("Error of protocol\n");
		return "failed";
	};
	string Text = S.substr(i+1);
	Trim(Text);
	if (Text.empty())
	{
		synan_daemon_log("Empty string recieved!\n");
		return "failed";
	};
	CSyntaxHolder* P = (Langua == morphRussian) ? &RussianSyntaxHolder : &GermanSyntaxHolder;
	if (!P->GetSentencesFromSynAn(Text, false))
	{
		synan_daemon_log("Synan has crushed\n");
		return "faled";
	};

	if( P->m_Synan.m_vectorSents.empty() )
		return "";
	JVisualSynAnParamBuilder paramBuilder(P);	

	string Result;
	for (size_t SentNo=0; SentNo< P->m_Synan.m_vectorSents.size(); SentNo++)
	  Result  += paramBuilder.BuildStrParam(*P->m_Synan.m_vectorSents[SentNo]);

	return Result;
};


const char* RegHosts =  "Software\\Dialing\\Synan\\Daemon";
extern void(*GlobalErrorMessage)(const string&);
bool LoadSynan()
{
	if (!CanGetRegistryString(RegHosts)) 
	{
		synan_daemon_log ( Format("  Cannot find the registry key %s\n", RegHosts) );
		return false;
	};

	string Path =  GetRegistryString(RegHosts);
	vector<CHost> Hosts;
	LoadHosts(Path, Hosts);
	if (Hosts.size() != 1)
	{
		synan_daemon_log ( Format("  in %s should be only on host \n", Path.c_str() ) );
		return false;
	};
	assert (pGlobalSynHost == 0);
	pGlobalSynHost = new  CSynHost(false,pdMediumDensity);
	pGlobalSynHost->CopyAddressParametersFrom(Hosts[0]);
	pGlobalSynHost->m_LogFunction = synan_daemon_log;
	GlobalErrorMessage = synan_daemon_log;
	
	

	synan_daemon_log ( "Loading Russian Syntax\n");
	if (!RussianSyntaxHolder.LoadSyntax(morphRussian))
	{
		synan_daemon_log ( "  cannot load Russian Syntax\n");
		return false;
	};

	//sleep(30000);

	try {
		synan_daemon_log ( "Loading German Syntax\n");
		if (!GermanSyntaxHolder.LoadSyntax(morphGerman))
		{
			synan_daemon_log ( "  cannot load German Syntax\n");
			return false;
		};
	}
	catch (...)
	{
		synan_daemon_log ( "  an exception occurred!\n");
		return false;
	};

	try {
		string FileName = GetRmlVariable() + "/Dicts/Bigrams/bigrams.txt";
		if (access(FileName.c_str(), 04) == 0)
			if (!InitializeBigrams(FileName))
				return false;
	}
	catch (CExpc c)
	{
		synan_daemon_log ( c.m_strCause);
		return false;
	}
	catch (...)
	{
		synan_daemon_log ( "  an exception occurred!\n");
		return false;
	};


	pGlobalSynHost->CreateListener();

	return true;

};

bool UnloadSynan()
{
	synan_daemon_log ( "Unloading syntax...\n");
	delete pGlobalSynHost;
	return true;
};


