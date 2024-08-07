#include  "SimpleGrammarLib.h"
#include "SimpleGrammar.h"
#include "morph_dict/agramtab/GerGramTab.h"


struct CInputSentence 
{
	MorphLanguageEnum m_Language;
	std::vector<CLemWord> m_Words;


	CInputSentence(MorphLanguageEnum l)
	{
		m_Language = l;
	};

	size_t	ReadSentence (const CLemmatizedText&	PlmLines, size_t StartLineNo);
	void	AddToResultPlmLinesCollection(CLemmatizedText& Result) const;
	size_t	GetOffsetInHomonyms(size_t  StartWordNo, size_t EndWordNo) const;

};

struct CInputSentenceGLR : public CInputSentence
{
	// this slot should be updated during processing the whole text
	//  it contains all words which can refer a named entity
	StringSet				m_AbrigedReferences;

	std::list<CGLRParser>		m_Parsers;
	

	CInputSentenceGLR(MorphLanguageEnum l) :
		CInputSentence(l) {	};

	void	ProcessRestart (const CWorkGrammar& G);
	std::vector<CFoundEntity>	ProcessFull (const CWorkGrammar& G);
	void	DeleteHomonyms (const std::vector< COccurrence>& Occurrences, const CWorkGrammar& WorkGrammar);
	void	AddDescriptors (const std::vector< COccurrence>& Occurrences, const CWorkGrammar& Grammar);
private:
	void	ApplyGLR_Parsing (const CWorkGrammar& G, size_t WordNo, CGLRParser& Parser);
	void	FindOccurrencesWithTrie (const CWorkGrammar& G, std::vector< COccurrence >& Occurrences);
};


size_t CInputSentence::ReadSentence (const CLemmatizedText&	text, size_t StartLineNo)
{
	m_Words.clear();
	for (size_t i=StartLineNo; i < text.m_LemWords.size(); i++)
	{
		m_Words.push_back(text.m_LemWords[i]);
		if (text.m_LemWords[i].HasDes(OSentEnd)) {
			return i + 1;
		}
	};
	return text.m_LemWords.size();

};

size_t CInputSentence::GetOffsetInHomonyms(size_t  StartWordNo, size_t EndWordNo) const
{
	size_t  Result = 0;
	for (size_t j=StartWordNo; j<EndWordNo; j++)
	{
		Result +=  m_Words[j].GetHomonymsCount();
	};
	return StartWordNo+Result;
};

void CInputSentence::AddToResultPlmLinesCollection(CLemmatizedText& text) const
{
	for (auto& w : m_Words) {
		CLemWord word = w;
		text.m_LemWords.push_back(word);
	}
};




//=============================================================
//================== GLR based on SLR(1) ======================
//=============================================================




void CInputSentenceGLR::DeleteHomonyms (const std::vector< COccurrence>& Occurrences, const CWorkGrammar& WorkGrammar)
{

	for (size_t i=0; i<Occurrences.size(); i++)
	{
		const COccurrence& C = Occurrences[i];
		size_t OccurSize = C.second - C.first;
		if (OccurSize == 1) continue;
		
		std::vector<size_t> Leaves;

		C.m_pParser->GetParseNodesRecursiveWithoutSyntaxAmbiguity(C.m_GrammarRuleNo, Leaves, true);
		assert (Leaves.size() == OccurSize);
		if (Leaves.size() != OccurSize)
		{
			ErrorMessage ("GLR", "A general GLR parsing error has occurred. Cannor read parse results!");
			return;
		};

		for (size_t WordNo = C.first; WordNo < C.second; WordNo++)
		{
			const CInputSymbol& Symbol = C.m_pParser->m_SymbolNodes[Leaves[WordNo-C.first]].m_Symbol;
		
			m_Words[WordNo].DeleteHomonymByNotTerminalSymbol(Symbol);
		};
		//C.m_pParser->DumpParser(true);
	};

};

void CInputSentenceGLR::FindOccurrencesWithTrie (const CWorkGrammar& G, std::vector< COccurrence >& Occurrences)
{
	std::set<size_t> CurrentStates;
	CurrentStates.insert(0);
	Occurrences.clear();
	
	for (size_t WordNo=0; WordNo < m_Words.size(); WordNo++)
	{
		CLemWord& W = m_Words[WordNo];

		//printf ("PossibleOutputSymbols: ");
		//for (size_t i=0; i<PossibleOutputSymbols.size(); i++)
		//	if (PossibleOutputSymbols[i])
		//		printf ("%s,", G.m_UniqueGrammarItems[i].GetDumpString().c_str());
		//printf ("\n");

		std::set<size_t> NextStates;

		
		for (std::set<size_t>::const_iterator it = CurrentStates.begin(); it != CurrentStates.end(); it++)
			for (std::set<CInputSymbol>::iterator it1 = W.m_AutomatSymbolInterpetationUnion.begin();it1 != W.m_AutomatSymbolInterpetationUnion.end() ; it1++)
			{
				TerminalSymbolType c = (int)it1->m_GrammarSymbolNo;
				int q = G.m_TrieHolder.NextState(*it, WordNo,c, Occurrences);
				NextStates.insert(q);
			};
		CurrentStates = NextStates;

		if (CurrentStates.empty())
			CurrentStates.insert(0);


		//printf ("Current states : ");
		//for (it = CurrentStates.begin(); it != CurrentStates.end(); it++)
		//	printf ("%i, ",*it);
		//printf ("\n");
		//printf ("Count of Occurrences = %i\n", Occurrences.size());

	};
	
};



void CInputSentenceGLR::AddDescriptors (const std::vector< COccurrence>& Occurrences, const CWorkGrammar& Grammar)
{
	std::string DumpString;
	size_t OccurNo = 0;
	assert (Grammar.GetCountOfRoots() == 3);
	int RootNo = Grammar.GetFirstRoot();
	assert (RootNo != -1);
	for (size_t WordNo=0; WordNo < m_Words.size();WordNo++) 
	{
		if	(		(OccurNo < Occurrences.size())
				&&	(Occurrences[OccurNo].first == WordNo)
			)
		{
			const COccurrence& C = Occurrences[OccurNo];
			size_t StartWordNo = C.first;
			size_t RootNo = C.m_GrammarRuleNo;
			const std::vector<CSymbolNode>& Output = C.m_pParser->m_SymbolNodes;
			std::vector<size_t> Nodes;
			C.m_pParser->GetParseNodesRecursiveWithoutSyntaxAmbiguity(RootNo, Nodes, false);
			
			for (size_t i=0; i<Nodes.size();i++)
			{
				const CSymbolNode&  Node = Output[ Nodes[i] ];
				std::string GroupName = Grammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;
					
				IF_PLOG(plog::debug)
				{ 
					std::string s;
					if (i > 0)
						s += "\t";

					s += Format("%s : ",GroupName.c_str());
					for (size_t j=Node.m_InputStart; j < Node.m_InputEnd; j++)
					{
						s += Format("%s ",m_Words[StartWordNo+j].GetWord().c_str());
					};
					PLOGD << s;
				}

				if (Node.m_InputEnd - Node.m_InputStart <= 1) 
				{
					std::string MainDescr = Format(" #_%s",GroupName.c_str());
					m_Words[StartWordNo+Node.m_InputStart].AddDescriptor(MainDescr);
					std::string WordStr = m_Words[StartWordNo+Node.m_InputStart].GetWord();
					if (Grammar.m_SecondPassSymbols.find(GroupName) != Grammar.m_SecondPassSymbols.end())
						m_AbrigedReferences.insert(WordStr);
					continue;
				};

				
				size_t MainWordNo = C.m_pParser->GetMainWordRecursive(Nodes[i]);
				std::string Descr = Format(" #SimGrm %s %i %i %i",
					GroupName.c_str(), Node.m_InputStart, 
					Node.m_InputEnd-1, MainWordNo);
				

				m_Words[StartWordNo].AddDescriptor(Descr);
			};

			OccurNo++;
			WordNo = C.second - 1;
			

		}
		else
		{
			std::string WordStr = m_Words[WordNo].GetWord();
			if (m_AbrigedReferences.find(WordStr) != m_AbrigedReferences.end())
			{
				IF_PLOG(plog::debug)
				{ 
					std::string GroupName = Grammar.m_UniqueGrammarItems[RootNo].m_ItemStrId;
					PLOGD << GroupName << " : " << WordStr;
				};

			};


		};
	};
};


/*
	This function  builds all parsing trees in the input sentence from the word WordNo.

*/
void CInputSentenceGLR::ApplyGLR_Parsing (const CWorkGrammar& G, size_t WordNo, CGLRParser& Parser)
{
	Parser.InitGLRParser(&G.m_GLRTable); 

	std::set<CInputSymbol> EndOfSentenceSet;
	EndOfSentenceSet.insert(CInputSymbol(G.GetEndOfStreamSymbol(),"", ""));


	for (size_t CurrWordNo = WordNo; CurrWordNo < m_Words.size(); CurrWordNo++)
	{
		const CLemWord& W = m_Words[CurrWordNo];

		//Parser.DumpParser(true);

		if (Parser.ParseSymbol(W.m_AutomatSymbolInterpetationUnion))
		{
			//  if it is the last word of the sentence and the lst symbol was parsed successfully 
			//  then we should add an end of stream symbol to the input stream and finish parsing
			if	(CurrWordNo+1 == m_Words.size())
			{
				Parser.ParseSymbol(EndOfSentenceSet);
				break;
			};
		}
		else
			break;

	};
	
	//Parser.DumpParser(true);
};


void CInputSentenceGLR::ProcessRestart (const CWorkGrammar& G)
{
	// first, we should  find all possible occurrences of prefixes in this sentence
	// since FindOccurrencesWithTrie works at O(n) then we should try to build from each 
	// found occcurrence of a prefix a full occurrence of the grammar root
	std::vector< COccurrence > Prefixes;
	FindOccurrencesWithTrie(G, Prefixes);

	std::set<size_t> StartPoints;
	for (auto& p : Prefixes)
		StartPoints.insert(p.first);


	std::vector< COccurrence > Occurrences;
	
	for (auto& WordNo: StartPoints)
	{
		m_Parsers.push_back(CGLRParser());

		CGLRParser& Parser = m_Parsers.back();

		ApplyGLR_Parsing(G, WordNo, Parser);

		const std::vector<CSymbolNode>& Output = Parser.m_SymbolNodes;
		for (size_t i=0; i<Output.size(); i++)
		{
			int SymbolNo = (int)Output[i].m_Symbol.m_GrammarSymbolNo;
			if	(		G.m_UniqueGrammarItems[SymbolNo].m_bGrammarRoot 
					&&	(Output[i].m_InputStart == 0)
					&&	(SymbolNo != G.GetEndOfStreamSymbol())
				)
			{
				
				COccurrence  C;
				C.first = WordNo+Output[i].m_InputStart;
				C.second = WordNo+Output[i].m_InputEnd;
				C.m_pParser = &(Parser);
				C.m_GrammarRuleNo = i;
				Occurrences.push_back(C);
			};
		};

	};

	

	//  then we should apply longest match algorithm ("SolveAmbiguity")
	std::vector< COccurrence > SavedOccurrences = Occurrences;
	SolveAmbiguity(Occurrences);

	//  delete homonyms which contradict the groups, which were built
	DeleteHomonyms(Occurrences, G);

	//  add information to the output results
	AddDescriptors(Occurrences, G);

	m_Parsers.clear();
};


extern void GetMaxCoverage (std::vector< COccurrence >& Occurrences);
std::vector<CFoundEntity> CInputSentenceGLR::ProcessFull (const CWorkGrammar& G)
{
	
	m_Parsers.push_back(CGLRParser());

	// parsing
	CGLRParser& Parser = m_Parsers.back();
	Parser.m_bRobust = true;
	ApplyGLR_Parsing(G, 0, Parser);
	//Parser.DumpParser(false);
	std::vector< COccurrence > Occurrences;
	const std::vector<CSymbolNode>& Output = Parser.m_SymbolNodes;
	for (size_t i=0; i<Output.size(); i++)
	{
		if	(G.m_UniqueGrammarItems[Output[i].m_Symbol.m_GrammarSymbolNo].m_bGrammarRoot )
		{
			COccurrence  C;
			C.first = Output[i].m_InputStart;
			C.second = Output[i].m_InputEnd;
			C.m_pParser = &(Parser);
			C.m_GrammarRuleNo = i;
			Occurrences.push_back(C);
		};
	};
	
	GetMaxCoverage(Occurrences);

	int RootNo = G.GetFirstRoot();
	std::vector<CFoundEntity> result;
	for (auto& o : Occurrences)
	{
			std::vector<size_t> Nodes;
			o.m_pParser->GetParseNodesRecursiveWithoutSyntaxAmbiguity(o.m_GrammarRuleNo, Nodes, false);
			for (const auto& n: Nodes)
			{
				const CSymbolNode&  Node = o.m_pParser->m_SymbolNodes[ n ];
				std::string GroupName = G.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;		
				result.push_back({ GroupName , Node.m_InputStart, Node.m_InputEnd });
			};
	};

	m_Parsers.clear();
	return result;
};


CLemmatizedText CWorkGrammar::FilterHomonymsByGrammar(const CLemmatizedText& text) const
{
	text.SaveToFile("input.lem");
	CLemmatizedText result(text.GetDictLanguage());

	CInputSentenceGLR Sentence(text.GetDictLanguage());
	
	for (size_t i=0; i < text.m_LemWords.size();)
	{		
		i = Sentence.ReadSentence(text, i);
		for (auto& w : Sentence.m_Words) {
			w.BuildTerminalSymbolsByWord(m_UniqueGrammarItems, GetEndOfStreamSymbol());
		}
		Sentence.ProcessRestart(*this);
		
		Sentence.AddToResultPlmLinesCollection(result);
	};
	return result;
};

std::vector<CFoundEntity> CWorkGrammar::GetFoundOccurrences(const CLemmatizedText& text) const
{
	CInputSentenceGLR Sentence(text.GetDictLanguage());
	Sentence.ReadSentence(text, 0); // read only the first sentence
	for (auto& w : Sentence.m_Words)
		w.BuildTerminalSymbolsByWord(m_UniqueGrammarItems, GetEndOfStreamSymbol());
	return Sentence.ProcessFull(*this);
}



