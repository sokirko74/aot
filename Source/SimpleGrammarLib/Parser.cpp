#include  "SimpleGrammarLib.h"
#include "SimpleGrammar.h"
#include "../AgramtabLib/GerGramTab.h"


const size_t MaxAutomatSymbolInterpetation = 10;

struct CInputHomonym : public CPlmLine
{
	//  all possible grammar symbols which can be interpetation of this Homonym in the grammar
	//  based upon Homonyme informatin and previous context
	CSmallVector<CInputSymbol,MaxAutomatSymbolInterpetation> m_AutomatSymbolInterpetation;

};


struct CInputWord 
{
	size_t					m_StartPlmLineNo;
	vector<CInputHomonym>	m_Homonyms;
	set<CInputSymbol>		m_AutomatSymbolInterpetationUnion;

	CInputWord (size_t StartPlmLineNo)
	{
		m_StartPlmLineNo = StartPlmLineNo;
	};

	void UpdateAfterDelete()
	{
		for (int i=0; i < m_Homonyms.size(); i++)
			m_Homonyms[i].m_bHomonym = (i != 0);

	};

	void DeleteHomonymByNotTerminalSymbol(const CInputSymbol& S)
	{
		for (int i=(int)m_Homonyms.size()-1; i >=0; i--)
		{
			size_t j=0;
			for (; j < m_Homonyms[i].m_AutomatSymbolInterpetation.size(); j++)
				if (m_Homonyms[i].m_AutomatSymbolInterpetation[j].m_GrammarSymbolNo == (int)S.m_GrammarSymbolNo )
					break;

			if ( j == m_Homonyms[i].m_AutomatSymbolInterpetation.size())
				m_Homonyms.erase(m_Homonyms.begin() + i);
		};

		UpdateAfterDelete();
	};

	void AddDescriptor(const string& Descriptor)
	{
		for (size_t j=0; j<m_Homonyms.size(); j++)
		{
			m_Homonyms[j].m_GraphDescr += " " + Descriptor;
		};
	};
	string GetWord() const
	{
		assert (!m_Homonyms.empty());
		return m_Homonyms[0].GetWord();
	};
};

struct CInputSentence 
{
	const CAgramtab*			m_pGramTab;
	vector<CInputWord>			m_Words;


	CInputSentence(const CAgramtab*	pGramTab) : m_pGramTab(pGramTab)
	{
	};

	size_t	ReadSentence (const CPlmLineCollection&	PlmLines, size_t StartLineNo);
	void	AddToResultPlmLinesCollection(CPlmLineCollection& Result) const;
	size_t	GetOffsetInHomonyms(size_t  StartWordNo, size_t EndWordNo) const;

};


struct CInputSentenceGLR : public CInputSentence
{
	// this slot should be updated during processing the whole text
	//  it contains all words which can refer a named entity
	StringSet				m_AbrigedReferences;

	list<CGLRParser>		m_Parsers;
	


	CInputSentenceGLR(const CAgramtab*	pGramTab):
		CInputSentence(pGramTab)	{	};

	void	ProcessRestart (const CWorkGrammar& G, bool bDumpOccurs);
	void	ProcessFull (const CWorkGrammar& G);
	void	DeleteHomonyms (const vector< COccurrence>& Occurrences, const CWorkGrammar& WorkGrammar);
	void	AddDescriptors (const vector< COccurrence>& Occurrences, const CWorkGrammar& Grammar, bool bDumpOccurs);
	void	DumpOccurrences (const CWorkGrammar& G, const vector< COccurrence>& Occurrences) const;
	void	BuildTerminalSymbolsByWord(const CWorkGrammar& G, CInputWord& W);
private:
	void	ApplyGLR_Parsing (const CWorkGrammar& G, size_t WordNo, CGLRParser& Parser);
	void	FindOccurrencesWithTrie (const CWorkGrammar& G, vector< COccurrence >& Occurrences);
};


bool CheckGrammems(const CPlmLine& L, const CGrammarItem& I)
{
	if (I.m_MorphPattern.m_SearchStatus != AnyStatus)
	{
		if ((I.m_MorphPattern.m_SearchStatus == FoundInDictionary) != L.IsFoundInMorphology())
			return false;
		
	};

	//  nor punctuation marks neither abbreviations can match a morphological pattern
	if (L.m_Pos == UnknownPartOfSpeech)
		return false;
	
	if	(		((I.m_MorphPattern.m_Grammems & L.m_Grammems) != I.m_MorphPattern.m_Grammems) 
			&&	!(		(I.m_MorphPattern.m_Grammems== _QM(gGenitiv))
					&&	!L.IsFoundInMorphology()
					&&	!L.GetUpperWord().empty()
					&&	(L.GetUpperWord()[L.GetUpperWord().length() -1 ] == 'S')
				)
		)
		return false;

	return		(I.m_MorphPattern.m_Poses == 0)
			||	(I.m_MorphPattern.m_Poses &  (1<<L.m_Pos)) > 0; 

};

bool AreEqual(const CPlmLine& L, const CGrammarItem& I)
{
	if (I.m_TokenType != L.m_TokenType)
		return false;

	if (!I.m_MorphPattern.m_GrmAttribute.empty())
		if (!CheckGrammems(L,I))
			return false;


	if (!I.m_Token.empty())
		if (L.m_Lemma != "-1" && !L.m_Lemma.empty())
		{
			if (L.m_Lemma != I.m_Token)
				return false;
		}
		else
		{
			if  (L.GetUpperWord() != I.m_Token)
				return false;
		};



	
	if (I.m_pListFile != 0)
	{
		const StringSet& PossibleLemmas = I.m_pListFile->m_PossibleLemmas;
		string debug = *(PossibleLemmas.begin());
		if	(		( PossibleLemmas.find(L.m_Lemma) == PossibleLemmas.end() ) // if we cannot find lemma
				&&	(		( L.m_Lemma == L.GetUpperWord()) // and if we cannot find the token itself
						||	L.IsFoundInMorphology()
						||	PossibleLemmas.find(L.GetUpperWord()) == PossibleLemmas.end() 
					)
			)
		{
				size_t hyphen = L.m_Lemma.rfind("-"); // and if we cannot find the token substring  after the last hyphen
				if	(		(hyphen == string::npos)
						||	( PossibleLemmas.find(L.m_Lemma.substr(hyphen+1)) == PossibleLemmas.end() )
					)
					return false;
		};
	};


	return true;
};


size_t CInputSentence::ReadSentence ( const CPlmLineCollection&	PlmLines, size_t StartLineNo)
{
	size_t EndLineNo = PlmLines.m_Items.size();
	assert (StartLineNo < EndLineNo);
	m_Words.clear();

	for (size_t i=StartLineNo; i<EndLineNo ; i++)
	{
		CInputHomonym Homonym;
		if (!Homonym.LoadPlmLineFromString(PlmLines.m_Items[i].c_str(), i==0, m_pGramTab))
		{
			ErrorMessage ("Cannot parse "+ PlmLines.m_Items[i]);
			return EndLineNo;
		};
		if (Homonym.m_TokenType == OTHER_TOKEN_TYPE) continue;

	
		if (!Homonym.m_bHomonym)
		{
			if (!m_Words.empty())
				if (m_Words.back().m_Homonyms.back().m_bSent2)
					return i;			
			m_Words.push_back(CInputWord(i));
		};

		m_Words.back().m_Homonyms.push_back(Homonym);			
	};



	return EndLineNo;

};




size_t CInputSentence::GetOffsetInHomonyms(size_t  StartWordNo, size_t EndWordNo) const
{
	size_t  Result = 0;
	for (size_t j=StartWordNo; j<EndWordNo; j++)
	{
		Result +=  m_Words[j].m_Homonyms.size();
	};
	return StartWordNo+Result;
};

void CInputSentence::AddToResultPlmLinesCollection(CPlmLineCollection& Result) const
{
	for (size_t i=0; i<m_Words.size(); i++)
		for (size_t j=0; j < m_Words[i].m_Homonyms.size(); j++)
		{
			const CPlmLine& L = m_Words[i].m_Homonyms[j];
			Result.m_Items.push_back(L.GetStr());
		};

};




//=============================================================
//================== GLR based on SLR(1) ======================
//=============================================================

void CInputSentenceGLR::BuildTerminalSymbolsByWord(const CWorkGrammar& G, CInputWord& W)							
{
	const vector<CGrammarItem>&	TerminalSymbols = G.m_UniqueGrammarItems;

	// adding an end of stream symbol to each word
	assert (!W.m_Homonyms.empty());
	W.m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(G.GetEndOfStreamSymbol(),"", ""));

	for (size_t i=0; i<TerminalSymbols.size(); i++)
	{
		const CGrammarItem& I = TerminalSymbols[i];

		if (I.m_bMeta) continue;

		if (!I.m_bCanHaveManyHomonyms && (W.m_Homonyms.size() > 1)) continue;

		if (I.m_Register != AnyRegister)
		{
				if (I.m_Register != W.m_Homonyms[0].m_Register)
					continue;
		};
				

		for (size_t j=0; j<W.m_Homonyms.size(); j++)
			if (AreEqual(W.m_Homonyms[j], I))
			{
				CInputSymbol N (i,W.m_Homonyms[j].GetGramCodes(), W.m_Homonyms[j].GetCommonGramCode());
				W.m_Homonyms[j].m_AutomatSymbolInterpetation.Add(N);
				W.m_AutomatSymbolInterpetationUnion.insert(N);
			};
	}

};



void CInputSentenceGLR::DeleteHomonyms (const vector< COccurrence>& Occurrences, const CWorkGrammar& WorkGrammar)
{

	for (size_t i=0; i<Occurrences.size(); i++)
	{
		const COccurrence& C = Occurrences[i];
		size_t OccurSize = C.second - C.first;
		if (OccurSize == 1) continue;
		
		vector<size_t> Leaves;

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

void CInputSentenceGLR::FindOccurrencesWithTrie (const CWorkGrammar& G, vector< COccurrence >& Occurrences)
{
	set<size_t> CurrentStates;
	CurrentStates.insert(0);
	Occurrences.clear();
	
	for (size_t WordNo=0; WordNo < m_Words.size(); WordNo++)
	{
		CInputWord& W = m_Words[WordNo];

		//printf ("PossibleOutputSymbols: ");
		//for (size_t i=0; i<PossibleOutputSymbols.size(); i++)
		//	if (PossibleOutputSymbols[i])
		//		printf ("%s,", G.m_UniqueGrammarItems[i].GetDumpString().c_str());
		//printf ("\n");

		set<size_t> NextStates;

		
		for (set<size_t>::const_iterator it = CurrentStates.begin(); it != CurrentStates.end(); it++)
			for (set<CInputSymbol>::iterator it1 = W.m_AutomatSymbolInterpetationUnion.begin();it1 != W.m_AutomatSymbolInterpetationUnion.end() ; it1++)
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



void CInputSentenceGLR::AddDescriptors (const vector< COccurrence>& Occurrences, const CWorkGrammar& Grammar, bool bDumpOccurs)
{
	string DumpString;
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
			const vector<CSymbolNode>& Output = C.m_pParser->m_SymbolNodes;
			vector<size_t> Nodes;
			C.m_pParser->GetParseNodesRecursiveWithoutSyntaxAmbiguity(RootNo, Nodes, false);
			
			for (size_t i=0; i<Nodes.size();i++)
			{
				const CSymbolNode&  Node = Output[ Nodes[i] ];
				string GroupName = Grammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;
					
				if ( bDumpOccurs )
				{ 
					// dumping 
					if (i > 0)
						DumpString+= "\t";

					DumpString += Format("%s : ",GroupName.c_str());
					for (size_t j=Node.m_InputStart; j < Node.m_InputEnd; j++)
					{
						DumpString += Format("%s ",m_Words[StartWordNo+j].GetWord().c_str());
					};
					DumpString += "\n";
				}

				if (Node.m_InputEnd - Node.m_InputStart <= 1) 
				{
					string MainDescr = Format(" #_%s",GroupName.c_str());
					m_Words[StartWordNo+Node.m_InputStart].AddDescriptor(MainDescr);
					string WordStr = m_Words[StartWordNo+Node.m_InputStart].GetWord();
					if (Grammar.m_SecondPassSymbols.find(GroupName) != Grammar.m_SecondPassSymbols.end())
						m_AbrigedReferences.insert(WordStr);
					continue;
				};

				
				size_t MainWordNo = C.m_pParser->GetMainWordRecursive(Nodes[i]);
				string Descr = Format(" #SimGrm %s %i %i %i",
					GroupName.c_str(), Node.m_InputStart, 
					Node.m_InputEnd-1, MainWordNo);
				

				m_Words[StartWordNo].AddDescriptor(Descr);
			};

			OccurNo++;
			WordNo = C.second - 1;
			

		}
		else
		{
			string WordStr = m_Words[WordNo].GetWord();
			if (m_AbrigedReferences.find(WordStr) != m_AbrigedReferences.end())
			{
				if ( bDumpOccurs )
				{ 
					string GroupName = Grammar.m_UniqueGrammarItems[RootNo].m_ItemStrId;
					DumpString += Format("%s : %s\n",GroupName.c_str(), WordStr.c_str());
				};

			};


		};
	};

	if (bDumpOccurs)
	{
		FILE* dump_fp = fopen ("occurrences.txt","a");
		if (!dump_fp) 
		{
			fprintf (stderr, "cannot dump occurrences!");
			return;
		};

		fprintf(dump_fp, "%s",DumpString.c_str());
		fclose(dump_fp);
	};

};


/*
	This function  builds all parsing trees in the input sentence from the word WordNo.

*/
void CInputSentenceGLR::ApplyGLR_Parsing (const CWorkGrammar& G, size_t WordNo, CGLRParser& Parser)
{
	Parser.InitGLRParser(&G.m_GLRTable); 

	set<CInputSymbol> EndOfSentenceSet;
	EndOfSentenceSet.insert(CInputSymbol(G.GetEndOfStreamSymbol(),"", ""));


	for (size_t CurrWordNo = WordNo; CurrWordNo < m_Words.size(); CurrWordNo++)
	{
		const CInputWord& W = m_Words[CurrWordNo];

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


void CInputSentenceGLR::ProcessRestart (const CWorkGrammar& G, bool bDumpOccurs)
{
	// first, we should  find all possible occurrences of prefixes in this sentence
	// since FindOccurrencesWithTrie works at O(n) then we should try to build from each 
	// found occcurrence of a prefix a full occurrence of the grammar root
	vector< COccurrence > Prefixes;
	FindOccurrencesWithTrie(G, Prefixes);

	set<size_t> StartPoints;
	for (size_t OccurNo =0; OccurNo < Prefixes.size(); OccurNo++)
		StartPoints.insert(Prefixes[OccurNo].first);


	vector< COccurrence > Occurrences;
	
	for (set<size_t>::const_iterator it = StartPoints.begin(); it != StartPoints.end(); it++)
	{
		size_t WordNo = *it;
		m_Parsers.push_back(CGLRParser());

		CGLRParser& Parser = m_Parsers.back();

		ApplyGLR_Parsing(G, WordNo, Parser);

		//Parser.DumpParser(true);


		const vector<CSymbolNode>& Output = Parser.m_SymbolNodes;
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
	vector< COccurrence > SavedOccurrences = Occurrences;
	SolveAmbiguity(Occurrences);

	//  delete homonyms which contradict the groups, which were built
	DeleteHomonyms(Occurrences, G);

	//  add information to the output results
	AddDescriptors(Occurrences, G, bDumpOccurs);

	m_Parsers.clear();
};


extern void GetMaxCoverage (vector< COccurrence >& Occurrences);
void CInputSentenceGLR::ProcessFull (const CWorkGrammar& G)
{
	
	m_Parsers.push_back(CGLRParser());

	// parsing
	CGLRParser& Parser = m_Parsers.back();
	Parser.m_bRobust = true;
	ApplyGLR_Parsing(G, 0, Parser);
	//Parser.DumpParser(false);


	// get occurrences
	vector< COccurrence > Occurrences;
	const vector<CSymbolNode>& Output = Parser.m_SymbolNodes;
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

	// dump
	FILE * fp = fopen ("test.dump","w");
	int RootNo = G.GetFirstRoot();
	for (size_t OccurNo=0; OccurNo < Occurrences.size(); OccurNo++)
	{
			const COccurrence& C = Occurrences[OccurNo];
			
			
			vector<size_t> Nodes;
			C.m_pParser->GetParseNodesRecursiveWithoutSyntaxAmbiguity(C.m_GrammarRuleNo, Nodes, false);
			for (size_t i=0; i< Nodes.size();i++)
			{
				const CSymbolNode&  Node = C.m_pParser->m_SymbolNodes[ Nodes[i] ];
				string GroupName = G.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;		
				fprintf (fp, "%s[%i,%i); ", GroupName.c_str(), Node.m_InputStart, Node.m_InputEnd);
			};
			fprintf (fp, "\n");
	};
	fclose (fp);

	m_Parsers.clear();
};






bool CWorkGrammar::ParseFile(ParseMethodEnum ParseMethod, const CPlmLineCollection& PlmLines, const CAgramtab*	pGramTab, CPlmLineCollection& Result, bool bDumpOccurrences) const
{
	//printf ("Parsing  file.... \n");
	remove ("occurrs.txt");
	if (PlmLines.m_Items.empty()) return false;
	PlmLines.SaveToFile("input.lem");
	size_t Count = PlmLines.m_Items.size();
	Result.m_Items.clear();	
	Result.m_Items.push_back(PlmLines.m_Items[0]);

	CInputSentenceGLR Sentence(pGramTab);
	
	for (size_t i=0; i<Count ;)
	{		
		//printf ("%i/%i\r", i, Count);

		i = Sentence.ReadSentence(PlmLines, i);

		for (size_t WordNo=0; WordNo < Sentence.m_Words.size(); WordNo++)
			Sentence.BuildTerminalSymbolsByWord(*this, Sentence.m_Words[WordNo]);


		if (ParseMethod == GLRRestartParsing)
		{
			Sentence.ProcessRestart(*this, bDumpOccurrences);
			Sentence.AddToResultPlmLinesCollection(Result);
		}
		else
		{
			Sentence.ProcessFull(*this);
		};
		
	};


	Result.SaveToFile("result.lem");
	//printf ("Parsing  was finished\n");
	return true;
};


