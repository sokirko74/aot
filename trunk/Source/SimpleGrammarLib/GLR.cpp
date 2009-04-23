#include  "SimpleGrammarLib.h"
#include  "GLR.h"
#include  "LR-items.h"
#include  "SimpleGrammar.h"

CGLRParser::CGLRParser()
{
	m_bRobust = false;
};

/*
  create a state node v0 labelled with the start state 0 of DFA 
  set the first input related closure set be  {v0}, 
	  m_ActiveNodes = 0
	  m_PendingShifts = 0
	  m_PendingReductions = 0

*/
void CGLRParser :: InitGLRParser(const CGLRTable*	pTable)
{
	m_StateNodes.clear();
	m_SymbolNodes.clear();
	CStateNode Node;
	m_pTable = pTable;
	assert (m_pTable);
	//assert (!m_pTable->m_WorkTable.empty());
	Node.m_StateNo = 0;
	m_StateNodes.push_back(Node);

	m_InputLevels.resize(1);
	m_InputLevels[0] = 1;
	
	m_PendingReductions.clear();
	m_PendingShifts.clear();
	m_ActiveNodes.clear();


};




/*
 ACTOR(i)
   remove v  from m_ActiveNodes, and let h be the label of v
   if 'shift k' is an action in position (h, SymbolNo) of Parsing Table, add (v,k) to  m_PendingShifts
   for each entry 'reduce j' in position (h,SymbolNo) of Parsing Table 
   {
		for  each successor node u  of v add (u,j) to m_PendingReductions
   }
*/
//void CGLRParser::Actor(size_t SymbolNo)
void CGLRParser::Actor(const set<CInputSymbol>& Symbols)
{
	assert (!m_ActiveNodes.empty());
	size_t StateNodeNo = *m_ActiveNodes.begin();
	m_ActiveNodes.erase(m_ActiveNodes.begin());
	const CStateNode& Node = m_StateNodes[StateNodeNo];

	//assert (Node.m_StateNo < m_pTable->m_Table.size());

	for (set<CInputSymbol>::const_iterator it = Symbols.begin(); it != Symbols.end(); it++)
	{
		const CInputSymbol& Symbol = *it;
		const CSLRCellWork& Cell = m_pTable->GetCell(Node.m_StateNo, Symbol.m_GrammarSymbolNo); 
		if (Cell.m_bShift)
		{
			assert (Cell.m_GotoLine != -1);
			CPendingShift S;
			S.m_StateNodeNo = StateNodeNo;
			S.m_NextStateNo = Cell.m_GotoLine;
			S.m_pSymbol = it;
			m_PendingShifts.insert(S);
		};

		size_t end = m_pTable->GetReduceRuleEnd(Node.m_StateNo, Symbol.m_GrammarSymbolNo);

		for (size_t i=Cell.m_ReduceRulesSetStart; i<end; i++)
		{
			for (size_t j=0; j < Node.m_SymbolChildren.size(); j++)
			{
				CPendingReduction R;
				R.m_pReduceRule = &m_pTable->m_RuleInfos[m_pTable->m_ReduceRuleSets[i]];
				R.m_SymbolNodeNo = Node.m_SymbolChildren[j];
				R.m_LastStateNodeNo = StateNodeNo;
				m_PendingReductions.insert(R);
			};

		};
	};

	
};



bool CGLRParser::HasPathOfLengthTwo(int SourceNodeNo, const CSymbolNode& BetweenNode, size_t& SymbolNodeNo) const
{
	assert (SourceNodeNo < m_StateNodes.size());
	assert (!BetweenNode.m_NodeChildren.empty() );
	int TargetNodeNo = BetweenNode.m_NodeChildren[0];
	assert (TargetNodeNo < m_StateNodes.size());

	const CStateNode& Node = m_StateNodes[SourceNodeNo];
	// going to the depth
	for (size_t j=0; j < Node.m_SymbolChildren.size(); j++)
	{
		SymbolNodeNo = Node.m_SymbolChildren[j];
		const CSymbolNode& SymbolNode = m_SymbolNodes[SymbolNodeNo];
		if(			(BetweenNode.m_InputStart == SymbolNode.m_InputStart) // this check is senseless if m_bRobust is false
																	// otherwise we should check the input borders in order to reduce a symbol correctly 	
				&&	(BetweenNode.m_Symbol == SymbolNode.m_Symbol)
				&&	(SymbolNode.FindChild(TargetNodeNo) !=  -1 )

			)
				return true;
	};
	return false;	
};


/*
 this function creates or re-uses a  node which should be in the current input related closure set 
 and which should have StateNo associated with it.
 Return Value: 
	NewNodeNo - the number of the result node
 if such a node was already  created then the fucntion return false, otherwise it returns true. 
*/
bool CGLRParser::CreateOrReuseStateNode(int StateNo, size_t& NewNodeNo)
{
	assert (!m_InputLevels.empty());

	if (m_bRobust)
		// if m_bRobust, then the root is included into the active  nodes set,
		//  if the root (m_StateNodes[0]) is included, we should first check whether
		// we can reuse it, we should do it specially, because 
		// the cycle which goes through the current input related closure set  doesn't include the root
		if (m_StateNodes[0].m_StateNo == StateNo)
		{
			NewNodeNo = 0;
			return false;
		};

	int FirstNodeNoOfCurrentClosure = (m_InputLevels.size () == 1) ? 0:m_InputLevels.back();

	for (NewNodeNo=FirstNodeNoOfCurrentClosure; NewNodeNo < m_StateNodes.size(); NewNodeNo++)
		if (m_StateNodes[NewNodeNo].m_StateNo == StateNo)
			return false;
	
	CStateNode Node;
	//assert (!m_pTable->m_Table.empty());
	Node.m_StateNo = StateNo;
	m_StateNodes.push_back(Node);
	NewNodeNo = m_StateNodes.size() - 1;

	return true;
};

inline CSymbolNode& CGLRParser::GetNodeFromPath(const vector<size_t>& Path, size_t RuleItemNo)
{
	assert (RuleItemNo > 0);  // 0 is reserved for the left-handed part
	size_t idx = Path.size()-RuleItemNo*2;
	assert (idx < Path.size());
	size_t NodeNo = Path[idx];
	assert (NodeNo < m_SymbolNodes.size());
	return m_SymbolNodes[NodeNo];
};

bool CGLRParser::GetGrammarFeatures(const CPendingReduction& Reduction, const vector<size_t>& Path, CSymbolNode& Node)
{
	const CGLRRuleInfo& Rule = *Reduction.m_pReduceRule;
	const CWorkGrammar& Grammar = *m_pTable->m_pWorkGrammar;
	const vector<CRuleFeature>&  Features = Grammar.m_RuleFeatures[Rule.m_RuleFeaturesNo];
	size_t  PathSize = Path.size();

	// by default copy all properties from the root child
	{
		const CSymbolNode& MainNode = GetNodeFromPath(Path, Rule.m_SynMainItemNo+1);
		Node.m_Symbol.CopyAttributes(MainNode.m_Symbol);
	};

	for (size_t i=0; i < Features.size(); i++)
	{
		const CRuleFeature& F =  Features[i];
		if (F.m_Type == foeAssignement) 
		{
			if (F.m_LeftItem.m_Id != 0) continue;
			if (F.m_LeftItem.m_AttribName == "det")
			{
				assert (F.m_FuncType == ffeAssign0);
				Node.m_Symbol.m_bDeterm = F.m_pAssign0( );
			}
			else
			if (F.m_LeftItem.m_AttribName == "grm")
			{
				switch (F.m_RightItems.size())
				{
					case 1: {
								assert (F.m_FuncType == ffeAssign1);
								assert (F.m_RightItems[0].m_AttribName == "grm");
								const CSymbolNode& N1 = GetNodeFromPath(Path, F.m_RightItems[0].m_Id);
								Node.m_Symbol.m_GramCodes = F.m_pAssign1(Grammar.m_pGramTab, N1.m_Symbol.m_GramCodes );
								break;
							};
					case 2: {
								assert (F.m_FuncType == ffeAssign2);
								assert (F.m_RightItems[0].m_AttribName == "grm");
								assert (F.m_RightItems[1].m_AttribName == "grm");
								const CSymbolNode& N1 = GetNodeFromPath(Path, F.m_RightItems[0].m_Id);
								const CSymbolNode& N2 = GetNodeFromPath(Path, F.m_RightItems[1].m_Id);
								Node.m_Symbol.m_GramCodes = F.m_pAssign2(Grammar.m_pGramTab, N1.m_Symbol.m_GramCodes, N2.m_Symbol.m_GramCodes );
								break;
							};
					case 3: {
								assert (F.m_FuncType == ffeAssign3);
								assert (F.m_RightItems[0].m_AttribName == "grm");
								assert (F.m_RightItems[1].m_AttribName == "grm");
								assert (F.m_RightItems[2].m_AttribName == "grm");
								const CSymbolNode& N1 = GetNodeFromPath(Path, F.m_RightItems[0].m_Id);
								const CSymbolNode& N2 = GetNodeFromPath(Path, F.m_RightItems[1].m_Id);
								const CSymbolNode& N3 = GetNodeFromPath(Path, F.m_RightItems[2].m_Id);
								Node.m_Symbol.m_GramCodes = F.m_pAssign3(Grammar.m_pGramTab, N1.m_Symbol.m_GramCodes, N2.m_Symbol.m_GramCodes, N3.m_Symbol.m_GramCodes );
								break;
							};
					default : assert(false);
				}
				if (Node.m_Symbol.m_GramCodes.empty()) return false;
			};
		};
		if (F.m_Type == foeCheck) 
		{
				switch (F.m_RightItems.size())
				{
					case 1:		{
									assert (F.m_FuncType == ffeCheck1);
									const CSymbolNode& N1 = GetNodeFromPath(Path, F.m_RightItems[0].m_Id);
									if (F.m_RightItems[0].m_AttribName == "det")
									{
										if (!F.m_pCheck1(N1.m_Symbol.m_bDeterm))
											return false;
									}
									else
										if (F.m_RightItems[0].m_AttribName == "size")
										{
											//  checking is_atomic
											if (		!F.m_pCheck1(N1.m_InputEnd - N1.m_InputStart == 1) 
													 ||	N1.m_Symbol.m_bClause	
												)
												return false;
										}
										else
											assert (false);
									break;
								};
					case 2:		{
									assert (F.m_FuncType == ffeCheck3);
									assert (F.m_RightItems[0].m_AttribName == "grm");
									const CSymbolNode& N1 = GetNodeFromPath(Path, F.m_RightItems[0].m_Id);
									if (!F.m_pCheck3(Grammar.m_pGramTab, N1.m_Symbol.m_GramCodes, F.m_RightItems[1].m_MorphPattern.m_Poses, F.m_RightItems[1].m_MorphPattern.m_Grammems))
										return false;
									break;
								}
					default : assert(false);
				
				};
		};

	};

	return true;
};


/*
  REDUCER (i) 
    remove (u,j) from R, where  u is a symbol node, and j is a rule no
	let RuleLength be the length of the right hand side od rule j 
	let LeftPart be the symbol on the left hand side of rule j
	for each  state node w which can be reached from u along a path of length (2*RuleLength-1) do
	{
		let k be the label of the w( = "ChildNode.m_StateNo")
		let "goto l" be the entry in position (k, LeftPart) in the parsing table (="Cell.m_GotoLine")
		if there is no node in the last input related closure set labeled l then 
		  create a new state node in the GSS labelled l and  add it to  the last input related closure set
										and to m_ActiveNodes
		let  v be the node in the last input related closure labelled l (==NewNodeNo)
		if there is a path of length 2  in the GSS from v to w then do nothing
		else
		{
			create a new symbol node u' in the GSS labelled LeftPart
			make u' successor of v and a predecessor of w
			if v  is not in A 
			{
				for all reductions rk in position  (l, SymbolNo) of the table add (u,k) to  R
			}
		};

	};

*/


void CGLRParser::ReduceOnePath(const set<CInputSymbol>& Symbols, const CPendingReduction& Reduction, const vector<size_t>& Path)
{
	const size_t RuleLength = Reduction.m_pReduceRule->m_RuleLength;
	const size_t LeftPart  = Reduction.m_pReduceRule->m_LeftPart;
	const size_t FirstStateNodeNo = Path.back();
	assert (2*RuleLength+1 == Path.size()); 
	assert (Path.size()  >= 2); // at least one symbol should be reduced
	//DumpParser(true);
	
	// initializes  properties of the symbol node
	CSymbolNode SN;
	//  InputStart is the place in the input text, where this reduced sequence is started) 
	SN.m_InputStart = m_SymbolNodes[Path[Path.size()-2]].m_InputStart;
	SN.m_InputEnd = m_SymbolNodes[Path[1]].m_InputEnd; 
	SN.m_Symbol.m_GrammarSymbolNo = LeftPart;
	SN.m_NodeChildren.push_back(FirstStateNodeNo);
	if (!GetGrammarFeatures(Reduction, Path, SN))
		return;


	// ChildNodeNo  is the first node of the sequnce nodes which should be reduced
	const CSLRCellWork& Cell = m_pTable->GetCell(m_StateNodes[FirstStateNodeNo].m_StateNo, LeftPart);
	assert (Cell.m_GotoLine != -1); // m_GotoLine can not be -1, since the after reduce we should go to a state
	size_t NewStateNodeNo;
	if (CreateOrReuseStateNode(Cell.m_GotoLine, NewStateNodeNo))  // creating a new state node
	{
		//  if a node was created then we should add it to ActiveNodes in order to call Actor 
		//  for it on the next iteration (and to add next pending reductions  and shifts)6
		m_ActiveNodes.insert(NewStateNodeNo);
	}


	
	size_t NewSymbolNode;
	if (!HasPathOfLengthTwo(NewStateNodeNo, SN, NewSymbolNode))
	{
		m_SymbolNodes.push_back(SN);

		NewSymbolNode = m_SymbolNodes.size() - 1;
		m_StateNodes[NewStateNodeNo].m_SymbolChildren.push_back(NewSymbolNode);

		/*
			if NewNodeNo is not in m_ActiveNodes, it means that it was already created before
			we have got in this  procedure. And it means that for this node 
			on the next iteration the Actor would be called. 
			Since we have just created a new path from NewNode we should reprocess all reductions
			from this node, since some reductions can use this new path.
		*/
		if (m_ActiveNodes.find(NewStateNodeNo) == m_ActiveNodes.end())
		{
			for (set<CInputSymbol>::const_iterator it = Symbols.begin(); it != Symbols.end(); it++)			
			{
				const CInputSymbol& Symbol = *it;
				const CSLRCellWork& C = m_pTable->GetCell(Cell.m_GotoLine, Symbol.m_GrammarSymbolNo);
				size_t end = m_pTable->GetReduceRuleEnd(Cell.m_GotoLine, Symbol.m_GrammarSymbolNo);
				for (size_t i=C.m_ReduceRulesSetStart; i<end; i++)
				{
					CPendingReduction R;
					R.m_pReduceRule = &m_pTable->m_RuleInfos[m_pTable->m_ReduceRuleSets[i]];
					R.m_SymbolNodeNo = NewSymbolNode;
					R.m_LastStateNodeNo = NewStateNodeNo;
					m_PendingReductions.insert(R);
				};
			};
		};
	};

	//  storing parse tree
	{
		CParsingChildrenSet S;
		S.m_ReduceRule = Reduction.m_pReduceRule;
		//assert(Path.size()%2 == 0);

		for	(int i=Path.size()-2; i>0; i-=2)
			S.m_Items.push_back(Path[i]);

		m_SymbolNodes[NewSymbolNode].m_ParseChildren.push_back(S);
	};
	//DumpParser(true);
};

// This  function is written mostly for non-recursive depth-first search in GSS (see Internet for documentation).
// when a path of  length 2*RuleLength-1 is found the reduction action (procedure ReduceOnePath)
// is called.
// Here is description of this procedure.  
// We use DFS-stack which is a sequence of pairs 
// <first[1],end[1]>, <first[2],end[2]>,...,<first[n],end[n]>,  where  for each 1<i<=n, 
//  <first[i],end[i]> are all unobserved children of node first[i-1]. Each iteration we made 
// the following:
// 1. the current  path is not long enough  and  the current top of the DFS stack has children
//  push to DFS stack all children of current top.
// 2. otherwise  we go to the neighbour or to the parent's neighbour of the current top, if a neighbour doesn't exist

//void CGLRParser::Reducer(size_t SymbolNo)
void CGLRParser::Reducer(const set<CInputSymbol>& Symbols)
{
	
	assert (!m_PendingReductions.empty());
	assert (m_ActiveNodes.empty());

	CPendingReduction Reduction = *m_PendingReductions.begin();
	m_PendingReductions.erase(m_PendingReductions.begin());
	size_t RuleLength = Reduction.m_pReduceRule->m_RuleLength;
	
	vector< size_t > Path;
	vector< size_t > Starts;
	{
		// adding the first item of the DFS-stack
		Path.push_back(Reduction.m_LastStateNodeNo);
		Starts.push_back(0); // the first value of Starts[0] is unused
		Path.push_back(Reduction.m_SymbolNodeNo);
		Starts.push_back(0);
	}
	
	do 
	{
		assert (Path.size() > 1);
		size_t d = Path.back();
		/*
			if the length of path is odd then the last element is a symbol node
			else it is a state node.
		*/
		
		if (Path.size() == 2*RuleLength+1)
		{
			ReduceOnePath(Symbols, Reduction, Path);
		};

		const vector<size_t>& children = (Path.size()%2==0) ? m_SymbolNodes[d].m_NodeChildren : m_StateNodes[d].m_SymbolChildren;
		if	(		(Path.size() <= 2*RuleLength )
				&&	!children.empty() 
			)
		{
			Path.push_back(*(children.begin()));
			Starts.push_back(0);
		}
		else
		{
			while (Path.size () > 1)
			{
				d = Path.back();
				const vector<size_t>& curr_children = (Path.size()%2==0) ? m_SymbolNodes[d].m_NodeChildren : m_StateNodes[d].m_SymbolChildren;

				Starts.back()++;
				if (Starts.back() < curr_children.size())
				{
					Path.push_back(curr_children[Starts.back()]);
					Starts.push_back(0);
					break;
				};
				
				Path.erase(Path.end()-1);
				Starts.erase(Starts.end()-1);
				
				
			}
			
		};
	}
	while (Path.size () != 1);
};

/*
	SHIFTER(i)
	while (m_PendingShifts != 0) {
		remove an element (v,k) from m_PendingShifts
		if there is no node, w,  labelled k  in the last input related closure create one
		if w does not have a successor mode,u, labelled SymbolNo create one
		if u is not already a predecessor of v, make it one
	};

*/

void CGLRParser::Shifter(const set<CInputSymbol>& Symbols)
{
	
	while (!m_PendingShifts.empty())
	{	
		// == remove an element (m_StateNodeNo, m_NextStateNo) from m_PendingShifts
		
		CPendingShift Shift = *m_PendingShifts.begin();
		m_PendingShifts.erase(m_PendingShifts.begin());
		
		// == if there is no node, w,  labelled m_NextStateNo  in m_InputLevels.back() create one

		size_t NewNode;
		CreateOrReuseStateNode(Shift.m_NextStateNo, NewNode);
		CStateNode& Node = m_StateNodes[NewNode];

		// == if w does not have a successor mode,u, labelled SymbolNo create one
		
		vector<size_t>::const_iterator it = Node.m_SymbolChildren.begin();
		for (; it != Node.m_SymbolChildren.end(); it++)
			if ( m_SymbolNodes[*it].m_Symbol == *Shift.m_pSymbol  )
				break;

		int SymbolNode;
		if (it == Node.m_SymbolChildren.end())
		{
			//  creating new symbol node 
			CSymbolNode SN;
			SN.m_Symbol = *Shift.m_pSymbol;
			SN.m_InputStart = m_InputLevels.size() - 2;
			SN.m_InputEnd = m_InputLevels.size() - 1;
			m_SymbolNodes.push_back(SN);
			SymbolNode = m_SymbolNodes.size() - 1;
			Node.m_SymbolChildren.push_back(SymbolNode);
		}
		else
			//  reusing old symbol node
			SymbolNode = *it;

		// == if u is not already a predecessor of v, make it one
		if (m_SymbolNodes[SymbolNode].FindChild(Shift.m_StateNodeNo) == -1)
			m_SymbolNodes[SymbolNode].m_NodeChildren.push_back(Shift.m_StateNodeNo);
		
	};


};



string CGLRParser::GetDotStringOfSymbolNode(size_t SymbolNodeNo) const
{
	string Result;
	const CSymbolNode& C = m_SymbolNodes[SymbolNodeNo];
	if (C.m_Symbol.m_GrammarSymbolNo == m_pTable->m_pWorkGrammar->GetEndOfStreamSymbol()) return "";
	const CGrammarItem& I = 	m_pTable->m_pWorkGrammar->m_UniqueGrammarItems[C.m_Symbol.m_GrammarSymbolNo];
	string Label = I.m_ItemStrId.c_str();
	if  (!I.m_MorphPattern.m_GrmAttribute.empty())
		Label += " "+I.m_MorphPattern.m_GrmAttribute;
	Label += Format("[%i,%i) N=%i", C.m_InputStart, C.m_InputEnd, SymbolNodeNo);

	string Subsets;
	int NumberOfSubset = 0;

	for (size_t i=0; i<C.m_ParseChildren.size(); i++)
	{
		const vector<size_t>& v = C.m_ParseChildren[i].m_Items;
		bool bFound = false;		
		string CurrentResult;
		for (size_t j=0; j < v.size(); j++)
		{
			// we should ignore all children sets, where a end of symbol is found 
			// since we want to show(!) only the largest tree
			if (m_SymbolNodes[v[j]].m_Symbol.m_GrammarSymbolNo == m_pTable->m_pWorkGrammar->GetEndOfStreamSymbol()) 
			{
				CurrentResult = "";		
				break;
			};
			CurrentResult += Format("s%i:f%i -> s%i ;\n", SymbolNodeNo, NumberOfSubset, v[j]);
		};

		if (!CurrentResult.empty())
		{
			Result += CurrentResult;
			Subsets +=  Format("| <f%i>*",NumberOfSubset);
			NumberOfSubset++;
		};
	};
	Label += Subsets ;
	Result += Format("s%i [shape=record label=\"%s\"];\n", SymbolNodeNo, Label.c_str() );

	return Result;
	
};


size_t CGLRParser::GetNumberOfClosureSet(size_t StateNodeNo) const
{
	for (size_t i=0; i<m_InputLevels.size(); i++)
	{
		size_t start = 0;
		if (i >0)
			start = m_InputLevels[i-1];
		size_t  end = m_InputLevels[i];
		if (		(StateNodeNo >= start)
				&&	(StateNodeNo < end)
			)
			return i;
	};
	return m_InputLevels.size();
};

string CGLRParser::GetDotStringOfStack(bool bShowStates) const
{
	string Result = "digraph G {\n";
	int size = max(m_SymbolNodes.size(), (size_t)10);
	Result += Format("size = \"%i.0,%i.0\";\n", size, size);


	for (size_t i=0; i< m_SymbolNodes.size();i++)
	{
		const CSymbolNode& C = m_SymbolNodes[i];
		if (bShowStates)
			for (size_t j=0; j< C.m_NodeChildren.size(); j++)
			{
				Result += Format("s%i -> n%i [style=dotted];\n", i, C.m_NodeChildren[j]);
			};
		Result += GetDotStringOfSymbolNode(i);
	};

	if (bShowStates)
	for (size_t i=0; i< m_StateNodes.size();i++)
	{
		const CStateNode& C = m_StateNodes[i];
		string Label = Format("State=%i [U=%i; N=%i]",C.m_StateNo, GetNumberOfClosureSet(i), i );
		Result += Format("n%i [label=\"%s\"];\n", i, Label.c_str() );
		for (size_t j=0; j< C.m_SymbolChildren.size(); j++)
		{
			Result += Format("n%i -> s%i [style=dotted];\n", i, C.m_SymbolChildren[j]);
		};
	};
	return Result+ "}\n";
};



void CGLRParser::DumpParser(bool bShowStates) const
{
	FILE* fp = fopen ("stack.dot","w");
	string s = GetDotStringOfStack(bShowStates);
	fprintf (fp,"%s", s.c_str());
	fclose(fp);
	system ("dot.exe stack.dot -o stack.jpg -Tjpg");
}

/*
return true if the main symbol of the grammar was recognized from the
start of the input
*/
bool	CGLRParser::HasGrammarRootAtStart() const
{
	for (size_t i=0; i< m_SymbolNodes.size();i++)
	{
		int SymbolNo = m_SymbolNodes[i].m_Symbol.m_GrammarSymbolNo;
		if	(		m_pTable->m_pWorkGrammar->m_UniqueGrammarItems[SymbolNo].m_bGrammarRoot 
				&&	m_SymbolNodes[i].m_InputStart == 0
			)
		return true;
	};

	return false;
};

void CGLRParser::GetParseNodesRecursiveWithoutSyntaxAmbiguity(size_t SymbolNodeNo, vector<size_t>& Nodes, bool bShouldBeLeaf) const
{

	if (bShouldBeLeaf ==  m_SymbolNodes[SymbolNodeNo].m_ParseChildren.empty())
		Nodes.push_back(SymbolNodeNo);

	//  we get only the first children set, since the  input coverage doesn't depend upon 
	// the "children ambiguity"
	if (!m_SymbolNodes[SymbolNodeNo].m_ParseChildren.empty())
	{
		const vector<size_t>& v = m_SymbolNodes[SymbolNodeNo].m_ParseChildren[0].m_Items;
		for (size_t j=0; j < v.size(); j++)
			GetParseNodesRecursiveWithoutSyntaxAmbiguity(v[j], Nodes, bShouldBeLeaf);
	};
};

void CGLRParser::GetParseNodesRecursive(size_t SymbolNodeNo, vector< set<size_t> >& Nodes, bool bShouldBeLeaf, size_t  RootInputStart) const
{
	assert (!Nodes.empty() );

	const CSymbolNode& Node = m_SymbolNodes[SymbolNodeNo];

	if (bShouldBeLeaf ==  Node.m_ParseChildren.empty())
		Nodes[Node.m_InputStart - RootInputStart].insert(SymbolNodeNo);

	for (size_t i=0; i< Node.m_ParseChildren.size(); i++)
	{
		const vector<size_t>& v = Node.m_ParseChildren[i].m_Items;

		for (size_t j=0; j < v.size(); j++)
		{
			GetParseNodesRecursive(v[j], Nodes, bShouldBeLeaf, RootInputStart);
		};
	};
};


/*
 if S is  m_SymbolNodes[SymbolNodeNo] and [i,j) is a period which is occupied by
 this symbol S in the input stream, then this function returns l, i<=l<j, where  l is the 
 index of the syntax main word  in the period [i,j).
*/
size_t CGLRParser::GetMainWordRecursive(size_t SymbolNodeNo)  const
{
	const CSymbolNode& S = m_SymbolNodes[SymbolNodeNo];

	//  it should be called only for non-terminals
	assert (!S.m_ParseChildren.empty());

	// we take into account only the first children set, since the next sets doesn't have an impact on the longest match algorithm
	const CParsingChildrenSet&  Set = S.m_ParseChildren[0];
	const CGLRRuleInfo* pRule = Set.m_ReduceRule; 
	assert(pRule);
	assert (pRule->m_SynMainItemNo < pRule->m_RuleLength);
	assert (pRule->m_RuleLength == Set.m_Items.size());

	
	size_t MainChildNo = Set.m_Items[pRule->m_SynMainItemNo];

	if (m_SymbolNodes[MainChildNo].m_ParseChildren.empty())
		return m_SymbolNodes[MainChildNo].m_InputStart;
	else
		return   GetMainWordRecursive(MainChildNo);
	
};



bool CGLRParser::ParseSymbol(const set<CInputSymbol>& Symbols)
{
	m_ActiveNodes.clear();
//	DumpParser(true);
	//  copying from previous input-related closure set to the current actve nodes set
	assert (m_StateNodes.size() == m_InputLevels.back());
	size_t start = 0;
	if (m_InputLevels.size() > 1)
		start = m_InputLevels[m_InputLevels.size() - 2];

	for (size_t i=start; i<m_InputLevels.back(); i++)
		m_ActiveNodes.insert(i);

	if (m_bRobust)
		m_ActiveNodes.insert(0);

	
	while (!m_ActiveNodes.empty() || !m_PendingReductions.empty())
	{
		if (!m_ActiveNodes.empty())
			Actor(Symbols);
		else
		{
			
			//DumpParser(true);
			Reducer(Symbols);
			//DumpParser(true);
			
		}
	};

	//  finish current input related closure set
	m_InputLevels.back() = m_StateNodes.size();

	// add a new one input related closure set
	m_InputLevels.push_back( m_StateNodes.size() );


	Shifter(Symbols);
	//DumpParser(true);

	bool bResult = m_StateNodes.size() - m_InputLevels.back() > 0 ;
	m_InputLevels.back() = m_StateNodes.size();
	if (m_bRobust)
		return true;
	else
		return bResult;
};


