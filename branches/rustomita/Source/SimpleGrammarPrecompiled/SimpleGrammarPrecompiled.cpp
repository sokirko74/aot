
#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include "../SimpleGrammarLib/SimpleGrammar.h"
#include "../common/MorphologyHolder.h"
#ifdef WIN32
	#include "direct.h"
#else
	
#endif



void PrintUsage()
{
		printf ("Simple Grammar Precompiled Maker (www.dwds.de)\n");
		printf ("Usage: SimpleGrammarPrecompiled (RUSSIAN|GERMAN|ENGLISH) <file.grm> [--print-rules]\n");
		printf ("Example: SimpleGrammarPrecompiled German test.grm\n");
		exit(-1);	
};

int main(int argc, char **argv)
{
	bool bPrintRules = false;;
	if (argc == 4)
	{
		if (string(argv[3]) == "--print-rules")
			bPrintRules = true;
		else
			PrintUsage();

		argc = 3;
	};

	if (argc != 3)
		PrintUsage();

	if (		!strcmp (argv[1],  "-h")
			||  !strcmp (argv[1],  "-help")
			||  !strcmp (argv[1],  "/h")
			||  !strcmp (argv[1],  "/help")
	)
		PrintUsage();



	MorphLanguageEnum langua;
	if (!GetLanguageByString(argv[1], langua)) PrintUsage();

	CWorkGrammar WorkGrammar;
	WorkGrammar.m_Language = langua;
	
	fprintf (stderr, "Loading morphology ...\n");
	CMorphologyHolder Holder;
	if (!Holder.LoadGraphanAndLemmatizer(WorkGrammar.m_Language))
		return 1;

	string GrammarFileName = argv[2];

	char currdir[256];
	#ifdef WIN32
		_getcwd(currdir, 256);
		_chdir(GetPathByFile(GrammarFileName).c_str());
	#else
		getcwd(currdir, 256);
		chdir(GetPathByFile(GrammarFileName).c_str());
	#endif

	WorkGrammar.m_pGramTab = Holder.m_pGramTab;
	WorkGrammar.m_SourceGrammarFile = GrammarFileName.substr(GetPathByFile(GrammarFileName).length());
	
	bool bResult = LoadGrammarForGLR(WorkGrammar, false, bPrintRules);

	if (!bResult)
	{
		fprintf (stderr, "Cannot load %s\n", GrammarFileName.c_str());
		return 1;
	};

	#ifdef WIN32
		_chdir(currdir);
	#else
		chdir(currdir);
	#endif

	if (!WorkGrammar.SavePrecompiled(GrammarFileName)) 
	{
		fprintf (stderr, "Cannot save precompiled version of %s\n", GrammarFileName.c_str());
		return 1;
	};

	if (!WorkGrammar.m_GLRTable.ConvertAndSaveGLRTable(MakeFName(GrammarFileName,"table")))
	{
		
		fprintf (stderr, "Cannot save %s\n", GrammarFileName.c_str());
		return 1;
	};
	
	fprintf (stderr, "exit\n");

	return 0;
	

};



