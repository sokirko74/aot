/*
Пример вызова интерфейса Seman. В stdin подаются предложения в  кодировке Win-1251(по одному на каждой строке). 
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


CSemStructureBuilder SemBuilder;
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


string GetWordStrOfNode (const CRusSemNode&  Node, bool bOnlyWords = false)
{
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
	    for (int i=0; i< Node.m_Words.size(); i++)
	    {
		    const CRusSemWord& Word = Node.m_Words[i];
		    NodeStr += Word.m_Word;
            
		    NodeStr += ": ";
		    NodeStr += Word.m_Lemma;
		    NodeStr += " ";
		    NodeStr += GetGramInfo(Word.m_Poses, Word.GetAllGrammems());
	    }
	    NodeStr += " Отредакт. морф. информация: ";
	    NodeStr += GetGramInfo(Node.GetNodePoses(), Node.GetGrammems());
	    if (!Node.m_AntecedentStr.empty())
	    {
		    NodeStr += " anteced=" + Node.m_AntecedentStr;
	    }
    }

	return NodeStr;
};
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
				for (int i = 0; i < SemBuilder.m_RusStr.m_Nodes.size(); i++)
				{
					printf ("%s",  GetWordStrOfNode(SemBuilder.m_RusStr.m_Nodes[i], true).c_str());
				    for (int k = 0; k < SemBuilder.m_RusStr.m_Relations.size(); k++)
				    {
                        const CRusSemRelation& R = SemBuilder.m_RusStr.m_Relations[k];
                        if (!R.m_Valency.m_RelationStr.empty() && R.m_TargetNodeNo == i) {
                            string q = R.m_Valency.m_RelationStr;
                            EngRusMakeLower(q);
                            printf ("[%s]", q.c_str());
                            break;
                        }
                    }
                    printf (" ");

				}
                printf ("\n");

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
