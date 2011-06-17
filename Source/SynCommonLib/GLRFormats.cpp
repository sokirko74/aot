

#include "stdafx.h"



#include "SyntaxInit.h"
#include "Sentence.h"
#include "../SimpleGrammarLib/SimpleGrammar.h"



// we should try to call this procedure only on the first stage, 
// not each time when it is needed
void CSentence::BuildTerminalSymbolsByClause(size_t ClauseNo)							
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	CClause& Clause = m_Clauses[ClauseNo];

	Clause.m_AutomatSymbolInterpetationUnion.clear();

	Clause.m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(G.GetEndOfStreamSymbol(), "", ""));

	for (size_t iType=0; iType<Clause.m_vectorTypes.size(); iType++)
	{
		SClauseType& type = Clause.m_vectorTypes[iType];
		if( type.m_Root.IsEmpty() )
				continue;
		const CSynWord& W = m_Words[type.m_Root.m_WordNo];
		const CSynHomonym& H = W.m_Homonyms[type.m_Root.m_HomonymNo];
		Clause.m_vectorTypes[iType].m_AutomatSymbolInterpetation.clear();
		for (set<CInputSymbol>::const_iterator it = H.m_AutomatSymbolInterpetation.begin(); it != H.m_AutomatSymbolInterpetation.end(); it++)
		{
			CInputSymbol I = *it;
			I.m_bClause = true;
            I.m_ClauseType = type.m_Type;
            I.m_bRelativeClause = Clause.IsRelative();
			Clause.m_vectorTypes[iType].m_AutomatSymbolInterpetation.insert(I);
		};
		Clause.m_AutomatSymbolInterpetationUnion.insert(Clause.m_vectorTypes[iType].m_AutomatSymbolInterpetation.begin(), Clause.m_vectorTypes[iType].m_AutomatSymbolInterpetation.end() );
	};
};


void CSentence::ApplyGLR_Parsing (CGLRParser& Parser, const vector<CBuildingUnit>& BuildingUnits)
{
	const CWorkGrammar& G = GetOpt()->m_FormatsGrammar;
	Parser.m_bRobust = true;
	Parser.InitGLRParser(&G.m_GLRTable); 


	for (size_t CurrWordNo = 0; CurrWordNo < BuildingUnits.size(); CurrWordNo++)
	{
		const CBuildingUnit& Unit = BuildingUnits[CurrWordNo];

		const set<CInputSymbol>& Symbols = (Unit.m_ChildClauseNo == -1) ? 
						m_Words[Unit.m_WordNo].m_AutomatSymbolInterpetationUnion 
					:	m_Clauses[Unit.m_ChildClauseNo].m_AutomatSymbolInterpetationUnion;

		// assert( !Symbols.empty() );
		/*for (set<CInputSymbol>::const_iterator it = Symbols->begin(); it != Symbols->end(); it++)
		{
			const CInputSymbol& debug = *it;
			int u = 0;

		};*/

		//Parser.DumpParser(true);


		if (Parser.ParseSymbol(Symbols))
		{
			//  if it is the last word of the sentence and the lst symbol was parsed successfully 
			//  then we should add an end of stream symbol to the input stream and finish parsing
			if	(CurrWordNo+1 == BuildingUnits.size())
			{
				set<CInputSymbol> EndOfSentenceSet;
				EndOfSentenceSet.insert(CInputSymbol(G.GetEndOfStreamSymbol(),"", ""));
				Parser.ParseSymbol(EndOfSentenceSet);
				break;
			};
		}
		else
			break;

	};


	//Parser.DumpParser(true);

};

bool IsGreaterBySize(const COccurrence& _X1, const COccurrence& _X2 )
{
	return   _X1.second - _X1.first > _X2.second - _X2.first;
};

extern void GetMaxCoverage (vector< COccurrence >& Occurrences);
void CSentence::GetBestChunks(CGLRParser& Parser, vector< COccurrence>& Occurrences) const
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

	sort (Occurrences.begin(), Occurrences.end(), IsGreaterBySize);
};


// converting GLR symbol node to a syntax group
CGroup CSentence::ConvertFromTomitaNodeToCGroup(const CGLRParser& Parser, const CClause& pClause, int SymbolNo ) const
{
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;
	const CSymbolNode& Node = Parser.m_SymbolNodes[SymbolNo];

	CGroup G;
	G.m_iFirstWord = Node.m_InputStart;
	G.m_iLastWord  = Node.m_InputEnd-1;
	string Symbol = Grammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;
	G.m_GroupType = m_pSyntaxOptions->GetSyntaxGroupOrRegister(Symbol.c_str());
	G.SetGrammems(GetOpt()->GetGramTab()->GetAllGrammems(Node.m_Symbol.m_GramCodes.c_str()));
	G.m_GramCodes = Node.m_Symbol.m_GramCodes;
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


void CSentence::BuildGroupByGLR(const CGLRParser& Parser, const COccurrence& C, const CClause& pClause, CGroups& Groups ) const
{
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;

	vector<size_t> ParseTree;
	Parser.GetParseNodesRecursiveWithoutSyntaxAmbiguity(C.m_GrammarRuleNo, ParseTree, false);
    //Parser.DumpParser(false);
	
	for (int j = ParseTree.size()-1; j >=0; j--)
	{
		int SymbolNo = ParseTree[j];
		const CSymbolNode& Node = Parser.m_SymbolNodes[SymbolNo];
        //{
        //    string SymbolStr = GetOpt()->m_FormatsGrammar.m_UniqueGrammarItems[Node.m_Symbol.m_GrammarSymbolNo].m_ItemStrId;
        //    if (SymbolStr == "WEAK_NP")
        //    {
        //        int yy = 0;
        //    }
        //}

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


};

void CSentence::ProjectUsedHomonyms(const CGLRParser& Parser, const COccurrence& C, const size_t OccurNo, vector<CBuildingUnit>& BuildingUnits, vector<set< pair<int, int> > >& UsedOccurrences ) const
{
	vector< set<size_t> > Leaves;
	Leaves.resize(C.second - C.first);
	Parser.GetParseNodesRecursive(C.m_GrammarRuleNo, Leaves, true, C.first);
	for (size_t j = 0; j < Leaves.size(); j++)
	{
		for (set<size_t>::const_iterator  it = Leaves[j].begin(); it != Leaves[j].end(); it++)
		{
			const CSymbolNode& Node = C.m_pParser->m_SymbolNodes[*it];
			CBuildingUnit& Unit = BuildingUnits[Node.m_InputStart];

			assert(Node.m_InputStart+1 == Node.m_InputEnd);
			const CInputSymbol& Symbol = Node.m_Symbol;
		
			if (Unit.m_ChildClauseNo == -1) 
			{
				const CSynWord& W = m_Words[Unit.m_WordNo];
				for (int i=0; i< W.m_Homonyms.size(); i++)
					if (W.m_Homonyms[i].m_AutomatSymbolInterpetation.find(Symbol) !=  W.m_Homonyms[i].m_AutomatSymbolInterpetation.end())
					{
						Unit.m_BestHomonyms |= _QM(i);
						UsedOccurrences[Node.m_InputStart].insert(make_pair(OccurNo,i) );
					};
			}
			else
			{
				const CClause& C = m_Clauses[Unit.m_ChildClauseNo];
				for (int i=0; i< C.m_vectorTypes.size(); i++)
					if (C.m_vectorTypes[i].m_AutomatSymbolInterpetation.find(Symbol) !=  C.m_vectorTypes[i].m_AutomatSymbolInterpetation.end())
					{
						Unit.m_BestHomonyms |= _QM(i);
						UsedOccurrences[Node.m_InputStart].insert( make_pair(OccurNo, i));
					};
			};
		}
	};


};



bool CSentence::BuildMorphVariantsByTomita(const CGLRParser& Parser, const vector< COccurrence>& Occurrences, CClause& pClause, vector<CBuildingUnit>& BuildingUnits)
{
	m_bShowAllGroups = false;
	pClause.m_SynVariants.clear();


	// prohibit all homonyms
	for (size_t i=0; i<BuildingUnits.size(); i++)
		BuildingUnits[i].m_BestHomonyms = 0;

	// permit only those homonyms, which are in the best coverages
	vector<set< pair<int, int> > > UsedOccurrences(BuildingUnits.size());

	vector< CGroups > GroupsList;
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;

	for (size_t OccurNo=0; OccurNo<Occurrences.size(); OccurNo++)
	{
		const COccurrence& C = Occurrences[OccurNo];
		GroupsList.push_back(CGroups(GetOpt()));

		{	//  build group for a symbol node with all subgroups
			CGroups& Groups = GroupsList.back();
			Groups.ResizeAtomicDummy(BuildingUnits.size());	
			BuildGroupByGLR(Parser, C, pClause, Groups);
		};
			
		ProjectUsedHomonyms(Parser, C, OccurNo, BuildingUnits, UsedOccurrences);

	};

	assert (GroupsList.size()  == Occurrences.size());


	// permit all homonyms, if there is no chunk above them 
	for (size_t i=0; i<BuildingUnits.size(); i++)
		if (!BuildingUnits[i].m_BestHomonyms)
			BuildingUnits[i].m_BestHomonyms = GetMaxQWORD();


	{
        CMorphVariant V(GetOpt()->GetGramTab());
		pClause.BuildSynVariantsRecursive(BuildingUnits.begin(),  V);
	}



	CGroups DummyGroups(GetOpt());
	DummyGroups.ResizeAtomicDummy(BuildingUnits.size());

	for(SVI  pVar  = pClause.m_SynVariants.begin() ; pVar != pClause.m_SynVariants.end(); pVar++ )
	{
		CMorphVariant& synVariant = *pVar;
		ChooseClauseType(pClause.m_vectorTypes, synVariant);
		assert (synVariant.m_SynUnits.size() == BuildingUnits.size());

		for (size_t UnitNo=0; UnitNo<BuildingUnits.size();UnitNo++)
		{
			const set< pair<int, int> >& Occur = UsedOccurrences[UnitNo];
			
			for (set< pair<int, int> >::const_iterator it = Occur.begin();  it != Occur.end(); it++)
			{
				size_t OccurNo = it->first;
				const COccurrence& C = Occurrences[OccurNo];
				bool bFound = true;
				if (UnitNo == C.first)
				{
					for (size_t WordNo = C.first; WordNo < C.second; WordNo++)
					{
						pair<int,int> p;
						p.first = OccurNo;
						if (synVariant.m_SynUnits[WordNo].m_Type == EClause)
							p.second = synVariant.m_SynUnits[WordNo].m_iClauseTypeNum;
						else
							p.second = synVariant.m_SynUnits[WordNo].m_iHomonymNum;

						if (UsedOccurrences[WordNo].find(p) == UsedOccurrences[WordNo].end())
						{
							bFound = false;
							break;
						};
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

		//  projecting grammems  of a group to the main word of the group 
		//  going from the smallest groups to the largest
		//  so it can be that one word is initialized twice or more
		for (int i=0; i < synVariant.m_vectorGroups.GetGroups().size(); i++)
		{
			const CGroup& G = synVariant.m_vectorGroups.GetGroups()[i];
			CSynUnit& U = synVariant.m_SynUnits[G.m_MainWordNo];
			U.m_iGrammems = G.GetGrammems();
			U.m_GramCodes = G.m_GramCodes;
		};


		pClause.AssignVariantWeight(synVariant);

		pClause.BuildSubjAndPredMember(synVariant);

		//  translating group coordinates to sentence coordinates 
		for (size_t GroupNo=0; GroupNo< synVariant.m_vectorGroups.GetGroups().size(); GroupNo++)
			pClause.TranslateCoordSynVarForGroup(*synVariant.m_vectorGroups.GetGroupPtr(GroupNo));

        for ( size_t i = 0; i < synVariant.m_vectorGroups.m_Relations.size(); i++ )
        {
            synVariant.m_vectorGroups.m_Relations[i].m_iFirstWord = pClause.SynVarToPrmClForFirstWord(synVariant.m_vectorGroups.m_Relations[i].m_iFirstWord);
            synVariant.m_vectorGroups.m_Relations[i].m_iLastWord = pClause.SynVarToPrmClForLastWord( synVariant.m_vectorGroups.m_Relations[i].m_iLastWord);
            synVariant.m_vectorGroups.m_Relations[i].AssertValid();
        }

	};




	return true;
}

void CSentence::BuildGLRGroupsInClause(CClause& pClause)
{
	vector<CBuildingUnit> BuildingUnits;
	pClause.GetBuildingUnits(BuildingUnits);


	for (size_t i=0; i < BuildingUnits.size(); i++)
	{
		const CBuildingUnit & Unit = BuildingUnits[i];
		if (Unit.m_ChildClauseNo != -1) 
			BuildTerminalSymbolsByClause(Unit.m_ChildClauseNo);
	};

	
	CGLRParser Parser;
	ApplyGLR_Parsing(Parser, BuildingUnits);

	vector< COccurrence> Occurrences;
	GetBestChunks(Parser, Occurrences);

	BuildMorphVariantsByTomita(Parser, Occurrences, pClause, BuildingUnits);
};


void CSentence::BuildGLRGroupsInSentence()
{
	CClause Clause(this,0, m_Words.size()-1);
	vector<CBuildingUnit> BuildingUnits;
	Clause.GetBuildingUnits(BuildingUnits);
	
	CGLRParser Parser;
	ApplyGLR_Parsing(Parser, BuildingUnits);

	vector< COccurrence> Occurrences;
	GetBestChunks(Parser, Occurrences);


	m_GroupsUnion.Clear();
	m_GroupsUnion.ResizeAtomicDummy(Clause.size());	
	
	const CWorkGrammar& Grammar = GetOpt()->m_FormatsGrammar;

	for (size_t OccurNo=0; OccurNo<Occurrences.size(); OccurNo++)
	{
		const COccurrence& C = Occurrences[OccurNo];
		BuildGroupByGLR(Parser, C, Clause, m_GroupsUnion);
	};

};




