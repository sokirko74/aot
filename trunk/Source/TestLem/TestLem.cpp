//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//================================

#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>

#include "../common/utilit.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/Paradigm.h"

bool				bPrintIds = true;;
bool				bPrintForms = false;
bool                bSortParadigms = false;
MorphLanguageEnum	Language;
CLemmatizer*		pLemmatizer;
CAgramtab*			pAgramtab;



string GetGrammems (const char* tab_str)
{
	QWORD G;
	pAgramtab->GetGrammems(tab_str, G);
	string s = pAgramtab->GrammemsToStr (G);
	if (!s.empty() && (s[s.length() -1] == ','))
		s.erase(s.length() - 1);
	return s;
}

std::string  GetMorphInfo (std::string  Form)
{
	

	bool bCapital   = is_upper_alpha((BYTE)Form[0], Language);

	std::vector<CFormInfo> Paradigms;

	pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);

	std::vector< std::string>  Results;
	for (int i=0; i < Paradigms.size(); i++)
	{
        std::string Result;
		const CFormInfo& F = Paradigms[i];
		Result += F.m_bFound ? "+ " : "- ";

		Result += F.GetWordForm(0) + " "; 

		{
			string GramCodes =	F.GetSrcAncode();
			BYTE  PartOfSpeech = pAgramtab->GetPartOfSpeech (GramCodes.c_str());
			Result	+= pAgramtab->GetPartOfSpeechStr(PartOfSpeech) + std::string(" ") ;	

			string  CommonAncode = F.GetCommonAncode();
			Result += Format ("%s ", (CommonAncode.empty()) ? "" : GetGrammems(CommonAncode.c_str()).c_str());

            for (long i=0; i < GramCodes.length(); i+=2) {
                if (i > 0)
                    Result += ";";
				Result += Format ("%s", GetGrammems(GramCodes.c_str()+i).c_str());
            }

	   }

		if (bPrintIds)
			Result += Format(" %i",F.GetParadigmId());

		BYTE Accent = F.GetSrcAccentedVowel();
		if (Accent != 0xff)
			Result += Format(" %s'%s", Form.substr(0, Accent+1).c_str(), Form.substr(Accent+1).c_str());

        if (bPrintForms) {
		    Result += " ";
		    for (int k=0; k < F.GetCount (); k++)
		    {
                if (k > 0)
                    Result += ",";
			    Result += Paradigms[i].GetWordForm(k);
		    };
        };
        Results.push_back(Result);
   	};

    if (bSortParadigms) {
        std::sort( Results.begin(), Results.end() );
    };
    std::string Result;
	for (int i=0; i < Results.size(); i++) {
        if (i > 0) 
            Result += "\t";
        Result  +=  Results[i] + "\n";
    }
	return Result;
};




void ProcessFile(std::string Filename)
{
	printf ("read %s\n", Filename.c_str());

	StringVector Forms;
	{
		FILE* fp = fopen (Filename.c_str(), "r");
		if (!fp)
		{
			printf ("cannot open file %s\n", Filename.c_str());
			return;
		};
		char buffer[256];
		while (fgets(buffer,256, fp))
		{
			std::string q =  buffer;
			Trim(q);
			if (q.empty()) continue;
			RmlMakeUpper(q,pLemmatizer->GetLanguage());
			Forms.push_back(q);
		};
		fclose (fp);
	};
	size_t Count = Forms.size();

	StringVector Results;
	Results.resize(Count, string (255,' ') );
	printf ("process %s\n", Filename.c_str());
	clock_t start = clock();
	const size_t OutBufferSize = 2000; 
	char OutBuffer[OutBufferSize];
	for (size_t i=0; i < Count; i++)
	{
		if (!pLemmatizer->GetAllAncodesAndLemmasQuick(Forms[i], true, OutBuffer, OutBufferSize, true))
		{
			printf ("Too many variants for %s\n", Forms[i].c_str());
		};
		//pLemmatizer->GetAllAncodesQuick((const BYTE*)Forms[i].c_str(), true, OutBuffer);
		Results[i] = (char*)OutBuffer;
	};
	long ticks = clock() - start;
	printf ("Count of words = %i\n", Count);
	double seconds = (double)ticks/(double)CLOCKS_PER_SEC;
	printf ("Time = %g seconds; %i ticks\n", seconds,ticks);
	if (seconds > 0)
		printf ("Speed = %g words per seconds\n", (((double)Count)/seconds));
	else
		printf ("too few words to measure the speed\n");
	

	std::string OutFileName = MakeFName(Filename,"lem");
	printf ("writing to %s\n",OutFileName.c_str());
	{
		FILE* out_fp = fopen (OutFileName.c_str(), "w");
		if (!out_fp)
		{
			printf ("cannot write file %s\n", OutFileName.c_str());
			return;
		};
		for (size_t i=0; i < Count; i++)
			fprintf (out_fp,"%s -> %s\n", Forms[i].c_str(), Results[i].c_str());
		fclose (out_fp);
	};


};


template <class  T, class Y>
bool InitMorphologySystem()
{
	std::string langua_str = GetStringByLanguage(Language);
	pLemmatizer = new T;
	string strError;
	if (!pLemmatizer->LoadDictionariesRegistry(strError))
	{
   		fprintf (stderr, "Cannot load %s  morphological dictionary\n", langua_str.c_str());
		fprintf (stderr, "Return error: %s\n", strError.c_str());
		return false;
	};
	pAgramtab = new Y;
	if (!pAgramtab->LoadFromRegistry())
	{
   		fprintf (stderr, "Cannot load %s  gramtab\n", langua_str.c_str() );
		return false;
	};
	return true;
};


void PrintUsage()
{
		printf ("Dialing Command Line Morphology (www.aot.ru)\n");
		printf ("Usage: TestLem (RUSSIAN|ENGLISH|GERMAN) [file]\n");
		printf ("if [file] is specified then TestLem builds a lem-file by [file] and exits\n");
		printf ("otherwise it reads from standard input\n");
		printf ("Example: TestLem Russian\n");
		printf ("Example: TestLem English test.txt\n");
		exit(-1);	
};


int main(int argc, char **argv)
{
   	
    vector<string> ARGV;
    bool bEchoInput = false;
    for (size_t i=1; i <  argc; i++)
    {
		string s = argv[i];
        if (s == "-h" || s == "-help" ||  s == "/h" || s == "/help") {
            PrintUsage();
        }
        else if (s == "-echo") 
        {
            bEchoInput = true;
        }
		else if (s == "-noids")
		{
			bPrintIds  = false;
		}
		else if (s == "-sort")
		{
			bSortParadigms  = true;
		}
		else if (s == "-forms")
		{
			bPrintForms  = true;
		}
        else {
            ARGV.push_back(s);
        }
    }
	if (ARGV.size() < 1)
		PrintUsage();
    
    if (!GetLanguageByString(ARGV[0], Language))
	{
		PrintUsage();
	};

	std::string FileName;
	if (ARGV.size() > 1)
		FileName = ARGV[1];

	// ===============  LOADING DICTS ================
	fprintf (stderr,"Loading..\n");
	bool bResult = false;
	switch (Language)
	{
		case morphRussian: 
					bResult = InitMorphologySystem<CLemmatizerRussian, CRusGramTab>();
					break;
		case morphEnglish : 
					bResult = InitMorphologySystem<CLemmatizerEnglish, CEngGramTab>();
					break;
		case morphGerman: 
					bResult = InitMorphologySystem<CLemmatizerGerman, CGerGramTab>();
					break;
		default : PrintUsage();
		
	};
	if (!bResult)
		return 1;

	if (!FileName.empty())
	{
		ProcessFile(FileName);
		delete pLemmatizer;
		delete pAgramtab;
		return 0;
	};
	

	// ===============  WORKING ===============
	
	fprintf (stderr,"Input a word..\n");
    char Form[100];	
	while (fgets(Form, 1000, stdin))
	{
		std::string s = Form;
		Trim(s);
		if (s.empty()) break;
		string Result = GetMorphInfo(s);

        if (bEchoInput)  {
            printf ("%s\t", s.c_str());
        }
		printf ("%s\n", Result.c_str());
		 
	};
	delete pLemmatizer;
	delete pAgramtab;
	return 0;
}





