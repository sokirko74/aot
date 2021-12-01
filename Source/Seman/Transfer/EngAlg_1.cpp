/////////////////////////////////////////////////////////////////////////////
// Английские алгоритмы 1
// общие утилиты
// GetChildNodes()
// GetAlgNodes()
// HasNeg()
// HasQuest()
// ChangeNegWord()
// CreateOneselfNodes()

// RefineEngCollocPreps()
// RefineComparativeMNAwithPreps()
// RefineUnknownRelsSynRel()
// CorrectNodeNumByRelNum()

// ApplyALG_AL1()

// CompareCortegeItems()

#include "StdAfx.h"


/////////////////////////////////////////////////////////////////////////////
// Выдает вектор дочерних нод с отслеживанием уникальности

void CEngSemStructure::GetChildNodes(int iNode,std::vector<long> &nodes) const
{
	std::vector<long> outRels;
	GetOutcomingRelations(iNode,outRels);
	if( outRels.size() < 1 )
		return;
	for( int i=0; i<outRels.size(); i++ )
	{
		if( m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo != 
			m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo )
			continue;
		nodes.push_back(m_Relations[outRels[i]].m_TargetNodeNo);
		GetChildNodes(m_Relations[outRels[i]].m_TargetNodeNo,nodes);
	}
	sort(nodes.begin(),nodes.end());
	std::vector<long>::iterator it = unique(nodes.begin(),nodes.end());
	int bad = nodes.size() - ( it-nodes.begin() );
	for( int i=0; i<bad; i++ )
		nodes.pop_back();
}

/////////////////////////////////////////////////////////////////////////////
// Выдает вектор узлов для корневых алгоритмов

void CEngSemStructure::GetAlgNodes(std::vector<long> &nodes) const
{
	nodes.clear();
	
	for( int i=0; i<m_Nodes.size(); i++ )
	{
		std::vector<long> inRels;
		GetIncomingRelations(i,inRels,false);
		bool bOk = true;
		for( int j=0; j<inRels.size(); j++ )
		{
			int iSrc = m_Nodes[m_Relations[inRels[j]].m_SourceNodeNo].m_ClauseNo;
			int iTrg = m_Nodes[m_Relations[inRels[j]].m_TargetNodeNo].m_ClauseNo;
			if( iSrc == iTrg )
			{
				bOk = false;
				break;
			}
		}
		if( bOk )
			nodes.push_back(i);
	}
}

/////////////////////////////////////////////////////////////////////////////
// HasNeg() определяет наличие 'Neg'

bool CEngSemStructure::HasNeg(long UnitNo, DictTypeEnum type )
{
	if( (UnitNo == ErrUnitNo) || (type == NoneRoss ) )
		return false;
	std::vector<TCortege> corteges;
	GetRossHolder(type)->GetFieldValues(std::string("GF"),UnitNo,corteges);
	bool bFound = false;

	if( corteges.size() == 0)
		return false;

	int i = 0;
	while( corteges[0].m_DomItemNos[i] != -1 )
	{
		std::string strVal = GetItemStr(corteges[0].m_DomItemNos[i],type);
		if( strVal.find("Neg") != std::string::npos )
			return true;
		i++;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// HasQuest() определяет наличие 'Quest'

bool CEngSemStructure::HasQuest(long UnitNo, DictTypeEnum type )
{
	if( (UnitNo == ErrUnitNo) || (type == NoneRoss ) )
		return false;
	std::vector<TCortege> corteges;
	GetRossHolder(type)->GetFieldValues(std::string("GF"),UnitNo,corteges);
	bool bFound = false;

	if( corteges.size() == 0)
		return false;

	int i = 0;
	while( corteges[0].m_DomItemNos[i] != -1 )
	{
		std::string strVal = GetItemStr(corteges[0].m_DomItemNos[i],type);
		if( strVal.find("Quest") != std::string::npos )
			return true;
		i++;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// ChangeNegWord() меняет отрицательное слово на положительное по LF

void CEngSemStructure::ChangeNegWord(int iEngNode)
{
	if( m_Nodes[iEngNode].GetType() == NoneRoss)
		return;

	std::vector<CLexicalFunctionField> LexFuncts;
	GetRossHolder(m_Nodes[iEngNode].GetType())->GetLexFuncts(m_Nodes[iEngNode].GetUnitNo(), LexFuncts, EngObor, GetRossHolder(EngObor));

	for( int i=0; i<LexFuncts.size(); i++ )
	{
		if( LexFuncts[i].m_LexFunct == "Anti" )
		{
			  std::string UnitStr = LexFuncts[i].m_Lemma;
			  EngRusMakeLower(UnitStr);
			  BYTE MeanNum = ( LexFuncts[i].m_MeanNum != -1)  ? LexFuncts[i].m_MeanNum : 1;
			  uint16_t UnitNo = GetRoss(m_Nodes[iEngNode].GetType())->LocateUnit(UnitStr.c_str(),MeanNum);
			  if( UnitNo == ErrUnitNo )	
				  continue;
			  CDictUnitInterp interp;
			  interp.m_DictType = m_Nodes[iEngNode].GetType();
			  interp.m_UnitNo = UnitNo;
			  CEngSemNode newNode = m_Nodes[iEngNode];
			  newNode.m_Words.clear();
			  CreateEnglNode(interp, newNode,RusStr.GetNode(m_Nodes[iEngNode].RusNode));	
			  m_Nodes[iEngNode] = newNode;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::ChangeClauseNo(int oldClauseNo,int newClauseNo)
{
	for( int i=0; i<m_Nodes.size(); i++ )
		if( m_Nodes[i].m_ClauseNo == oldClauseNo)
			m_Nodes[i].m_ClauseNo = newClauseNo;
}

/////////////////////////////////////////////////////////////////////////////
// создает #oneself - "он написал себе письмо"
// см. "l:\documents\one's и oneself"

void CEngSemStructure::CreateOneselfNodes()
{
// создание
	for( int i=0; i<m_Nodes.size(); i++ )
	{
		for( int k=0; k<m_Nodes[i].m_Words.size(); k++ )
		{
			if( m_Nodes[i].m_Words[k].m_OneselfStr.empty() )
				continue;

			CEngSemNode newNode;
			CRossInterp OneselfInterp(Aoss,GetRoss(Aoss)->LocateUnit(m_Nodes[i].m_Words[k].m_OneselfStr.c_str(),1));
			CreateSimpleEnglNodeWithInterp(OneselfInterp, newNode);

			newNode.RusNode = -1;
			newNode.m_ClauseNo = m_Nodes[i].m_ClauseNo;

			long wNo = m_Nodes[i].GetMinWordNo()+1;
			FreeWordNo(wNo);
			newNode.m_Words[0].m_WordNo = wNo;

			m_Nodes.push_back(newNode);

			CEngSemRelation newRel(CValency("#oneself",A_C,GetRossHolder(Aoss)),i,m_Nodes.size()-1,"obj");
			m_Relations.push_back(newRel);
		}
	}

	// проведение доп. связи THESAME	
	for( int NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++ )
	{
		if( !( m_Nodes[NodeNo].IsPrimitive() )
			|| !(  (m_Nodes[NodeNo].m_Words[0].m_Lemma == "#oneself")
				|| (m_Nodes[NodeNo].m_Words[0].m_Lemma == "#one's")
				)
		  )
			continue;

		// если в русской структуре в отношениях были прописана стрелка DEGREE ( пассивная валентность "сам"),		
		// тогда нужно взять эту стрелку и успокоиться
		// например,  "сам он этого не сделает"
		if( m_Nodes[NodeNo].RusNode != -1 )
		{
			std::vector<long> Rels;
			RusStr.GetIncomingRelations(m_Nodes[NodeNo].RusNode, Rels, false);
			long i=0;
			for (; i < Rels.size();i++)
			  if  (   (RusStr.GetRelation(Rels[i])->m_bReverseRel)
					&& (RusStr.GetRelation(Rels[i])->m_Valency.m_RelationStr == "DEGREE")				  
					)
				  break;

            if (i <Rels.size())
			{
				long parent = GetEngNodeByRusNode(RusStr.GetRelation(Rels[i])->m_SourceNodeNo);
				if (parent != -1)
				m_DopRelations.push_back(CEngSemRelation(CValency("THESAME", A_C),  NodeNo,  parent,  ""));				
				continue;
			};
		};

		if (GetEquNode(NodeNo) != -1) continue;
		// в противном случае, нужно  искать узел, у которого  есть субъектная стрелка
	    int parent = NodeNo;
		// есть ли среди хозяев узла хотя бы один инфинитив
		bool bHasParentInfinitive = false;
		while(true)
		{
			std::vector<long> Rels;
			GetIncomingRelations(parent, Rels);
			if (Rels.size() == 0) break;
			int rel = Rels[0];
			parent = m_Relations[rel].m_SourceNodeNo;
			bHasParentInfinitive |= is_infinitive(m_Nodes[parent].GetTense());

			// ищем первый узел, у которго есть субъектная стрелка доходим до первого не инфинитива
		    if( GetSubj(parent) != -1 )
				break;
		}

        if( parent != NodeNo )
		{
			long AntecedentNodeNo = GetSubj(parent);
			if (bHasParentInfinitive)
			{
				std::vector<long> Rels;
				GetOutcomingRelations(parent,Rels);
				for (long i=0; i< Rels.size(); i++)
				if  (    (m_Relations[Rels[i]].m_Valency.m_RelationStr == "C-AGENT")
						|| (m_Relations[Rels[i]].m_Valency.m_RelationStr == "OBJ")
						|| (m_Relations[Rels[i]].m_Valency.m_RelationStr == "ADR")						
					)
					if (IsObjPattern(m_Relations[Rels[i]]))
					 if (		(m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo].GetGrammemsRich() & _QM(eAnimative))
							||	HasSemFet(m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo],"ANIM")							
						)
							AntecedentNodeNo = m_Relations[Rels[i]].m_TargetNodeNo;
			}

			if( AntecedentNodeNo != -1 )
				m_DopRelations.push_back(CEngSemRelation(CValency("THESAME", A_C),NodeNo,AntecedentNodeNo,""));		}
	}
}

/*
 RefineEngCollocPreps()
 при наличии PREP = устанавливает этот предлог а все иное стирает
 PREP(i) игнорируются.
 Предполагается, что словосочетания еще не были разделены на разные узлы.
*/

void  CEngSemStructure::RefineEngCollocPreps()
{
	for( int i=0; i<m_Nodes.size(); i++ )
	{
		DictTypeEnum type = m_Nodes[i].GetType();
		if( type!=EngCollocRoss )
			continue;
		int unit = m_Nodes[i].GetUnitNo();
		std::vector<TCortege> vec;
		for( int j=GetRoss(type)->GetUnitStartPos(unit); j<=GetRoss(type)->GetUnitEndPos(unit); j++ )
		{
			TCortege C = GetCortege(GetRoss(type),j);
			if (C.m_BracketLeafId > 0) continue;
			if (C.m_LeafId > 0) continue;
			std::string field = (const char*)GetRoss(type)->Fields[C.m_FieldNo].FieldStr;
			if(	field == "PREP" )
			{
				vec.push_back(C);
				break;
			}
		}
		if( vec.size()<1 )
			continue;
		std::string value = GetCortegeStr(type,vec[0]);
		if (value == "-")
			m_Nodes[i].m_SynReal.m_Preps.clear();
		else
		   SetSimpleEngPrep(value,i,-1);
		
		std::vector<long> inRels;
		GetIncomingRelations(i,inRels,false);
		for( int k=0; k<inRels.size(); k++ )
		{
			if( m_Relations[inRels[k]].m_Valency.m_RelationStr != "THESAME" )			{
				if (value == "-")
				  m_Relations[inRels[k]].m_SynReal.m_Preps.clear();
		        else
				  SetSimpleEngPrep(value,-1,inRels[k]);
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// RefineComparativeMNAwithPreps()
// разрешает ситуацию с ComparativeMNA и добавляет than

void  CEngSemStructure::RefineComparativeMNAwithPreps()
{
	PrintRelations();
	for( int i=0; i<m_Nodes.size(); i++ )
	{
		if( m_Nodes[i].m_NodeType != MNA )
			continue;
		int iRusNode = m_Nodes[i].RusNode;
		if( iRusNode == -1 )
			continue;
		int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
		if( iSemMainWord == -1 )
			continue;
		const CSemNode& rusNode = RusStr.GetNode(iRusNode);
		if( !rusNode.GetWord(iSemMainWord).HasOneGrammem(rComparative) )
			continue;

		if( m_Nodes[i].m_NodeType = SimpleNode )
			continue;

		std::vector<long> outRelsRaw;
		GetOutcomingRelations(i,outRelsRaw);

		std::vector<long> outRels;
		for( int j=0; j<outRelsRaw.size(); j++ )
		{
			if( m_Relations[outRelsRaw[j]].m_Valency.m_RelationStr == "" &&
				m_Relations[outRelsRaw[j]].m_SyntacticRelation == "" )
			{
				outRels.push_back(outRelsRaw[j]);
			}
		}

		if( outRels.size() < 1 )
			continue;

		int iRelThan = outRels[0];
		int iWord0 = RusStr.GetNode(m_Nodes[m_Relations[iRelThan].m_TargetNodeNo].RusNode).GetMinWordNo();
		for( int k=1; k<outRels.size(); k++ )
		{
			int iWord1 = RusStr.GetNode(m_Nodes[m_Relations[outRels[k]].m_TargetNodeNo].RusNode).GetMinWordNo();
			if( iWord1>iWord0 )
			{
				iRelThan = outRels[k];
				iWord0 = iWord1;
			}
		}

		m_Relations[iRelThan].m_Position = ">";
		m_Relations[iRelThan].m_PosType = FromAlgorithmPosType;
		SetSimpleEngPrep("than", m_Relations[iRelThan].m_TargetNodeNo,iRelThan);
	}
}

/////////////////////////////////////////////////////////////////////////////
// RefineUnknownRelsSynRel()

void CEngSemStructure::RefineUnknownRelsSynRel()
{
	for( int i=0; i<m_Relations.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[i].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			continue;
		if( m_Relations[i].m_bInterpreted )
			continue;
		if( m_Relations[i].m_SynReal.m_Preps.empty() )
			continue;
		if( !m_Relations[i].m_Position.empty() )
			continue;
		m_Relations[i].m_SyntacticRelation = "indir_obj";
	}
}

/////////////////////////////////////////////////////////////////////////////
// CorrectNodeNumByRelNum()

void CEngSemStructure::CorrectNodeNumByRelNum()
{
	for( int i=0; i<m_Relations.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[i].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			continue;
		if( !m_Relations[i].m_bInterpreted )
			continue;

		int iNode1,iNode2;
		if( m_Relations[i].m_bReverseRel )
		{
			iNode1 = m_Relations[i].m_TargetNodeNo;
			iNode2 = m_Relations[i].m_SourceNodeNo;
		}
		else
		{
			iNode1 = m_Relations[i].m_SourceNodeNo;
			iNode2 = m_Relations[i].m_TargetNodeNo;
		}

		CEngSemNode& Node1 = m_Nodes[iNode1];
		CEngSemNode& Node2 = m_Nodes[iNode2];

		if( Node2.GetType()!=NoneRoss && Node2.GetUnitNo()!=ErrUnitNo &&
			GetRossHolder(Node2.GetType())->HasFieldValue("RESTR",_R("ед"),Node2.GetUnitNo()) )
		{
			if( Node1.m_MainWordNo!=-1 && Node1.m_Words[Node1.m_MainWordNo].m_Lemma=="many" )
			{
				CEngSemNode newNode;
				CreateSimpleEnglNodeByOldNode("much",newNode,0,false,Node1);
				m_Nodes[iNode1] = newNode;
			}
		}
		else
		{
			if( m_Nodes[iNode2].m_MainWordNo == -1 )
				continue;
			CEngSemWord &word = m_Nodes[iNode2].m_Words[m_Nodes[iNode2].m_MainWordNo];

			if( IsGramFet(m_Relations[i],"NP_sg") )
			{
				word.SetFormGrammems((word.GetFormGrammems() & ~eAllNumbers) | _QM(eSingular));
				continue;
			}
			if( IsGramFet(m_Relations[i],"NP_pl") )
			{
				word.SetFormGrammems((word.GetFormGrammems() & ~eAllNumbers) | _QM(ePlural));
				continue;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ApplyALG_AL1()
// обработка пассивных глаголов

void CEngSemStructure::ApplyALG_AL1(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode==-1 )
		return;
	if( !RusStr.GetNode(iRusNode).IsPassiveVerb() )
		return;

	// если слову приписан алгоритм AL1, тогда пассивизацию  делать не надо 
	// Дверь открылась -> The door opened 
	if( HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL1") )
		return;

	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;

	CEngSemWord& engWord = m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo];
	engWord.m_bMorphologicalPassiveForm = true;
}

/////////////////////////////////////////////////////////////////////////////
// CompareCortegeItems()
// это появилось из-за ">> Х! : ИГ"

bool CEngSemStructure::CompareCortegeItems(const CRossHolder* RossHolder,
										   const TCortege &X,const TCortege &Y) const
{
	int iX = 0;
	while( RossHolder->IsPosition(X.m_DomItemNos[iX]) && iX<10 )
		iX++;
	int iY = 0;
	while( RossHolder->IsPosition(Y.m_DomItemNos[iY]) && iY<10 )
		iY++;

	while( iX<10 && iY<10 )
	{
		if( X.m_DomItemNos[iX] != Y.m_DomItemNos[iY] )
			return false;

		if (X.m_DomItemNos[iX] == -1) 
			break;

		iX++;
		iY++;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
