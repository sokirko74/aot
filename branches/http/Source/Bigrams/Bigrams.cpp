// GraphmatThick.cpp : Defines the entry point for the console application.
//

#include "../common/util_classes.h"
#include "../GraphanLib/GraphmatFile.h"
#include "../TrigramLib/TrigramModel.h"

#ifndef WIN32
	#include <syslog.h>
	#include <sys/wait.h>
	#include <sys/signal.h>
#endif


void PrintUsageAndExit()
{
	fprintf (stderr,"Bigrams Builder,  created by Alexey Sokirko 2003 (www.dwds.de)\n");
	fprintf (stderr,"Usage: Bigrams  <language> <file-list> <options>\n");
	fprintf (stderr,"\t <language> can be Russian, English, or German\n");
	fprintf (stderr,"\t <file-list> is a file with file names (one per line)\n");
	fprintf (stderr,"\t options can be:\n");
	fprintf (stderr,"\t   if [--window=N] is specified then the program collects all bigrams \n");
	fprintf (stderr,"\t      within window (=context) of a N by default N = 1\n");
	fprintf (stderr,"\t   if [--output=s] is specified then the program writes all bigrams \n");
	fprintf (stderr,"\t      to file s (by default it is \"bigrams.txt\")\n");
	fprintf (stderr,"\t   if [--maxmemory=N] is specified then the program stores only N bigrams in the memory \n");
	fprintf (stderr,"\t      (by default it is 1000000)\n");
	fprintf (stderr,"\t   if [--only-words] is specified then the program stores ignores all punctuation marks\n");
	fprintf (stderr,"\t   if [--hmm-config s] is specified then the program uses HMM specified in s as a preprocessor\n");
	
	

	exit(1);
	
};

map<string, size_t> WordFreqs;

typedef map<pair<string,string>, size_t> BigramsType;


/*
 this function writes bigrams to the main file, bigrams can be reoccur in this file.
 It is the task of the following scripts to factorize this file by bigrams.
 if the input file is more than 2 Gb, the program starts writing another file
*/

bool WriteBigramsAndClear(BigramsType& Bigrams, string FileName)
{
	FILE* big_fp = fopen (FileName.c_str(), "a");
	try {	
		
		if (!big_fp) 
		{
			fprintf (stderr, "cannot write to %s, may be the file is too large",  FileName.c_str());
			return false;
		};
		for(BigramsType::const_iterator it = Bigrams.begin();it != Bigrams.end(); it++)
		{
			fprintf(big_fp, "%s\t%s\t%i\n", it->first.first.c_str(), it->first.second.c_str(), it->second);
		};
		fclose (big_fp);
	}
	catch (...)
	{
		fprintf (stderr, "cannot write to %s, may be the file is too large",  FileName.c_str());
		return false;
	};
	Bigrams.clear();
	return true;
};

void RemoveTempFiles(string AllTempFilesStr)
{
	fprintf (stderr,"remove temporary files \n");
	StringTokenizer  tok (AllTempFilesStr.c_str()," ");
	while (tok())
		remove(tok.val());

};


typedef list<vector<string> > interp_t;
bool IsBigramToken(const CGraLine& L)
{
	return !L.IsNotPrint()  && !L.IsSoft();
};

interp_t GetTokensBySentences(CGraphmatFile& Graphan)
{
	interp_t Result;
	size_t TokensCount = Graphan.GetUnits().size();
	vector<string> tokens;
	for (size_t LineNo=1; LineNo < TokensCount; LineNo++)
	{
		tokens.clear();
		size_t LineNoEnd=LineNo;
		for (; LineNoEnd < TokensCount; LineNoEnd++)
		{
			if (IsBigramToken(Graphan.GetUnit(LineNoEnd)) )
			{
				string TokenStr = Graphan.GetToken(LineNoEnd);
				assert (!TokenStr.empty());
				tokens.push_back( TokenStr  );
			}

			if (Graphan.HasDescr(LineNoEnd, OSentEnd)) 
					break;
		}
		Result.push_back(tokens);
		LineNo = LineNoEnd;
	}

	return Result;
};

interp_t GetLemmasFromHMM(const interp_t& Tokens, const CTrigramModel& M)
{
	interp_t Result;
	vector<string> lemmas;
	for (interp_t::const_iterator it = Tokens.begin(); it != Tokens.end(); it++)
	{
		M.lemmatize_sentence(*it, lemmas);
		Result.push_back(lemmas);
	}

	return Result;
};

interp_t DeletePunctuationMarks(const interp_t& Tokens)
{
	interp_t Result;
	vector<string> lemmas;
	for (interp_t::const_iterator it = Tokens.begin(); it != Tokens.end(); it++)
	{
		lemmas.clear();
		int SentSize = (*it).size();
		for (size_t LineNo = 0; LineNo < SentSize; LineNo++)
		{
			string  Word = (*it)[LineNo];
			if (ispunct((BYTE)Word[0]))
				continue;
			else
				lemmas.push_back(Word);
		}
		Result.push_back(lemmas);
	}
	return Result;
}



string AllTempFilesStr;

void  termination_handler(int signum)
{
	RemoveTempFiles(AllTempFilesStr);
	exit(1);
};

//======================================================
int BuildBigrams(int argc, char* argv[])
{
	if (argc  < 3) 
			PrintUsageAndExit();


	MorphLanguageEnum Langua;
	bool bOnlyWords = false;
	if (!GetLanguageByString (argv[1], Langua))
	{
		fprintf (stderr,"False language - \"%s\"\n",argv[2]);
		return 1;
	};

	int MaxWindowSize=1;	
	string OutputFile = "bigrams.txt", HmmConfigFile;
	bool bUseHMM = false;
	size_t AllFileSize = 0;
	int MaxBigramsInMemory = 1000000;
	for (size_t i=3; i<argc; i++)
	{
		string s = argv[i];
		EngMakeLower(s);
		if (		(s == "-help") 
				||	(s == "--help") 
				||	(s == "/?") 
			)
			PrintUsageAndExit();
		else
		if (s == "--only-words")
			bOnlyWords = true;
		else
		if (s == "--window")
		{
			i++;
			if (i == argc) PrintUsageAndExit();
			MaxWindowSize = atoi(argv[i]);
			fprintf (stderr,"Window = %i\n",MaxWindowSize);
		}
		else
		if (s == "--hmm-config")
		{
			if (i+1 == argc)
				PrintUsageAndExit();
			HmmConfigFile = argv[i+1];
			i++;
			bUseHMM = true;
		}
		else
		if (s == "--maxmemory")
		{
			i++;
			if (i == argc) PrintUsageAndExit();
			MaxBigramsInMemory = atoi(argv[i]);
			fprintf (stderr,"MaxBigramsInMemory = %i\n",MaxBigramsInMemory);
		}
		else
		if (s == "--output")
		{
			i++;
			if (i == argc) PrintUsageAndExit();
			OutputFile = argv[i];
			fprintf (stderr,"OutputFile = %s\n",OutputFile.c_str());
		}
		else
		{
			fprintf (stderr, "Unknown parameter %s\n", s.c_str());
			PrintUsageAndExit();
		}

	};
try 
{
	CTrigramModel HMM;
	if (bUseHMM) {
		HMM.InitModelFromConfigAndBuildTagset(HmmConfigFile);
    }
	
	CGraphmatFile Graphan;
	Graphan.m_Language = Langua;
	

	if (!Graphan.LoadDicts())
	{
		fprintf (stderr,"Cannot load dictionaries \n");
		return 1;
	};

	Graphan.m_bUseParagraphTagToDivide = true;
	Graphan.m_bUseIndention = false;
	Graphan.m_MaxSentenceLength = 1000;

	const char* FileName = argv[2];

	StringVector Files;
	{
		FILE * fp =  fopen( FileName, "r");
		if (!fp)
		{
			fprintf (stderr,"Cannot open %s\n",FileName);
			return -1;
		};
		char buffer[1000];
		while (fgets(buffer, 1000, fp))
		{	
			rtrim(buffer);
			if (!strlen(buffer)) continue;
			if (!FileExists(buffer))
			{
				fprintf (stderr,"Cannot open %s\n",buffer);
				fclose (fp);
				return 1;
			};
				Files.push_back(buffer);
		};
		fclose(fp);
	}

		#ifndef  WIN32
			//  we should close the socket if user aborts the program (presses Ctrl-c)
			typedef void (*sighandler_t) (int);
			sighandler_t oldHandler;
			oldHandler = signal(SIGINT, termination_handler);
		#endif

		BigramsType Bigrams;
		for (int FileNo=0;FileNo < Files.size(); FileNo++)
		{
			string InputFileName = Files[FileNo];
			fprintf (stderr,"===== [%i/%i] %s ===== \n",FileNo+1, Files.size(), InputFileName.c_str());
			AllFileSize += FileSize(InputFileName.c_str());
			if (!Graphan.LoadFileToGraphan(InputFileName))
			{
				fprintf (stderr,"Cannot load file %s \n", InputFileName.c_str());
				continue;
			};
			size_t TokensCount = Graphan.GetUnits().size();
			interp_t Tokens =  GetTokensBySentences(Graphan);
			if  (bUseHMM)
				Tokens = GetLemmasFromHMM(Tokens, HMM);

			if (bOnlyWords)
				Tokens = DeletePunctuationMarks(Tokens);

			for (interp_t::const_iterator it = Tokens.begin(); it != Tokens.end(); it++)
			{
				int SentSize = (*it).size();
				for (size_t LineNo = 0; LineNo < SentSize; LineNo++)
				{
					string  s1 = (*it)[LineNo];
					map<string,size_t>::iterator freq_it  = WordFreqs.find(s1);
					if (freq_it == WordFreqs.end())
						WordFreqs[s1] = 1;
					else
						freq_it->second++;

					

					// add words from this window size 
					int WindowEnd = std::min(SentSize, (int)(MaxWindowSize + LineNo + 1));
					for (size_t k=LineNo+1; k < WindowEnd; k++)
					{
							string  s2 = (*it)[k];

							pair<string,string> p = make_pair(s1, s2);
							BigramsType::iterator it = Bigrams.find(p);
							if (it != Bigrams.end())
								it->second++;
							else
								Bigrams[p] = 1;
					};
				};
			}

			// spooling bigrams
			if (Bigrams.size() > MaxBigramsInMemory)
			{
				string TempFile = CreateTempFileName();
				fprintf (stderr,"write to temporary file %s\n", TempFile.c_str());
				if (!WriteBigramsAndClear(Bigrams, TempFile))
					return 1;
				AllTempFilesStr += " "+TempFile;
				
			};
		};
		string TempFile = CreateTempFileName();
		if (!WriteBigramsAndClear(Bigrams, TempFile))
			return 1;
		AllTempFilesStr += " "+TempFile;
		
		fprintf (stderr,"AllFilesSize = %i\n", AllFileSize);

		fprintf (stderr,"writing bigrams to %s\n", OutputFile.c_str());
		string  UniteCommand;
		#ifdef WIN32
			UniteCommand = Format ("gsort -m %s > %s", AllTempFilesStr.c_str(), OutputFile.c_str());
		#else
			UniteCommand = Format ("sort -m %s > %s", AllTempFilesStr.c_str(), OutputFile.c_str());
		#endif
		fprintf (stderr,"%s\n", UniteCommand.c_str());

		if (system (UniteCommand.c_str()) != 0)
		{
			fprintf (stderr,"!!! an exception occurred (cannot sort) !!!\n");
			RemoveTempFiles(AllTempFilesStr);
			return 0;
		};
		RemoveTempFiles(AllTempFilesStr);

		#ifndef  WIN32
			signal(SIGINT, oldHandler);
		#endif

		string WFFileName = MakeFName(OutputFile, "wrd_freq");
		fprintf (stderr,"Writing word frequences %s\n", WFFileName.c_str());
		FILE* wfp =  fopen (WFFileName.c_str(), "w");
		if (!wfp)
		{
			fprintf (stderr,"Error! Cannot write  word frequences\n");
		}
		else
		{
			for (map<string,size_t>::iterator freq_it = WordFreqs.begin(); freq_it != WordFreqs.end(); freq_it++)
			{
				fprintf (wfp,"%s %u\n", freq_it->first.c_str(), freq_it->second);
			}
			fclose(wfp);
		}

		fprintf (stderr,"exiting\n");

		return 0;
	}
	catch (...)
	{
		fprintf (stderr,"An exception occurred!\n");
		return 1;
		
	};
	 

};



int main(int argc, char* argv[])
{

	string Error;
	if (!IsRmlRegistered(Error)) 
	{
		fprintf (stderr,"%s", Error.c_str()  );
		return 1;
	};


	if (argc  < 2) 
		PrintUsageAndExit();

	return BuildBigrams(argc, argv);
}


