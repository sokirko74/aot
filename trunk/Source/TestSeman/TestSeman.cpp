/*
Пример вызова интерфейса Seman. В stdin подаются предложения в  кодировке Win-1251(по одному на каждой строке). 
Определите дерективу DOS для кодировки DOS (для работы в консоли Windows)
Программа печатает семантические узлы и отношения для каждого входного предложения.
Для каждого слова  семантического узла печатается лемма, часть речи и граммемы.

Для фразы "Мама мыла раму" программа должна напечатать:
Nodes:
	Node 0 МАМА: МАМА С од,жр,им,ед,
	Node 1 МЫЛА: МЫТЬ Г дст,пе,нс,прш,жр,ед,
	Node 2 РАМУ: РАМА С но,жр,вн,ед,
Relations:
	SUB (0, 1)
	OBJ (2, 1)
*/
#include "../SemanLib/SemStructureBuilder.h"

static CSemStructureBuilder SemBuilder;
extern void(*GlobalErrorMessage)(const string&);
void MyGlobalErrorMessage(const string& s) 
{
	throw CExpc (s);

}

string GetGramInfo (long Poses, QWORD Grammems )
{
	string Result;
	for (size_t i =0; i < sizeof(Poses)*8; i++)
	if ( (1<<i) &  Poses)
	{
		Result += (const char*)SemBuilder.m_RusStr.m_pData->GetRusGramTab()->GetPartOfSpeechStr(i);
		Result +=  " ";
	}
	Result += SemBuilder.m_RusStr.m_pData->GetRusGramTab()->GrammemsToStr(Grammems);
	return  Result;
}

void show_register (string& s, RegisterEnum Register)  {
    
    EngRusMakeLower(s);
    if (!s.empty())
        if (Register == UpLow)
            s[0] = rtoupper(s[0]);
        else 
            if (Register == UpUp)
                EngRusMakeUpper(s);
}
vector<string> words;

string GetWordStrOfNode (CRusSemStructure SS, int ssi, bool bOnlyWords = false)
{
	CRusSemNode&  Node = SS.m_Nodes[ssi];
    string NodeStr;
    if (bOnlyWords){
        if (Node.m_Words.size() > 1)
            NodeStr += "(";
        for (int i=0; i< Node.m_Words.size(); i++)
	    {
            string w = Node.m_Words[i].m_Word;
            show_register(w,Node.m_Words[i].m_CharCase);
		    NodeStr += w;
            if (i+1 < Node.m_Words.size())
                NodeStr += " ";
        }
        if (Node.m_Words.size() > 1)
            NodeStr += ")";
     
    }
    else
    {
		if ( Node.m_Words.size() == 0 || Node.m_Words[0].m_Word != SemBuilder.m_RusStr.GetNodeStr1(ssi))
			NodeStr += SemBuilder.m_RusStr.GetNodeStr1(ssi) + ": ";
	    for (int i=0; i< Node.m_Words.size(); i++)
	    {
		    const CRusSemWord& Word = Node.m_Words[i];
			if( !( i==0 && Word.m_Word != SemBuilder.m_RusStr.GetNodeStr1(ssi)) ) 
				NodeStr += Word.m_Word + ": ";

		    NodeStr += Word.m_Lemma;
		    NodeStr += " ";
		    NodeStr += GetGramInfo(Word.m_Poses, Word.GetAllGrammems());
		}
		if(Node.m_PrepWordNo >= 0 ) 
		{
			NodeStr = SS.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord + " " + NodeStr;
			//words[Node.m_PrepWordNo] = SS.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord;
		}
		if(!NodeStr.empty())
			NodeStr += " -> "; // Отредакт. морф. информация:
		NodeStr += GetGramInfo(Node.GetNodePoses(), Node.GetGrammems());
		if (!Node.m_AntecedentStr.empty())
		{
			NodeStr += " anteced=" + Node.m_AntecedentStr;
		}
	}

	return NodeStr;
};

char* Decode_DOS_to_Win(char * str)
{
#ifdef DOS
   char *buf = new char[std::strlen(str) + 1];
   OemToCharA(str, buf);
   return buf;
#endif
    return str;
}

//из Windows в DOS
char* Decode_Win_to_DOS(const char * str)
{
#ifdef DOS
   char *buf = new char[std::strlen(str) + 1];
   CharToOemA(str, buf); 
   return buf;
#endif

	return (char *)str;
}
char* w2d(string str) {return Decode_Win_to_DOS(str.c_str());}

/*Use in .NET to import dll:
[DllImport(dll)]
static extern int InitDicts();
[DllImport(dll)]
static extern string FindSituations(string s);*/


#ifdef WIN32
extern "C" __declspec(dllexport) int __stdcall InitDicts()
#else
int InitDicts()
#endif
{
	try {
		fprintf (stderr, "initialize presemantic dictionaries...\n");
		if (!SemBuilder.m_RusStr.m_pData->Init())
		{
			fprintf (stderr, "cannot initialize presemantic dictionaries.\n");
			return 1;
		}
		SemBuilder.m_RusStr.m_pData->Initialize();
		SemBuilder.m_RusStr.m_pData->InitializeIndices();
		SemBuilder.m_bShouldBuildTclGraph = false;
	}
	catch(...)
	{
		printf ("general initialize exception");
		return 1;
	}
	return 0;
} 

static char res [10000];
#ifdef WIN32
extern "C" __declspec(dllexport) char* __stdcall FindSituations(char* Source)
#else
char * FindSituations(const char * Source)
#endif
{
	char  result[100];// = new char[100]();
	string r;
	string ResGraph;
	SemBuilder.m_RusStr.m_pData->GetSynan()->SetKillHomonymsMode(CoverageKillHomonyms);//DontKillHomonyms CoverageKillHomonyms
	SemBuilder.FindSituations(Source,0,"общ",20000,-1,"", ResGraph);
	words.resize(SemBuilder.m_RusStr.m_piSent->m_Words.size());
	for (int i = 0; i < SemBuilder.m_RusStr.m_piSent->m_Words.size(); i++)
		words[i] = SemBuilder.m_RusStr.m_piSent->m_Words[i].m_strWord;
	printf ("Nodes:\n");
	for (int i = 0; i < SemBuilder.m_RusStr.m_Nodes.size(); i++)
	{
		string Nstr = GetWordStrOfNode(SemBuilder.m_RusStr, i).c_str();
		printf ("  Node %i %s\n",  i, w2d((Nstr.empty() ? SemBuilder.m_RusStr.GetNodeStr1(i) : Nstr)));
	}
	printf ("Relations:\n");

	for (int i = 0; i < SemBuilder. m_RusStr.m_Relations.size(); i++)
	{
		const CRusSemRelation& R = SemBuilder.m_RusStr.m_Relations[i];
		long m_TargetNodeNo = R.m_Valency.m_Direction == C_A && !R.m_bReverseRel ? R.m_SourceNodeNo : R.m_TargetNodeNo;
		long m_SourceNodeNo = !(R.m_Valency.m_Direction == C_A && !R.m_bReverseRel) ? R.m_SourceNodeNo : R.m_TargetNodeNo;
		//if (!R.m_Valency.m_RelationStr.empty())
		printf ("  %s (%s, %s) = %s (%i, %i)\n", 
			w2d(R.m_Valency.m_RelationStr), 
			w2d(SemBuilder.m_RusStr.GetNodeStr1(m_TargetNodeNo)), w2d(SemBuilder.m_RusStr.GetNodeStr1(m_SourceNodeNo)),
			w2d(R.m_SyntacticRelation),m_TargetNodeNo,m_SourceNodeNo);

	};
	if (SemBuilder.m_RusStr.m_DopRelations.size() > 0)
	{
		printf ("Aux Relations:\n");
		for (int i = 0; i < SemBuilder.m_RusStr.m_DopRelations.size(); i++)
		{
			const CRusSemRelation& R = SemBuilder.m_RusStr.m_DopRelations[i];
			long m_TargetNodeNo = R.m_Valency.m_Direction == C_A && !R.m_bReverseRel ? R.m_SourceNodeNo : R.m_TargetNodeNo;
			long m_SourceNodeNo = !(R.m_Valency.m_Direction == C_A && !R.m_bReverseRel) ? R.m_SourceNodeNo : R.m_TargetNodeNo;
			//if (!R.m_Valency.m_RelationStr.empty())
			printf ("  %s (%s, %s) = %s (%i, %i)\n", 
				w2d(R.m_Valency.m_RelationStr), 
				w2d(SemBuilder.m_RusStr.GetNodeStr1(m_TargetNodeNo)), w2d(SemBuilder.m_RusStr.GetNodeStr1(m_SourceNodeNo)),
				w2d(R.m_SyntacticRelation),m_TargetNodeNo,m_SourceNodeNo);

		}
	}
	for (int i = 0; i < words.size(); i++) 
		if (words[i]!="")
			r += words[i] + " ";

	//printf (w2d(r.c_str()));
	//printf ("\n");
	if(log_fp != 0) { fclose(log_fp); log_fp = 0; }

	res[r.size()]=0;
	strcpy(res, r.c_str());
	return res;
}

int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		fprintf (stderr, "TestSeman prints the best semantic relations for the input stream (www.aot.ru)\n");
		fprintf (stderr, "Usage:  TestSeman <input.txt >output.txt\n");
		return 1;
	};
	clock_t m_TimeSpan = clock();
	GlobalErrorMessage = MyGlobalErrorMessage;
	if(InitDicts()!=0) exit(1);

	fprintf (stderr, "%f Parsing sentences!\n", (double)(clock() - m_TimeSpan));
	char buffer[10000];
	size_t SentenceNo = 1;
	string Source;
	size_t LineCount = 0;
	while (fgets(buffer,  10000, stdin))
	{
		LineCount++;
		fprintf (stderr,  "%i) %s\n", LineCount, buffer);
		fflush(stderr);
		Source = Decode_DOS_to_Win(buffer);
		int comment = 0;
		if ((comment = Source.find("//")) > -1) 
			Source = Source.substr(0, comment);
		rtrim(buffer);
		if (strlen(buffer) > 250)
		{
			fprintf (stderr, "skip the sentence of %i chars\n", strlen(buffer));
			continue;
		}
		if (strlen(buffer) == 0 || Source.length() == 0)
		{
			fprintf (stderr, "skip empty sentence\n");
			continue;
		}
		try 
		{
			printf ("%s\n", buffer);
			FindSituations((char *)Source.c_str());
		} 
		catch(...)
		{
			fprintf (stderr, "an exception occurred(%i, %s)!\n", LineCount, buffer );
			printf ("!!!! an exception occurred(%i, %s)!\n", LineCount, buffer );

		}

	}
	//fclose (fp);
	fprintf (stderr, "exit\n");	

	return 0;
}

/*
int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		fprintf (stderr, "TestSeman prints the best semantic relations for the input stream (www.aot.ru)\n");
		fprintf (stderr, "Usage:  TestSeman <input.txt >output.txt\n");
		return 1;
	};

	GlobalErrorMessage = MyGlobalErrorMessage;

	try {
		fprintf (stderr, "initialize presemantic dictionaries...\n");
		if (!SemBuilder.m_RusStr.m_pData->Init())
		{
			fprintf (stderr, "cannot initialize presemantic dictionaries.\n");
			exit(1);
		}


		fprintf (stderr, "initialize semantic dictionaries...\n");
		SemBuilder.m_RusStr.m_pData->Initialize();

		SemBuilder.m_RusStr.m_pData->InitializeIndices();
		SemBuilder.m_bShouldBuildTclGraph = false;
	}
	catch(...)
	{
		printf ("general initialize exception");
		return 1;
	}

		fprintf (stderr, "Parsing sentences!\n");
		char buffer[10000];
		size_t SentenceNo = 1;
		string Source;
		size_t LineCount = 0;
		while (fgets(buffer,  10000, stdin))
		{
			LineCount++;
			fprintf (stderr,  "%i\n", LineCount);
			fflush(stderr);
			Source = buffer;
			rtrim(buffer);
			string ResGraph;
			if (strlen(buffer) > 250)
			{
					fprintf (stderr, "skip the sentence of %i chars\n", strlen(buffer));
					continue;
			}
			if (strlen(buffer) == 0)
			{
					fprintf (stderr, "skip empty sentence\n");
					continue;
			}
			try 
			{
				SemBuilder.FindSituations(Source,0,"общ",20000,-1,"", ResGraph);
				printf ("Nodes:\n");
				for (int i = 0; i < SemBuilder.m_RusStr.m_Nodes.size(); i++)
				{
					printf ("\tNode %i %s\n",  i, GetWordStrOfNode(SemBuilder.m_RusStr.m_Nodes[i]).c_str());
				}
				printf ("Relations:\n");
				for (int i = 0; i < SemBuilder.m_RusStr.m_Relations.size(); i++)
				{
					const CRusSemRelation& R = SemBuilder.m_RusStr.m_Relations[i];
					if (!R.m_Valency.m_RelationStr.empty())
						printf ("\t%s (%i, %i)\n", R.m_Valency.m_RelationStr.c_str(), R.m_TargetNodeNo,R.m_SourceNodeNo);
				};
			} 
			catch(...)
			{
				fprintf (stderr, "an exception occurred(%i, %s)!\n", LineCount, buffer );
				printf ("!!!! an exception occurred(%i, %s)!\n", LineCount, buffer );
				
				
			}


		}
		//fclose (fp);
		fprintf (stderr, "exit\n");	

	return 0;
}
*/
