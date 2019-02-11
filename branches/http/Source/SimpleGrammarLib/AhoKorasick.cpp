// AhoKorasick.cpp : Defines the entry point for the console application.
//

#include  "SimpleGrammarLib.h"

#include "AhoKorasick.h"
#include "SimpleGrammar.h"
#include "../common/bserialize.h"


CTrieNode::CTrieNode()
{
	m_FailureFunction = -1;
	m_Parent = -1;
	m_IncomingSymbol = -1;
	m_GrammarRuleNo = -1;
	m_Depth = 0;
	m_ChildrenIndex = 0;
};




void CTrieHolder::PrintChildren(size_t NodeNo) const
{	
	printf("%i", NodeNo);
	/*if (!m_TerminatedPeriods.empty())
	{
		printf(" Lengths=");	

		for (set<CPattern>::const_iterator it = m_TerminatedPeriods.begin(); it !=  m_TerminatedPeriods.end(); it++)
		{
				printf("<%i,%s>,", it->size(), it->m_PeriodNameStr.c_str() );	
		};
	};*/

	if (m_Nodes[NodeNo].m_FailureFunction != -1)
		printf(" failure(%i) ", m_Nodes[NodeNo].m_FailureFunction);
	printf(" -> ");
	

	size_t i=0;
	size_t Count =  GetChildrenCount(NodeNo);
	for (; i<Count; i++)
	{
		const CTrieRelation& p = GetChildren(NodeNo)[i];
		SymbolInformationType::const_iterator it = m_pSymbolInformation->find(p.m_RelationChar);
		assert (it != m_pSymbolInformation->end());
		printf("%i %s,", p.m_ChildNo, it->second.c_str() );
	};

	printf("\n");

	for (; i<Count; i++)
		PrintChildren(GetChildren(NodeNo)[i].m_ChildNo);
};




int CTrieHolder::FindChild(int NodeNo, TerminalSymbolType RelationChar) const
{
	size_t Count =  GetChildrenCount(NodeNo);
	for (size_t i=0; i<Count; i++)
	{
		const CTrieRelation& p = GetChildren(NodeNo)[i];
		if (p.m_RelationChar == RelationChar)
			return p.m_ChildNo;
	};
	return -1;
};

int  CTrieHolder::GetTerminatedPeriodNext (int NodeNo) const
{
	if (NodeNo == 0) return -1;

	TerminalSymbolType RelationChar = m_Nodes[NodeNo].m_IncomingSymbol;
	size_t r =  m_Nodes[m_Nodes[NodeNo].m_Parent].m_FailureFunction;
	while (r != -1)
	{
		while ((r != -1) &&  (FindChild(r,RelationChar) == -1))
					r = m_Nodes[r].m_FailureFunction;

		if ( r != -1)
			return FindChild(r,RelationChar);
	};

	return -1;
};



void CTrieHolder::InitFailureFunction()
{
	assert (!m_Nodes.empty());

	vector<size_t> BFS;
	BFS.push_back(0);

	//  going breadth-first search
	for (size_t i=0; i < BFS.size(); i++)
	{
		const CTrieNode& Parent = m_Nodes[BFS[i]];
		size_t ChildrenCount =  GetChildrenCount(BFS[i]);
 		for (int j=0; j<ChildrenCount; j++)
		{
			const CTrieRelation& p = GetChildren(BFS[i])[j];
	
			//  going upside using following failure function in order to find the first node which 
			//  has a way downward with symbol RelationChar
			int r =  Parent.m_FailureFunction;
			while ((r != -1) &&  (FindChild(r,p.m_RelationChar) == -1))
				r = m_Nodes[r].m_FailureFunction;

			if (r != -1) 
			{
				//  the way is found
				m_Nodes[p.m_ChildNo].m_FailureFunction = FindChild(r,p.m_RelationChar);
			}
			else
				// no way is found, should start from the beginning
				m_Nodes[p.m_ChildNo].m_FailureFunction = 0;


			BFS.push_back(p.m_ChildNo);
		};
	};
};

void CTrieHolder::AddNode(const CTrieNode& T)
{
	m_Nodes.push_back(T);
	m_ChildrenAux.insert(m_ChildrenAux.end(),  GetAlphabetSize(), -1);
};

void CTrieHolder::CreateChildrenSequence(CTSI begin, CTSI end, size_t ParentNo, size_t WorkRuleNo)
{
	assert (begin < end);
	
	//  creating a child	
	CTrieNode T;
	T.m_Parent = ParentNo;
	T.m_Depth =  m_Nodes[ParentNo].m_Depth+1;
	T.m_IncomingSymbol = *begin;
	assert (T.m_IncomingSymbol < GetAlphabetSize());
	AddNode(T);

	//  registering this child
	size_t ChildNo = m_Nodes.size() - 1;
	assert (GetChildrenAux(ParentNo)[T.m_IncomingSymbol] == -1);
	GetChildrenAux(ParentNo)[T.m_IncomingSymbol] = ChildNo;

	//  inserting the next child
	if (end - begin > 1)
		CreateChildrenSequence(begin+1, end, ChildNo, WorkRuleNo);
	else
		m_Nodes[ChildNo].m_GrammarRuleNo = WorkRuleNo;
};


void CTrieHolder::ConvertAuxChildrenToNormal()
{
	m_Children.clear();
	m_Children.reserve(m_ChildrenAux.size());
	for (size_t NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	{
		m_Nodes[NodeNo].m_ChildrenIndex = m_Children.size();
		for (size_t i=0; i<GetAlphabetSize(); i++)
			if (GetChildrenAux(NodeNo)[i] != -1)
			{
				CTrieRelation R;
				R.m_ChildNo = GetChildrenAux(NodeNo)[i];
				R.m_RelationChar = i;
				m_Children.push_back( R );
			};	
	};

	ClearVector(m_ChildrenAux);
};

void CTrieHolder::CreateTrie(const set< CWorkRule >& Patterns)
{
	assert(!Patterns.empty());
	m_Nodes.clear();
	m_ChildrenAux.clear();
	m_Nodes.reserve(2*Patterns.size());
	m_ChildrenAux.reserve(2*Patterns.size()*GetAlphabetSize());

	//  inserting root 
	AddNode(CTrieNode ());
	

	set< CWorkRule >::const_iterator iter, prev_iter;
	iter = prev_iter = Patterns.begin();
	size_t RuleNo = 0; 
	CreateChildrenSequence(iter->m_RightPart.m_Items.begin(), iter->m_RightPart.m_Items.end(),  0, RuleNo);
	RuleNo++;
	
	
	
	for (iter++; iter != Patterns.end(); iter++, RuleNo++)
	{
		const CWorkRule& P = *iter;
		assert (!P.m_RightPart.m_Items.empty());

		//  Starter should be the node of the previous pattern, from which we should start 
		//  current sequence.

		//  Example1:
		//  Previous = abcd
		//  Current  = abd
		//  We have graph (1) -a-> (2) -b-> (3) -c-> (4) -d-> (5) 
		//  Starter should be pointed to node 3.

		//  Example2:
		//  Previous = abc
		//  Current  = abcd
		//  We have graph (1) -a-> (2) -b-> (3) -c-> (4) 
		//  Starter should be pointed to node 4.

		
		size_t Starter = 0;
		int CharNo =0;
		for (; CharNo < P.m_RightPart.m_Items.size(); CharNo++)
		{
			if	(		(CharNo == prev_iter->m_RightPart.m_Items.size() )
					||	(P.m_RightPart.m_Items[CharNo] !=  (*prev_iter).m_RightPart.m_Items[CharNo])
				)
			break;

			Starter = GetChildrenAux(Starter)[P.m_RightPart.m_Items[CharNo]];
			assert (Starter != -1);
		};


		if	(CharNo < P.m_RightPart.m_Items.size())
		{
			
			CreateChildrenSequence(P.m_RightPart.m_Items.begin()+CharNo, P.m_RightPart.m_Items.end(),   Starter, RuleNo);
		}
		else
		{
			assert (P.m_RightPart.m_Items.size() ==  prev_iter->m_RightPart.m_Items.size());
			// a grammar can has structural ambiguity, which causes dublicates  in patterns
			//ErrorMessage( "a dublicate is found");
		};


		prev_iter = iter;
	};

	ConvertAuxChildrenToNormal();
};


void CTrieHolder::Create(const set< CWorkRule >& Patterns, const SymbolInformationType* Info)
{
	m_pSymbolInformation = Info;
	m_AlphabetSize = m_pSymbolInformation->size();

	time_t t1,t2;
	fprintf (stderr, "CreateTrie\n");
	time(&t1) ;
	CreateTrie(Patterns);
	time(&t2);fprintf (stderr, "Seconds = %i\n",  t2-t1);

	
	//PrintChildren(0);
	fprintf (stderr, "InitFailureFunction\n");
	time(&t1) ;
	InitFailureFunction();
	time(&t2);fprintf (stderr, "Seconds = %i\n",  t2-t1);

	
	
	//PrintChildren(0);

};


void CTrieHolder::UpdatePossibleOutputSymbolsbyOnState(size_t NodeNo,  vector<bool>& PossibleOutputSymbols) const
{
	size_t Count =  GetChildrenCount(NodeNo);
	for (size_t i=0; i<Count; i++)
	{
		const CTrieRelation& p = GetChildren(NodeNo)[i];
		PossibleOutputSymbols[p.m_RelationChar] = true;
	};
};

void CTrieHolder::UpdatePossibleOutputSymbols (const set<size_t>& CurrentStates, vector<bool>& PossibleOutputSymbols) const 
{
	PossibleOutputSymbols.resize(GetAlphabetSize(), false);

	for (	set<size_t>::const_iterator it = CurrentStates.begin(); 
			it != CurrentStates.end(); 
			it++
		)
	{
		UpdatePossibleOutputSymbolsbyOnState(*it, PossibleOutputSymbols);

		for (int r = m_Nodes[(*it)].m_FailureFunction; r != -1; r = m_Nodes[r].m_FailureFunction)
			UpdatePossibleOutputSymbolsbyOnState(r, PossibleOutputSymbols);
	};

};



int CTrieHolder::NextState (int State, size_t TextPositionNo, TerminalSymbolType Symbol, vector< COccurrence >& Occurrences) const 
{
	while ( State!=-1 &&  (FindChild(State,Symbol) == -1)) 
		State =  m_Nodes[State].m_FailureFunction;

	if (State == -1)
		State  = 0;
	else
		State = FindChild(State,Symbol);

	for (int r = State; r != -1; r = GetTerminatedPeriodNext(r))
	{
		if (m_Nodes[r].m_GrammarRuleNo != -1)
		{
			COccurrence C;
			C.first = TextPositionNo - m_Nodes[r].m_Depth + 1;
			C.second =  TextPositionNo + 1;
			C.m_GrammarRuleNo = m_Nodes[r].m_GrammarRuleNo;
			Occurrences.push_back( C );
		}
	};


	return State;
};

void CTrieHolder::GetOccurrences (const TerminalString& Text, vector< COccurrence >& Occurrences) const 
{
	int r = 0;
	Occurrences.clear();
	for (size_t i=0; i<Text.size(); i++)
	{
		r = NextState(r,i,Text[i],Occurrences);
	};
};



inline size_t get_size_in_bytes (const CTrieNode& t)
{
	return		get_size_in_bytes(t.m_FailureFunction) 
			+	get_size_in_bytes(t.m_Parent) 
			+	get_size_in_bytes(t.m_IncomingSymbol) 
			+	get_size_in_bytes(t.m_GrammarRuleNo)
			+   get_size_in_bytes(t.m_Depth)
			+   get_size_in_bytes(t.m_ChildrenIndex);
};

inline size_t save_to_bytes(const CTrieNode& t, BYTE* buf)
{
	buf += save_to_bytes(t.m_FailureFunction, buf);
	buf += save_to_bytes(t.m_Parent, buf);
	buf += save_to_bytes(t.m_IncomingSymbol, buf);
	buf += save_to_bytes(t.m_GrammarRuleNo, buf);
	buf += save_to_bytes(t.m_Depth, buf);
	buf += save_to_bytes(t.m_ChildrenIndex, buf);
	return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CTrieNode& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.m_FailureFunction, buf);
	buf += restore_from_bytes(t.m_Parent, buf);
	buf += restore_from_bytes(t.m_IncomingSymbol, buf);
	buf += restore_from_bytes(t.m_GrammarRuleNo, buf);
	buf += restore_from_bytes(t.m_Depth, buf);
	buf += restore_from_bytes(t.m_ChildrenIndex, buf);
	
	return get_size_in_bytes(t);
};



inline size_t get_size_in_bytes (const CTrieRelation& t)
{
	return		get_size_in_bytes(t.m_RelationChar) 
			+	get_size_in_bytes(t.m_ChildNo);
};

inline size_t save_to_bytes(const CTrieRelation& t, BYTE* buf)
{
	buf += save_to_bytes(t.m_RelationChar, buf);
	buf += save_to_bytes(t.m_ChildNo, buf);
	return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CTrieRelation& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.m_RelationChar, buf);
	buf += restore_from_bytes(t.m_ChildNo, buf);
	return get_size_in_bytes(t);
};

bool CTrieHolder::Load(const SymbolInformationType* Info, string GrammarFileName)
{
	m_pSymbolInformation = Info;
	m_AlphabetSize = m_pSymbolInformation->size();
	string PrecompiledFile = MakeFName(GrammarFileName,"automat");
	FILE * fp = fopen(PrecompiledFile.c_str(), "rb");
	if (!fp)
	{
		ErrorMessage (Format("Cannot open %s", PrecompiledFile.c_str()));
		return false;
	};
	int Count;
	char buffer[256];
	{	
		if (!fgets(buffer, 256, fp)) return false;
		Count = atoi(buffer);
	}
	m_Nodes.clear();
	ReadVectorInner(fp, m_Nodes, Count);


	{	
		if (!fgets(buffer, 256, fp)) return false;
		Count = atoi(buffer);
	}

	ReadVectorInner(fp, m_Children, Count);
	
	fclose(fp);
	return true;
};

bool CTrieHolder::Save(string GrammarFileName) const
{
	string PrecompiledFile = MakeFName(GrammarFileName,"automat");
	FILE * fp = fopen(PrecompiledFile.c_str(), "wb");
	if (!fp)
	{
		ErrorMessage (Format("Cannot open %s", PrecompiledFile.c_str()));
		return false;
	};
	fprintf(fp, "%i\n", m_Nodes.size());
	if (!WriteVectorInner(fp, m_Nodes)) return false;
	fprintf(fp, "%i\n", m_Children.size());
	if (!WriteVectorInner(fp, m_Children)) return false;
	fclose(fp);
	return true;
};



size_t  CTrieHolder::GetChildrenCount(size_t NodeNo)  const
{  
	if (NodeNo+1 == m_Nodes.size())
			return m_Children.size() - m_Nodes[NodeNo].m_ChildrenIndex;
	else
			return m_Nodes[NodeNo+1].m_ChildrenIndex - m_Nodes[NodeNo].m_ChildrenIndex;
};

size_t CTrieHolder::GetAlphabetSize() const	
{
	return m_AlphabetSize; 
};

vector<CTrieRelation>::const_iterator  CTrieHolder::GetChildren(size_t NodeNo) const 
{ 
	return m_Children.begin() + m_Nodes[NodeNo].m_ChildrenIndex; 
};
