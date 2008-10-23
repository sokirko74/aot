// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru), 
// ==========  Copyright by Alexey Sokirko (2004)


// The main function of this file is CMorphAutomatBuilder::AddStringDaciuk 
// This function is an implementation of algorithm for constructing minimal,
// deterministic, acyclic FSM from unordered set of strings, which was described 
// in  "Jan Daciuk, Stoyan Mihov, Bruce Watson, and Richard Watson, 
//      Incremental Construction of Minimal Acyclic Finite State Automata, 
//		Computational Linguistics, 26(1), March 2000."

#pragma warning  (disable : 4996)

#include "StdMorph.h"
#include "MorphAutomBuilder.h"
#include "queue"
#include "assert.h"

#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif


//======================================================
//=============		CTrieNodeBuild	   =============
//======================================================
int NodeId = 0;
size_t RegisterSize = 0;

void CTrieNodeBuild::Initialize()
{
	m_bFinal = false;
	m_IncomingRelationsCount = 0;
	m_bRegistered = false;
	m_NodeId = NodeId++;
	memset(m_Children,0, sizeof(CTrieNodeBuild*)*MaxAlphabetSize);
	m_FirstChildNo = 0xff;
	m_SecondChildNo = 0xff;

};


void CTrieNodeBuild::SetFinal(bool bFinal)
{
	m_bFinal = bFinal;

};



void  CTrieNodeBuild::AddChild(CTrieNodeBuild* Child, BYTE ChildNo )
{
	assert (Child != this);
	Child->m_IncomingRelationsCount++;
	m_Children[ChildNo] = Child;

	if (ChildNo < m_FirstChildNo)
	{
		m_SecondChildNo = m_FirstChildNo;
		m_FirstChildNo = ChildNo;
	}
	else
		if (		(ChildNo != m_FirstChildNo) 
				&&	(ChildNo < m_SecondChildNo)
			)
		{
			m_SecondChildNo = ChildNo;
			assert ( m_FirstChildNo < m_SecondChildNo);
		};
};

void  CTrieNodeBuild::ModifyChild(CTrieNodeBuild* Child, BYTE ChildNo, bool bUpdateIncoming)
{
	CTrieNodeBuild* OldChild = m_Children[ChildNo];
	assert (OldChild);
	if (OldChild == Child) return;

	if (bUpdateIncoming)
		OldChild->m_IncomingRelationsCount--;

	// adding the new child 
	AddChild(Child, ChildNo);
};





CTrieNodeBuild* CTrieNodeBuild::GetNextNode(BYTE ChildNo)  const
{
	return m_Children[ChildNo];
};




void CTrieNodeBuild::GetIncomingRelationsCountRecursive(map<const CTrieNodeBuild*, size_t>& Node2Incoming) const
{
	
	for (size_t i=m_FirstChildNo; i < MaxAlphabetSize; i++)
	if (m_Children[i])
	{
		if (Node2Incoming.find(m_Children[i]) == Node2Incoming.end())
			m_Children[i]->GetIncomingRelationsCountRecursive(Node2Incoming);

		Node2Incoming[m_Children[i]]++;
	};
};

bool CTrieNodeBuild::CheckIncomingRelationsCountRecursive(map<const CTrieNodeBuild*, size_t>& Node2Incoming) const
{
	size_t debug = Node2Incoming[this];
	assert (Node2Incoming[this] == m_IncomingRelationsCount);
	if (Node2Incoming[this] != m_IncomingRelationsCount)
		return false;

	for (size_t i=m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (m_Children[i])
			if (!m_Children[i]->CheckIncomingRelationsCountRecursive( Node2Incoming))
				return false;

	return true;
};

bool CTrieNodeBuild::CheckRegisterRecursive() const
{
	if (m_bRegistered)
	{
		assert (*m_pRegister == this);
		if (*m_pRegister != this)
			return false;
	};
	
	for (size_t i=m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (m_Children[i])
			if (!m_Children[i]->CheckRegisterRecursive( ))
				return false;

	return true;
};

void	CTrieNodeBuild::SetNodeIdNullRecursive ()
{
	m_NodeId = -1;
	for (size_t i=m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (m_Children[i])
			m_Children[i]->SetNodeIdNullRecursive( );
	return;			
};

void	CTrieNodeBuild::UnregisterRecursive()
{
	m_bRegistered = false;
	for (size_t i=m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (m_Children[i])
			m_Children[i]->UnregisterRecursive( );
	return;			
};




//======================================================
//=============		IsLessRegister	   =============
//======================================================


bool IsLessRegister::operator ()(const CTrieNodeBuild* pNodeNo1, const CTrieNodeBuild* pNodeNo2) const
{
	if (pNodeNo1->m_bFinal != pNodeNo2->m_bFinal)
		return pNodeNo1->m_bFinal < pNodeNo2->m_bFinal;

	assert (pNodeNo1->m_FirstChildNo == pNodeNo2->m_FirstChildNo);
	if (pNodeNo1->m_FirstChildNo == 0xff) return false;
	if (pNodeNo1->m_Children[pNodeNo1->m_FirstChildNo] < pNodeNo2->m_Children[pNodeNo2->m_FirstChildNo]) 
		return true;
	if (pNodeNo1->m_Children[pNodeNo1->m_FirstChildNo] > pNodeNo2->m_Children[pNodeNo2->m_FirstChildNo]) 
		return false;


	assert (pNodeNo1->m_SecondChildNo == pNodeNo1->m_SecondChildNo);
	if (pNodeNo1->m_SecondChildNo == 0xff) return false;

	return lexicographical_compare
			(pNodeNo1->m_Children+pNodeNo1->m_SecondChildNo, pNodeNo1->m_Children+MaxAlphabetSize,
			pNodeNo2->m_Children+pNodeNo2->m_SecondChildNo, pNodeNo2->m_Children+MaxAlphabetSize);
}

//======================================================
//=============		CMorphAutomatBuilder	   =============
//======================================================

CMorphAutomatBuilder::CMorphAutomatBuilder(MorphLanguageEnum Language, BYTE AnnotChar) :	CMorphAutomat(Language, AnnotChar)
{
	m_pRoot = 0;
	
};

CMorphAutomatBuilder::~CMorphAutomatBuilder()
{
	ClearBuildNodes();
};

void CMorphAutomatBuilder::DeleteNode(CTrieNodeBuild* pNode)
{
	for (size_t i=pNode->m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (pNode->m_Children[i])
		{
			if (pNode->m_Children[i]->m_IncomingRelationsCount==1)
				DeleteNode(pNode->m_Children[i]);
			else
				//  we decrement incoming relations count; we will back soon using another path.
				pNode->m_Children[i]->m_IncomingRelationsCount--;

		};
	
	m_DeletedNodes.push_back(pNode);

};

CTrieNodeBuild* CMorphAutomatBuilder::CreateNode()
{
	CTrieNodeBuild* pNode; 
	if (!m_DeletedNodes.empty())
	{
		pNode	= m_DeletedNodes.back();
		m_DeletedNodes.erase(m_DeletedNodes.end() - 1);
		
	}
	else
		pNode = new CTrieNodeBuild;

	pNode->Initialize();
	
	return pNode;
};


CTrieNodeBuild* CMorphAutomatBuilder::CloneNode(const CTrieNodeBuild* pPrototype) 
{
	// creating a clone
	CTrieNodeBuild* N = CreateNode();
	N->m_FirstChildNo = pPrototype->m_FirstChildNo;
	N->m_SecondChildNo = pPrototype->m_SecondChildNo;
	N->SetFinal( pPrototype->m_bFinal );
	N->m_IncomingRelationsCount = 0;
	N->m_bRegistered = false;

	/// copying children and incrementing m_IncomingRelationsCount
	for (size_t i=pPrototype->m_FirstChildNo; i < MaxAlphabetSize; i++)
		if (pPrototype->m_Children[i])
		{
			N->m_Children[i] = pPrototype->m_Children[i];
			pPrototype->m_Children[i]->m_IncomingRelationsCount++;
		}
	
	return N;
};


void CMorphAutomatBuilder::ClearBuildNodes()
{
	if (m_pRoot)
		DeleteNode(m_pRoot);

	for (size_t i=0; i < m_DeletedNodes.size();i++)	
		delete m_DeletedNodes[i];

	m_DeletedNodes.clear();

	m_pRoot = 0;
};

void CMorphAutomatBuilder::ClearRegister()
 {
	for (size_t k=0; k < MaxAlphabetSize+1; k++)
		for (size_t i=0; i<MaxAlphabetSize+1; i++)
			m_RegisterHash[k][i].clear();

	m_pRoot->UnregisterRecursive();
	RegisterSize = 0;
};

void CMorphAutomatBuilder::InitTrie()
{
	ClearBuildNodes();
	m_pRoot = CreateNode();
	ClearRegister();
	
};

void CMorphAutomatBuilder::UpdateCommonPrefix(const string& WordForm)
{
	m_Prefix.resize(1);
	m_Prefix[0] = m_pRoot;
	
	size_t Length = WordForm.length();
	for (size_t i=0; i < Length; i++)
	{
		BYTE CharNo = m_Alphabet2Code[(BYTE)WordForm[i]];
		CTrieNodeBuild* pNode =  m_Prefix.back()->GetNextNode(CharNo);
		if (!pNode) 
			break;
		m_Prefix.push_back(pNode);
	};
	
};


int CMorphAutomatBuilder::GetFirstConfluenceState() const
{
	for (size_t i =0; i <m_Prefix.size(); i++)
		if (m_Prefix[i]->m_IncomingRelationsCount > 1)
				return i;
	return -1;
};


CTrieRegister& CMorphAutomatBuilder::GetRegister(const CTrieNodeBuild* pNode)
{
	BYTE Register1 = MaxAlphabetSize;
	BYTE Register2 = MaxAlphabetSize;

	if (pNode->m_FirstChildNo !=  0xff)
	{
		Register1 = pNode->m_FirstChildNo;
		
		if (pNode->m_SecondChildNo !=  0xff)
			Register2 = pNode->m_SecondChildNo;
	}

	return  m_RegisterHash[Register1][Register2];
};



CTrieNodeBuild* CMorphAutomatBuilder::ReplaceOrRegister(CTrieNodeBuild* pNode)
{
	CTrieRegister& Register = GetRegister(pNode);

	CTrieRegister::const_iterator it = Register.find(pNode);
	if (it != Register.end())
	{
		DeleteNode(pNode);
		pNode  = *it;
		assert (pNode->m_bRegistered);
		assert (pNode->m_pRegister == it);
	}
	else
	{
		pNode->m_pRegister = Register.insert(pNode).first;
		pNode->m_bRegistered = true;
		RegisterSize++;
	}
	
	return  pNode;
};


bool CheckRegisterOrder(const CTrieRegister& Register)
{
	const CTrieNodeBuild* pPrevNode = 0;
	IsLessRegister Less;
	for (CTrieRegister::const_iterator it = Register.begin(); it != Register.end(); it++)
	{
		const CTrieNodeBuild* pNode = *it;
		if (pPrevNode)
		{	
			if (!Less(pPrevNode,pNode))
			{
				assert (Less(pPrevNode,pNode));
				return false;
			};
			
		};
		pPrevNode = pNode;
	};
	return true;
}

bool CMorphAutomatBuilder::CheckRegister() const
{
	//printf ("Register size= %i\n",m_Register.size());
	for (size_t k=0; k < MaxAlphabetSize + 1; k++)
		for (size_t i=0; i<MaxAlphabetSize + 1; i++)
		{
			const CTrieRegister& Register =  m_RegisterHash[k][i];
			if (!CheckRegisterOrder(Register)) return false;
			
			for (CTrieRegister::const_iterator it = Register.begin(); it != Register.end(); it++)
			{
				const CTrieNodeBuild* pNode = *it;
						
				if (pNode->m_bRegistered)
				{
					assert (pNode->m_pRegister == it);
					if (pNode->m_pRegister != it)
					return false;
				};
			};
		};
	
	return m_pRoot->CheckRegisterRecursive();
};


bool CMorphAutomatBuilder::IsValid() const
{
	// en empty automat is OK
	if (!m_pRoot)  return true;

	// checking register
	if (!CheckRegister())
		return false;

	// checking CTrieNodeBuild::m_IncomingRelationsCount
	map<const CTrieNodeBuild*, size_t> Node2Incoming;
	m_pRoot->GetIncomingRelationsCountRecursive(Node2Incoming);
	if (!m_pRoot->CheckIncomingRelationsCountRecursive(Node2Incoming))
		return false;

	return true;
};

void CMorphAutomatBuilder::UnregisterNode(CTrieNodeBuild* pNode)
{
	if (pNode->m_bRegistered)
	{
		pNode->m_bRegistered = false;
		GetRegister(pNode).erase(pNode->m_pRegister);
		RegisterSize --;
	};
};

// we do not register the parent node; we  register only the children
CTrieNodeBuild* CMorphAutomatBuilder::AddSuffix(CTrieNodeBuild* pParentNodeNo, const char* WordForm)
{
	// save current char
	BYTE RelationChar = (BYTE)*WordForm;
	WordForm++;

	//  adding new node child
	CTrieNodeBuild* pChildNode = CreateNode();
	 
	//  adding the rest of the suffix 
	if (*WordForm)
		AddSuffix(pChildNode, WordForm); 

	// making it final
	if (*WordForm == 0)
		pChildNode->SetFinal( true ) ;

	//  replace or register (the children should be already registered)
	pChildNode = ReplaceOrRegister(pChildNode);
		

	//  adding this child to the parent
	{
		assert (!pParentNodeNo->m_bRegistered);
		pParentNodeNo->AddChild(pChildNode, m_Alphabet2Code[RelationChar]);
	}
	return 	pChildNode;
};




bool CMorphAutomatBuilder::AddStringDaciuk(const string& WordForm)
{
	if (!CheckABCWithAnnotator(WordForm))
	{
		fprintf (stderr, "%s - bad ABC    \n", WordForm.c_str());
		return false;
	};

	if (WordForm.rfind(m_AnnotChar) == WordForm.length() - 1)
	{
		fprintf (stderr, "%s - bad annotation   \n", WordForm.c_str());
		return false;
	};
		


	UpdateCommonPrefix(WordForm);

	if	(		(m_Prefix.size() == WordForm.length()+1) 
			&&	m_Prefix.back()->m_bFinal
		)
	{
		// an equal string is already in the dictionary 
		return true;
	};

	CTrieNodeBuild*	pLastNode = m_Prefix.back();

	int FirstConfluenceState = GetFirstConfluenceState();
	if (FirstConfluenceState != -1)
		pLastNode = CloneNode(pLastNode);
	else
	{
		// we should unregister Prefix.back() otherwize some node of suffix can be minimized to 
		// Prefix.back()
		UnregisterNode(pLastNode);
	};

	

	if (m_Prefix.size() == WordForm.length()+1) 
		pLastNode->SetFinal(  true );
	else
	{
		AddSuffix(pLastNode, WordForm.c_str()+m_Prefix.size()-1);
		assert (!pLastNode->m_bRegistered);
	}

	// CurrentIndex is a pointer to prefix node, which was not yet registered
	int  CurrentIndex = m_Prefix.size() - 1;
	
	
	if (FirstConfluenceState != -1)
	{	
		FirstConfluenceState = GetFirstConfluenceState();


		if (FirstConfluenceState != -1)
			for (; CurrentIndex > FirstConfluenceState;  CurrentIndex--)
			{
				// clone the parent
				CTrieNodeBuild*	pParent = CloneNode(m_Prefix[CurrentIndex-1]);

				// register the child
				pLastNode = ReplaceOrRegister(pLastNode);

				// Modify the child of the parent
				{
					BYTE CharNo = m_Alphabet2Code[(BYTE)WordForm[CurrentIndex-1]];
					pParent->ModifyChild(pLastNode, CharNo, true);
				}

				pLastNode = pParent;

                FirstConfluenceState = GetFirstConfluenceState();
				
			};
		
	};
	
	for (; CurrentIndex > 0; CurrentIndex--)
	{
		UnregisterNode(m_Prefix[CurrentIndex-1]);

		CTrieNodeBuild*	pOldLastNode = pLastNode;

		pLastNode = ReplaceOrRegister(pLastNode);

		if (pLastNode == m_Prefix[CurrentIndex])
		{
			ReplaceOrRegister(m_Prefix[CurrentIndex-1]);
			break;
		};
		
		// if pOldLastNode==pLastNode then it is the first iteration of the cycle
		// and the current cycle is the only confluence state on the path to the root automat.
		// If a state is confluent, then we should update CTrieNodeBuild::m_IncomingRelationsCount
		// otherwise it leads to the error, since we have just deleted the old child
		BYTE CharNo = m_Alphabet2Code[(BYTE)WordForm[CurrentIndex-1]];
		m_Prefix[CurrentIndex-1]->ModifyChild(pLastNode, CharNo, FirstConfluenceState==CurrentIndex);

		pLastNode = m_Prefix[CurrentIndex-1];
		
	};
	

//	PrintTreeJpg();

	//assert (IsValid());
	return true;
};



void CMorphAutomatBuilder::ConvertBuildRelationsToRelations()
{
	if (!m_pRoot) return;
	m_pRoot->SetNodeIdNullRecursive();
	queue<CTrieNodeBuild*> NodesQueue;
	NodesQueue.push(m_pRoot);
	m_pRoot->m_NodeId = 0;

	vector<CMorphAutomNode> Nodes;
	vector<CMorphAutomRelation> Relations;

	while (!NodesQueue.empty())
	{
		//  getting an element from the queue
		CTrieNodeBuild* pNode = NodesQueue.front();
		NodesQueue.pop();

		CMorphAutomNode N;
		N.SetFinal(pNode->m_bFinal);
		
		N.SetChildrenStart(Relations.size());
		assert (N.GetChildrenStart() == Relations.size());
		assert (N.IsFinal() == pNode->m_bFinal);

		Nodes.push_back(N);

		int CurrentNodeId = Nodes.size() + NodesQueue.size();

		for (size_t i=0; i < MaxAlphabetSize; i++)
		if (pNode->m_Children[i])
		{
			CTrieNodeBuild* Child = pNode->m_Children[i];
			if (Child->m_NodeId == -1)
			{
				Child->m_NodeId = CurrentNodeId++;
				NodesQueue.push(Child);
			};

			// adding new relation
			CMorphAutomRelation R;
			R.SetRelationalChar(m_Code2Alphabet[i]);
			R.SetChildNo(Child->m_NodeId);
			assert (R.GetChildNo() == Child->m_NodeId);
			assert (R.GetRelationalChar() == m_Code2Alphabet[i]);

			Relations.push_back(R);
			if (Relations.size() > 0xffffff)
			{
				throw CExpc("Too many children in the automat. It cannot be more than 0xffffff");
			};
		};
	};

	Clear();

	m_NodesCount = Nodes.size();
	m_pNodes = new CMorphAutomNode[m_NodesCount];
	std::copy(Nodes.begin(), Nodes.end(), m_pNodes);

	m_RelationsCount = Relations.size();
	m_pRelations = new CMorphAutomRelation[m_RelationsCount];
	copy(Relations.begin(), Relations.end(), m_pRelations);

};

