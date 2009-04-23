#include "stdafx.h"
#include "SemanticStructure.h"


//====================================================================
//===================       Relations         ========================
//====================================================================


void CSemanticStructure::GetIncomingRelations (long NodeNo, vector<long>& Relations, bool UseUse) const 
{
  Relations.clear();
  for (size_t i = 0; i<GetRelationsSize(); i++)
   if (!UseUse || GetRelation(i)->m_bRelUse) 
     if (GetRelation(i)->m_TargetNodeNo == NodeNo)
	    Relations.push_back(i);

};

void CSemanticStructure::GetIncomingInClauseRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<GetRelationsSize(); i++)
     if  (   (GetRelation(i)->m_TargetNodeNo == NodeNo)
		  && !IsRelBetweenClauses(*GetRelation(i))
		 )

	    Relations.push_back(i);
};
void CSemanticStructure::GetIncomingClauseRelations (long ClauseNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<GetRelationsSize(); i++)
     if  (   (GetNode(GetRelation(i)->m_TargetNodeNo).m_ClauseNo == ClauseNo)
		  && IsRelBetweenClauses(*GetRelation(i))
		 )

	    Relations.push_back(i);
};



void CSemanticStructure::GetOutcomingRelations (long NodeNo, vector<long>& Relations, bool UseUse) const 
{
 Relations.clear();

  for (size_t i = 0; i<GetRelationsSize(); i++)
   if (!UseUse || GetRelation(i)->m_bRelUse) 
     if (GetRelation(i)->m_SourceNodeNo == NodeNo)
	   Relations.push_back(i);
};

long CSemanticStructure::GetOutcomingRelationsCount (long NodeNo,  bool UseUse) const 
{
 vector<long> Relations;
 GetOutcomingRelations(NodeNo, Relations, UseUse);
 return Relations.size();
};

void CSemanticStructure::GetOutcomingNodes (long NodeNo, vector<long>& Nodes, bool UseUse) const
{
  Nodes.clear();
  vector <long> Rels;
  GetOutcomingRelations (NodeNo, Rels, UseUse);
  for (size_t i = 0; i<Rels.size(); i++)
  	   Nodes.push_back(GetRelation(Rels[i])->m_TargetNodeNo);
};


void CSemanticStructure::GetRelations (long NodeNo, vector<long>& Relations, bool UseUse) const 
{
  Relations.clear();

  for (size_t i = 0; i<GetRelationsSize(); i++)
   if (!UseUse || GetRelation(i)->m_bRelUse) 
     if  (    (GetRelation(i)->m_SourceNodeNo == NodeNo)
		   || (GetRelation(i)->m_TargetNodeNo == NodeNo)
		 )
	   Relations.push_back(i);


};
void CSemanticStructure::GetBrothers (long NodeNo, vector<long>& Nodes, bool UseUse) const
{
  Nodes.clear();
  vector<long> Rels;
  GetIncomingRelations (NodeNo, Rels, UseUse);
  for (long i=0; i < Rels.size(); i++)
  {
    vector <long> CurrNodes;
    GetOutcomingNodes (GetRelation(Rels[i])->m_SourceNodeNo, CurrNodes, UseUse);
	for (long j=0; j < CurrNodes.size();j++)
	 if (!_find(Nodes, CurrNodes[j]))
		Nodes.push_back(CurrNodes[j]);

	vector <long> DopRels;
    GetOutcomingDopRelations (GetRelation(Rels[i])->m_SourceNodeNo, DopRels);
	for ( long j=0; j < DopRels.size();j++)
	 if (!_find(Nodes, GetDopRelation(DopRels[j])->m_TargetNodeNo))
		Nodes.push_back(GetDopRelation(DopRels[j])->m_TargetNodeNo);

  };
  /*
  придется повторить это же для дополнительных отношений
  */
  GetIncomingDopRelations (NodeNo, Rels);
  for (long i=0; i < Rels.size(); i++)
  {
    vector <long> CurrNodes;
    GetOutcomingNodes (GetDopRelation(Rels[i])->m_SourceNodeNo, CurrNodes, UseUse);
	for (long j=0; j < CurrNodes.size();j++)
	 if (!_find(Nodes, CurrNodes[j]))
		Nodes.push_back(CurrNodes[j]);

	vector <long> DopRels;
    GetOutcomingDopRelations (GetDopRelation(Rels[i])->m_SourceNodeNo, DopRels);
	for ( long j=0; j < DopRels.size();j++)
	 if (!_find(Nodes, GetDopRelation(DopRels[j])->m_TargetNodeNo))
		Nodes.push_back(GetDopRelation(DopRels[j])->m_TargetNodeNo);

  };

};



long CSemanticStructure::GetIncomingRelationsCount (long NodeNo, bool UseUse) const
{
 vector<long> Relations;
 GetIncomingRelations(NodeNo, Relations, UseUse);
 return Relations.size();
};







void CSemanticStructure::PrintRelations() const 
{ 
 rml_TRACE ("\n\nRelations : \n");

 for (size_t i=0; i<GetRelationsSize(); i++)
 {
	 string Label = GetRelation(i)->m_Valency.m_RelationStr;
	 string Q = Format ("%s %s ",  GetNodeStr1(GetRelation(i)->m_SourceNodeNo).c_str(), Label.c_str());
	 {
		 string  W = Format(" %s %s ", GetNodeStr1(GetRelation(i)->m_TargetNodeNo).c_str(), (GetRelation(i)->m_bRelUse ? " used"  : " unused"));
		 Q += W;
	 };
	 Q += " SF = "+GetSemFetsInOneStr( GetRelation(i)->m_SemFets);
	 Q += string("\n");

	 rml_TRACE (Q.c_str());
 };


};

void CSemanticStructure::FindRelations (long NodeNo1, long NodeNo2, vector<long>& Rels ) const 
{
 Rels.clear();
 for (size_t i=0; i<GetRelationsSize(); i++)
	 if  (    (GetRelation(i)->m_SourceNodeNo == NodeNo1) 
		   && (GetRelation(i)->m_TargetNodeNo == NodeNo2)
		 )
		 Rels.push_back(i);
};


long CSemanticStructure::FindFirstRelation (long NodeNo1, long NodeNo2) const
{
 vector<long> Rels;
 FindRelations (NodeNo1, NodeNo2, Rels);
 if (Rels.size() == 0) return -1;
 return Rels[0];
}

long CSemanticStructure::FindDopFirstRelation (long NodeNo1, long NodeNo2) const
{
  for (size_t i=0; i<GetDopRelationsSize(); i++)
	 if  (    (GetDopRelation(i)->m_SourceNodeNo == NodeNo1) 
		   && (GetDopRelation(i)->m_TargetNodeNo == NodeNo2)
		 )
		 return i;
 return -1;
}




void CSemanticStructure::GetClauseRoots (size_t ClauseNo, vector<long>& Roots) const
{
 Roots.clear();
 for (size_t i=0; i<GetNodesSize(); i++)
  if  ( IsInClause(i, ClauseNo) )
  {
	  vector <long> IncomRels;

	  GetIncomingRelations(i, IncomRels, false);
	  for (size_t k=0; k <IncomRels.size(); k++)
		  if (   !IsInClause(GetRelation(IncomRels[k])->m_SourceNodeNo, ClauseNo) )
		  {
			  IncomRels.erase(IncomRels.begin() +k);
			  k--;
		  };

	 if (IncomRels.size() == 0) Roots.push_back(i);
	  
  }
  
};




bool CSemanticStructure::AreConnectedClauses (long ClauseNo1, long ClauseNo2) const
{
	for (size_t i=0; i<GetRelationsSize(); i++)
      	  if (    (     IsInClause(GetRelation(i)->m_SourceNodeNo, ClauseNo1) 
			        &&  IsInClause(GetRelation(i)->m_TargetNodeNo, ClauseNo2))
			  ||  (     IsInClause(GetRelation(i)->m_SourceNodeNo, ClauseNo2) 
			        &&  IsInClause(GetRelation(i)->m_TargetNodeNo, ClauseNo1))

			 )
          return true; 

     return false;
};


void CSemanticStructure::DeleteDubleRelations()
{
	for (size_t i=0; i < GetRelationsSize(); i++)
     for (size_t k=i+1; k < GetRelationsSize(); k++)
		 if (    (GetRelation(i)->m_SourceNodeNo ==  GetRelation(k)->m_SourceNodeNo)
			  && (GetRelation(i)->m_TargetNodeNo ==  GetRelation(k)->m_TargetNodeNo) 
			  && (GetRelation(i)->m_Valency ==  GetRelation(k)->m_Valency) 
			)
		 {
			 EraseRelation (k);
			 k--;
		 };
           

};

void CSemanticStructure::DeleteRelations(vector<long>& Rels)
{
	if (Rels.size() == 0) return;
	sort (Rels.begin(), Rels.end());
	for (long i=Rels.size() - 1; i >= 0; i--)
			 EraseRelation(Rels[i]);
};






//====================================================================
//===================       Nodes             ========================
//====================================================================


string  CSemanticStructure::GetNodeStr1(size_t NodeNo, size_t MaxLength) const
{
 string Q = GetNodeStr(NodeNo, MaxLength);
 string T = Q;
 if (GetNode(NodeNo).IsAbstract()) 
	 T = Format("%s%u",Q.c_str(), NodeNo);
 return T;
};

string  CSemanticStructure::GetNodeStr(const CSemNode& N, size_t MaxLength) const
{
	string S;
	if (N.m_NodeType == Copul) return "Copul";
	if (N.m_NodeType == ModalCopul) return "ModalCopul";
	if (N.m_NodeType == SJA) return "СЯ";
	if (N.m_NodeType == Situat) return "SIT";	
	int WordsCount = N.GetWordsSize();


	for (size_t i=0; i < N.GetWordsSize(); i++)
	  if (!N.GetWord(i).IsQuoteMark())
			S = S + GetInterfaceWordStr(&N, i) + string(" ");

	Trim(S);
	size_t l= S.length();
	if (l > MaxLength)
	{
		S = S.substr(0, MaxLength/2) + string ("...") + S.substr(l-MaxLength/2);
	};
	for (size_t i=0; i < N.m_RelOperators.size(); i++)
	{
		 string Q = Format(" (%s)",N.m_RelOperators[i].c_str());
		 S += Q;
	};

	if (N.m_NodeType == MNA)
		S = "MUA: " + S;	

	return S;
};
string  CSemanticStructure::GetNodeStr(size_t NodeNo, size_t MaxLength) const
{
	const CSemNode& N = GetNode(NodeNo);
	return GetNodeStr(N, MaxLength);
}



string  CSemanticStructure::GetNodeLemStr(size_t NodeNo) const
{
	const CSemNode& N = GetNode(NodeNo);
	string S;
	for (size_t i=0; i < N.GetWordsSize(); i++)
	{
		if (!ispunct ((BYTE)N.GetWord(i).m_Word[0]))
		{
			S = S + N.GetWord(i).m_Lemma + string(" ");
		}
		else
			S = S + N.GetWord(i).m_Word + string(" ");

		if (!N.GetWord(i).m_NumeralPrefix.empty())
			S += "(NumeralPrefix = " + N.GetWord(i).m_NumeralPrefix +") ";
	};

	Trim(S);
	return S;
};

void CSemanticStructure::PrintNodes() const 
{
 rml_TRACE ("\n\nNodes : \n");

 for (size_t i=0; i<GetNodesSize(); i++)
 {
	 rml_TRACE(GetNodeStr1(i).c_str());
	 string Q = Format(" ClauseNo = %i ", GetNode(i).m_ClauseNo);
	 rml_TRACE(Q.c_str());
	 if (GetNode(i).GetType() == LocRoss) rml_TRACE (" geo");
	 if (GetNode(i).m_bReached) rml_TRACE( " reached");
	 if (GetNode(i).m_bToDelete) rml_TRACE( " to_delete");
	 rml_TRACE( " TagId=%i", GetNode(i).m_Tag);
	 if (GetNode(i).IsWordContainer())
	  rml_TRACE( " (MinWordNo=%i, MaxWordNo=%i)", GetNode(i).GetMinWordNo(), GetNode(i).GetMaxWordNo());
	 if (GetNode(i).m_bToDelete) rml_TRACE( " to_delete");
	 Q = " SF = "+GetSemFetsInOneStr( GetNode(i).m_NodeSemFets);
	 rml_TRACE (Q.c_str());
	 rml_TRACE ("\n");
 };


};

string CSemanticStructure::GetNodePosesStr(long NodeNo) const 
{
	string  POS = " ";
	if (GetNode(NodeNo).m_MainWordNo != -1)
		for (long i=0; i < 32; i++)
			if ( ( ( 1 << i) & GetNode(NodeNo).GetWord(GetNode(NodeNo).m_MainWordNo).m_Poses) > 0)
			{
				POS = POS + string(" ") + string((const char*)m_pData->GetRusGramTab()->GetPartOfSpeechStr(i));
			};
	return POS;
};


void CSemanticStructure::PrintLemNodes() const 
{
 rml_TRACE ("\n\nNodes : \n");

 for (size_t i=0; i<GetNodesSize(); i++)
 {
	 rml_TRACE(GetNodeLemStr(i).c_str());
	 rml_TRACE(Format(" ClauseNo = %i ", GetNode(i).m_ClauseNo).c_str());
	 if (GetNode(i).GetType() == LocRoss) rml_TRACE (" geo");
	 if (GetNode(i).GetType() == OborRoss) rml_TRACE (" obor");
	 string Q = Format(" GF = %s %s",  GetNodePosesStr(i).c_str(),  
		 m_pData->GetRusGramTab()->GrammemsToStr(  GetNode(i).GetGrammems()).c_str() );
	 rml_TRACE (Q.c_str());
	 
	 rml_TRACE ("\n");
 };


};









void CSemanticStructure::DelNode(long NodeNo)
{

  for (size_t k=0; k<GetRelationsSize(); k++)
  {
     if  (    (GetRelation(k)->m_TargetNodeNo == NodeNo)
		    || (GetRelation(k)->m_SourceNodeNo == NodeNo)
		 )
	 {
		  EraseRelation(k);
		  k--;
		  continue;

	 };
	      
     if (GetRelation(k)->m_SourceNodeNo > NodeNo)    GetRelation(k)->m_SourceNodeNo--;
	 if (GetRelation(k)->m_TargetNodeNo > NodeNo) GetRelation(k)->m_TargetNodeNo--;
  };

  for (size_t k=0; k<GetDopRelationsSize(); k++)
  {
     if  (    (GetDopRelation(k)->m_TargetNodeNo == NodeNo)
		    || (GetDopRelation(k)->m_SourceNodeNo == NodeNo)
		 )
	 {
		  EraseDopRelation(k);
		  k--;
		  continue;

	 };
	      
     if (GetDopRelation(k)->m_SourceNodeNo > NodeNo)    GetDopRelation(k)->m_SourceNodeNo--;
	 if (GetDopRelation(k)->m_TargetNodeNo > NodeNo) GetDopRelation(k)->m_TargetNodeNo--;
  };


  for (size_t k=0; k<GetSynRels().size(); k++)
  {

    if  (    (GetSynRels()[k].m_TargetNodeNo == NodeNo)
		    || (GetSynRels()[k].m_SourceNodeNo == NodeNo)
		 )
	 {
		  GetSynRels().erase(GetSynRels().begin() +k);
		  k--;
		  continue;
	 };


     if (GetSynRels()[k].m_SourceNodeNo > NodeNo)    GetSynRels()[k].m_SourceNodeNo--;
	 if (GetSynRels()[k].m_TargetNodeNo > NodeNo) GetSynRels()[k].m_TargetNodeNo--;
  };



  for (size_t k=0; k<GetLexFuncts().size(); k++)
  {

    if  (    (GetLexFuncts()[k].m_TargetNodeNo == NodeNo)
		    || (GetLexFuncts()[k].m_SourceNodeNo == NodeNo)
		 )
	 {
		  GetLexFuncts().erase(GetLexFuncts().begin()+k);
		  k--;
		  continue;
	 };


     if (GetLexFuncts()[k].m_SourceNodeNo > NodeNo)    GetLexFuncts()[k].m_SourceNodeNo--;
	 if (GetLexFuncts()[k].m_TargetNodeNo > NodeNo) GetLexFuncts()[k].m_TargetNodeNo--;
  };



  
  EraseNode(NodeNo);
  
};



void CSemanticStructure::MoveAllRelations(long FromNode, long ToNode)
{

  for (size_t k=0; k<GetRelationsSize(); k++)
  {
     if  ( GetRelation(k)->m_TargetNodeNo == FromNode )
			GetRelation(k)->m_TargetNodeNo = ToNode;

	 if  ( GetRelation(k)->m_SourceNodeNo  == FromNode )
		 GetRelation(k)->m_SourceNodeNo  = ToNode;
  };

  for (size_t k=0; k<GetDopRelationsSize(); k++)
  {
     if  ( GetDopRelation(k)->m_TargetNodeNo == FromNode )
			GetDopRelation(k)->m_TargetNodeNo = ToNode;

	 if  ( GetDopRelation(k)->m_SourceNodeNo  == FromNode )
		 GetDopRelation(k)->m_SourceNodeNo  = ToNode;
  };


  for (size_t k=0; k<GetSynRels().size(); k++)
  {
     if  ( GetSynRels()[k].m_TargetNodeNo == FromNode )
			GetSynRels()[k].m_TargetNodeNo = ToNode;

	 if  ( GetSynRels()[k].m_SourceNodeNo  == FromNode )
		 GetSynRels()[k].m_SourceNodeNo  = ToNode;
  };



  for (size_t k=0; k<GetLexFuncts().size(); k++)
  {

     if  ( GetLexFuncts()[k].m_TargetNodeNo == FromNode)
			GetLexFuncts()[k].m_TargetNodeNo = ToNode;

	 if  ( GetLexFuncts()[k].m_SourceNodeNo  == FromNode )
		 GetLexFuncts()[k].m_SourceNodeNo  = ToNode;
  };

};



void CSemanticStructure::MoveRelations(long FromNode, long ToNode)
{
	if (FromNode == -1) return;
	if (ToNode == -1) return;

	for (long i=0; i<GetRelationsSize(); i++)
	{
		if (GetRelation(i)->m_SourceNodeNo == FromNode )
			GetRelation(i)->m_SourceNodeNo = ToNode;

		if (GetRelation(i)->m_TargetNodeNo == FromNode )
			GetRelation(i)->m_TargetNodeNo = ToNode;
	};
};

void CSemanticStructure::MoveIncomingRelations(long FromNode, long ToNode)
{
	if (FromNode == -1) return;
	if (ToNode == -1) return;

	for (long i=0; i<GetRelationsSize(); i++)
	{
		if (GetRelation(i)->m_TargetNodeNo == FromNode )
			GetRelation(i)->m_TargetNodeNo = ToNode;
	};
};

void CSemanticStructure::MoveIncomingDopRelations(long FromNode, long ToNode)
{
	if (FromNode == -1) return;
	if (ToNode == -1) return;

	for (long i=0; i<GetDopRelationsSize(); i++)
	{
		if (GetDopRelation(i)->m_TargetNodeNo == FromNode )
			GetDopRelation(i)->m_TargetNodeNo = ToNode;
	};

};





long CSemanticStructure::FindLexFunctBySemRel(long RelNo) const
{
	for (long i=0; i < GetLexFuncts().size(); i++)
		if  (   (GetLexFuncts()[i].m_SourceNodeNo == GetRelation(RelNo)->m_SourceNodeNo) 
			 && (GetLexFuncts()[i].m_TargetNodeNo == GetRelation(RelNo)->m_TargetNodeNo) 
			)
			return i;
		else
		if  (   (GetLexFuncts()[i].m_TargetNodeNo == GetRelation(RelNo)->m_SourceNodeNo) 
			 && (GetLexFuncts()[i].m_SourceNodeNo == GetRelation(RelNo)->m_TargetNodeNo) 
			)
			return i;
		return -1;
	
};

long CSemanticStructure::FindLexFunct(int iW1, int iW2) const
{
	for (long i=0; i < GetLexFuncts().size(); i++)
		if  (   (GetLexFuncts()[i].m_SourceNodeNo == iW1) 
			 && (GetLexFuncts()[i].m_TargetNodeNo == iW2) 
			)
			return i;
		else
		if  (   (GetLexFuncts()[i].m_TargetNodeNo == iW1) 
			 && (GetLexFuncts()[i].m_SourceNodeNo == iW2) 
			)
			return i;
		return -1;
	
};

long  CSemanticStructure::FindLexFunctBySituationNodeNo(long NodeNo) const
{
	for(int i = 0 ; i < GetLexFuncts().size(); i++ )	
	{
		if( GetLexFuncts()[i].m_SourceNodeNo ==  NodeNo )
			return i;
	}
	return -1;
}

long  CSemanticStructure::FindLexFunctByParameterNodeNo(long NodeNo) const
{
	for(int i = 0 ; i < GetLexFuncts().size(); i++ )	
	{
		if( GetLexFuncts()[i].m_TargetNodeNo ==  NodeNo )
			return i;
	}
	return -1;
}






// получает все вершины  предложения
void CSemanticStructure::GetRoots(vector<long>& Roots) const
{
	long count = GetNodesSize();
	long relscount = GetRelationsSize();

	for(int i = 0 ; i < count ; i++ )
	{
	  vector <long> IncomRels;
	  GetIncomingRelations(i, IncomRels, false);
	  /*
	    дополнительные отношения, которые на время интерпретации русской структуры были скопированы
        в основные игнорируем.
		Иначе во фразе "люди, живущие" - нет ни одного root
	  */
	  long NotDopRelationsCount = 0;
	  for (long j=0; j < IncomRels.size(); j++)
		 if (!GetRelation(IncomRels[j])->m_bDopRelation)
			 NotDopRelationsCount++;
		  
	  if( NotDopRelationsCount == 0 )
	  {
		  Roots.push_back(i);
	  }

	}
}



long CSemanticStructure::GetSynRelBySemRel(long iSemRelNo) const
{
	const CSemRelation* semRel = GetRelation(iSemRelNo);

	for(int i = 0 ; i < GetSynRels().size() ; i++ )
	{
		if( (semRel->m_SourceNodeNo == GetSynRels()[i].m_SourceNodeNo) && 
			(semRel->m_TargetNodeNo == GetSynRels()[i].m_TargetNodeNo) )
			return i;
	}
	return -1;
}



void CSemanticStructure::GetIncomingDopRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();
  for (size_t i = 0; i<GetDopRelationsSize(); i++)
     if (GetDopRelation(i)->m_TargetNodeNo == NodeNo)
	    Relations.push_back(i);

};

void CSemanticStructure::GetOutcomingDopRelations (long NodeNo, vector<long>& Relations) const 
{
  Relations.clear();

  for (size_t i = 0; i<GetDopRelationsSize(); i++)
     if (GetDopRelation(i)->m_SourceNodeNo == NodeNo)
	   Relations.push_back(i);
};

long CSemanticStructure::GetAnaphoraTarget(long NodeNo) const 
{
  for (size_t i = 0; i<GetDopRelationsSize(); i++)
     if (GetDopRelation(i)->m_SourceNodeNo == NodeNo)
		 if (GetDopRelation(i)->m_SyntacticRelation == "анафора") 
		   return GetDopRelation(i)->m_TargetNodeNo;
  return -1;	
};

long CSemanticStructure::GetEquNode(long NodeNo) const 
{
  for (size_t i = 0; i<GetDopRelationsSize(); i++)
     if (GetDopRelation(i)->m_SourceNodeNo == NodeNo)
		if (GetDopRelation(i)->m_Valency.m_RelationStr == "THESAME")		 
			return GetDopRelation(i)->m_TargetNodeNo;
  return -1;	
};
