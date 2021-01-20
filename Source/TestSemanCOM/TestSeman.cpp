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
#include <iostream>
#include <fstream>
#include "assert.h"
#import "../agramtab/agramtab.tlb"
#import "../Seman/seman.tlb"

AGRAMTABLib::IGramTabPtr piRusGramTab;

std::string GetGramInfo (long Poses, __int64 Grammems )
{
	std::string Result;
	for (size_t i =0; i < sizeof(Poses)*8; i++)
	if ( (1<<i) &  Poses)
	{
		Result += (const char*)piRusGramTab->GetPartOfSpeechStr(i);
		Result +=  " ";
	}
	Result += (const char*)piRusGramTab->GrammemsToStr(Grammems);

	return  Result;
}

std::string GetRelOperatorsOfNode (SEMANLib::IComSemNodePtr Node)
{
	std::string R;
	for(int i=0; i < Node->RelOperatorsCount; i++)
	{
		R += std::string("(")+(const char*)Node->RelOperators[i]+")";

	}
	return R;

}

std::string GetWordRealizationOfIncominRelation (SEMANLib::ISemStructurePtr piSeman, int NodeNo)
{
	for (int i = 0; i < piSeman->RelationsCount; i++)
	{
		auto R = piSeman->Relations[i];
		if (R->TargetNodeNo == NodeNo)
		{
			return (const char*)R->WordRealization;
		}
	};
	return "";
}


std::string GetWordStrOfNode (SEMANLib::ISemStructurePtr piSeman, int NodeNo)
{
	auto Node = piSeman->Nodes[NodeNo];
	std::string NodeStr;

	std::string PrepOrConj = 
		GetWordRealizationOfIncominRelation(piSeman, NodeNo);

	if (PrepOrConj.empty())
		PrepOrConj = Node->Prep;
	
	
	for (int i=0; i< Node->WordsCount; i++)
	{
		auto Word = Node->Words[i];
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
	if (argc != 2)
	{
		std::cerr << "TestSeman prints the best semantic relations\n";
		std::cerr << " for the input stream (www.aot.ru)\n";
		std::cerr << "Usage:  TestSeman input.txt\n";
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
		std::cerr << "agramtab.dll is not registered!\n";
		return 1;
	}

	SEMANLib::ISemStructurePtr piSeman;
	try 
	{
		piSeman.CreateInstance( __uuidof(SEMANLib::SemStructure));
	}
	catch(...)
	{
		std::cerr << "Seman.dll is not registered!\n";
		return 1;
	}
	if (piSeman == 0)
	{
		std::cerr << "Seman.dll is not registered!\n";
		return 1;
	}
	
	try 
	{
		std::cerr << "initialize presemantic dictionaries...\n";
		piSeman->InitPresemanDicts();
		std::cerr << "initialize semantic dictionaries...\n";
		piSeman->InitSemanDicts();
		piSeman->InitializeIndices();
		piSeman->ShouldBuildTclGraph = 0;
	}
	catch(...)
	{
		std::cerr << "Cannot load semantic dictionaries!\n";
		return 1;
	};


	try 
	{
		std::cerr << "Parsing sentences!\n";
		size_t SentenceNo = 1;
		std::string inputFileName = argv[1];
		std::ifstream  inp(inputFileName);
		assert(inp.is_open());
		
		std::ofstream  outp(inputFileName + ".seman", std::ios::binary);
		assert(outp.is_open());
		std::string s;
		while (std::getline(inp, s))
		{
			piSeman->FindSituations(s.c_str(), 0, "общ",20000,-1,"");
			do {
				outp << "Nodes:\n";
				for (int i = 0; i < piSeman->NodesCount; i++)
				{
					outp <<  "\tNode "<< i << " " << GetWordStrOfNode(piSeman, i) << "\n";
				}
				outp << "Relations:\n";
				for (int i = 0; i < piSeman->RelationsCount; i++)
				{
					auto R = piSeman->Relations[i];
					outp << "\t" << R->Name << "(" << R->TargetNodeNo << "," << R->SourceNodeNo << ")\n";
				};

				if (piSeman->DopRelationsCount > 0)
				{
					outp << "Aux Relations:\n";
					for (int i = 0; i < piSeman->DopRelationsCount; i++)
					{
						auto R = piSeman->DopRelations[i];
						outp << "\t" << R->Name << "(" << R->TargetNodeNo << "," << R->SourceNodeNo << ")\n";
					};
				}
			}
			while (piSeman->FindSituationsForNextSentence()); 
								// вызов следующего предложения
		};
		std::cerr << "exit\n";	
		piSeman = 0;
		piRusGramTab = 0;
		CoUninitialize();
		return 0;

	} catch(...)
	{
		std::cerr << "an exception occurred:" << (const char*)piSeman->GetLastError() << "\n";
		return 1;
	}

}
