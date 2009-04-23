
#include "stdafx.h"
#include "SemanticRusStructure.h"



const CSemNode&		CRusSemStructure::GetNode(int NodeNo) const 
{
	return m_Nodes[NodeNo];
}
CSemNode*		CRusSemStructure::GetNodePtr(int NodeNo)	
{
	return &(m_Nodes[NodeNo]);
}

int		CRusSemStructure::GetNodesSize() const 
{
	return m_Nodes.size();
};

string	CRusSemStructure::GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const  
{ 
	const CRusSemNode& Node = *(CRusSemNode*)pNode;
	string L = Node.m_Words[WordNo].m_Word;
	if (Node.m_Words[WordNo].m_NumeralPrefix != "")
		L = Node.m_Words[WordNo].m_NumeralPrefix + "-" + L;
	return L; 
}; 





// перечень всех СемО
const CSemRelation*	CRusSemStructure::GetRelation(int RelNo) const 
{
	return &(m_Relations[RelNo]);
}

CSemRelation*	CRusSemStructure::GetRelation(int RelNo)		 
{
	return &(m_Relations[RelNo]);
}

int		CRusSemStructure::GetRelationsSize() const	 
{
	return m_Relations.size();
};

void	CRusSemStructure::EraseRelation(int RelNo) 	 
{
	m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_IncomRelsCount--;
	string Label = m_Relations[RelNo].m_Valency.m_RelationStr;
	rml_TRACE("%s", Format ("Delete relation %s %s %s\n",  GetNodeStr1(GetRelation(RelNo)->m_SourceNodeNo).c_str(), Label.c_str(), GetNodeStr1(GetRelation(RelNo)->m_TargetNodeNo).c_str()).c_str());
	m_Relations.erase(m_Relations.begin()+RelNo);

};

void	CRusSemStructure::AddRelation(const CRusSemRelation& R) 
{
	m_Relations.push_back(R);
	m_Nodes[R.m_TargetNodeNo].m_IncomRelsCount++;
};

void	CRusSemStructure::GetColorAndWidthOfRelation(int RelNo, float& Width, string& Color) 
{
	Width = 1;
	Color  = m_Relations[RelNo].m_bSemFetAgree ? "blue" : "darkgreen";
}
// перечень всех дополнительных отношений, найденных в предложении
const CSemRelation*	CRusSemStructure::GetDopRelation(int RelNo) const 
{
	return &(m_DopRelations[RelNo]);
}
CSemRelation*	CRusSemStructure::GetDopRelation(int RelNo)		 
{
	return &(m_DopRelations[RelNo]);
}
int		CRusSemStructure::GetDopRelationsSize() const	 
{
	return m_DopRelations.size();
};
void	CRusSemStructure::EraseDopRelation(int RelNo) 	 
{
	m_DopRelations.erase(m_DopRelations.begin() +RelNo);
};


// проверяет, что узлы, которые связывает отношение не являются абстрактными
bool			CRusSemStructure::IsWordContainerRel(int iRel)  const
{
	return  m_Nodes[m_Relations[iRel].m_SourceNodeNo].IsWordContainer() 
		&&	m_Nodes[m_Relations[iRel].m_TargetNodeNo].IsWordContainer();
}

const vector<CSynRelation>&		CRusSemStructure::GetSynRels()  	const 
{
	return m_SynRelations;
};
vector<CSynRelation>&		CRusSemStructure::GetSynRels()  	
{
	return m_SynRelations;
};
const vector<CLexFunctRel>&		CRusSemStructure::GetLexFuncts()	const 
{
	return m_LexFuncts;
};
vector<CLexFunctRel>&		CRusSemStructure::GetLexFuncts()
{
	return m_LexFuncts;
};


const CSemClause&	CRusSemStructure::GetSemClause(long ClauseNo) const 
{
	return m_Clauses[ClauseNo];
};

const long			CRusSemStructure::GetSemClausesCount() const 
{
	return m_Clauses.size();
};

bool  CRusSemStructure::IsOptional(long RelNo) const 
{
  if (m_Relations[RelNo].m_SyntacticRelation == "врем_группа") return false;

  return  m_Relations[RelNo].m_Valency.m_bOptional;
};

string CSemClauseVariantResult :: GetStr()
{
	 return Format ("SyntaxClauseNo=%i ClauseVariant=%i AllPanicRelationsCount=%i AllTopNodesCount=%i ",
		    m_SyntaxClauseNo+1, m_ClauseVariantNo+1, m_PanicRelationsCount, m_NodesCount);
}

bool CRusSemStructure::CanBePrefixRelationOperator (long NodeNo) const
{
	return     m_Nodes[NodeNo].IsPrimitive()
		   &&  (   (m_Nodes[NodeNo].m_Words[0].m_Lemma == "НЕ")
                || (m_Nodes[NodeNo].m_Words[0].m_Lemma == "ТОЛЬКО")
				|| (m_Nodes[NodeNo].m_Words[0].m_Lemma == "ДАЖЕ")
				|| (m_Nodes[NodeNo].m_Words[0].m_Lemma == "НИ")
			   );
}

bool CRusSemStructure::CanBePostfixRelationOperator (long NodeNo) const
{
	return     m_Nodes[NodeNo].IsPrimitive()
		   &&  (   (m_Nodes[NodeNo].m_Words[0].m_Lemma == "ЖЕ")
			   );
}

bool CRusSemStructure::ContainsSemCopul(long NodeNo) const
{
	const CDictUnitInterp* Interp =  m_Nodes[NodeNo].GetInterp();
	if (Interp == 0) return false;
	if (Interp->m_DictType != Ross) return false;
	if (HasSemFet(m_Nodes[NodeNo], "Copul")) return true;
	WORD UnitNo = Interp->m_UnitNo;
	if (UnitNo ==  ErrUnitNo) 
		return false;

	const CRossHolder* pHolder = GetRossHolder(Interp->m_DictType);
	const CDictionary* Ross = pHolder->GetRoss();
	if (Ross->IsEmptyArticle(UnitNo))	return false;
	long EndCortegeNo = Ross->GetUnitEndPos(UnitNo);

	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= EndCortegeNo; i++)
	{
		TCortege C = GetCortege(Ross,i);
		if	(		(C.m_LeafId > 0) 
				&&	(C.m_FieldNo == pHolder->GramFetFieldNo) 
				&&	C.m_DomItemNos[0] != -1 
				&&	(pHolder->GetDomItemStrInner(C.m_DomItemNos[0]) == "Copul")
			)
			return true;
	}
	return false;
}


long CRusSemStructure::GetRusSubj(int NodeNo)
{
	vector<long> Rels; 
	GetOutcomingRelations(NodeNo, Rels, false);
	long  i=0;
	for (; i < Rels.size(); i++)
		if ( IsRusSubj (Rels[i]) )
  		  break;
		else
			/*
			   считаем, что копул прозрачен! 
			*/
			if (m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo].m_NodeType == Copul)
			{
				vector<long> Rels1; 
				GetOutcomingRelations(m_Relations[Rels[i]].m_TargetNodeNo, Rels1, false);
				long  j=0;
				for (; j < Rels1.size(); j++)
				  if ( IsRusSubj (Rels1[j]) )
  						break;
				if (j < Rels1.size()) 
					break;
			};

    if (i < Rels.size())
		return m_Relations[Rels[i]].m_TargetNodeNo;
	else 
		return -1;
}






void CRusSemStructure::FreeWordNo (long WordNo)
{
  for (long i=0; i < GetNodesSize(); i++)
	  ::FreeWordNo<CRusSemNode>(m_Nodes[i], WordNo);

  for (long i=0; i < m_Clauses.size(); i++)
	  m_Clauses[i].FreeWordNo(WordNo);
};








bool CRusSemStructure::IsPostSpecifAdjNode(long NodeNo) const
{
	CRelSet R = GetIncomingRelations(NodeNo, false);
	if (R.m_RelsCount != 1) return false;
return    (GetRelation(R.m_Rels[0])->m_Valency.m_RelationStr == "PROPERT")		   && (GetRelation(R.m_Rels[0])->m_SyntacticRelation == "уточн");
};




long  CRusSemStructure::InsertNode (CRusSemNode& N)
{
	if ( N.IsAbstract() )
		N.m_AbstractNodeId = m_Nodes.size();

	vector<CRusSemNode>::iterator It =  lower_bound (m_Nodes.begin(),m_Nodes.end(),N);

	vector<CRusSemNode>::const_iterator Inserted = m_Nodes.insert(It, N);

    long NodeNo = Inserted - m_Nodes.begin();


	if (m_Nodes[NodeNo].IsAbstract())
		if  (   ( NodeNo  > 0 )
			 && ( m_Nodes[NodeNo-1].IsAbstract() )
			 && (m_Nodes[NodeNo].m_ClauseNo == m_Nodes[NodeNo-1].m_ClauseNo)
			)
			m_Nodes[NodeNo].m_AbstractNodeId = m_Nodes[NodeNo-1].m_AbstractNodeId + 1;
		else
		  m_Nodes[NodeNo].m_AbstractNodeId  = 1;

	IncrementNodeRef(NodeNo, 1);

	

	for (long i=0; i < m_Clauses.size();i++)
	  if (i == N.m_ClauseNo)
	  {
		  if (m_Clauses[i].GetNodesCount() == 0)
		  {
			  m_Clauses[i].m_BeginNodeNo =  NodeNo;
			  m_Clauses[i].m_EndNodeNo = NodeNo +1;
		  }
		  else
			  m_Clauses[i].m_EndNodeNo ++;

		  if (m_Clauses[i].m_ClauseSyntaxTop != -1)
            if (m_Clauses[i].m_ClauseSyntaxTop >  NodeNo)
			   m_Clauses[i].m_ClauseSyntaxTop++;

	  }
	  else
		  if (m_Clauses[i].m_BeginNodeNo >= NodeNo)
		  {
			  m_Clauses[i].m_BeginNodeNo++;
			  if (m_Clauses[i].m_ClauseSyntaxTop != -1)
				  m_Clauses[i].m_ClauseSyntaxTop++;
			  m_Clauses[i].m_EndNodeNo++;
		  };


	return NodeNo;
};




void CRusSemStructure::AssertValidGraph()
{
	if (m_Nodes.empty()) return;

	CSemanticStructure::AssertValidGraph();

	for(long i = 0 ; i < m_Nodes.size() - 1; i++ )
	{
		assert ( m_Nodes[i] <  m_Nodes[i + 1]);
	}


	for (long i=0; i < m_Clauses.size(); i++)
	{
		assert (m_Clauses[i].m_BeginNodeNo < m_Clauses[i].m_EndNodeNo);
		for (long NodeNo = m_Clauses[i].m_BeginNodeNo;  NodeNo < m_Clauses[i].m_EndNodeNo; NodeNo ++)
		{
			assert ( NodeNo < m_Nodes.size());
			assert (m_Nodes[NodeNo].m_ClauseNo == i);
		};
	}
	for(long i = 0 ; i < m_Nodes.size(); i++ )
	{
		const CRusSemNode& N = m_Nodes[i];
		assert (N.m_ClauseNo != -1);

		// проверка, что индекс диатезы не выходит за пределы числа валентностей
		for (int j=0; j < N.m_Vals.size(); j++)
		{
			if (N.m_ValsLayout[j] >=  N.m_Vals.size() )
			{
				int u = 0;
			}
			assert (N.m_ValsLayout[j] < N.m_Vals.size());
		}
	}

}



long CRusSemStructure::FindFirstNode(long ClauseNo) const
{
	if (m_Clauses[ClauseNo].GetNodesCount() == 0) return -1;

	return  m_Clauses[ClauseNo].m_BeginNodeNo;
};



void    CRusSemStructure::EraseNode(int NodeNo) 
{ 
	for (long i=0; i <m_Clauses.size(); i++)
	{
		if (m_Clauses[i].m_ClauseSyntaxTop > NodeNo) 
			m_Clauses[i].m_ClauseSyntaxTop--;
		else
			if (m_Clauses[i].m_ClauseSyntaxTop == NodeNo) 
				m_Clauses[i].m_ClauseSyntaxTop = -1;

		if (m_Clauses[i].m_BeginNodeNo > NodeNo) m_Clauses[i].m_BeginNodeNo--;
		if (m_Clauses[i].m_EndNodeNo > NodeNo) m_Clauses[i].m_EndNodeNo--;

		assert (m_Clauses[i].m_BeginNodeNo <= m_Clauses[i].m_EndNodeNo);
		if (m_Nodes[NodeNo].IsWordContainer())
		{
			if (     (m_Nodes[NodeNo].GetMinWordNo() <= m_Clauses[i].m_RootWordNo)
				&&  (m_Clauses[i].m_RootWordNo <= m_Nodes[NodeNo].GetMaxWordNo())
				)
				m_Clauses[i].m_RootWordNo = -1;
			if (     (m_Nodes[NodeNo].GetMinWordNo() <= m_Clauses[i].m_RootWordNo)
				&&  (m_Clauses[i].m_RootWordNo <= m_Nodes[NodeNo].GetMaxWordNo())
				)
				m_Clauses[i].m_RootWordNo = -1;

			if (     (m_Nodes[NodeNo].GetMinWordNo() <= m_Clauses[i].m_PredicateWordNo)
				&&  (m_Clauses[i].m_PredicateWordNo <= m_Nodes[NodeNo].GetMaxWordNo())
				)
				m_Clauses[i].m_RootWordNo = -1;

			if (     (m_Nodes[NodeNo].GetMinWordNo() <= m_Clauses[i].m_SubjectWordNo)
				&&  (m_Clauses[i].m_SubjectWordNo <= m_Nodes[NodeNo].GetMaxWordNo())
				)
				m_Clauses[i].m_RootWordNo = -1;
		};

	};



	m_Nodes.erase(m_Nodes.begin() +NodeNo);
};



void			CRusSemStructure::SaveClausesNodeReferences() 
{ 
	for (long i=0; i <  m_Clauses.size(); i++) m_Clauses[i].SaveNodesReferences();
};
void			CRusSemStructure::RestoreClausesNodeReferences() 
{
	for (long i=0; i <  m_Clauses.size(); i++) m_Clauses[i].RestoreNodesReferences();
};

void			CRusSemStructure::PopClausesNodeReferences() 
{
	for (long i=0; i <  m_Clauses.size(); i++) m_Clauses[i].PopNodesReferences();
};

bool CRusSemStructure::FullGleiche(long NounNodeNo, long AdjNodeNo) const
{
	return  m_pData->GetRusGramTab()->GleicheGenderNumberCase 
						(	m_Nodes[NounNodeNo].m_TypeAncode.c_str(),  
							m_Nodes[NounNodeNo].m_GramCodes.c_str(),
							m_Nodes[AdjNodeNo].m_GramCodes.c_str()
						) > 0 ;

};



//====================================================================
//===================       TextPositions    ========================
//====================================================================

long CRusSemStructure::FindNodeByWordNo(long WordNo) const
{
 for (size_t i=0; i< m_Nodes.size(); i++)
	 for (size_t k=0; k< m_Nodes[i].m_Words.size(); k++)
		if (m_Nodes[i].m_Words[k].m_WordNo == WordNo)
			return i;
 return -1;
};

long CRusSemStructure::FindNodeByWordNo(long WordNo, long ClauseNo) const
{
 for (size_t i=m_Clauses[ClauseNo].m_BeginNodeNo; i<m_Clauses[ClauseNo].m_EndNodeNo; i++)
	 for (size_t k=0; k< m_Nodes[i].m_Words.size(); k++)
		if (m_Nodes[i].m_Words[k].m_WordNo == WordNo)
			return i;
 return -1;
};




void CRusSemStructure::GetAllTextOrderedNodes(vector <long>& Nodes) const
{
	Nodes.clear();

    for (size_t i=0; i <m_Nodes.size(); i++)
		if (    m_Nodes[i].IsWordContainer()  )
		   Nodes.push_back(i);

    sort(Nodes.begin(), Nodes.end(), IsLessByMinWordNo(this));
};




long   CRusSemStructure::GetDistance(size_t NodeNo1, size_t NodeNo2) const
{
	size_t StartWordNo, EndWordNo;
	FindClosestWords(NodeNo1,NodeNo2,StartWordNo, EndWordNo);
	return EndWordNo- StartWordNo;
};

void   CRusSemStructure::FindClosestWords(size_t NodeNo1, size_t NodeNo2, size_t& StartWordNo, size_t& EndWordNo) const
{
	long w1 = GetNode(NodeNo1).GetMinWordNo();
	long w2 = GetNode(NodeNo1).GetMaxWordNo();
	long w3 = GetNode(NodeNo2).GetMinWordNo();
	long w4 = GetNode(NodeNo2).GetMaxWordNo();
	if (w1 < w3)
	{
		 StartWordNo = w2;
		 EndWordNo = w3;
	}
	else
	{
		 StartWordNo = w4;
		 EndWordNo = w1;
	}

}




long CRusSemStructure::FindRightClosestNode(size_t NodeNo) const
{
 long ClosestNodeNo = -1;
 size_t MinGap = 10;

 for (size_t i=0; i< GetNodesSize(); i++)
   if (i!=NodeNo)
     if (GetNode(i).IsWordContainer() )
	 {
	   long value = GetNode(i).GetMinWordNo() - GetNode(NodeNo).GetMaxWordNo();
       if (( value < MinGap) && (value > 0))
	   {
	     MinGap = value; 
		 ClosestNodeNo = i;
	   };
	 };

 return ClosestNodeNo; 
};



string CRusSemStructure::GetMorphologyOfNode(long NodeNo) const 
{
  string Result =  Format ("%s = %s %s", 
	  GetNodeLemStr (NodeNo).c_str(),  
	  GetNodePosesStr(NodeNo).c_str(), 
	  m_pData->GetRusGramTab()->GrammemsToStr(GetNode(NodeNo).GetGrammems()).c_str()
	 );

  if (m_Nodes[NodeNo].m_MinActantWordNo != -1)
	  Result += Format("\nMinActantSynWordNo = %i", m_Nodes[NodeNo].m_MinActantWordNo);

  if (m_Nodes[NodeNo].m_MaxActantWordNo != -1)
	  Result += Format("\nMaxActantSynWordNo = %i", m_Nodes[NodeNo].m_MaxActantWordNo);

  if (m_Nodes[NodeNo].IsWordContainer())
  {
	  Result += Format("\nSynWordNo = %i", m_Nodes[NodeNo].m_Words[0].m_SynWordNo);
  }
  if ( m_Nodes[NodeNo].IsPrimitive() )
  {
	  Result += Format("\nWordWeight = %i", m_Nodes[NodeNo].m_Words[0].m_WordWeight);
  }
  return Result;
};

bool	CRusSemStructure::IsParenthesis (long NodeNo) const
{
	  return          (m_Nodes[NodeNo].GetType() != NoneRoss) 
			   && (   HasItem (m_Nodes[NodeNo].GetType(),m_Nodes[NodeNo].GetUnitNo(),"GF","ВВОДН", "D_PART_OF_SPEECH",0,0)
				   || HasItem (m_Nodes[NodeNo].GetType(),m_Nodes[NodeNo].GetUnitNo(),"GF","ВВОДН", "D_GROUPS",0,0)
				   || HasGramFetAfterColon (NodeNo, "ВВОДН")
				  );
};

void CRusSemStructure::CopyDopRelationsExceptAnaphor()
{

	for(int i = 0 ; i < m_DopRelations.size() ; i++ )
		if (    (m_DopRelations[i].m_SyntacticRelation != "анафора") 
			 && (m_DopRelations[i].m_SyntacticRelation != "ModalCopul") 
		   )
		{
			AddRelation(m_DopRelations[i]);
			m_Relations[m_Relations.size() - 1].m_bDopRelation = true;
		}		
	
}

/*
   функция разбирает строчку, в которой записан список разрешенных лексических
   вариантов, напрмер
	(1) { {"Росс:дом 1" 0} {"Росс:дом 2" 1}}
   В этом списке стоят две интерпретации 
	1)  статья из РОССа дом 1 
	2)  статья из РОССа дом 2 
*/
void  CRusSemStructure::ProcessAllowableLexVars (string LexVarsStr)
{
	m_UserProhibitedLexVars.clear();
	if (LexVarsStr.length() == 0) return;

	StringVector InterpsAndValue;
	long start = 0;
	long OpenBracketCount  = 0;
	long CloseBracketCount  = 0;
	for (long i=0; i < LexVarsStr.length(); i++)
	{
		if (LexVarsStr[i] == '{') OpenBracketCount++;
		if (LexVarsStr[i] == '}') CloseBracketCount++;
		if ( (OpenBracketCount == 2) && (CloseBracketCount == 2))
		{
		   while  ((start < LexVarsStr.length()) && (LexVarsStr[start] == ' ')) start++;
		   string s = LexVarsStr.substr(start+1, i - start - 1);
		   Trim(s);	
           InterpsAndValue.push_back(s);
		   start = i+1;
		   OpenBracketCount  = 0;
		   CloseBracketCount  = 0;
		};
	};
			
	for (long i=0; i < InterpsAndValue.size(); i++)
	{
		int pos = InterpsAndValue[i].find (" ");
		if (pos == -1) continue;
		CRossInterpAndLemma I;
		I.m_Lemma = InterpsAndValue[i].substr(0, pos);
		Trim(I.m_Lemma);
		InterpsAndValue[i].erase(0, pos+1);

		pos = InterpsAndValue[i].find_last_of (" ");
		if (pos == -1) continue;
		long State = atoi ( InterpsAndValue[i].substr(pos).c_str());
		if (State != 0) continue;
		string s = InterpsAndValue[i].substr(0, pos);
		Trim(s);
		if (s.length() == 0) continue;
		if (s[0] == '{') s.erase(0,1);
		if (s.length() == 0) continue;
		if (s[s.length()-1] == '}') s.erase(s.length()-1);

		pos = s.find (":");
		if (pos == -1) continue;
		string DictStr = s.substr (0, pos);
		
		I.m_DictType = m_pData->GetTypeByStr(DictStr);
		if (I.m_DictType == NoneRoss) continue;

		string UnitStr = s.substr (pos+1);
		Trim(UnitStr);
		if ( !isdigit((unsigned char)UnitStr[UnitStr.length() - 1])) continue;
		int MeanNum =  UnitStr[UnitStr.length() - 1] - '0';
		UnitStr.erase(UnitStr.length() - 1);
		Trim(UnitStr);
		I.m_UnitNo = GetRossHolder(I.m_DictType)->LocateUnit(UnitStr.c_str(), MeanNum);
		if (I.m_UnitNo == ErrUnitNo) continue;
		m_UserProhibitedLexVars.push_back(I);

	};
	
	
};


CRelSet CRusSemStructure::GetIncomingRelations (long NodeNo, bool UseUse) const 
{
  CRelSet R;
  WORD RelsCount = m_Relations.size();
  for (WORD i = 0; i < RelsCount; i++)
   if (!UseUse || m_Relations[i].m_bRelUse) 
     if (m_Relations[i].m_TargetNodeNo == NodeNo)
		 R.Add(i);
  return R;	
};



void CRusSemStructure::DeleteRelSet(CRelSet& R)
{
	if (R.m_RelsCount == 0) return;
	sort (R.m_Rels, R.m_Rels+R.m_RelsCount);
	for (long i=R.m_RelsCount - 1; i >= 0; i--)
			 EraseRelation(R.m_Rels[i]);
};




// проверяет, стоит ли после двоеточия в GF-главном ItemStr
// например, для GF = НАР:ВОПР  => HasGramFetAfterColon(i, "ВОПР")== true
bool CRusSemStructure::HasGramFetAfterColon (long NodeNo, string ItemStr)  const
{
  if (m_Nodes[NodeNo].GetType() == NoneRoss)  return false;
  StringVector GramFets;
  GetRossHolder(m_Nodes[NodeNo].GetType())->GetFullFieldItemsFromArticle (m_Nodes[NodeNo].GetUnitNo(), "GF", 0, 0, GramFets);
  if (GramFets.size() == 0) return false;
  for (long i=0; i < GramFets.size(); i++)
  {
     int c = GramFets[i].find_first_of(":");
	 string q = GramFets[i].substr(c+1);
	 Trim(q);
     if ((c != -1) && (q == ItemStr)) return true;
  };
  return false;
};




// возращает вектор номеров валентностей в том порядке, как они идут в тексте
// CRelSet& OutRelations - выходящие отношения, упорядоченные  по возрастанию  m_TargetNodeNo
// возвращаемый CRelSet содержит номера валентностей, которые выражают OutRelations  в том же порядке
CRelSet CRusSemStructure::GetValencyPlaces(long NodeNo, CRelSet& OutRelations) const 
{
	//валентности
	CRelSet Result;
	OutRelations.m_RelsCount = 0;
	const vector<CValency>& Vals = m_Nodes[NodeNo].m_Vals;
	if (Vals.empty())  return Result;
	if (m_Nodes[NodeNo].m_NodeType == MNA ) return Result;

	for (long i = 0;  i < m_Relations.size(); i++)
		if (    (m_Relations[i].m_SourceNodeNo == NodeNo)
			&&	m_Relations[i].m_bRelUse
			&&	!m_Relations[i].m_Valency.m_RelationStr.empty()
			&&	!m_Relations[i].m_bReverseRel
			)
		{
			vector<CValency>::const_iterator It = find(Vals.begin(),Vals.end(), m_Relations[i].m_Valency); 
			if (It != Vals.end())
			{
				// устанавливаем узел в последовательность Nodes
				long j =0;
				for (;j < OutRelations.m_RelsCount; j++)
					if (m_Relations[OutRelations.m_Rels[j]].m_TargetNodeNo >= m_Relations[i].m_TargetNodeNo)
						break;
				OutRelations.Insert(j, i);
				// добавляем валентность на то же место, что и узел
				Result.Insert(j, It - Vals.begin());
			};
		};
	assert (Result.m_RelsCount == OutRelations.m_RelsCount);
	return Result;
};

// выдаем все пропущенные валентности
void CRusSemStructure::GetValencyMisses(long NodeNo, vector<CValency>& ValencyMisses) const 
{
	CRelSet OutRels;
    CRelSet R = GetValencyPlaces(NodeNo, OutRels);

	for (size_t i =0; i < m_Nodes[NodeNo].m_Vals.size(); i++)
		if (find(R.m_Rels, R.m_Rels + R.m_RelsCount, i) ==  R.m_Rels + R.m_RelsCount)
			ValencyMisses.push_back(m_Nodes[NodeNo].m_Vals[i]); 
};


void CRusSemStructure::DelNode(long NodeNo)
{
  for (long k=0; k< m_ThesSemRelations.size(); k++)
  {

    if  (    (m_ThesSemRelations[k].m_TargetNodeNo == NodeNo)
		    || (m_ThesSemRelations[k].m_SourceNodeNo == NodeNo)
		 )
	 {
		 m_ThesSemRelations.erase(m_ThesSemRelations.begin() +k );
		  k--;
		  continue;
	 };


     if (m_ThesSemRelations[k].m_SourceNodeNo > NodeNo)    m_ThesSemRelations[k].m_SourceNodeNo--;
	 if (m_ThesSemRelations[k].m_TargetNodeNo > NodeNo) m_ThesSemRelations[k].m_TargetNodeNo--;
  };

  CSemanticStructure::DelNode(NodeNo);
};


bool CRusSemStructure::CheckAllIncomingRelations(long NodeNo, const string& RelationStr)  const
{
	CRelSet R = GetIncomingRelations(NodeNo,false);
	if (R.m_RelsCount == 0) return false;
	for (int i=0; i < R.m_RelsCount; i++)
	  if ( m_Relations[R.m_Rels[i]].m_Valency.m_RelationStr != RelationStr)
		return false;

	return true;
};


void  CRusSemStructure::FindIf(long ClauseNo,
			                bool (CRusSemStructure::*Fun)(long) const, 
					        vector <long>& Nodes) const
{
			Nodes.clear();
			for (long  i=m_Clauses[ClauseNo].m_BeginNodeNo; i <  m_Clauses[ClauseNo].m_EndNodeNo; i++)
			if ( ! m_Nodes[i].m_bToDelete )
				if ((this->*Fun) (i))
					Nodes.push_back(i);
};

// выдает омоним слова  WordNo для текущей морфологической интерпретации клаузы ClauseNo
bool   CRusSemStructure::GetHomonym (long ClauseNo, long WordNo, CSynHomonym& H) const
{
	long AbstrClauseNo = m_piSent->m_vectorPrClauseNo[ClauseNo];
	long ClausVar = m_Clauses[ClauseNo].m_ClauseVariantNo;
	bool bRes = m_piSent->GetHomonymByClauseVariantInterface(WordNo,ClausVar, AbstrClauseNo,H);
	if (!bRes)
		throw CExpc ("Cannot get homonym from syntax");
	return true;

};


// проверяет часть речи узла
bool   CRusSemStructure::HasRichPOS (const CRusSemNode& N, size_t POS) const
{
	return (N.m_RichPoses & (1<<POS)) > 0;
};

bool   CRusSemStructure::HasRichPOS (size_t NodeNo, size_t POS) const
{
	return (m_Nodes[NodeNo].m_RichPoses & (1<<POS)) > 0;
};

bool CRusSemStructure::IsVerbForm(const CRusSemNode& Node) const
{
	return  (   HasRichPOS (Node, VERB) 
			|| HasRichPOS (Node, ADVERB_PARTICIPLE) 
			|| HasRichPOS (Node, PARTICIPLE) 
			|| HasRichPOS (Node, PARTICIPLE_SHORT) 
			|| HasRichPOS (Node, INFINITIVE) 
			);
};

bool CRusSemStructure::IsInfinitive(long NodeNo) const 
{
	return  HasRichPOS (NodeNo, INFINITIVE) && m_Nodes[NodeNo].IsPrimitive();
};

bool CRusSemStructure::IsFiniteVerb(long NodeNo) const 
{
	return  HasRichPOS (NodeNo, VERB) && m_Nodes[NodeNo].IsPrimitive();
};

bool CRusSemStructure::IsModalCopul(long NodeNo) const 
{
	return  m_Nodes[NodeNo].m_NodeType == ModalCopul;
};


bool CRusSemStructure::IsImperativeVerb(long NodeNo) const
{
	return     m_Nodes[NodeNo].HasOneGrammem(rImperative)
			&& m_Nodes[NodeNo].IsPrimitive();
};

bool CRusSemStructure::IsPredicative(long NodeNo) const
{
	return     HasRichPOS (NodeNo, PREDK);
};

bool CRusSemStructure::CanBeDeleted(long NodeNo) const
{
	return m_Clauses[m_Nodes[NodeNo].m_ClauseNo].GetNodesCount() > 1;
};

bool CRusSemStructure::IsCompAdj (long NodeNo) const
{
	return      m_Nodes[NodeNo].IsPrimitive()
				&& m_Nodes[NodeNo].HasOneGrammem(rComparative)
				&& ( 
					    HasRichPOS (NodeNo, ADJ_FULL)
					||  HasRichPOS (NodeNo, NUMERAL)
				);
	/*
	Нам нужно считать сравнительные числительные  MUA, для того чтобы				фраза "Я хочу больше тебя" переводилась бы "I want more than you",
				а  не "I want bigger than you"
		*/
};
bool CRusSemStructure::IsPrep(long NodeNo) const 
{
	return  (   m_Nodes[NodeNo].IsPrimitive() 
				&& m_Nodes[NodeNo].m_Words[0].HasPOS (PREP)
			)
			|| (m_Nodes[NodeNo].GetType() == OborRoss);
};

bool CRusSemStructure::IsAdjFull(long NodeNo) const 
{
	return  HasRichPOS (NodeNo, ADJ_FULL);
};

bool CRusSemStructure::IsSimpleNounGroupUnderPrep(long NodeNo) const
{
	return      (      m_Nodes[NodeNo].IsPrimitive() 
					&& (
							m_Nodes[NodeNo].m_Words[0].HasPOS(NOUN) 
						|| m_Nodes[NodeNo].m_Words[0].HasPOS(PRONOUN) 
						|| m_Nodes[NodeNo].m_Words[0].m_ILE
						|| (m_Nodes[NodeNo].m_Words[0].m_Lemma == "КОТОРЫЙ")
						)
				)
					||  (m_Nodes[NodeNo].m_SynGroupTypeStr == NOUN_ADJ)
					||  (m_Nodes[NodeNo].m_SynGroupTypeStr == NOUN_NUMERAL)
					||  (m_Nodes[NodeNo].m_SynGroupTypeStr == NUMERAL_NOUN)
					||  (m_Nodes[NodeNo].m_SynGroupTypeStr == NUMERAL_ADVERB)
					||  (m_Nodes[NodeNo].m_SynGroupTypeStr == SELECTIVE_GR);
};


CRusSemNode		CRusSemStructure::CreateCopulNode(long ClauseNo)
{
	CRusSemNode N;
	N.m_NodeType = Copul;
	N.m_Vals.push_back (CValency ("F-ACT",  A_C,GetRossHolder(Ross)));
	N.m_Vals.push_back (CValency ("S-ACT",  A_C, GetRossHolder(Ross)));		  
	N.m_ClauseNo = ClauseNo;
	return N;
};

CRusSemNode	CRusSemStructure::CreateModalCopulNode(long ClauseNo)
{
	CRusSemNode N;
	N.m_NodeType =  ModalCopul;
	N.m_Vals.push_back (CValency ("SUB",  A_C,GetRossHolder(Ross)));
	N.m_Vals.push_back (CValency ("CONTEN",  A_C, GetRossHolder(Ross)));		  
	N.m_ClauseNo = ClauseNo;
	return N;
};


// ===================================================================
// =============   CRusSemNode =====================================
// ===================================================================




CRusSemNode::CRusSemNode() : CSemNode() 
{
		m_PrepWordNo = -1;
		m_SyntaxGroupNo = -1;
		m_bInCycle = false;
		for (size_t i=0; i < MaxValsCount; i++)
			m_ValsLayout.push_back(i);
		m_TerminId = -1;
		m_ThesaurusId = NoneThes;
		m_bFirstInTermin = false;
		m_bFullGleiche = false;
		m_bSaveIsParenthesis = false;
		m_bCompAdj = false;
		m_bCompWithOneActant  = false;
		m_MemNodeNo = -1;
		m_MinActantWordNo = -1;
		m_MaxActantWordNo = -1;
		m_IncomRelsCount = 0;
		m_RichPoses = 0;
		m_bConjWord = false;
		m_bQuestionWord = false;
};

bool	CRusSemNode::IsILENode() const 	
{
	return (m_MainWordNo != -1) &&  m_Words[m_MainWordNo].m_ILE; 
};

// проверяет, стоит ли после последнего слова узла знак препинания
bool CRusSemNode::HasPostPunct(BYTE punct_char) const 
{
	return (m_Words.size() > 0) && (m_Words[m_Words.size() - 1].m_PostPuncts.find(punct_char) != -1);
};
// проверяет, стоит ли сразу же после последнего слова узла запятая
bool CRusSemNode::HaveCommaAfter() const { return HasPostPunct(',');	};


// дает  номер статьи для текущей интерпреции для проверки SF
WORD         CRusSemNode::GetSemCorrectUnitNo() const 
{ 
	return (m_CurrInterp == -1)  ? ErrUnitNo : m_Interps[m_CurrInterp].GetSemCorrectUnitNo();
};

// проверяет, что валентности не являются несовместными
// (здесь используется слот m_RelationId, в котором хранится уникальный номер валентности
// в графе, который строит функция СRusSemStructure::IndexRelations)
bool CRusSemNode::RelsCanBeTogether(const long RelationId1,  const long RelationId2) const
{
	if (RelationId1 == RelationId2) return false;
	for (long i=0; i <ProhibitedPairOfVals.size(); i++)
		if (  (   (RelationId1 == m_Vals[ProhibitedPairOfVals[i].first].m_RelationId)
				&& (RelationId2 == m_Vals[ProhibitedPairOfVals[i].second].m_RelationId) 
			    )
			||(   (RelationId1 == m_Vals[ProhibitedPairOfVals[i].second].m_RelationId)
				&& (RelationId2 == m_Vals[ProhibitedPairOfVals[i].first].m_RelationId) 
			    )
	        )
			return false;
	return true;
};
// является ли данный узел кавычкой
bool   CRusSemNode::IsOneQuoteMark () const 
{
	return IsPrimitive() && GetWord(0).IsQuoteMark();
};

// является ли данный узел тире
bool   CRusSemNode::IsOneDash () const 
{
	return IsPrimitive() && (GetWord(0).m_Word == "-");
};

bool CRusSemNode::IsMNA_ButNotCHEM () const  
{
		return (m_NodeType == MNA) && (m_MNAType != CHEMOborot) ;
};
// сначала лежат IsWordContainer(),потом IsAbstract(), и для каждой клаузы отдельно
bool CRusSemNode::operator < (const CRusSemNode& Node) const
{
    if (m_ClauseNo  !=  Node.m_ClauseNo)
		return m_ClauseNo < Node.m_ClauseNo;

 	if ( Node.IsAbstract()  && IsWordContainer() )
		return true;

	if ( IsAbstract()  && Node.IsWordContainer() )
		return false;

	if ( IsAbstract() && Node.IsAbstract() )
		return m_AbstractNodeId  < Node.m_AbstractNodeId;

	return GetMinWordNo() < Node.GetMinWordNo();
}

bool CRusSemNode::IsAnaphoricPronoun () const
{
	return (     IsPrimitive() 
		&& (    (m_Words[0].m_Lemma == "ОН")
		    || (m_Words[0].m_Lemma == "ОНА")
            || (m_Words[0].m_Lemma == "ОНО")
			|| (m_Words[0].m_Lemma == "ОНИ")
			)
		);
};

QWORD CRusSemNode::GetInnerGrammems() const
{
	if (m_MainWordNo == -1)
		return GetGrammems();
	else
		return m_Words[m_MainWordNo].GetAllGrammems();
};

bool CRusSemNode::IsLemma(string Lemma) const 
{
	return IsPrimitive() && (m_Words[0].m_Lemma == Lemma);
};

bool CRusSemNode::IsWordForm(string WordForm) const 
{
	return IsPrimitive() && (m_Words[0].m_Word == WordForm);
};

bool CRusSemNode::IsLemmaList( const char*  FirstLemma, ... ) const
{
		if (!IsPrimitive()) return false; 
		if (m_Words[0].m_Lemma.empty()) return false;
	    va_list arglst;
		string CurrLemma = FirstLemma;
		va_start( arglst, FirstLemma);
		while( CurrLemma != "" )
		{
			if (CurrLemma == m_Words[0].m_Lemma)
				break;
			CurrLemma = va_arg( arglst, const char*);
		}
   	    va_end( arglst );
		return CurrLemma == m_Words[0].m_Lemma;
};

bool CRusSemNode::IsEqualMorph (const CRusSemNode& N) 	const 
{
	if (m_Words.size() != N.m_Words.size()) return false;
	for (long i=0; i < m_Words.size(); i++)
		if (!m_Words[i].IsEqualMorph(N.m_Words[i]))
			return false;
	return true;
};

void  CRusSemNode::SetMainWordNo (long WordNo)
{
	m_MainWordNo = WordNo;
	if (WordNo != -1)
		m_RichPoses = m_Words[WordNo].GetRusRichPoses();
	else
		m_RichPoses = 0;
};


//================================================

void CRusSemWord::Init() 
{
   	m_IsPunct = false;
  	m_ILE = false;
	m_bRomanNumber = false;
	m_SynWordNo = -1;
};

CRusSemWord::CRusSemWord() : CSemWord()
{
	Init();
};
CRusSemWord::CRusSemWord   ( long WordNo, string Lemma ) : CSemWord(WordNo, Lemma)
{
	Init();
};


bool CRusSemWord::IsReflexiveVerb () const 
{
	return  (   HasPOS (VERB) 
				|| HasPOS (INFINITIVE) 
				|| HasPOS (PARTICIPLE) 
				|| HasPOS (ADVERB_PARTICIPLE) 
			) 
			&& HasReflexiveSuffix(m_Word);
};

UINT CRusSemWord::GetRusRichPoses () const
{
    UINT Poses = m_Poses;

	if (m_Lemma == "ЛИ")
		Poses &= ~(1 << CONJ);

	if (    (m_Lemma == "ПЕРВОЕ")
		|| (m_Lemma == "ВТОРОЕ")
		|| (m_Lemma == "ТРЕТЬЕ")
		|| (m_Lemma == "ЧЕТВЕРТОЕ")
		)
		Poses |= (1 << NUMERAL_P);


	return Poses;
};

bool   CRusSemWord::HasPOS (size_t POS) const
{
	return  (GetRusRichPoses()  & 1<<POS) > 0;
};

bool CRusSemWord::IsEqualMorph  (const CRusSemWord& W) 	const
{
	return    (m_Lemma == W.m_Lemma)
			&& (m_Word == W.m_Word)
			&& (GetAllGrammems() == W.GetAllGrammems())
			&& (m_Poses == W.m_Poses);
};


//===========================

CRusMorphHomonym::CRusMorphHomonym() 
{
	m_SynWordNo = -1;
	m_ParadigmId = -1;
};

CRusMorphHomonym::CRusMorphHomonym(const CRusSemWord& X) 
{
	m_SynWordNo = X.m_SynWordNo;
	m_ParadigmId = X.m_ParadigmId;
};


//=========================
const string PossPronoun [] = {"НАШ", "ВАШ", "МОЙ", "ТВОЙ", "ЕЕ", "ЕГО", "ИХ"};
bool IsPossesivePronoun(const CSemNode& N)
{
	if (!N.IsPrimitive() ) return false;
    return find (PossPronoun, PossPronoun + 7, N.GetWord(0).m_Lemma ) != PossPronoun + 7;
};


bool HasReflexiveSuffix (const string& s) 
{
	if ( s.length () <  3)   return false;
	string suffix  = s.substr (s.length() - 2);
	EngRusMakeUpper(suffix);
	return   (    ( suffix == "СЯ")
				|| ( suffix == "СЬ")
				);
};


bool IsBetween (const CRusSemNode& Node, const CRusSemNode& LowerBound, const CRusSemNode& UpperBound)
{
	if (Node.IsAbstract()) return true;
	return      LowerBound.GetMinWordNo() < Node.GetMinWordNo()
			 && Node.GetMinWordNo()		  < UpperBound.GetMinWordNo();
};

TCortege GetSubjCortege (const CRossHolder* RossDoc)
{
          TCortege C;
		  C.m_FieldNo = RossDoc->GramFetFieldNo;
		  C.m_LeafId = 1;
		  C.m_LevelId = 0;
		  C.m_DomItemNos[0] = RossDoc->SubjSynONo;
		  C.m_DomItemNos[1] = RossDoc->NominativeNo;
		  return C;
};

TCortege GetInstrObj (const CRossHolder* RossDoc)
{
          TCortege C;
		  C.m_FieldNo = RossDoc->GramFetFieldNo;
		  C.m_LeafId = 2;
		  C.m_LevelId = 0;
		  C.m_DomItemNos[0] = RossDoc->IndirObjSynONo;
		  C.m_DomItemNos[1] = RossDoc->InstrumentalisNo;
		  return C;
};


// =====================  CRusSemClause
// освобождает номер слова WordNo (на это место будет поставлено новое слово)
void CRusSemClause::FreeWordNo (long WordNo)
{
	if (m_SubjectWordNo >= WordNo) m_SubjectWordNo++;
	if (m_PredicateWordNo >= WordNo) m_PredicateWordNo++;
	if (m_RootWordNo >= WordNo) m_RootWordNo++;
}

CRusSemClause::CRusSemClause() : CSemClause()
{
	m_ClauseVariantNo =0;
	m_ClauseSyntaxTop = -1;
	m_HostClause =-1;
	m_ClauseRuleNo = -1;
	m_SubjectWordNo = -1;
	m_AlreadyBuiltClauseVariantNo = -1;
	m_CurrLexVariantNo = -1;
	m_RootWordNo = -1;
	m_BeginNodeNo = 0;
	m_EndNodeNo = 0;
	m_bHasNumeralComp = false;
	m_bHasCHEM = false;
	m_BestPONodesCount = 0;
	m_bSaveClauseHasNegationInVerb = false;
};
// выдает число узлов клаузы
long CRusSemClause::GetNodesCount() const 
{ 
	return  m_EndNodeNo - m_BeginNodeNo;
}

void CRusSemClause::InitGramMatrix () 
{
	size_t size = GetNodesCount();
	if (size != m_GrammarMatrix.size())
	{
		m_GrammarMatrix.clear();
		m_GrammarMatrix.resize(size, vector<BYTE>(size, 0));
		return;
	};

	for (long i=0; i < size;i++)
		for (long k=0; k < size;k++)
			m_GrammarMatrix[i][k] = 0;
};
void CRusSemClause::UpdateSizeOfGramMatrix()
{
	size_t size = GetNodesCount();
	if (size == m_GrammarMatrix.size()) return;
	m_GrammarMatrix.resize(size, vector<BYTE>(size, 0));
	for (long i=0; i < size;i++)
		m_GrammarMatrix[i].resize(size, 0);
};

void CRusSemClause::AddFlagToGramMatrix (long NodeNo1, long NodeNo2, BYTE Value) 
{
	int nd1 = NodeNo1 - m_BeginNodeNo;
	int nd2 = NodeNo2 - m_BeginNodeNo;
	assert (nd1 < m_GrammarMatrix.size());
	assert (nd2 < m_GrammarMatrix.size());
	m_GrammarMatrix[nd1][nd2] |= Value;
	m_GrammarMatrix[nd2][nd1] |= Value;
};

// выдает значение позиционных оценок для пары узлов 
const BYTE CRusSemClause::GetGramMatrix (long NodeNo1, long NodeNo2) const 
{
	int nd1 = NodeNo1 - m_BeginNodeNo;
	int nd2 = NodeNo2 - m_BeginNodeNo;
	assert (nd1 < m_GrammarMatrix.size());
	assert (nd2 < m_GrammarMatrix.size());
	return m_GrammarMatrix[nd1][nd2];
};

// сохраняет и восстанавливает ссылки на узлы 
void CRusSemClause::SaveNodesReferences() 
{
	m_NodeRef.push(m_BeginNodeNo);
	m_NodeRef.push(m_EndNodeNo);
	m_NodeRef.push(m_ClauseSyntaxTop);
};

void CRusSemClause::RestoreNodesReferences() 
{
	m_ClauseSyntaxTop = m_NodeRef.top();
	m_NodeRef.pop();
	m_EndNodeNo = m_NodeRef.top();
	m_NodeRef.pop();
	m_BeginNodeNo = m_NodeRef.top();
	m_NodeRef.pop();
};

void CRusSemClause::IncrementNodesReferences(long Delta) 
{
	if (m_ClauseSyntaxTop != -1)
	m_ClauseSyntaxTop += Delta;
	m_EndNodeNo += Delta;
	m_BeginNodeNo += Delta;
};

void CRusSemClause::PopNodesReferences() 
{
	m_NodeRef.pop();
	m_NodeRef.pop();
	m_NodeRef.pop();
};
