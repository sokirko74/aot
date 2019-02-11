#include "../common/utilit.h"
#include "../TrigramLib/TrigramModel.h"



void PrintUsageAndExit()
{
	fprintf (stderr,"Convert Trigram Lex file to binary format \n");
	fprintf (stderr,"Usage(1): Trigram <trigram_config>\n");
	exit(1);
};


/* ------------------------------------------------------------ */
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        PrintUsageAndExit();
    }
    string ConfigFile = argv[1];
    CTrigramModel M;
    try {
        
		fprintf (stderr, "init model  from %s\n", ConfigFile.c_str() );
        M.InitModelFromConfigAndBuildTagset(ConfigFile, 0, 0, false);

        fprintf (stderr, "register tags from ngram file\n");
        M.register_tags_from_ngram_file();

        fprintf (stderr, "convert_to_lex_binary, lexfile=%s\n", M.m_DictionaryFile.c_str());
        if (!M.convert_to_lex_binary())
            return 1;

        if (!M.read_ngram_file()) return 1;

        fprintf (stderr, "compute_counts_for_boundary ... \n");
	    M.compute_counts_for_boundary();

	    fprintf (stderr, "compute_bucketed_lambdas ... \n");
	    M.compute_bucketed_lambdas();

	    M.compute_transition_probs();

        M.SaveBinary();
        fprintf (stderr, "exiting \n");
    }
    catch ( CExpc c)
    {
        fprintf (stderr, "%s\n", c.m_strCause.c_str());
        return 1;
    }
    catch ( ...)
    {
        fprintf (stderr, "General exception\n");
        return 1;
    }
    return 0;
	
}

