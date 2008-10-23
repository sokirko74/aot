// GraphmatThick.cpp : Defines the entry point for the console application.
//

#include "../common/util_classes.h"
#include "../GraphanLib/GraphmatFile.h"
#include "math.h"



void PrintUsageAndExit()
{
	printf ("GraphmatThick, Test for Dialing Graphematical Analysis(www.aot.ru)\n");
	printf ("Usage: GraphmatThick <language> <file> <opt>\n\n");
	printf ("where language can be Russian, English, or German\n");
	printf (" file is an text or html file  in  CP-1251 coding\n");
	printf ("    1. Russian -  CP-1251 coding\n");
	printf ("    2. German  -  ISO-8859-1 coding\n");
	printf (" Here are possible options:\n");
	printf (" if \"-gra file\" specify file for g-table \n");
	printf (" if \"-xml file\" specify file for macrosyntax structure \n");
    printf (" if \"-sents file\" specify file for sentence division\n");
	printf (" if \"-abbrs file\" specified file the found abbreviation\n");
	printf (" if \"-disable-sent-breaker\" is specified then the program disabled sentence breaker\n");
	printf (" if \"-use-paragraph-tag\" is specified then the program considers \"</p>\" as  a paragraph delimiter\n");
	printf (" if \"-write-non-printable\" is specified then the program considers \"</p>\" as  a paragraph delimiter\n");
	
	
	exit(1);
	
};

int main(int argc, char* argv[])
{
	double a = log(8.0);
	try 
	{
		if (argc  < 3) 
			PrintUsageAndExit();

		string strErr;
		if (!IsRmlRegistered(strErr))
		{
			printf ("Error: %s!", strErr.c_str());
		};
		MorphLanguageEnum Langua;
		if (!GetLanguageByString (argv[1], Langua))
		{
			printf ("False language - \"%s\"\n",argv[1]);
			return 1;
		};
		string InputFile = argv[2];

		string XmlFile,GraFile,SentsFile,AbbrsFile;
		bool bEnableSentenceBreaker = true;
		bool bWriteNonPrintable = false;
		bool bUseParagraphTagToDivide = false;
		string FileName;
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
			if (s == "-xml")
			{
				if (i+1 == argc)
				{
					fprintf (stderr, "no xml file is specified\n");
					PrintUsageAndExit();
				}
				i++;
				XmlFile = argv[i+1];
			}
			else
			if (s == "-gra")
			{
				if (i+1 == argc)
				{
					fprintf (stderr, "no gra file is specified\n");
					PrintUsageAndExit();
				}
				i++;
				GraFile = argv[i];
			}
			else
			if (s == "-sents")
			{
				if (i+1 == argc)
				{
					fprintf (stderr, "no sents file is specified\n");
					PrintUsageAndExit();
				}
				i++;
				SentsFile = argv[i];

			}
			else
			if (s == "-abbrs")
			{
				if (i+1 == argc)
				{
					fprintf (stderr, "no abbrs file is specified\n");
					PrintUsageAndExit();
				}
				i++;
				AbbrsFile = argv[i];

			}
			else
			if (s == "-write-non-printable")
				bWriteNonPrintable = true;
			else
			if (s == "-disable-sent-breaker")
				bEnableSentenceBreaker = false;
			else
			if (s == "-use-paragraph-tag")
				bUseParagraphTagToDivide = true;
			else
			{
				fprintf (stderr, "Unknown parameter %s\n", s.c_str());
				PrintUsageAndExit();
			}

		};

		
		CGraphmatFile Graphan;
		Graphan.m_Language = Langua;
		if (!Graphan.LoadDicts())
		{
			printf ("Cannot load dictionaries \n");
			printf ("Error: %s\n", Graphan.GetLastError().c_str());
			return 1;
		};
	
		Graphan.m_GraOutputFile = GraFile;
		Graphan.m_XmlMacSynOutputFile = XmlFile;
		Graphan.m_bSentBreaker = bEnableSentenceBreaker;
		Graphan.m_bUseParagraphTagToDivide = bUseParagraphTagToDivide;
		Graphan.m_bFilterUnprintableSymbols = bWriteNonPrintable;
		//Graphan.m_bRecognizeShortFIOs = true;
		
		
	
		Graphan.m_bUseIndention = true;
		fprintf (stderr,"Loading file %s\n", InputFile.c_str());
		CMyTimeSpanHolder  Profiler;
		Profiler.StartTimer("LoadFileToGraphan",1);
		if (!Graphan.LoadFileToGraphan(InputFile))
		{
			fprintf (stderr,"Cannot load file %s \n", InputFile.c_str());
			fprintf (stderr,"Error: %s\n", Graphan.GetLastError().c_str());
			return 1;
		};
		Profiler.EndTimer("LoadFileToGraphan");
		printf ("%s ", Profiler.GetStrRepresentation().c_str());
		int Count = Graphan.GetUnits().size();
		if (!SentsFile.empty())
		{
			printf ("Creating %s\n", SentsFile.c_str());
			FILE * sents = fopen (SentsFile.c_str(), "w");
			if (!sents)
			{
				printf ("cannot open sents file %s\n",SentsFile.c_str()),
				exit(1);
			};
			for (size_t i=1; i < Count; i++)
			{
				string s = Graphan.GetToken(i);
				if (!Graphan.GetUnit(i).IsSoft())
					fprintf (sents,"%s ",s.c_str());
				if (Graphan.HasDescr(i,OSentEnd))
					fprintf (sents, "\n",s.c_str());

			};
			fclose (sents);
		};
		if (!AbbrsFile.empty())
		{
			printf ("Creating %s\n", AbbrsFile.c_str());
			FILE * abbrs = fopen (AbbrsFile.c_str(), "w");
			if (!abbrs)
			{
				printf ("cannot open sents file %s\n",AbbrsFile.c_str()),
				exit(1);
			};
			bool bAbbrStart	 = false;
			for (size_t i=1; i < Count; i++)
			{
				if (Graphan.HasDescr(i,OAbbr1))
					bAbbrStart = true;

				if (bAbbrStart)
				{
					string s = Graphan.GetToken(i);
					if (!Graphan.GetUnit(i).IsSoft())
						fprintf (abbrs,"%s ",s.c_str());
					
					if (Graphan.HasDescr(i,OAbbr2))
					{
						bAbbrStart = false;
						fprintf (abbrs,"\n");
					};
				};

			};
			fclose (abbrs);
		};
				
		for (size_t i=1; i < Count; i++)
		{
			string s1 = Graphan.GetUppercaseToken(i);
			string s2 = Graphan.GetToken(i);
			RmlMakeUpper(s2, Graphan.m_Language);
			assert (s1 == s2);
		};

		printf ("Found Units %u\n", Count);

		/*for (size_t i=0; i < 256; i++)
			if (isdelim(i))
				if (i > 32)
					printf ("1, // %c\n",(BYTE)i);
				else
					printf ("1, // non-printable\n",(BYTE)i);
			else
					printf ("0, \n");*/

		return 0;
	}
	catch (CExpc C)
	{
		printf ("An exception occurred:%s!\n", C.m_strCause.c_str());
	}
	catch (...)
	{
		printf ("An exception occurred!\n");
		
	};
	
	return 1;
}

