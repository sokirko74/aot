// программа читает xml-файл в стандарте MorphCorpusCreator 
// На  выходе программа строит файл, где на каждой строке стоит отдельное предложение.
// После каждого слова стоит тэг этого слова.  Файл в таком формате может читать пакет acopost, такой 
// формат в этом пакете называется "cooked" формат.
#include "../common/util_classes.h"
#include "../common/MorphXmlToken.h"
#include "../common/RusCorpXml.h"
#include "../TrigramLib/TrigramModel.h"



void PrintUsageAndExit()
{
	fprintf (stderr,"TrigramRusCorporaXml \n");
	fprintf (stderr,"Usage1: TrigramRusCorporaXml cook   <in-file> \n");
    fprintf (stderr,"Usage2: TrigramRusCorporaXml cook_lemma   <in-file> \n");
    fprintf (stderr,"Usage3: TrigramRusCorporaXml disamb <in-file> <trigram.cfg>\n\n");
    fprintf (stderr,"<in-file>  is a ruscorpus .xhtml3  file\n");
    fprintf (stderr,"<trigram.cfg>  is a trigram model config\n");
    fprintf (stderr,"if <in-file> has extension 'lst', then it is treated as a list of input files\n");
    
	exit(1);
};


extern void PrintCookedSentence (const vector<CXmlToken>& Tokens, bool bPrintLemmas);
bool CookSentence(FILE* fp, bool bPrintLemmas)
{
    size_t TokensCount = 0;
    
    CRusCorpXmlFile XmlFile;
    try {
        while (XmlFile.ReadNextSentence (fp, false))
        {
            XmlFile.CreateGraTable();
            PrintCookedSentence (XmlFile.m_CurrSentTokens, bPrintLemmas);
            TokensCount += XmlFile.m_CurrSentTokens.size();
        }
    }
    catch (CExpc c)
    {
        fprintf (stderr, "Error! %s, processed tokens count = %i\n", c.m_strCause.c_str(), TokensCount );
        return false;
    }
    return true;
}

bool Disambiguate(const CTrigramModel& M,  FILE* fp, FILE* out_fp)
{
    size_t TokensCount = 0;
    string Sent;
    try {
        CRusCorpXmlFile XmlFile;
        XmlFile.m_OutFile = out_fp;
        size_t save_offset = ftell(fp);
        size_t SentNo = 0;
        while (XmlFile.ReadNextSentence (fp, true))
        {
            XmlFile.CreateGraTable();
            M.DisambiguateRusCorpXml(XmlFile.m_CurrSentTokens);
            XmlFile.PrintDisambiguatedXmlNodes();
            TokensCount += XmlFile.m_CurrSentTokens.size();
            SentNo++;
            if (SentNo%100)
                fprintf (stderr, ".");
        }
        fprintf (stderr, "\n");
    }
    catch (CExpc c)
    {
        fprintf (stderr, "Error! %s, processed tokens count = %i\n", c.m_strCause.c_str(), TokensCount );
        return false;
    }
    
    return true;
}


int main(int argc, char* argv[])
{
    CTrigramModel M;    
    if (argc < 3)
        PrintUsageAndExit();
    string Action  = argv[1];
    if (Action != "cook"  && Action != "disamb" && Action != "cook_lemma" )
        PrintUsageAndExit();
    if (Action == "disamb")
    {
        if (argc != 4)
            PrintUsageAndExit();
        try {
            M.InitModelFromConfigAndBuildTagset(argv[3], 0, 0, true);
            M.ReadBinary();
            M.m_bQuiet = true;
        }
        catch (CExpc c)
        {
            fprintf (stderr, "%s",c.m_strCause.c_str());
            return 1;
        }
    }

    string FileName = argv[2];
    vector<string> Files;
    if (FileName.length() > 3 && FileName.substr(FileName.length() -3) == "lst")
    {
        FILE * fp =  fopen( FileName.c_str(), "r");
        if (!fp)
        {
            fprintf (stderr,"Cannot open %s\n",FileName.c_str());
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
    else
        Files.push_back(FileName);

    bool bRes = true;
    for (size_t i=0; bRes && i <Files.size(); i++)
    {
        if (Files.size() > 1)
            fprintf (stderr, "%i/%i: %s\n",i, Files.size(), Files[i].c_str());
        FILE* fp  = fopen (Files[i].c_str(), "r" );
        if (!fp )
        {
            fprintf (stderr, "cannot open input file:%s\n", Files[i].c_str());
            exit (1);
        }
        FILE* outfp = stdout;
        if (Files.size() > 1)
        {
            string outfile = MakeFName(Files[i], "trigram");
            outfp = fopen (outfile.c_str(), "w");
            if (!outfp)
            {
                fprintf (stderr, "cannot open %s\n", outfile.c_str());
                exit(1);
            }
        }

        if (Action == "cook" || Action == "cook_lemma")
            bRes = CookSentence(fp, Action == "cook_lemma");
        else
            bRes = Disambiguate(M, fp, outfp);
        fclose (fp);
        if (Files.size() > 1)
            fclose (outfp);
    }

    return bRes ? 0 : 1;
}


