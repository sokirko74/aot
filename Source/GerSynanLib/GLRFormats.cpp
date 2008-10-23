
#include "GerSynan.h"
#include "GerSentence.h"
#include "GerFormatCaller.h"
#include "GerWord.h"
#include "../SimpleGrammarLib/SimpleGrammar.h"


bool CheckGrammems(const CHomonym& L, const CGrammarItem& I)
{
	//  nor punctuation marks neither abbreviations can match a morphological pattern
	if (!L.m_iPoses)
		return false;
	
	if	((I.m_Grammems & L.m_iGrammems) != I.m_Grammems) 
		return false;

	return (I.m_Poses & L.m_iPoses) > 0; 

};

bool AreEqual(const CHomonym& L, const CWord& W, const CGrammarItem& I)
{
	if (!I.m_GrmAttribute.empty())
	{
		if (!CheckGrammems(L,I))
			return false;
		if (I.m_Type == siMorphPattern)
			return true;
	};

	switch (I.m_Type) {
		case siGraph:
						return		I.m_Lemma.empty() // [GRAPH register="AA"];
								||	(L.m_strLemma == I.m_Lemma); 

		case siFileList :	
				{
					assert (I.m_pListFile != NULL);
					const StringSet& PossibleLemmas = I.m_pListFile->m_PossibleLemmas;
					return  PossibleLemmas.find(L.m_strLemma) != PossibleLemmas.end();
				}

		case siRomanNumber :	
					assert (false); // we do not  use in formats grammar
					return		false;

		case siAnyWord:	
					return		W.m_bWord;
					

		case siNumber :	
					return		W.m_bDigit;

		case siMorphologicalyUnknown :
					return	W.m_bPredicted;
					
	};
	
	return false;
};



// we should try to call this procedure only on the first stage, 
// not each time when it is needed
void CGerSentence::BuildTerminalSymbolsByWord(CWord& W)							
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	const vector<CGrammarItem>&	TerminalSymbols = G.m_UniqueGrammarItems;

	// adding an end of stream symbol to each word
	assert (!W.m_Homonyms.empty());
	W.m_AutomatSymbolInterpetationUnion.clear();
	W.m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(G.GetEndOfStreamSymbol(), ""));

	for (size_t j=0; j<W.m_Homonyms.size(); j++)
		W.m_Homonyms[j].m_AutomatSymbolInterpetation.clear();


	for (size_t i=0; i<TerminalSymbols.size(); i++)
	{
		const CGrammarItem& I = TerminalSymbols[i];

		if (I.m_Type == siMeta) continue;

		if (!I.m_bCanHaveManyHomonyms && (W.m_Homonyms.size() > 1)) continue;

		if (I.m_Register != AnyRegister)
		{
				if (I.m_Register != W.m_Register)
					continue;
		};
				

		for (size_t j=0; j<W.m_Homonyms.size(); j++)
			if (AreEqual(W.m_Homonyms[j], W, I))
			{
				CInputSymbol N(i, W.m_Homonyms[j].m_strGramcodes);
				W.m_Homonyms[j].m_AutomatSymbolInterpetation.insert(N);
				W.m_AutomatSymbolInterpetationUnion.insert(N);
			};
	}

};

// we should try to call this procedure only on the first stage, 
// not each time when it is needed
void CGerSentence::BuildTerminalSymbolsByClause(size_t ClauseNo)							
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	const vector<CGrammarItem>&	TerminalSymbols = G.m_UniqueGrammarItems;

	CClause& Clause = m_Clauses[ClauseNo];

	// adding an end of stream symbol to each word
	Clause.m_AutomatSymbolInterpetationUnion.clear();
	Clause.m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(G.GetEndOfStreamSymbol(), ""));
	for (size_t iType=0; iType<Clause.m_vectorTypes.size(); iType++)
		Clause.m_vectorTypes[iType].m_AutomatSymbolInterpetation.clear();

	for (size_t i=0; i<TerminalSymbols.size(); i++)
	{
		const CGrammarItem& I = TerminalSymbols[i];

		if (I.m_Type == siMeta) continue;

		
		for (size_t iType=0; iType<Clause.m_vectorTypes.size(); iType++)
		{
			SClauseType& type = Clause.m_vectorTypes[iType];
			if( type.m_Root.IsEmpty() )
				continue;
			CWord& W = m_Words[type.m_Root.m_WordNo];
			CHomonym& H = W.m_Homonyms[type.m_Root.m_HomonymNo];
			if (AreEqual(H, W, I))
			{
				CInputSymbol N(i, H.m_strGramcodes);
				type.m_AutomatSymbolInterpetation.insert(N);
				Clause.m_AutomatSymbolInterpetationUnion.insert(N);
			};
		};
	}

};


void CGerSentence::ApplyGLR_Parsing (CGLRParser& Parser, const vector<CBuildingUnit>& BuildingUnits)
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	Parser.m_bRobust = true;
	Parser.InitGLRParser(&G.m_GLRTable); 

	set<CInputSymbol> EndOfSentenceSet;

	EndOfSentenceSet.insert(CInputSymbol(G.GetEndOfStreamSymbol(),""));

	for (size_t CurrWordNo = 0; CurrWordNo < BuildingUnits.size(); CurrWordNo++)
	{
		const CBuildingUnit& Unit = BuildingUnits[CurrWordNo];
		const set<CInputSymbol>& Symbols = (Unit.m_ChildClauseNo == -1) ? 
					m_Words[Unit.m_WordNo].m_AutomatSymbolInterpetationUnion 
				:	m_Clauses[Unit.m_ChildClauseNo].m_AutomatSymbolInterpetationUnion;

		//Parser.DumpParser(true);


		if (Parser.ParseSymbol(Symbols))
		{
			//  if it is the last word of the sentence and the lst symbol was parsed successfully 
			//  then we should add an end of stream symbol to the input stream and finish parsing
			if	(CurrWordNo+1 == BuildingUnits.size())
			{
				Parser.ParseSymbol(EndOfSentenceSet);
				break;
			};
		}
		else
			break;

	};


	// Parser.DumpParser(true);
	
};

extern void GetMaxCoverage (vector< COccurrence >& Occurrences);
void CGerSentence::GetBestChunks(CGLRParser& Parser, vector< COccurrence>& Occurrences) const
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	Occurrences.clear();
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

			if (C.second - C.first > 1) // ignore atomic groups
				Occurrences.push_back(C);

		};
	};
	//Parser.DumpParser(true);

	GetMaxCoverage(Occurrences);
};


// converting GLR symbol node to a syntax group
CGroup CGerSentence::ConvertFromTomitaNodeToCGroup(const CGLRParser& Parser, CClause& pClause, int SymbolNo ) const
{
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;
	const CSymbolNode& Node = Parser.m_SymbolNodes[SymbolNo];

	CGroup G;
	G.m_iFirstWord = Node.m_InputStart;
	G.m_iLastWord  = Node.m_InputEnd-1;
	string Symbol = Grammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;
	G.m_GroupType = m_pSyntaxOptions->GetSyntaxGroupOrRegister(Symbol.c_str());
	G.SetGrammems(GetGerGramTab()->GetAllGrammems(Node.m_Symbol.m_GramCodes.c_str()));
	assert (!Node.m_ParseChildren.empty());

	// determining the main word  and the main group
	// we take into account only the first children set
	const CParsingChildrenSet&  Set = Node.m_ParseChildren[0];
	const CGLRRuleInfo* pRule = Set.m_ReduceRule; 
	size_t MainChildNo = Set.m_Items[pRule->m_SynMainItemNo];
	G.m_MainGroup.m_iFirstWord = Parser.m_SymbolNodes[MainChildNo].m_InputStart;
	G.m_MainGroup.m_iLastWord = Parser.m_SymbolNodes[MainChildNo].m_InputEnd - 1;

	return G;
};



bool CGerSentence::BuildMorphVariantsByTomita(const CGLRParser& Parser, const vector< COccurrence>& Occurrences, CClause& pClause, vector<CBuildingUnit>& BuildingUnits )
{
	//m_bShowAllGroups = true;
	pClause.m_SynVariants.clear();


	// prohibit all homonyms
	for (size_t i=0; i<BuildingUnits.size(); i++)
		BuildingUnits[i].m_BestHomonyms = 0;

	// permit all homonyms, which are in the best coverages
	
	vector<set<int> > UsedOccurrences(BuildingUnits.size());
	vector< CGroups > GroupsList;
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;

	for (size_t OccurNo=0; OccurNo<Occurrences.size(); OccurNo++)
	{
		const COccurrence& C = Occurrences[OccurNo];
		GroupsList.push_back(CGroups(GetOpt()));
		
		{	// get all chunks
			CGroups& Groups = GroupsList.back();
			Groups.ResizeAtomicDummy(BuildingUnits.size());

			vector<size_t> ParseTree;
			Parser.GetParseNodesRecursive(C.m_GrammarRuleNo, ParseTree, false);
			
			for (int j = ParseTree.size()-1; j >=0; j--)
			{
				int SymbolNo = ParseTree[j];
				const CSymbolNode& Node = Parser.m_SymbolNodes[SymbolNo];
				if (!m_bShowAllGroups)
					if (Node.m_InputStart+1 == Node.m_InputEnd) continue;

				if (Grammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_bGrammarRoot) continue;

				
				CGroup G = ConvertFromTomitaNodeToCGroup(Parser, pClause, SymbolNo);

				if	(		m_bShowAllGroups
						||	(Groups.find_group(G) == -1)
					)

				{
					Groups.SetMainWordNo(G);
					size_t  GroupNo = Groups.create_group(G);
					Groups.BuildDefaultSynrels(GroupNo);
					

				};
			};

		}


		{	// get all leaves

			vector<size_t> Leaves;
			Parser.GetParseNodesRecursive(C.m_GrammarRuleNo, Leaves, true);
			for (size_t j = 0; j < Leaves.size(); j++)
			{
				const CSymbolNode& Node = C.m_pParser->m_SymbolNodes[Leaves[j]];
				assert(Node.m_InputStart+1 == Node.m_InputEnd);
				CInputSymbol Symbol = Node.m_Symbol;
				CBuildingUnit& Unit = BuildingUnits[Node.m_InputStart];
				if (Unit.m_ChildClauseNo == -1) 
				{
					const CWord& W = m_Words[Unit.m_WordNo];
					for (int i=0; i< W.m_Homonyms.size(); i++)
						if (W.m_Homonyms[i].m_AutomatSymbolInterpetation.find(Symbol) !=  W.m_Homonyms[i].m_AutomatSymbolInterpetation.end())
						{
							Unit.m_BestHomonyms |= _QM(i);
							UsedOccurrences[Node.m_InputStart].insert(OccurNo);
						};
				}
				else
				{
					const CClause& C = m_Clauses[Unit.m_ChildClauseNo];
					for (int i=0; i< C.m_vectorTypes.size(); i++)
						if (C.m_vectorTypes[i].m_AutomatSymbolInterpetation.find(Symbol) !=  C.m_vectorTypes[i].m_AutomatSymbolInterpetation.end())
						{
							Unit.m_BestHomonyms |= _QM(i);
							UsedOccurrences[Node.m_InputStart].insert(OccurNo);
						};
				};
			};
		}

	};

	assert (GroupsList.size()  == Occurrences.size());


	// permit all homonyms, if there is no chunk above them 
	for (size_t i=0; i<BuildingUnits.size(); i++)
		if (!BuildingUnits[i].m_BestHomonyms)
			BuildingUnits[i].m_BestHomonyms = GetMaxQWORD();


	pClause.BuildSynVariantsRecursive(BuildingUnits.begin(),  CMorphVariant());


	CGroups DummyGroups(GetOpt());
	DummyGroups.ResizeAtomicDummy(BuildingUnits.size());

	for(SVI  pVar  = pClause.m_SynVariants.begin() ; pVar != pClause.m_SynVariants.end(); pVar++ )
	{
		CMorphVariant& synVariant = *pVar;
		synVariant.ChooseClauseType(pClause.m_vectorTypes);
		assert (synVariant.m_SynUnits.size() == BuildingUnits.size());
		for (size_t UnitNo=0; UnitNo<BuildingUnits.size();UnitNo++)
		{
			const set<int>& Occur = UsedOccurrences[UnitNo];

			for (set<int>::const_iterator it = Occur.begin();  it != Occur.end(); it++)
			{
				size_t OccurNo = *it;
				const COccurrence& C = Occurrences[OccurNo];
				bool bFound = true;
				if (UnitNo == C.first)
				{
					for (size_t WordNo = C.first+1; WordNo < C.second; WordNo++)
						if (UsedOccurrences[WordNo].find(OccurNo) == UsedOccurrences[WordNo].end())
						{
							bFound = false;
							break;
						};
				}
				else 
						bFound = false;

				if (bFound)
				{
					//  adding groups checking projectivity
					const vector<CGroup> G =  GroupsList[OccurNo].GetGroups();
					
					for (size_t j=0; j< G.size(); j++)
					{
						if (		synVariant.m_vectorGroups.GetGroups().empty()
								||	GladkijPeriodLess(synVariant.m_vectorGroups.GetGroups().back(), G[j])
							)
							synVariant.m_vectorGroups.create_group(G[j]);
					};
					UnitNo =  C.second-1;
					break;
				};
			};
		}

		//sort(synVariant.m_vectorGroups.begin(), synVariant.m_vectorGroups.end(), GladkijPeriodLess);

		// we did not copy grammems  from the parse tree, 
		//  to be implemented... 
		pClause.AssignVariantWeight(synVariant);

		//  translating group coordinates to sentence coordinates 
		for (size_t GroupNo=0; GroupNo< synVariant.m_vectorGroups.GetGroups().size(); GroupNo++)
			pClause.TranslateCoordSynVarForGroup(*synVariant.m_vectorGroups.GetGroupPtr(GroupNo));


	};




	return true;
}

bool CGerSentence::BuildGroups(CClause& pClause)
{
	vector<CBuildingUnit> BuildingUnits;
	pClause.GetBuildingUnits(BuildingUnits);

	for (size_t i=0; i < BuildingUnits.size(); i++)
	{
		const CBuildingUnit & Unit = BuildingUnits[i];
		if (Unit.m_ChildClauseNo == -1) 
			BuildTerminalSymbolsByWord(m_Words[Unit.m_WordNo]);
		else
		{
			BuildTerminalSymbolsByClause(Unit.m_ChildClauseNo);
		};
	};

	
	CGLRParser Parser;
	ApplyGLR_Parsing(Parser, BuildingUnits);

	vector< COccurrence> Occurrences;
	GetBestChunks(Parser, Occurrences);
	
	BuildMorphVariantsByTomita(Parser, Occurrences, pClause, BuildingUnits);

	return true;
};




