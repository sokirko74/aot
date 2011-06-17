#include "../common/utilit.h"
#include "../TrigramLib/TrigramModel.h"



void PrintUsageAndExit()
{
	fprintf (stderr,"Trigram Tagger (based on Acopost project) \n");
	fprintf (stderr,"Usage(1): Trigram <action1> <config-file> \n");
	fprintf (stderr,"where <action1> can be print-tagset, dump_transition_probs\n");
	fprintf (stderr,"Example: Trigram print-tagset default.config\n");
	fprintf (stderr,"\n");
	fprintf (stderr,"Usage(2): Trigram <action2> <config-file> <input-file> [options]\n");
	fprintf (stderr,"where <action2> can be tagging, testing,  adjusting\n");
	fprintf (stderr,"where [options] can be:\n");
    fprintf (stderr,"  -b  read binary model \n");
	fprintf (stderr,"  --raw-texts - use graphan to find sentences in the input  text \n");
	fprintf (stderr,"  --supress-morph-errors - do not print morph errors to stderr \n");
    fprintf (stderr,"  --second-local-coef <a> - set second local coefe\n");
    

    
	
	fprintf (stderr,"Example: Trigram testing   default.config test.cooked\n");
	exit(1);
};


/* ------------------------------------------------------------ */
int mymain(int argc, char **argv)
{
    if (argc < 3)	
        PrintUsageAndExit();

	CTrigramModel M;
    string ConfigFile = argv[2];
    M.m_Language = morphRussian;
    bool bReadBinary = false;
    fprintf (stderr, "initialize Russian morphology\n");
	if (!M.InitDicts())
		return 1;
    string Action = argv[1];
    string InputFile;
   	for (size_t i=3; i <  argc; i++)
	{
		string s = argv[i];
		if (s == "--raw-texts")
		{
			M.m_bRawTexts = true;
		}
        else
		if (s == "--check-only-amb-words")
		{
            M.m_bCheckOnlyAmbiguosWords = true;
		}
        else
		if (s == "--second-local-coef")
		{
			M.m_bUseSecondLocalMax = true;
            if (i+1 >= argc)
                PrintUsageAndExit();
            i++;
            M.m_SecondLocalCoef = atoi(argv[i]);
  			if (!M.m_SecondLocalCoef)
			{
				fprintf(stderr, "wrong second local coef: %s\n",  argv[i]);
				return 1;
			};
            fprintf(stderr, "second local coef: %i\n",  M.m_SecondLocalCoef);
		}
        else
        if (s == "--supress-morph-errors")
		{
            M.m_bQuiet = true;
		}
        else
            
            if (InputFile.empty())
                InputFile = s;
            else
                PrintUsageAndExit();
    }

    M.InitModelFromConfigAndBuildTagset(ConfigFile, 0, 0, true);
    M.ReadBinary();

	
	if (Action == "tagging")
	{
		if (InputFile.empty())
			fprintf (stderr, "no input file!\n");
		else
		{
				M.tagging(InputFile);
		}
	}
	else
	if (Action == "print-tagset")
		M.print_tag_set();
	else
	if (Action == "testing")
	{
		if (InputFile.empty())
			fprintf (stderr, "no input file!\n");
		else
			M.testing(InputFile);
	}
	else
	if (Action == "dump_transition_probs")
		M.dump_transition_probs();
	else
	if (Action == "adjusting")
	{
		if (InputFile.empty())
			fprintf (stderr, "no input file!\n");
		else
			M.adjusting_homonyms_coef(InputFile);
	}
	else
	if (Action == "lemmatize")
	{
		M.lemmatize_file(InputFile);
	}
	else
		fprintf (stderr, "Error! unknown action\n");

	return 0;
}

int main(int argc, char **argv)
{
	
    try {
        int Res = mymain(argc,argv);
        return Res;
    }
    catch (CExpc c)
    {
        fprintf (stderr, "%s", c.m_strCause);
        return 1;
    }
	
	
};
