#include <stdio.h>
#include "../SynCommonLib/RelationsIterator.h"
#include "../common/SyntaxHolder.h"


void GetAnanlytForms(const CSentence& Sentence, string& Res)
{
    for( int WordNo = 0; WordNo<Sentence.m_Words.size(); WordNo++ )
    {
       const CSynWord& W = Sentence.m_Words[WordNo];
       if (!W.m_MainVerbs.empty())
        {
            Res += string("\t<analyt> ")+  W.m_strWord.c_str();
            for (size_t i=0; i< W.m_MainVerbs.size(); i++)
		    {
                Res += string(" ") + Sentence.m_Words[W.m_MainVerbs[i]].m_strWord;

			    const CSynWord& W_1 = Sentence.m_Words[W.m_MainVerbs[i]];
			    for (size_t j=0; j< W_1.m_MainVerbs.size(); j++)
				    Res += string(" ") + Sentence.m_Words[W_1.m_MainVerbs[j]].m_strWord;
		    };
            Res += "</analyt>\n";
       }
    }
}
string GetWords(const CSentence& Sentence, const CPeriod& P)
{
    string S;
    for (int WordNo = P.m_iFirstWord; WordNo <= P.m_iLastWord; WordNo++)
    {
        S += Sentence.m_Words[WordNo].m_strWord;
        if (WordNo < P.m_iLastWord)
            S += " ";
    };
    return S;
}

void GetGroups(const CSentence& Sentence, const CAgramtab& A, string& Res)
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
            Res += Format("\t<synvar>\n");

            // print the clause
            {
                int ClauseType = (V.m_ClauseTypeNo == -1) ? UnknownSyntaxElement  :  Clause.m_vectorTypes[V.m_ClauseTypeNo].m_Type;;
                string Type;
                if (ClauseType != UnknownSyntaxElement)
                    Type = (const char*)A.GetClauseNameByType(ClauseType);
                else
                    Type = "EMPTY";
                Res += Format("\t\t<clause type=\"%s\">%s</clause>\n", Type.c_str(), GetWords(Sentence, Clause).c_str());
            }


            for (int GroupNo = 0; GroupNo < V.m_vectorGroups.GetGroups().size(); GroupNo++)
            {
                const CGroup& G  = V.m_vectorGroups.GetGroups()[GroupNo];
                Res += Format("\t\t<group type=\"%s\">%s</group>\n", 
                    Sentence.GetOpt()->GetGroupNameByIndex(G.m_GroupType), 
                    GetWords(Sentence, G).c_str());
            };
            Res += Format("\t</synvar>\n");
        }
    }


}

string GetNodeStr(const CSentence& Sentence, const CRelationsIterator& RelIt, int GroupNo, int WordNo)
{
    if (GroupNo != -1)
        return GetWords(Sentence, RelIt.GetFirmGroups()[GroupNo]);
    else
        return Sentence.m_Words[WordNo].m_strWord;
}

string GetNodeGrmStr(const CSentence& Sentence, const CRelationsIterator& RelIt, int GroupNo, int WordNo, string& Lemma)
{
    Lemma =  "";
    if (GroupNo != -1)
        return "";
    else {
        size_t ClauseNo = Sentence.GetMinClauseByWordNo(WordNo);
        const CClause& Clause = Sentence.GetClause(ClauseNo);
        const CMorphVariant* pSynVar  = &*Clause.GetSynVariantByNo(0);
        int UnitNo = pSynVar->UnitNoByWordNo(WordNo);
        const CSynUnit& U = pSynVar->m_SynUnits[UnitNo];
        Lemma = Sentence.GetWords()[WordNo].GetHomonym(U.m_iHomonymNum)->m_strLemma;
        return Sentence.GetOpt()->GetGramTab()->GrammemsToStr(U.m_iGrammems|U.m_TypeGrammems);
    }
}

void GetRelations(const CSentence& Sentence, string& Result)
{
    CRelationsIterator RelIt; 
	RelIt.SetSentence(&Sentence);
    for( int i = 0; i<Sentence.m_vectorPrClauseNo.size(); i++ )
		RelIt.AddClauseNoAndVariantNo(Sentence.m_vectorPrClauseNo[i], 0);
    RelIt.BuildRelations();
   	for(long RelNo = 0 ; RelNo < RelIt.GetRelations().size() ; RelNo++ )
	{
		const CSynOutputRelation& piRel = RelIt.GetRelations()[RelNo];
		string RelName = Sentence.GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type);
        string Src = GetNodeStr(Sentence, RelIt,piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord);
        string Trg = GetNodeStr(Sentence, RelIt,piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord);
        string SrcLemma, TrgLemma;
        string SrcGrm = GetNodeGrmStr(Sentence, RelIt,piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord, SrcLemma);
        string TrgGrm = GetNodeGrmStr(Sentence, RelIt,piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord, TrgLemma);
        string GramRel = Sentence.GetOpt()->GetGramTab()->GrammemsToStr(piRel.m_Relation.m_iGrammems);
        
        Result += Format("\t<rel name=\"%s\" gramrel=\"%s\" lemmprnt=\"%s\" grmprnt=\"%s\" lemmchld=\"%s\" grmchld=\"%s\" > %s -> %s </rel>\n", 
            RelName.c_str(), GramRel.c_str(), SrcLemma.c_str(), SrcGrm.c_str(), TrgLemma.c_str(), TrgGrm.c_str(), Src.c_str(),  Trg.c_str() );
    }
}

string GetStringBySyntax(const CSentencesCollection& SC, const CAgramtab& A, string input)
{
    string Result;
    Result += Format("<chunk>\n");
    Result += Format("<input>%s</input>\n",input.c_str());
    for (size_t nSent=0;  nSent < SC.m_vectorSents.size(); nSent++)
    {
	    const CSentence& Sentence = *SC.m_vectorSents[nSent];
	    int iWord=0,iClau=0,iCvar=0;
        Result += "<sent>\n";
        GetAnanlytForms(Sentence, Result);
        GetGroups(Sentence, A, Result);
        GetRelations(Sentence, Result);
        Result += "</sent>\n";
    }
    Result += Format("</chunk>\n");

    fprintf (stderr, "sentences count: %i\n", SC.m_vectorSents.size());
	return Result;
};
	


void PrintUsage()
{
		printf ("Dialing DWDS Command Line Syntax Parser(www.aot.ru, www.dwds.de)\n");
		printf ("Usage: TestSynan (RUSSIAN|GERMAN) [filename.txt] - \n");
		printf ("Example: TestSynan Russian\n");
		exit(1);	
};


int main(int argc, char* argv[])
{
try {
	

	MorphLanguageEnum langua = morphUnknown;
    string  FileName;
	for (size_t i=1; i<argc; i++)
	{
		string s = argv[i];
		EngMakeLower(s);
		if (		(s == "-help") 
				||	(s == "--help") 
				||	(s == "/?") 
			)
			PrintUsage();
        else {
            if (langua == morphUnknown) {
                if (!GetLanguageByString(s, langua)) 
                        PrintUsage();
            }
            else {
                FileName = argv[i];
                if (!FileExists(FileName.c_str()))
                {
                    fprintf (stderr, "cannot open file %s\n",  FileName.c_str());
                    exit(1);
                }
            }
        }

    }
	if ((langua == morphUnknown)) PrintUsage();

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
	    	string s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab, Files[i].c_str());
		    printf  ("%s\n\n", s.c_str());
        }
        return 0;
    }


	// ===============  WORKING ===============
    char buffer[10000] ;
	while  (fgets(buffer, 10000, stdin))
	{
		string s = buffer;
		Trim(s);
		if (s.empty()) continue;
		
		H.GetSentencesFromSynAn(s, false);
		s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab, s);
		printf  ("%s", s.c_str());
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


