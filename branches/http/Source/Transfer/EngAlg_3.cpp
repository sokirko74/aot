/////////////////////////////////////////////////////////////////////////////
// Английские алгоритмы 3

// ApplyComparativeRule()
// ApplyAdjShortRule()
// ApplyPredicativeRule()
// ApplyTwoNegationsRule()

// ImpersonalVerb()
// InfinitiveAndIf()

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// ApplyComparativeRule()
// золото и серебро красивее и дороже бронзы
// в 2000 году бюджетные ассигнования и гранты будут в два раза больше, чем в прошлом году
// я люблю Петю больше, чем Васю
// я пел в два раза дольше тебя
// --> определить subj и obj, учитывая возможность MUA//		создать be и привесить к нему сomparativ'ы, учитывая возможность MUA//		перевесить subj к be

void CEngSemStructure::ApplyComparativeRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;

	const CSemNode& rusNode = RusStr.GetNode(iRusNode);
	if( rusNode.m_NodeType != MNA )
		return;
	if( ! rusNode.GetWord(iSemMainWord).HasOneGrammem(rComparative) )
		return;

// у меня больше денег, чем у тебя
	if( m_Nodes[iEngNode].HasPOS(eVBE) )
		return;



// будем искать Subj
	vector<long> inRels;
	GetIncomingRelationsInThisClause(iEngNode,inRels);
	if( inRels.size() > 1 )
		return; // это нечто, чего мы не понимаем


	PrintNodes();
	PrintRelations();

	/*
     IsSimilarAdjCase - истина, если обрабатывается однородный ряд прилагательных, например:
	 "золото было красивее и дороже серебра"
	*/
	bool IsSimilarAdjCase =    (inRels.size() == 1)
							&& (m_Nodes[m_Relations[inRels[0]].m_SourceNodeNo].m_NodeType == MNA);
	int iRelSubj = -1;
	int iEnode, iRnode, iRword;
	if( inRels.size()==1 )
	{
		iEnode = m_Relations[inRels[0]].m_SourceNodeNo;
		iRnode = m_Nodes[iEnode].RusNode;
		if( iRnode==-1 )
			return;
		iRword = RusStr.GetNode(iRnode).m_MainWordNo;
		if( iRword==-1 )
			return;
		// если есть (единственная) входящая связь от сущ., 
		//  то будем считать Subj это сущ. или его MUA родителя		
		if(    RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(NOUN) 
			|| RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(PRONOUN) 
			
		 )
		{
			iRelSubj = inRels[0];
			vector<long> inRelsSubj;
			GetIncomingRelationsInThisClause(iEnode,inRelsSubj);
			if( inRelsSubj.size() > 1 )
				return; // это нечто, чего мы не понимаем

			if( inRelsSubj.size()==0 )
			{
				ReverseRelation(iRelSubj);
				m_Relations[iRelSubj].m_bReverseRel = false;
				m_Relations[iRelSubj].m_SynReal.m_Preps.clear();

				// приписываем отношению первую валентность глагола to be
				m_Relations[iRelSubj].m_Valency = m_Nodes[iEngNode].m_Vals[0];
				m_Relations[iRelSubj].m_SynReal.m_Cortege = m_Nodes[iEngNode].m_Patterns[0].m_GramCorteges[0];
			}
			else if( inRelsSubj.size()==1 )
			{
				//  Случай однородных подлежащих
				int iSubjMNA = m_Relations[inRelsSubj[0]].m_SourceNodeNo;
				if( m_Nodes[iSubjMNA].m_NodeType != MNA )
					return; // это нечто, чего мы не понимаем
				m_Relations[iRelSubj].m_SourceNodeNo = iEngNode;
				m_Relations[iRelSubj].m_TargetNodeNo = iSubjMNA;

				// приписываем отношению первую валентность глагола to be
				m_Relations[iRelSubj].m_Valency = m_Nodes[iEngNode].m_Vals[0];
				m_Relations[iRelSubj].m_SynReal.m_Cortege = m_Nodes[iEngNode].m_Patterns[0].m_GramCorteges[0];
			}
		}
		else if(   RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(VERB) 
				|| RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(ADJ_SHORT) 
				|| RusStr.GetNode(iRnode).m_NodeType==MNA 
			   )
		{
		}
		else
		{
			return; // это нечто, чего мы не понимаем
		}
	}

// ищем субъект (неименованные связи)
	vector<long> outRels;
	if( iRelSubj==-1 )
	{
		vector<long> outRelsRaw;
		GetOutcomingRelations(iEngNode,outRelsRaw);

		for( int j=0; j<outRelsRaw.size(); j++ )
		{
			if( m_Relations[outRelsRaw[j]].m_Valency.m_RelationStr == "" &&
				m_Relations[outRelsRaw[j]].m_SyntacticRelation == "" )
			{
				outRels.push_back(outRelsRaw[j]);
			}
		}

		if( outRels.size() < 1 )
			return;
		iRelSubj = outRels[0];
		int iWord0 = RusStr.GetNode(m_Nodes[m_Relations[iRelSubj].m_TargetNodeNo].RusNode).GetMinWordNo();
		for( int k=1; k<outRels.size(); k++ ) // возможен неверный порядок
		{
			int iWord1 = RusStr.GetNode(m_Nodes[m_Relations[outRels[k]].m_TargetNodeNo].RusNode).GetMinWordNo();
			if( iWord1<iWord0 )
			{
				iRelSubj = outRels[k];
				iWord0 = iWord1;
			}
		}
	}

// проверим Subj
	CEngSemNode& engSubj = m_Nodes[m_Relations[iRelSubj].m_TargetNodeNo];
	int iRusSubj = engSubj.RusNode;
	if( iRusSubj == -1 )
		return;
	int iSemSubjWord = RusStr.GetNode(iRusSubj).m_MainWordNo;
	if( iSemSubjWord == -1 )
		return;
// Subj Ok

	int iNodeMNA = iEngNode;
	int iNodeTim = iEngNode;
// золото было красивее и дороже серебра
	if(IsSimilarAdjCase)
	{
		int iParent = m_Relations[inRels[0]].m_SourceNodeNo;
		int iParRus = m_Nodes[iParent].RusNode;
		const CSemNode& rusNode = RusStr.GetNode(iParRus);
		if(	RusStr.GetNode(iParRus).m_NodeType == MNA )
			iNodeMNA = m_Relations[inRels[0]].m_SourceNodeNo;
		vector<long> outMna;
		GetOutcomingRelations(iParent,outMna);
		for( int i=0; i<outMna.size(); i++ )
		{
			/*
пустые стрелки идут только из узла типа MUA			*/
			if( m_Relations[outMna[i]].m_Valency.m_RelationStr == "" )
			{
				iNodeTim = m_Relations[outMna[i]].m_TargetNodeNo;
				break;
			}
		}
	}

// если есть (единственная) входящая от глагола или краткого прилагательного 
//  то надо только перевесить Subj и создать связь Modifier
	if(    inRels.size()==1 
		&& (   RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(VERB) 
		    || RusStr.GetNode(iRnode).GetWord(iRword).HasPOS(ADJ_SHORT) 
		   )
	  )
	{
		CEngSemRelation newRel(CValency("Modifier",A_C),m_Relations[inRels[0]].m_SourceNodeNo,m_Relations[inRels[0]].m_TargetNodeNo,"");
		/*
		 конструкция с than должна стоять после прямой син. валентности
		 например
		 I love him more than Peter.
		*/
		newRel.m_Position = ">";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel);
		m_Relations[inRels[0]].m_TargetNodeNo = m_Relations[iRelSubj].m_TargetNodeNo;
		EraseRelation(iRelSubj);
		return;
	}

	CEngSemNode newNode;
	int iSubjPattern = -1, iObjPattern = -1;
	CreateBeSemNode(newNode,iSubjPattern,iObjPattern);
	assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
	assert(newNode.m_Words.size()==1);

	newNode.RusNode = iRusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

// время
	TransferTimeGrammems(RusStr.GetNode(m_Nodes[iNodeTim].RusNode),newNode,"ApplyComparativeRule");
	if( newNode.m_Words[0].GetTense() == zero_tn )
		newNode.m_Words[0].SetTense(present_smp_tn, "ApplyComparativeRule");
	m_Nodes[iNodeTim].m_Words[0].SetTense(zero_tn, "ApplyComparativeRule");
// число
	if( RusStr.GetNode(iRusSubj).m_NodeType == MNA )
		newNode.m_Words[0].AddFormGrammem(ePlural);
	else
		TransferNumberGrammems(RusStr.GetNode(iRusSubj),newNode.m_Words[0]);

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode); // be

	int iNewNode = m_Nodes.size()-1;

	m_Relations[iRelSubj].m_SourceNodeNo = iNewNode; // be->subj
	m_Relations[iRelSubj].m_bInterpreted = true;
	m_Relations[iRelSubj].m_Valency.m_RelationStr = "rComparative";
	m_Relations[iRelSubj].m_Valency.m_RossHolder = GetRossHolder(newNode.GetType());
	m_Relations[iRelSubj].m_Valency.m_UnitNo = newNode.GetUnitNo();
	m_Relations[iRelSubj].m_Valency.m_LeafId = 1;
	m_Relations[iRelSubj].m_Pattern = newNode.m_Patterns[iSubjPattern];
	m_Relations[iRelSubj].m_SynReal.m_Cortege =  newNode.m_Patterns[iSubjPattern].m_GramCorteges[0];

	CEngSemRelation newRel(CValency("rComparative",A_C),iNewNode,iNodeMNA,"");
	newRel.m_bInterpreted = true;
	newRel.m_Valency.m_RossHolder = GetRossHolder(newNode.GetType());
	newRel.m_Valency.m_UnitNo = newNode.GetUnitNo();
	newRel.m_Valency.m_LeafId = 2;
	newRel.m_Pattern = newNode.m_Patterns[iObjPattern];
	newRel.m_SynReal.m_Cortege = newNode.m_Patterns[iObjPattern].m_GramCorteges[0];

	m_Relations.push_back(newRel); // be->comp|mna

// связи
	MoveMeaningRelsToNewNode(iNodeMNA,iNewNode);
}

////////////////////////////////////////////////////////////////////////////
// ApplyAdjShortRule() 
// "лекарство полезно"
// "Словарь окажется полезен"

void  CEngSemStructure::ApplyAdjShortRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;
//
	if( !RusStr.GetNode(iRusNode).HasPOS(ADJ_SHORT) )
		return;

	if( m_Nodes[iEngNode].HasPOS(eVBE) || 
		m_Nodes[iEngNode].HasPOS(eMOD) ||
		m_Nodes[iEngNode].HasPOS(eVERB) )
		return;

	/*
	 если валентность требует, чтобы краткое прилагательное 
	 стояло  в инфинитиве, тогда  нужно сделать, как хочет валентность
	 Например, "он оказался полезен" -> "He proved to be useful"
	 Здесь для краткого прилагательного "полезен" будет 
	 создан инфинитив "to be"
	 Сокирко 31 июня 2001 года
	*/
	vector<long> Rels;
	GetIncomingRelations(iEngNode, Rels, false);
	if (Rels.size()  == 1) 
	 if (HasGramFet(m_Relations[Rels[0]], "to be+ADJECTIVE"))
	 {
		 MakeBeNodeForEngNode(iEngNode,"ApplyAdjShortRule");
		 GetIncomingRelations(iEngNode, Rels, false);
		 assert (Rels.size()  == 1);
		 m_Nodes[m_Relations[Rels[0]].m_SourceNodeNo].m_Words[0].SetTense(present_inf_tn,"ApplyAdjShortRule");
		 return;
	 };


//
	if( !RusStr.GetNode(iRusNode).HasOneGrammem(rPerfective) )
	{
		MakeBeNodeForEngNode(iEngNode,"ApplyAdjShortRule");
		return;
	}
//
	CEngSemNode newNode;
	CreateSimpleEnglNode("become",newNode,eVERB,false);
	assert( newNode.m_Words.size()==1 );

	newNode.RusNode = m_Nodes[iEngNode].RusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

	TransferTimeGrammems(RusStr.GetNode(iRusNode),newNode,"ApplyAdjShortRule");
	if( newNode.m_Words[0].GetTense() == zero_tn )
		newNode.m_Words[0].SetTense(present_smp_tn,"ApplyAdjShortRule");
	m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].SetTense(zero_tn,"ApplyAdjShortRule");

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode); // become
	int iNewNode = m_Nodes.size()-1;
	
	CEngSemRelation newRel(CValency("AdjShort",A_C),iNewNode,iEngNode,"");
	m_Relations.push_back(newRel); // become->Node

// поищем Subj
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	int iSubRel = -1;
	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
		{
			iSubRel = outRels[i];
			break;
		}
		
		if( m_Relations[outRels[i]].m_RusRel != -1 && 
			RusStr.IsRusSubj(m_Relations[outRels[i]].m_RusRel) )
		{
			iSubRel = outRels[i];
			break;
		}
	}

	if( iSubRel!=-1 )
	{
		m_Relations[iSubRel].m_SourceNodeNo = iNewNode;
		m_Relations[iSubRel].m_Valency.m_RelationStr = "AdjShort";
		m_Relations[iSubRel].m_bInterpreted = false;

		//приписываем отношению первую валентость
		m_Relations[iSubRel].m_SynReal.m_Cortege = newNode.m_Patterns[0].m_GramCorteges[0];
		m_Relations[iSubRel].m_Valency = newNode.m_Vals[0];

		CEngSemNode& subNode = m_Nodes[m_Relations[iSubRel].m_TargetNodeNo];
		if( subNode.m_NodeType == MNA )
			m_Nodes[iNewNode].AddOneGrammemRich(ePlural);
		else
			m_Nodes[iNewNode].AddGrammemsRich( subNode.GetGrammemsRich() & eAllNumbers );
	}
	else
		m_Nodes[iNewNode].AddOneGrammemRich( eSingular);

// связи
	MoveMeaningRelsToNewNode(iEngNode,iNewNode);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyPredicativeRule() для всех "ПРЕДК:ГГ" 
// ex. "вам вредно пить много"
// --> добавит it be + предлог for для subj
// ex. "мне совестно уйти"
// --> добавит be

void CEngSemStructure::ApplyPredicativeRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;

	if( RusStr.GetNode(iRusNode).GetType() != Ross )
		return;
	if(		!GetRossHolder(Ross)->HasFieldValue(string("GF"),string("ПРЕДК:ГГ"),RusStr.GetNode(iRusNode).GetUnitNo())
		&&	!GetRossHolder(Ross)->HasFieldValue(string("GF"),string("ГЛ:ГГ_безл"),RusStr.GetNode(iRusNode).GetUnitNo())
		)
		return;

	if( m_Nodes[iEngNode].HasPOS(eVBE) ||
		m_Nodes[iEngNode].HasPOS(eMOD) ||
		m_Nodes[iEngNode].HasPOS(eVERB) )
		return;

	vector<long> inRels;
	GetIncomingRelations(iEngNode,inRels,false);
	for( int i=0; i<inRels.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[inRels[i]].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[inRels[i]].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			continue;
		CEngSemNode& engNode = m_Nodes[m_Relations[inRels[i]].m_SourceNodeNo];
		if( engNode.HasPOS(eVBE) || engNode.HasPOS(eMOD) || engNode.HasPOS(eVERB) )
			return;
	}

// определим тип
	bool bTypeIam = false;
	if( m_Nodes[iEngNode].GetType() != NoneRoss )
	{
		if( GetRossHolder(m_Nodes[iEngNode].GetType())->HasFieldValue("RESTR","predic",m_Nodes[iEngNode].GetUnitNo()) ||
			GetRossHolder(m_Nodes[iEngNode].GetType())->HasFieldValue("RESTR","subj",  m_Nodes[iEngNode].GetUnitNo()) )
			bTypeIam = true;
	}

	CEngSemNode newNode;
	int iSubjPattern = -1,iObjPattern = -1;
	CreateBeSemNode(newNode,iSubjPattern,iObjPattern);
	assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );

	newNode.RusNode = m_Nodes[iEngNode].RusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

	TransferTimeGrammems(RusStr.GetNode(iRusNode), newNode,"ApplyPredicativeRule");
	if( newNode.m_Words[0].GetTense() == zero_tn )
		newNode.m_Words[0].SetTense(present_smp_tn, "ApplyPredicativeRule");
	m_Nodes[iEngNode].m_Words[0].SetTense(zero_tn, "ApplyPredicativeRule");

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	assert(newNode.m_Words.size()==1);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode); // be
	int iBeNode = m_Nodes.size()-1;
	
CEngSemRelation newRel(CValency("S-ACT",A_C),m_Nodes.size()-1,iEngNode,"");	newRel.m_bInterpreted = true;
	newRel.m_Valency.m_RossHolder = GetRossHolder(m_Nodes[iBeNode].GetType());
	newRel.m_Valency.m_UnitNo = m_Nodes[iBeNode].GetUnitNo();
	newRel.m_Valency.m_LeafId = 2;
	newRel.m_Pattern = m_Nodes[iBeNode].m_Patterns[iObjPattern];
	newRel.m_SynReal.m_Cortege = m_Nodes[iBeNode].m_Patterns[iObjPattern].m_GramCorteges[0];
	newRel.m_Position = ">";
	newRel.m_PosType = FromAlgorithmPosType;
	m_Relations.push_back(newRel); // be->Pred

// поищем Subj, поставим предлог for
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	int iSubjRel = -1;
	for( int i=0; i<outRels.size(); i++ )
	{
		if( m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo != 
			m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo )
			continue;
//
	if( m_Relations[outRels[i]].m_Valency.m_RelationStr == "CONTEN" )			
			m_Relations[outRels[i]].m_SynReal.m_Cortege.SetItem(0, -1);
//
	if( m_Relations[outRels[i]].m_Valency.m_RelationStr == "SUB" ||m_Relations[outRels[i]].m_Valency.m_RelationStr == "PACIEN" )		
	{
			iSubjRel = outRels[i];
			if( !bTypeIam )
			{
				SetSimpleEngPrep("for",-1,outRels[i]);
				m_Relations[outRels[i]].m_Position = ">";
				m_Relations[outRels[i]].m_PosType = FromAlgorithmPosType;
			}
			continue;
		}
	}

// два варианта
	if( !bTypeIam || iSubjRel<0 )
	{
		newNode.m_Words.clear();
		CreateSimpleEnglNode("it",newNode,0,true);
		assert(newNode.m_Words.size()==1);
		newNode.m_Words[0].AddFormGrammem(eSingular);

		newNode.RusNode = -1;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);

CEngSemRelation subRel(CValency("F-ACT",A_C),iBeNode,m_Nodes.size()-1,"");		subRel.m_bInterpreted = true;
		subRel.m_Pattern = m_Nodes[iBeNode].m_Patterns[iSubjPattern];

		subRel.m_Valency = m_Nodes[iBeNode].m_Vals[0];
		subRel.m_SynReal.m_Cortege = m_Nodes[iBeNode].m_Patterns[iSubjPattern].m_GramCorteges[0];

		subRel.m_Position = "<";
		subRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(subRel); // be->it

		m_Nodes[iBeNode].m_Words[0].AddFormGrammem (eSingular);
	}
	else
	{
		m_Relations[iSubjRel].m_SourceNodeNo = iBeNode;
		m_Relations[iSubjRel].m_bInterpreted = true;
		m_Relations[iSubjRel].m_Pattern = m_Nodes[iBeNode].m_Patterns[iSubjPattern];

		m_Relations[iSubjRel].m_Valency = m_Nodes[iBeNode].m_Vals[0];
		m_Relations[iSubjRel].m_SynReal.m_Cortege = m_Nodes[iBeNode].m_Patterns[iSubjPattern].m_GramCorteges[0];

		CEngSemNode& engSubj = m_Nodes[m_Relations[iSubjRel].m_TargetNodeNo];
		int iRusSubj = engSubj.RusNode;
		if( iRusSubj != -1 )
		{
			if( RusStr.GetNode(iRusSubj).m_NodeType == MNA )
				newNode.m_Words[0].AddFormGrammem(ePlural);
			else
				TransferNumberGrammems(RusStr.GetNode(iRusSubj),m_Nodes[iBeNode].m_Words[0]);
		}
	}

// связи
	MoveMeaningRelsToNewNode(iEngNode,iBeNode);
}

/////////////////////////////////////////////////////////////////////////////
// ApplyTwoNegationsRule()
// ex. "никто ничего не хочет"  "он не видит ничего и никого"
// --> убирание лишних отрицаний

void CEngSemStructure::ApplyTwoNegationsRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;

	if( !m_Nodes[iEngNode].HasRelOperator("НЕ") )
		return;
//
	vector<long> outChilds;
	GetChildNodes(iEngNode,outChilds);
	if( outChilds.size() < 1 )
		return;
//
	vector<long> negChilds;
	for( int i=0; i<outChilds.size(); i++ )
	{
		if( HasNeg(m_Nodes[outChilds[i]].GetUnitNo(),m_Nodes[outChilds[i]].GetType()) )
			negChilds.push_back(outChilds[i]);
	}
	if( negChilds.size() < 1 )
		return;
//
	m_Nodes[iEngNode].DelRelOperator("НЕ");
	if( negChilds.size() == 1 )
		return;
//
	vector<long> rusNodes;
	for( int i=0; i<negChilds.size(); i++ )
	{			
		if( m_Nodes[negChilds[i]].RusNode != -1 )
			rusNodes.push_back(m_Nodes[negChilds[i]].RusNode);
	}

	sort(rusNodes.begin(),rusNodes.end(),IsLessByMinWordNo(&RusStr));

	int iFirst = rusNodes[0];
	for( int i=0; i<negChilds.size(); i++ )
	{
		if( m_Nodes[negChilds[i]].RusNode == iFirst )
			continue;

// весь следующий код на тему "ничего и никого"
		bool bCaseMNA = false;
		vector<long> inRels;
		GetIncomingRelations(negChilds[i],inRels,false);
		if( inRels.size() == 1 )
		{
			int iParent = m_Relations[inRels[0]].m_SourceNodeNo;
			int iParRus = m_Nodes[iParent].RusNode;
			if( iParRus != -1 )
			{
				const CSemNode& rusNode = RusStr.GetNode(iParRus);
				if(	RusStr.GetNode(iParRus).m_NodeType == MNA )
					bCaseMNA = true;
			}
		}
		if( !bCaseMNA )
		{
			ChangeNegWord( negChilds[i] );
			continue;
		}

		int	iNodeMNA = m_Relations[inRels[0]].m_SourceNodeNo;
		vector<long> outMna;
		GetOutcomingRelations(iNodeMNA,outMna);
		bool bInFirstMNA = false;
		for( int j=0; j<outMna.size(); j++ )
		{
			if( m_Nodes[m_Relations[outMna[j]].m_TargetNodeNo].RusNode == iFirst )
			{
				bInFirstMNA = true;
				break;
			}
		}
		if( bInFirstMNA )
			continue;

		ChangeNegWord( negChilds[i] );
	}
}

/////////////////////////////////////////////////////////////////////////////
// ImpersonalVerb()
// 1. c объектом "за ним пристально следят" | "дом завалило снегом" | "человека бьют"
// --> переведет obj в subj, проставит Passive и число
// 2. без объекта "видно поле" | "пристально следило" | "спросили, куда идти"
// --> добавит one | it | they
// 3. идется, отмечается (пассивные глаголы без подлежащего)

void CEngSemStructure::ImpersonalVerb(int iEngNode)
{
// не применяется, если граф не собран
	if( !IsConnected() )
		return;
//	
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;

	const CSemNode& rusNode = RusStr.GetNode(iRusNode);
//
	CEngSemNode& engNode = m_Nodes[iEngNode];
	CEngSemWord& engWord = engNode.m_Words[engNode.m_MainWordNo];

	if( !engWord.HasPOS(eVERB) && !engWord.HasPOS(eMOD) )
		return;

	if( engNode.HasRelOperator("_мягк_пригласит_наклонение") ||
		engNode.HasRelOperator("_пригласит_наклонение") )
		return;

	if( engNode.m_Words[0].m_Lemma=="have" )
		return;
	if( engNode.m_Words[0].m_Lemma=="keep" )
		return;

	vector<long> inRels;
	GetIncomingRelations(iEngNode, inRels, false);
	for( int k=0; k<inRels.size(); k++ )
	{
		if( m_Nodes[m_Relations[inRels[k]].m_SourceNodeNo].m_Words[0].m_Lemma=="keep" )
			return;
	}

	if (!HasSubjAsFirstValency(engNode))
		return;

// модальный глагол
	bool bModCX = ( engNode.GetType()!=NoneRoss &&  GetRossHolder(engNode.GetType())->HasFieldValue("SF","MODL",engNode.GetUnitNo()) );
// (3|прш) + мн
	bool bCase1 = (engWord.HasOneGrammem(eThirdPerson) || is_past(engWord.GetTense()))
					&& engWord.HasOneGrammem(ePlural);

// (3|(ср&прш)) + ед
	bool bCase2 = ( engWord.HasOneGrammem(eThirdPerson)
					|| (is_past(engWord.GetTense())
						&& !engWord.HasOneGrammem(eMasculinum)
						&& !engWord.HasOneGrammem(eFeminum))
				  )
				  && engWord.HasOneGrammem(eSingular);

	if( !bCase1 && !bCase2 )
		return;

	// если есть subj
	if ( GetSubj(iEngNode) != -1 ) return;

	// для помет pass(ADR) ищем адресата	
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);

	int iAdrRel = -1;
	if( HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"pass(ADR)") )	
	{
		for( int i=0; i<outRels.size(); i++ )	
		{
			int iSrc = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo;
			int iTrg = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo;
			if( iSrc != iTrg )
				continue;

if( m_Relations[outRels[i]].m_Valency.m_RelationStr=="ADR" )			{
				iAdrRel = outRels[i];
				break;
			}
		}
	}
	
// ищем объект, который не subj
	int iObjRel = -1;
	for(int  i=0; i<outRels.size(); i++ )	
	{
		int iSrc = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			continue;

		if( IsObjPattern(m_Relations[outRels[i]]) )
		{
			iObjRel = outRels[i];
			break;
		}
	}

// будующий subj
	int iSubRel = iAdrRel;
	if( iSubRel==-1 )
		iSubRel = iObjRel;

// если есть кого - переведем его в subj, проставим Passive и число
	if( iSubRel != -1 )		
	{		
		m_Relations[iSubRel].m_SynReal.m_Cortege = engNode.m_Patterns[0].m_GramCorteges[0];
		m_Relations[iSubRel].m_Valency = engNode.m_Vals[0];

		m_Relations[iSubRel].m_Position = "<";
		m_Relations[iSubRel].m_PosType = FromAlgorithmPosType;
		CEngSemNode& impNode = m_Nodes[m_Relations[iSubRel].m_SourceNodeNo];
		CEngSemNode& objNode = m_Nodes[m_Relations[iSubRel].m_TargetNodeNo];
		int iImpMainWord = impNode.m_MainWordNo;
		assert( iImpMainWord != -1 );
		impNode.m_Words[iImpMainWord].m_bMorphologicalPassiveForm = true;

		int iRusObj = objNode.RusNode;
		if( iRusObj != -1 )
			TransferNumberGrammems(RusStr.GetNode(iRusObj),impNode.m_Words[iImpMainWord]);

// тот же пассив надо проставить всем "и" узлам без Subj
		for( int i=0; i<outRels.size(); i++ )
		{
			int iSrc = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo;
			int iTrg = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo;
			if( iSrc == iTrg )
				continue;
			if( m_Relations[outRels[i]].m_Valency.m_RelationStr != "AND" )
				continue;
			int iNode = m_Relations[outRels[i]].m_TargetNodeNo;
			if( !m_Nodes[iNode].HasPOS(eVERB) )
				continue;
			vector<long> outRels2;
			GetOutcomingRelations(iNode,outRels2);
			bool bHasSub = false;
			for( int j=0; j<outRels2.size(); j++ )
			{
				if( IsSubj(m_Relations[outRels2[j]]) )
					bHasSub = true;
			}
			if( bHasSub )
				continue;
			if( m_Nodes[iNode].m_MainWordNo == -1 )
				continue;
			m_Nodes[iNode].m_Words[m_Nodes[iNode].m_MainWordNo].m_bMorphologicalPassiveForm = true;
			if( iRusObj != -1 )
				TransferNumberGrammems(RusStr.GetNode(iRusObj),m_Nodes[iNode].m_Words[m_Nodes[iNode].m_MainWordNo]);
		}

		return;
	}

	for( int k=0; k<inRels.size(); k++ )
	{
		int iSrc = m_Nodes[m_Relations[inRels[k]].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[inRels[k]].m_TargetNodeNo].m_ClauseNo;
		if( iSrc == iTrg )
			continue;
		if( m_Relations[inRels[k]].m_Valency.m_RelationStr == "AND" )
		return; // часть глагольной MUA	
	}
	
// нет объекта	
	CEngSemNode newNode;
	if( bCase1 )
	{
		CreateSimpleEnglNode("they",newNode,0,true);
		newNode.m_Words[0].AddFormGrammem (ePlural);
	}
	else if( !bModCX )
	{
		CreateSimpleEnglNode("it",newNode,0,true);
		newNode.m_Words[0].AddFormGrammem(eSingular);
	}
	else
	{
		CreateSimpleEnglNode("one",newNode,0,true);
		newNode.m_Words[0].AddFormGrammem(eSingular);
	}

	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;

	m_Nodes.push_back(newNode);

	CEngSemRelation newRel(CValency("ImpersonalVerb",A_C),iEngNode,m_Nodes.size()-1,"");
	newRel.m_bInterpreted = false;

	if (!engNode.m_Patterns.empty()  && !engNode.m_Patterns[0].m_GramCorteges.empty())
		newRel.m_SynReal.m_Cortege = engNode.m_Patterns[0].m_GramCorteges[0];

	if (!engNode.m_Vals.empty())
		newRel.m_Valency = engNode.m_Vals[0];

	newRel.m_Position = "<";
	newRel.m_PosType = FromAlgorithmPosType;
	m_Relations.push_back(newRel);		
}

/////////////////////////////////////////////////////////////////////////////
// InfinitiveAndIf()
// ex. "если съесть это яблоко, можно уснуть"
// --> добавит one к глаголу с если

void CEngSemStructure::InfinitiveAndIf(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return;



	CEngSemNode& engNode = m_Nodes[iEngNode];
	if (!HasSubjAsFirstValency(engNode))
		return;

	// проверим, что это (строго) инфинитив
	CEngSemWord& engWord = engNode.m_Words[engNode.m_MainWordNo];

	const CSemNode& rusNode = RusStr.GetNode(engNode.RusNode);
	const CSemWord& rusWord = rusNode.GetWord(iSemMainWord);
	if( !rusWord.HasPOS(INFINITIVE) )
		return;

// такие конструкции возможны только с глаголом, у которого SF1 = 1 ANIM	
	if( rusNode.GetType()==NoneRoss || rusNode.GetUnitNo()==ErrUnitNo )
		return;
if( !GetRossHolder(rusNode.GetType())->HasFieldValue(string("SF"),string("ANIM"),rusNode.GetUnitNo(),1) )		return;

// проверим, что есть межклаузная связь с "если"
	vector<long> inRels;
	GetIncomingRelations(iEngNode, inRels, false);
	if( inRels.size() != 1 )
		return;
	if( !IsRelBetweenClauses(m_Relations[inRels[0]]))
		return;
	if( m_Relations[inRels[0]].m_RusRel == -1)
		return;
	const CSemRelation& rusRel = *RusStr.GetRelation(m_Relations[inRels[0]].m_RusRel);
	if( rusRel.m_SynReal.m_Conj.m_DictType != OborRoss )
		return;
	if( rusRel.m_SynReal.m_Conj.m_UnitNo == ErrUnitNo )
		return;
	string strOb = GetRoss(OborRoss)->GetEntryStr(rusRel.m_SynReal.m_Conj.m_UnitNo);
	if( strOb.find("если") == -1)
		return;

// проверим, что нет subj
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
			return;
	}

// добавим "one"
	CEngSemNode newNode;
	CreateSimpleEnglNode("one",newNode,0,true);
	assert(newNode.m_Words.size()==1);
	newNode.m_Words[0].AddFormGrammem(eSingular);

	newNode.m_ClauseNo = engNode.m_ClauseNo;

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	
	m_Nodes.push_back(newNode);
	
	CEngSemRelation newRel(CValency("InfinitiveAndIf",A_C),iEngNode,m_Nodes.size()-1,"");

	newRel.m_Valency = engNode.m_Vals[0];
	newRel.m_SynReal.m_Cortege = engNode.m_Patterns[0].m_GramCorteges[0];

	newRel.m_Position = "<";
	newRel.m_PosType = FromAlgorithmPosType;
	m_Relations.push_back(newRel);
//
	m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].SetTense(present_smp_tn,"InfinitiveAndIf");
	m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].AddFormGrammem(eSingular);
}

/////////////////////////////////////////////////////////////////////////////
