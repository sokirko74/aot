#include <stdio.h>
#include "../SynCommonLib/RelationsIterator.h"
#include "../common/SyntaxHolder.h"


void GetAnanlytForms(const CSentence& Sentence, StringSet& Groups)
{
    for( int WordNo = 0; WordNo<Sentence.m_Words.size(); WordNo++ )
    {
       const CSynWord& W = Sentence.m_Words[WordNo];
       if (!W.m_MainVerbs.empty())
        {
            string S = string("AnalytForm\t=\t")+  W.m_strWord.c_str();
            for (size_t i=0; i< W.m_MainVerbs.size(); i++)
		    {
                S += string(" ") + Sentence.m_Words[W.m_MainVerbs[i]].m_strWord;

			    const CSynWord& W_1 = Sentence.m_Words[W.m_MainVerbs[i]];
			    for (size_t j=0; j< W_1.m_MainVerbs.size(); j++)
				    S += string(" ") + Sentence.m_Words[W_1.m_MainVerbs[j]].m_strWord;

		    };
		    Groups.insert(S);
       }
    }
}

void GetGroups(const CSentence& Sentence, const CAgramtab& A, StringSet& Groups)
{
    int nClausesCount = Sentence.GetClausesCount();

    for( int ClauseNo = 0; ClauseNo<nClausesCount; ClauseNo++ )
    {
        const CClause& Clause = Sentence.GetClause(ClauseNo);
        int nCvar = Clause.m_SynVariants.size();

        if (Clause.m_SynVariants.empty()) continue;

        int nVmax = Clause.m_SynVariants.begin()->m_iWeight;
        for( CSVI pSynVar=Clause.m_SynVariants.begin(); pSynVar != Clause.m_SynVariants.end(); pSynVar++ )
        {
            if( pSynVar->m_iWeight < nVmax ) break;

            const CMorphVariant& V = *pSynVar;

            // print the clause
            int ClauseType = (V.m_ClauseTypeNo == -1) ? UnknownSyntaxElement  :  Clause.m_vectorTypes[V.m_ClauseTypeNo].m_Type;;

            string S;
            if (ClauseType != UnknownSyntaxElement)
                S = (const char*)A.GetClauseNameByType(ClauseType);
            else
                S = "EMPTY";

            S += "\t=\t";
            for (int WordNo = Clause.m_iFirstWord; WordNo <= Clause.m_iLastWord; WordNo++)
            {
                S += Sentence.m_Words[WordNo].m_strWord;
                S += " ";
            };
            Groups.insert(S);

            // распечатываем все группы 


            for (int GroupNo = 0; GroupNo < V.m_vectorGroups.GetGroups().size(); GroupNo++)
            {
                const CGroup& G  = V.m_vectorGroups.GetGroups()[GroupNo];

                S = (const char*)Sentence.GetOpt()->GetGroupNameByIndex(G.m_GroupType);
                S += "\t=\t";
                for (int WordNo = G.m_iFirstWord; WordNo <= G.m_iLastWord; WordNo++)
                {
                    S += Sentence.m_Words[WordNo].m_strWord;
                    S += " ";
                };
                Groups.insert(S);
            };
        }
    }


}

string GetNodeStr(const CSentence& Sentence, const CRelationsIterator& RelIt, int GroupNo, int WordNo)
{
    string Result;
    if (GroupNo != -1)
    {
        for (size_t  i=RelIt.GetFirmGroups()[GroupNo].m_iFirstWord; i <= RelIt.GetFirmGroups()[GroupNo].m_iLastWord; i++)
        {
            Result += Sentence.m_Words[i].m_strWord;
            Result += " ";
        }

    }
    else
    {
        Result += Sentence.m_Words[WordNo].m_strWord;
        Result += " ";
    }
    return Result;
}

void GetRelations(const CSentence& Sentence, string& Result)
{
    CRelationsIterator RelIt; 
	RelIt.SetSentence(&Sentence);
    for( int i = 0; i<Sentence.m_vectorPrClauseNo.size(); i++ )
		RelIt.AddClauseNoAndVariantNo(Sentence.m_vectorPrClauseNo[i], 0);
    RelIt.BuildRelations();
    StringSet Rels;
   	for(long RelNo = 0 ; RelNo < RelIt.GetRelations().size() ; RelNo++ )
	{
		const CSynOutputRelation& piRel = RelIt.GetRelations()[RelNo];
		string RelName = Sentence.GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type);
        string Src = GetNodeStr(Sentence, RelIt,piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord);
        string Trg = GetNodeStr(Sentence, RelIt,piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord);
        Rels.insert( Format(" %s ( %s-> %s)\n", RelName.c_str(), Src.c_str(),  Trg.c_str()) );
    }
    for (StringSet::const_iterator it = Rels.begin(); !(it == Rels.end()); it++)
	    Result += *it;



}
string GetStringBySyntax(const CSentencesCollection& SC, const CAgramtab& A)
{
    string Result;
    for (size_t nSent=0;  nSent < SC.m_vectorSents.size(); nSent++)
    {
        if (SC.m_vectorSents.size() > 1)
            Result += Format("SentNo=%i\n", nSent);



	    const CSentence& Sentence = *SC.m_vectorSents[nSent];
	    int iWord=0,iClau=0,iCvar=0;
	    StringSet Groups;

        GetAnanlytForms(Sentence, Groups);
        GetGroups(Sentence, A, Groups);
	    for (StringSet::const_iterator it = Groups.begin(); !(it == Groups.end()); it++)
	    {
		    Result += *it;
		    Result += "\n";
	    };
        GetRelations(Sentence, Result);

    }

    fprintf (stderr, "sentences count: %i\n", SC.m_vectorSents.size());
	return Result;

	//return "ddd";

};
	
extern void KOI8ToWin (string& s);
extern void WinToKOI8 (string& s);
void RussianConvertToScreen (string& S)
{
	#ifdef WIN32
		CharToOemBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		
	#else
		WinToKOI8(S);
	#endif

};
void RussianConvertToDictionary (string& S)
{
	#ifdef WIN32
		OemToCharBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		
	#else
		KOI8ToWin(S);
	#endif

};


void PrintUsage()
{
		printf ("Dialing DWDS Command Line Syntax Parser(www.aot.ru, www.dwds.de)\n");
		printf ("Usage: TestSynan (RUSSIAN|GERMAN) [filename.txt]\n");
		printf ("Example: TestSynan Russian\n");
		exit(1);	
};


int main(int argc, char* argv[])
{
try {
	

	MorphLanguageEnum langua;
    string  FileName;
    if (argc == 3)
    {
        FileName = argv[2];
        if (!FileExists(FileName.c_str()))
        {
            fprintf (stderr, "cannot open file %s\n",  FileName.c_str());
            exit(1);
        }
        argc--;
    }
	if (argc != 2) PrintUsage();
	if (!GetLanguageByString(argv[1], langua)) PrintUsage();
	CSyntaxHolder H;
	
	if (!H.LoadSyntax(langua))
	{
		fprintf (stderr, "initialization error\n");
		return 1;
	};
	
	
	fprintf (stderr, "ok\n");   	
    if (!FileName.empty())
    {
        vector<string> Files;
        if ((FileName.length() > 4) && FileName.substr(FileName.length() -4) == ".lst")
        {
            FILE* fp = fopen(FileName.c_str(), "r");
            if (!fp) 
            {
                fprintf( stderr, "cannot open %s\n", FileName.c_str());
                return 1;
            }
            char buffer[1024];
            while (fgets (buffer, 1024, fp))
            {
                rtrim (buffer);
                Files.push_back(buffer);
            }
            fclose (fp);

        }
        else
            Files.push_back(FileName);
        for (size_t i=0; i < Files.size(); i++)
        {
            fprintf( stderr, "File %s\n", Files[i].c_str());
            fflush (stderr);
            H.m_bTimeStatis = true;
            H.GetSentencesFromSynAn(Files[i], true);
	    	string s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab);
		    printf  ("%s\n\n", s.c_str());
        }
        return 0;
    }


	printf ("Input a sentence..\n");
	// ===============  WORKING ===============
	for (;;)
	{
		printf (">");
		char buffer[1000] ;
		fgets(buffer, 1000, stdin);
		if (strlen(buffer) == 0) break;
		string s = buffer;
		Trim(s);
		if (s.empty()) break;
		
		RussianConvertToDictionary(s);
		
		//string s = "мама мыла раму";
		//string s = "die schwache Resolution";
		//string s = "test";
		H.GetSentencesFromSynAn(s, false);
		s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab);
		RussianConvertToScreen(s);	
		printf  ("%s\n\n", s.c_str());
        fflush(stdout);
	};
}
catch(...)
{
 	fprintf (stderr, "an exception occurred!\n");   	
	return 1;
};

	return 0;
};


