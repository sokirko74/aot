#include "StdAfx.h"

// ищет потомка глагола, для которого ГХi=obj
long CEngSemStructure::FindDirObj(long VerbNodeNo) const
{
	vector<long> rels;
	GetOutcomingRelations(VerbNodeNo,rels);
	for(int i = 0; i < rels.size(); i++){
		const CEngSemRelation &rel = m_Relations[rels[i]];
		if(rel.m_Type != NotBetweenClauses) continue;
		if(rel.m_Position != "") continue;
		if(IsObjPattern(rel) && !IsSubj(rel)){
			return rel.m_TargetNodeNo;
		}
	}
	return -1;
}




bool CEngSemStructure::dir_obj_is_long(int NodeNo) const
{
	if( m_Nodes[NodeNo].m_NodeType == MNA )
		return true;

// выходящие межклаузные связи
	vector<long> outRels;
	GetOutcomingRelations(NodeNo,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			return true;
	}

// много детей
	vector<long> outChilds;
	GetChildNodes(NodeNo,outChilds);
	if( outChilds.size()>=3 )
		return true;

    return false;
}

CAdvPosType get_adv_pos_kind(const CEngSemNode& Node)
{
	WORD unit_no = Node.GetUnitNo();
	DictTypeEnum ross = Node.GetType();

	if(ross == TimeRoss || Node.IsTimeRossNode()) return adv_time;

	assert(!Node.IsAbstract());
	assert(Node.m_Words.size() >= 1);
	string foo;
	const CEngSemWord &word = Node.m_Words[0];
	if(unit_no == ErrUnitNo) return adv_other;

	const CRossHolder* RossDoc = word.GetRossHolder(Node.GetType());
	if (RossDoc == 0) return adv_other;
	if(RossDoc->HasFieldValue("GF", "ADVERB:DEGREE", unit_no)) return adv_degree;
	if(RossDoc->HasFieldValue("GF", "ADVERB:FREQ",unit_no)) return adv_freq;
	if(RossDoc->HasFieldValue("GF", "ADVERB:MANNER",unit_no)) return adv_manner;
	if(RossDoc->HasFieldValue("GF", "ADVERB:PLACE",unit_no)) return adv_place;
	if(RossDoc->HasFieldValue("GF", "ADVERB:SENT",unit_no)) return adv_sent;
	if(RossDoc->HasFieldValue("GF", "ADVERB:TIME",unit_no)) return adv_time;
	if(RossDoc->HasFieldValue("GF", "ADVERB:Quest",unit_no)) return adv_quest;
	if(RossDoc->HasFieldValue("GF", "ADVERB:Neg",unit_no)) return adv_neg;
	return adv_other;
}




void EnrichPositionsByAdvKind (CAdvPosType adv_kind, StringVector& Positions)
{
	switch(adv_kind){
		case adv_degree:
			if(!_find(Positions, "VA"))
			{
				Positions.push_back("AV");
				Positions.push_back("НAV");
			};
			break;
		case adv_freq:
			if (Positions.size() == 0)
			{
				Positions.push_back("BA");
				Positions.push_back("НAV");
				Positions.push_back("AV");
				Positions.push_back("HAB");
			};
			break;
		case adv_manner:
			if (Positions.size() == 0)
			{
				Positions.push_back("VA");
				Positions.push_back("VDA");
				Positions.push_back("VPDA");
				Positions.push_back("HAV");
			};
			break;
		case adv_place:
			if (Positions.size() == 0)
			{
				Positions.push_back("VOA");
				Positions.push_back("VA");
			};
			break;
		case adv_quest:
			if (Positions.size() == 0)
			{
				Positions.push_back("begin");
			};
			break;

	};
};


void CEngSemStructure::HandleAdverbPositions(long NodeNo)
{
	vector<long> adv_rels;
	vector<long> rels;
	GetOutcomingRelations(NodeNo,rels);
	long DirObjNodeNo = FindDirObj(NodeNo);

	for(int i = 0; i < rels.size(); i++)
	{
		const CSemRelation &rel = m_Relations[rels[i]];
		int n = rel.m_TargetNodeNo;
		const CEngSemNode &node = m_Nodes[n];
		bool ok = false;
		if(node.GetType() == TimeRoss || node_is_adv(node))
			ok = true;
		else if(node.IsTimeRossNode())
			ok = true;
		else if(		(node.GetType()==EngObor) 
					||	(node.GetType()==EngCollocRoss)  // TITLЕ = in_opinion			    
				)
			ok = true;
		if(n == DirObjNodeNo) ok = false;
		if(IsSubj(rel)) ok = false;
		if(ok)
			adv_rels.push_back(rels[i]);
	}

	bool is_be = m_Nodes[NodeNo].IsPrimitive() && m_Nodes[NodeNo].m_Words[0].m_Lemma == "be";
	bool has_obj = DirObjNodeNo != -1 && !dir_obj_is_long(DirObjNodeNo);
	bool has_long_obj = DirObjNodeNo != -1 && !has_obj;
	bool has_prep_obj = DirObjNodeNo != -1 && m_Nodes[DirObjNodeNo].HasSomePrep();



	for(int i = 0; i < adv_rels.size(); i++)
	{
		int n = m_Relations[adv_rels[i]].m_TargetNodeNo;
		const CEngSemNode &node = m_Nodes[n];
		const CSemWord &w = node.m_Words[0];
		long InRelNo = adv_rels[i];
		
		DictTypeEnum type = node.GetType();
		WORD         unit = node.GetUnitNo();
		
		if(node.IsTimeRossNode())
		{
			type = TimeRoss;
			unit = node.m_Colloc.GetRossInterp().m_UnitNo;
		}


		bool is_clause_beg = false;
		bool is_clause_end = false;

		if(node.RusNode != -1)
		{
			int  w_pos = RusStr.GetNode(node.RusNode).GetWord(0).m_WordNo;
			int cl_no = RusStr.GetNode(node.RusNode).m_ClauseNo;
			is_clause_beg = w_pos != -1 && GetClauseFirstWordNo(cl_no)== w_pos;
		    is_clause_end = w_pos != -1 && GetClauseLastWordNo(cl_no) == w_pos;
		}

		CAdvPosType adv_kind = get_adv_pos_kind(node);

		// загружаем поле POS  из словарной статьи
		StringVector Positions;
		if( type != NoneRoss && unit != ErrUnitNo )
			GetRossHolder(type)->GetSimpleFieldItemsFromArticle(unit, "POS", 0, 0, Positions);

		EnrichPositionsByAdvKind (adv_kind, Positions);
		
		if(	   _find(Positions, "begin")
			|| _find(Positions, "end")
		  )
		  
		{
			/*
			  если это наречие подчинено прилагательному, например: 
			  "оба думали о том , как велика эта страна" -> ",,, how large"
			  тогда нужно перевегивать вместе с прилагательным
			*/ 
			if (   (m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].GetPos() == eADJ)
			    && (m_Nodes[m_Relations[InRelNo].m_TargetNodeNo].GetPos() == eADV)
			   )
			{
				vector<long>  Rels;
				GetIncomingInClauseRelations(m_Relations[InRelNo].m_SourceNodeNo, Rels);
				if (Rels.size()  == 1)
					InRelNo = Rels[0];
				else
					InRelNo = -1;
			};

			if (InRelNo != -1)
			{
				// перевешиваем узел на главный узел клаузы
				//Gri по-видимому, он хороший преподаватель
				long RootNodeNo = GetLeftClauseTop(m_Nodes[m_Relations[InRelNo].m_SourceNodeNo].m_ClauseNo);
				if  (RootNodeNo != -1 )
				{
					m_Relations[InRelNo].m_SourceNodeNo = RootNodeNo;
					m_Relations[InRelNo].m_Position = (_find(Positions, "end")) ? "end" : "begin";
				};
			}
		}
		else 
		{
			if(!(    (m_Nodes[NodeNo].GetPos()==eVERB) 
				  || (m_Nodes[NodeNo].GetPos()==eMOD) 
				  || (m_Nodes[NodeNo].GetPos()==eVBE) 
				  || (   (m_Nodes[NodeNo].GetPos()==eADJ)  // he is clever enough
				       && HasBeVerb(NodeNo)
				     )
			     )
			  )
			continue;

			/*
				Позицию для реверсивных отношений нужно писать наоборот.
				Предполагается, что здесь обрабатывабтся только реверсивные отношения.
			*/
			if(_find(Positions, "VOA") && !is_be && has_obj )
			{
				// перевешиваем на прямое дополнение
				m_Relations[InRelNo].m_SourceNodeNo =  DirObjNodeNo;
				m_Relations[InRelNo].m_Position = ">";
			}
			else if(_find(Positions, "VA") && !is_be )
			{
				m_Relations[InRelNo].m_Position =  "<<";
			}
			else if(_find(Positions, "AB") && is_be)
			{
				m_Relations[InRelNo].m_Position = ">>";
			}
			else if(_find(Positions, "BA") && is_be)
			{
				m_Relations[InRelNo].m_Position = "<<";
			}
			else if(_find(Positions, "AV") && !is_be )
			{
				m_Relations[InRelNo].m_Position = ">";
			}
		}
	}
}
