#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif 

#include "../common/cgic.h"

#include "../common/string_socket.h"
#include "../common/utilit.h"
#include "../common/util_classes.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <fstream>



void print_error(const string &err)
{
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "%s\n", err.c_str());

}


static void print_log(string t)
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

struct CBigramLine 
{
	string m_Word1;
	string m_Word2;
	size_t m_WordFreq1;
	size_t m_WordFreq2;
	size_t m_BigramsFreq;
	double m_MutualInformation;
	size_t m_CorpusSize;

	bool ReadFromStr(const string& S);
	string GetTableLine() const;
	string GetFirstLine() const;
	void	CopyCommonParameters(const CBigramLine& L);
	void	ReadCommonParameters(const string& S);
	string	BuildDDCQuery() const;

};

void CBigramLine::ReadCommonParameters(const string& S)
{
	char w1[500];
	sscanf(S.c_str(), "%i %i %[^ ]", &m_CorpusSize, &m_WordFreq1, &w1);
	m_Word1 = w1;
}
void CBigramLine::CopyCommonParameters(const CBigramLine& L)
{
	m_CorpusSize = L.m_CorpusSize;
	m_Word1 = L.m_Word1;
	m_WordFreq1 = L.m_WordFreq1;
};

bool CBigramLine::ReadFromStr(const string& S)
{
	
	char w2[500];
	if (sscanf(S.c_str(), "%[^ ] %i %i", w2, &m_WordFreq2, &m_BigramsFreq) != 3)
		return false;
	m_Word2 = w2;
	double p_x_y = ((double)m_BigramsFreq/(double)m_CorpusSize);
	double p_x_p_y = ((double)m_WordFreq1/(double)m_CorpusSize)*((double)m_WordFreq2/(double)m_CorpusSize);
	m_MutualInformation =  log (p_x_y/p_x_p_y)/log((double)2);
	return true;
}



string CBigramLine::BuildDDCQuery() const
{
	string lower1 = m_Word1;
	RmlMakeLower(lower1, morphRussian);
	string lower2 = m_Word2;
	RmlMakeLower(lower2, morphRussian);
	return Format("&quot;%s %s&quot; || &quot;%s %s&quot; || &quot;%s %s&quot; || &quot;%s %s&quot;",
		m_Word1.c_str(), m_Word2.c_str(),
		lower1.c_str(), lower2.c_str(),
		lower1.c_str(), m_Word2.c_str(),
		m_Word1.c_str(), lower2.c_str()
		);

}
string CBigramLine::GetTableLine() const
{

	string query = BuildDDCQuery();
	//string query = "&quot;мама папа&quot;";
	return Format("<tr align=\"center\"><td>%s</td> <td>%s</td> <td>%u</td> <td>%u</td> <td>%u</td> <td>%f</td>  <td> <input name=CS1 type=\"submit\" onclick=\"this.form.SearchText.value='%s';this.form.action='../cgi-bin/ConcordHttp.exe'\" value=\"контекст\" > </td></tr>\n", 
		m_Word1.c_str(),
		m_Word2.c_str(), 
		m_WordFreq1, 
		m_WordFreq2, 
		m_BigramsFreq, 
		m_MutualInformation,
		query.c_str()
		);
}
string CBigramLine::GetFirstLine() const
{
	return "<tr align=\"center\" bgcolor=\"#00FFFF\"><td >Word1</td> <td>Word2</td> <td>WordFreq1</td> <td>WordFreq2</td> <td>BigramsFreq</td> <td>MI</td><td>Контекст</td></tr>\n";
}


bool GreaterByMutualInformaton(const CBigramLine& _X1, const CBigramLine& _X2)
{
	return _X1.m_MutualInformation >  _X2.m_MutualInformation;
}

bool GreaterByWordFreq2(const CBigramLine& _X1, const CBigramLine& _X2)
{
	return _X1.m_WordFreq2 >  _X2.m_WordFreq2;
}

bool GreaterByBigramsFreq(const CBigramLine& _X1, const CBigramLine& _X2)
{
	return _X1.m_BigramsFreq >  _X2.m_BigramsFreq;
}


string BuildTable(string& BigramsStr, string SortOperator, bool bUseFirstWord)
{
	StringTokenizer lines (BigramsStr.c_str(), "\n");
	vector<CBigramLine> Table;
	if (!lines()) return "";
	CBigramLine CommonParms;
	CommonParms.ReadCommonParameters(lines.val());
	while (lines())
	{
		CBigramLine L;
		L.CopyCommonParameters(CommonParms);
		if (L.ReadFromStr(lines.val()))
			Table.push_back(L);
	}
	if (SortOperator == "SortByMutualInformation")
		sort (Table.begin(), Table.end(),GreaterByMutualInformaton);
	else
	if (SortOperator == "SortByWordFreq2")
		sort (Table.begin(), Table.end(),GreaterByWordFreq2);
	else
	if (SortOperator == "SortByBigramsFreq")
		sort (Table.begin(), Table.end(),GreaterByBigramsFreq);

	string Result;
	if (!Table.empty())
	{
		Result = "<br><table border=1 cellpadding=\"5\" >";
		Result += Table[0].GetFirstLine();
		for (size_t i=0; i < Table.size(); i++)
		{
			if (!bUseFirstWord)
				swap( Table[i].m_Word1, Table[i].m_Word2);
			Result += Table[i].GetTableLine();
		}
		Result += "</table>";
	}
	return Result;
}


void print_bigrams_result(const string &InputText, string& BigramsStr, string SortOperator, const char* TemplateFile, int MinBigramFreq, bool bUseFirstWord)
{
	string path = string(cgiPathTranslated) +TemplateFile;

	ifstream file(path.c_str());
	if(!file.is_open())
	{
		print_error(string("Error in template file: "+path).c_str());
		return ;
	}

	cgiHeaderContentType("text/html");	
	string s;
	
	string table = BuildTable(BigramsStr, SortOperator, bUseFirstWord);
	string Result;
	while(true)
	{
		file >> s;
		if(!file) break;

		// processing language combo box
		if (s == "selected")
			continue;
		if (s == "checked")
			continue;
		
		if (s == SortOperator)
			s += " selected";

		if (s == "value=\"\"")
		{
			Result += Format(" value=\"%s\"", InputText.c_str());
			continue;
		};
		if (s == "value=\"2\"")
		{
			Result += Format(" value=\"%i\"", MinBigramFreq);
			continue;
		};
		if (s == "<!--ResultText-->")
		{
			Result += table;
			continue;
		}
		if (bUseFirstWord && (s == "FirstWord") )
		{
			Result += " " + s;
			Result += " checked";
			continue;
		};
		if (!bUseFirstWord && (s == "SecondWord") )
		{
			Result += " " + s;
			Result += " checked";
			continue;
		};

		Result += " " + s;
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
		print_log ( Format("  in %s should be one host (there are %i host(s)) \n", Path.c_str(), Hosts.size() ) );
		return false;
	};
	string strError;
	T = Hosts[0].CreateAndConnectSocket(strError);
	if (T == -1)
	{
		print_log ( Format("cgi-script failed to open socket %s: %s", Hosts[0].GetAddressStr().c_str(), strError.c_str()) );
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
	print_log ("enter cgiMain\n");
	//cgiReadEnvironment("capcgi.dat");

	//_sleep(20000);
	            	
	

	if (! SocketInitialize(false) )
	{
		print_log("Service could invoke AfxSocketInit()");
		return -1;
	};
	
	if (!GetDaemon(SynanDaemonSocket))
	{
		SocketDeinitialize();	
		print_log("Cannot connect to synan daemon");
		print_error("Cannot connect to synan daemon");
		return -1;
	};




	cout.sync_with_stdio();

	char TextStr[1024*10],  TemplateFile[256], SortOperator[256];
	int MinBigramFreq;
	cgiFormStringNoNewlines("InputText", TextStr, 10240);
	cgiFormStringNoNewlines("BigramsTemplateFile", TemplateFile, 256);
	cgiFormStringNoNewlines("SortOperator", SortOperator, 256);
	char *Switch[2]={"FirstWord", "SecondWord"};
	int BigramElementNo;
	cgiFormRadio("BigramElement", Switch, 2, &BigramElementNo, 0);
	
	cgiFormInteger("MinBigramFreq", &MinBigramFreq, 2);
	if (strlen (TemplateFile) == 0)
	{
		print_error("Cannot read template file from html");
		return 1;

	};



	try {
		print_log ("BigramsAction\n");
		string S = Format ("bigrams#%s#%i %i", TextStr, MinBigramFreq,BigramElementNo+1);
		string ErrorStr;
		if (!SendString(SynanDaemonSocket, S.c_str(), S.length(), ErrorStr))
		{
			print_log ("Cannot send the query to daemon\n");
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

		print_bigrams_result(TextStr, S, SortOperator, TemplateFile, MinBigramFreq, BigramElementNo == 0);
		print_log ("SynAnAction was ok\n");
	}
	catch (...)
	{
		print_log ("Bigrams failed\n");
		print_error("Cannot find  bigrams! ");
		return -1;
	}
	SocketDeinitialize();

	return 0;
}


