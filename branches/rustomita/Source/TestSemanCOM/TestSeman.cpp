/*
Пример вызова интерфейса Seman. В stdin подаются предложения в  кодировке Win-1251
		(по одному на каждой строке). 
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

#include "string"
#import "../../bin/seman.tlb"
#import "../../bin/agramtab.tlb"
using namespace SEMANLib;

AGRAMTABLib::IGramTabPtr piRusGramTab;
using namespace std;


string GetGramInfo (long Poses, __int64 Grammems )
{
	string Result;
	for (size_t i =0; i < sizeof(Poses)*8; i++)
	if ( (1<<i) &  Poses)
	{
		Result += (const char*)piRusGramTab->GetPartOfSpeechStr(i);
		Result +=  " ";
	}
	Result += (const char*)piRusGramTab->GrammemsToStr(Grammems);

	return  Result;
}

string GetRelOperatorsOfNode (IComSemNodePtr Node)
{
	string R;
	for(int i=0; i < Node->RelOperatorsCount; i++)
	{
		R += string("(")+(const char*)Node->RelOperators[i]+")";

	}
	return R;

}

string GetWordRealizationOfIncominRelation (ISemStructurePtr piSeman, int NodeNo)
{
	for (int i = 0; i < piSeman->RelationsCount; i++)
	{
		IComSemRelationPtr R = piSeman->Relations[i];
		if (R->TargetNodeNo == NodeNo)
		{
			return (const char*)R->WordRealization;
		}
	};
	return "";
}


string GetWordStrOfNode (ISemStructurePtr piSeman, int NodeNo)
{
	IComSemNodePtr Node = piSeman->Nodes[NodeNo];
	string NodeStr;

	string PrepOrConj = 
		GetWordRealizationOfIncominRelation(piSeman, NodeNo);

	if (PrepOrConj.empty())
		PrepOrConj = Node->Prep;
	
	
	for (int i=0; i< Node->WordsCount; i++)
	{
		IComSemWordPtr Word = Node->Words[i];
		NodeStr += (const char*)Word->WordStr;
		NodeStr += "\t";
		NodeStr += (const char*)Word->Lemma;
		NodeStr += "\t";
		NodeStr += GetRelOperatorsOfNode(Node);
		NodeStr += "\t";
		NodeStr += PrepOrConj;
		NodeStr += "\t";
		NodeStr += GetGramInfo(Word->Poses, Word->Grammems);
	}
	NodeStr += " Отредакт. морф. информация: ";
	NodeStr += GetGramInfo(Node->Poses, Node->Grammems);

	return NodeStr;
};

int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		fprintf (stderr, "TestSeman prints the best semantic relations");
		fprintf (stderr, " for the input stream (www.aot.ru)\n");
		fprintf (stderr, "Usage:  TestSeman <input.txt >output.txt\n");
		return 1;
	};

	CoInitialize(NULL);
	
	try 
	{
		piRusGramTab.CreateInstance(__uuidof(AGRAMTABLib::RusGramTab));
		piRusGramTab->Load();
	}
	catch(...)
	{
		fprintf (stderr, "agramtab.dll is not registered!\n");
		return 0;
	}

	ISemStructurePtr piSeman;
	try 
	{
		piSeman.CreateInstance( __uuidof(SemStructure));
	}
	catch(...)
	{
		fprintf (stderr, "Seman.dll is not registered!\n");
		return 0;
	}
	if (piSeman == 0)
	{
		fprintf (stderr, "Seman.dll is not registered!\n");
		return 0;
	}
	
	try 
	{
		fprintf (stderr, "initialize presemantic dictionaries...\n");
		piSeman->InitPresemanDicts();
		fprintf (stderr, "initialize semantic dictionaries...\n");
		piSeman->InitSemanDicts();
		piSeman->InitializeIndices();
		piSeman->ShouldBuildTclGraph = FALSE;
	}
	catch(...)
	{
		fprintf (stderr, "Cannot load semantic dictionaries!\n");
		return 0;
	};


	try 
	{
		fprintf (stderr, "Parsing sentences!\n");
		char buffer[10000];
		size_t SentenceNo = 1;
		while (fgets(buffer,  10000, stdin))
		{
			printf ("SentenceNo = %i\n",SentenceNo++);

			piSeman->FindSituations(buffer,0,"общ",20000,-1,"");
			do {
				printf ("Nodes:\n");
				for (int i = 0; i < piSeman->NodesCount; i++)
				{
					printf ("\tNode %i %s\n",  
								i, 
								GetWordStrOfNode(piSeman, i).c_str());
				}
				printf ("Relations:\n");
				for (int i = 0; i < piSeman->RelationsCount; i++)
				{
					IComSemRelationPtr R = piSeman->Relations[i];
					printf ("\t%s (%i, %i)\n", 
						(const char*) R->Name,
									R->TargetNodeNo,
									R->SourceNodeNo);
				};

				if (piSeman->DopRelationsCount > 0)
				{
					printf ("Aux Relations:\n");
					for (int i = 0; i < piSeman->DopRelationsCount; i++)
					{
						IComSemRelationPtr R = piSeman->DopRelations[i];
						printf ("\t%s (%i, %i)\n", 
							(const char*) R->Name,
										R->TargetNodeNo,
										R->SourceNodeNo);
					};
				}
			}
			while (piSeman->FindSituationsForNextSentence()); 
								// вызов следующего предложения
		};
		fprintf (stderr, "exit\n");	
		piSeman = 0;
		piRusGramTab = 0;
		CoUninitialize();

	} catch(...)
	{
		printf ("an exception occurred(%s)!", 
				(const char*)piSeman->GetLastError());
		return 0;
	}

	return 0;
}
