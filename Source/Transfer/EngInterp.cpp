/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../SemanLib/HierarchyHolder.h"

/////////////////////////////////////////////////////////////////////////////

void CEngSemStructure::InitEngVals(CEngSemNode& Node)
{
	InitVals(Node);

	vector<long> Articles;
	FindAbstractAdditionArticle (Aoss, Node, Articles, false, -1);
	AddAbstractAdditionVals(Aoss, Node, Articles);


	Node.m_Patterns.clear();
	Node.m_CopulPatterns.clear();

	for (int i=0; i< Node.m_Vals.size(); i++)
	{
		CSemPattern semPattern;
		semPattern.InitSemPattern(GetRossHolder(Node.GetType()), Node.GetUnitNo(), Node.m_Vals[i].m_LeafId, Node.m_Vals[i].m_BracketLeafId);
		semPattern.LoadGramFromDict();
		if (semPattern.m_GramCorteges.empty())
		{
			// Добавляем пустой кортеж, если в статье не было найдено GFi
			semPattern.m_GramCorteges.push_back(TCortege());
		};

		Node.m_Patterns.push_back(semPattern);

		/*
			добавляем  паттерны копулы в специальный  массив, у копула не может больше двух валентностей
		*/
		if (HasThisGX(semPattern.m_GramCorteges,"Copul", Node.GetType()))
			for (size_t ValNo=1;  ValNo<=2; ValNo++)
			{
				CSemPattern P;
				P.InitSemPattern(GetRossHolder(Node.GetType()), Node.GetUnitNo(), Node.m_Vals[i].m_LeafId, ValNo);
				if( P.LoadGramFromDict() )
					Node.m_CopulPatterns.push_back(P);
			};

	};


}

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::FindEngVal(string strRusVal,CEngSemNode& engNode,vector<long>& iBadVals,bool bUseHierarchy)
{
	for( int i=0; i<engNode.m_Vals.size(); i++ )
	{
		if( !bUseHierarchy )
		{
			if( strRusVal == engNode.m_Vals[i].m_RelationStr )
				return i;
		}
		else
		{
			if( ValencyEq(strRusVal,engNode.m_Vals[i].m_RelationStr) &&
				(find(iBadVals.begin(),iBadVals.end(),i)==iBadVals.end()) )
				return i;
		}
	}
	
	return -1;
}

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::ValencyEq(string strRusVal,string strEngVal)
{

	if( strRusVal == strEngVal )
		return true;


	for( int i=0; i<m_pData->m_HierarchySemRelDoc.m_TransitiveRels.size(); i++ )
	{
		string strVal1 = m_pData->m_HierarchySemRelDoc.m_TransitiveRels[i].m_Source;
		string strVal2 = m_pData->m_HierarchySemRelDoc.m_TransitiveRels[i].m_Target;

		if( (strVal1==strEngVal ) && (strVal2==strRusVal) )
			return true;
	}

	for( int i=0; i<m_SemRelEquivs.size() ; i++ )
	{
		if( (m_SemRelEquivs[i].first  == strRusVal) && (m_SemRelEquivs[i].second == strEngVal) )
			return true;
		if( (m_SemRelEquivs[i].second == strRusVal) && (m_SemRelEquivs[i].first  == strEngVal) )
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::GetEnglishNodeBadWeight(int iRusNode, CEngInterp& UnitInterp, vector<long>& RusRelationsToValsOfNode, int& iSubjRel)
{
	vector<long> vectorTargetRelationsNum;
	RusStr.GetOutcomingRelations(iRusNode,vectorTargetRelationsNum);	

	iSubjRel = -1;
	RusRelationsToValsOfNode.clear();
	int iBadWeight = 0;
	
	CEngSemNode engNode;
	CreateSimpleEnglNodeWithInterp(UnitInterp, engNode);
	engNode.m_HigherConcepts = RusStr.GetNode(iRusNode).m_HigherConcepts;
	InitEngVals(engNode);

	bool bHasAL1 = HasALG(UnitInterp.m_DictType,UnitInterp.m_UnitNo,"AL1");
	bHasAL1 = false;


	bool bHasSJa = false;
	for( int j=0; j<vectorTargetRelationsNum.size(); j++ )
	{
		if( RusStr.GetNode(RusStr.GetRelation(vectorTargetRelationsNum[j])->m_TargetNodeNo).m_NodeType == SJA )
			bHasSJa = true;
		RusRelationsToValsOfNode.push_back(-1);
	}

	for( int j=0; j<vectorTargetRelationsNum.size(); j++ )
	{
		const CSemRelation& rusRel = *RusStr.GetRelation(vectorTargetRelationsNum[j]);

		if( !IsValFromRossArticle(rusRel) )
			continue;

		if( RusStr.IsRusSubj(vectorTargetRelationsNum[j]) 
			&& bHasAL1
			&& bHasSJa
			&& ( engNode.m_Vals.size() > 1 )
			&& ( find(RusRelationsToValsOfNode.begin() , RusRelationsToValsOfNode.end(), 1 ) == RusRelationsToValsOfNode.end() )
		  )
		{
			RusRelationsToValsOfNode[j] = 1;
			iSubjRel = 1;
			continue;
		}

		string strRusVal = rusRel.m_Valency.m_RelationStr;

		if( RusStr.GetNode(rusRel.m_TargetNodeNo).m_NodeType == Copul )
			if( !HasCopul(engNode.m_Vals, strRusVal) )
				iBadWeight += 3;
		
		int iFoundEngVal = j;
		bool bFound = false;

		bFound = ( (iFoundEngVal = FindEngVal(strRusVal, engNode, RusRelationsToValsOfNode, false ) ) != -1 );

		if( !bFound )
		{
			bFound =  ((iFoundEngVal = FindEngVal(strRusVal, engNode, RusRelationsToValsOfNode, true ) ) != -1 );
			iBadWeight += 2;
		}

		if( !bFound )
		{
			iBadWeight += 10;
			continue;
		}

		RusRelationsToValsOfNode[j] = iFoundEngVal;
	}

	for( int i=0; i<engNode.m_Vals.size(); i++ )
	{
		int j=0;
		for( ; j<vectorTargetRelationsNum.size(); j++ )
		{
			string strRusVal = RusStr.GetRelation(vectorTargetRelationsNum[j])->m_Valency.m_RelationStr;
			string strEngVal = engNode.m_Vals[i].m_RelationStr;
			if( ValencyEq(strRusVal,strEngVal) )
				break;
		}

// если валентность не найдена и нет пометы, что эта валентность может отсутствовать, то это ошибка
		if( j == vectorTargetRelationsNum.size() )
		{
			CSemPattern semPattern;
			semPattern.InitSemPattern(GetRossHolder(UnitInterp.m_DictType), engNode.m_Vals[i].m_UnitNo, engNode.m_Vals[i].m_LeafId, engNode.m_Vals[i].m_BracketLeafId);
			semPattern.LoadGramFromDict();
			if( !HasThisGX(semPattern.m_GramCorteges, "-", UnitInterp.m_DictType) )
			{
				iBadWeight++;
				break;
			}
		}
	}

	return iBadWeight;
}

/////////////////////////////////////////////////////////////////////////////
// выдает номер валентности для реверсивной связи

int CEngSemStructure::GetReverseRelValNo(const CSemRelation& rusRel,CEngSemNode& reverseNode)
{
	//if( !IsValFromRossArticle(rusRel) )
	//	return( -1 );

	string strRusVal = rusRel.m_Valency.m_RelationStr;

	for( int i=0; i<reverseNode.m_Vals.size(); i++ )
	{
		if( strRusVal == reverseNode.m_Vals[i].m_RelationStr )
			return( i );
	}

	for( int i=0; i<reverseNode.m_Vals.size(); i++ )
	{
		if( ValencyEq(strRusVal,reverseNode.m_Vals[i].m_RelationStr) )
			return( i );
	}

	return( -1 );
}

/////////////////////////////////////////////////////////////////////////////

CEngInterp CEngSemStructure::FindBestEnglishNodes(int iRusNode,vector<CEngInterp>& vectorEngEquivs,vector<long>& RusRelationsToValsOfNode,int& iBestSubjRel)
{
	/*
		если мы сюда поппали, тогда хоть какие-то переводы  должны  быть (хотя бы по заглушкам)
	*/
	assert ( !vectorEngEquivs.empty() );
	int iBestEquiv = 0;
	int iSubjRel;
	vector<long> goodRels;		
	int iMinBadWeight = 0;

	//перебираем найденные по АОССу англ. эквиваленты
	for( int i=0; i<vectorEngEquivs.size(); i++ )
	{
		int iBadWeight = GetEnglishNodeBadWeight(iRusNode, vectorEngEquivs[i], goodRels, iSubjRel);

		if( (iBadWeight < iMinBadWeight) || (i == 0) )
		{
			iBestEquiv = i;
			iMinBadWeight = iBadWeight;
			RusRelationsToValsOfNode = goodRels;	
			iBestSubjRel = iSubjRel;
		}
	}

	return vectorEngEquivs[iBestEquiv];
}

/////////////////////////////////////////////////////////////////////////////

bool  CEngSemStructure::AddGXiFromLexFunOperWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iOperNum)
{
	if( (LexFunctWordUnitNo != -1) && (LexFunctWordUnitNo != ErrUnitNo) )
	{
		if( iValNum == iOperNum - 1 )
		{
			semPattern1.InitSemPattern(GetRossHolder(Aoss), LexFunctWordUnitNo, 1, 0);
			semPattern1.LoadGramFromDict();
			if( !semPattern1.m_GramCorteges.empty() )
					return true;
		}
		else
		{
			int iValNumInLexFunctWord = ((iValNum + 1) > iOperNum) ? iValNum + 1 : iValNum + 2;
			semPattern1.InitSemPattern(GetRossHolder(Aoss), LexFunctWordUnitNo, iValNumInLexFunctWord+1, 0);
			if( !semPattern1.m_GramCorteges.empty() )
					return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::InterpretOneNode( CEnglishEquivMap& mapRNodeToENode, int iRusNode, vector<long>& RusRelationsToValsOfNode,int& iSubjRel)
{
	CEnglishEquivMap::iterator res = mapRNodeToENode.find(iRusNode);
	CEngSemNode engNode;
	engNode.m_SynReal = TranslateRelization(RusStr.GetNode(iRusNode).m_SynReal, RusStr.GetNode(iRusNode));
	string synRelAlg = engNode.m_SynReal.m_AlgStr;
	engNode.m_SynGroupTypeStr = RusStr.GetNode(iRusNode).m_SynGroupTypeStr;
	
	
	if(		res != mapRNodeToENode.end() 
		)
	{
		assert( RusStr.GetNode(iRusNode).m_NodeType != Copul );		
		CEngInterp UnitInterp = FindBestEnglishNodes(iRusNode,(*res).second,RusRelationsToValsOfNode,iSubjRel);

		CreateEnglNode(UnitInterp, engNode,RusStr.GetNode(iRusNode));
		InitEngVals(engNode);
				
		if(    (UnitInterp.m_DictType != NoneRoss)
			&& GetRossHolder(UnitInterp.m_DictType)->HasFieldValue(  string("RESTR") , string("pass"),  UnitInterp.m_UnitNo ) 
		  )
		if( engNode.m_MainWordNo>=0 )
			engNode.m_Words[engNode.m_MainWordNo].m_bMorphologicalPassiveForm = true;

		
		engNode.RusNode = iRusNode;
		engNode.m_ClauseNo = RusStr.GetNode(iRusNode).m_ClauseNo;		
		bool bInfinitive = engNode.HasGrammemRich(eInfinitive);
		TransferGrammems (RusStr.GetNode(iRusNode), engNode, "InterpretOneNode");
		string debug =  m_pData->GetEngGramTab()->GrammemsToStr(engNode.GetGrammems());
		

		if (bInfinitive)
			engNode.m_Words[engNode.m_MainWordNo].SetTense(present_inf_tn,"Colloc-infinitive");
		
		// "RUSETENSE"
		EngVerbTenseEnum AVREM_Tense = handle_AVREM_field(iRusNode);
		if( engNode.m_MainWordNo >= 0 )
		{
			if (AVREM_Tense != zero_tn) 
				engNode.m_Words[engNode.m_MainWordNo].SetTense(AVREM_Tense,"по полю RUSETENSE");
			CorrectTenseByDictVerbFeatures(engNode); // "RESTR" "not_cont"
		}
	}
	else
	{
 		if( RusStr.GetNode(iRusNode).m_NodeType == Copul )
		{
			engNode = RusStr.GetNode(iRusNode);
			engNode.m_NodeType = Copul;
			engNode.RusNode = iRusNode;
		}

		else
		{
			long iLexFun = FindLexFunctBySituationNodeNo(iRusNode);
			if( iLexFun == -1 )
			{
				engNode = (CSemNode&)RusStr.GetNode(iRusNode);
				engNode.SetGrammemsRich( 0 );
				engNode.RusNode = iRusNode; 
				TransferGrammems (RusStr.GetNode(iRusNode), engNode, "InterpretOneNode");
				// инициализируем части речи с помомощью функции EngPOSByRusPOS
				for (long i=0; i < engNode.m_Words.size(); i++ )
				{
					engNode.m_Words[i].m_Poses = EngPOSByRusPOS(GetOnePOS(RusStr.GetNode(iRusNode).GetWord(i).m_Poses), RusStr.GetNode(iRusNode).GetWord(i).m_Lemma);
				}

			}
			else
				{
					engNode.RusNode = iRusNode; 		
					CEngSemNode T = (CSemNode&)RusStr.GetNode(iRusNode);
					T.SetGrammemsRich(0 );
					CreateSimpleEnglNodeByOldNode(m_LexFuncts[iLexFun].m_LexFunParamLemma,engNode,0,true,T);
					TransferGrammems(RusStr.GetNode(iRusNode),engNode,"InterpretOneNode");
				}
		}
	}

	if( engNode.GetType()!=NoneRoss && engNode.GetUnitNo()!=ErrUnitNo &&
		GetRossHolder(engNode.GetType())->HasFieldValue("RESTR","ед",engNode.GetUnitNo()) )
	{
		engNode.DeleteGrammemsRich( eAllNumbers );
		engNode.AddOneGrammemRich( eSingular );
	}

	engNode.m_SynReal.m_AlgStr = synRelAlg;


	m_Nodes.push_back(engNode);
	return m_Nodes.size() - 1;
}

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::AddGXiFromLexFunFuncWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iFuncNum )
{
	if( (LexFunctWordUnitNo != -1) && (LexFunctWordUnitNo != ErrUnitNo) )
	{
		if( iValNum == iFuncNum - 1 )
		{
			semPattern1.InitSemPattern(GetRossHolder(Aoss), LexFunctWordUnitNo, 1, 0);
			if( !semPattern1.m_GramCorteges.empty() )
					return true;
		}
		else
		{
			int iValNumInLexFunctWord = ((iValNum + 1) > iFuncNum) ? iValNum + 1 : iValNum + 2;
			if( iFuncNum == 0 )
				iValNumInLexFunctWord--;

			semPattern1.InitSemPattern(GetRossHolder(Aoss), LexFunctWordUnitNo, iValNumInLexFunctWord +1, 0);
			if( !semPattern1.m_GramCorteges.empty() )
					return true;
		}


	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::InterpretCopulNodeAndChildren( CEnglishEquivMap& mapRNodeToENode, int iRusCopulNode, int iEngCopulParentNode, int iValNum)
{
	vector<long> RusRelsToEngRels;
	int ii;
	int iEngCopulNode = InterpretOneNode(mapRNodeToENode, iRusCopulNode, RusRelsToEngRels, ii);

	m_Nodes[iEngCopulNode].CopyInterps( m_Nodes[iEngCopulParentNode]);

 	vector<long> vectorTargetRelationsNum;
	RusStr.GetOutcomingRelations(iRusCopulNode,vectorTargetRelationsNum);	

	for(int i = 0 ; i < vectorTargetRelationsNum.size() ; i++ )
	{
		
		const CSemRelation& rusRel = *RusStr.GetRelation(vectorTargetRelationsNum[i]);
		
		CEngSemRelation engRel = rusRel;
		engRel.m_SynReal = TranslateRelization(rusRel.m_SynReal, RusStr.GetNode(rusRel.m_TargetNodeNo));

		/*
			предполагается, что копулы не могут быть взяты добавочных статей, поэтому  грузим GF из основной статьи.
			но лучше как-нибудь это переделать  и грузить GF из engRel.m_Valency.m_UnitNo,
			а еще предполагется, что копулы есть только в AOCC, хотя зачем такое предположение нужно?
		*/
		vector<CValency>::const_iterator it =  find(m_Nodes[iEngCopulParentNode].m_Vals.begin(), m_Nodes[iEngCopulParentNode].m_Vals.end(), rusRel.m_Valency);
		if (it == m_Nodes[iEngCopulParentNode].m_Vals.end())
		{
			engRel.m_Valency.m_UnitNo = m_Nodes[iEngCopulParentNode].GetUnitNo();
			engRel.m_Valency.m_RossHolder  = GetRossHolder(Aoss);
		}
		else
		{
			engRel.m_Valency = *it;
		};


		engRel.m_SourceNodeNo = iEngCopulNode;
		if( !WasInterpreted(rusRel.m_TargetNodeNo)  )
		{
			SetInterpreted(rusRel.m_TargetNodeNo);
			engRel.m_TargetNodeNo = InterpretRusNodeAndItsChildren(mapRNodeToENode, rusRel.m_TargetNodeNo);
		}
		else
			engRel.m_TargetNodeNo = GetEngNodeByRusNode(rusRel.m_TargetNodeNo);

		engRel.m_RusRel = vectorTargetRelationsNum[i];
		

		if( (iValNum != -1 ) && (engRel.m_Valency.m_UnitNo != ErrUnitNo) )
		{
			CSemPattern semPattern;
			semPattern.InitSemPattern(GetRossHolder(Aoss), engRel.m_Valency.m_UnitNo, engRel.m_Valency.m_LeafId, engRel.m_Valency.m_BracketLeafId);
			if(  semPattern.LoadGramFromDict() )
			{
				m_Nodes[iEngCopulNode].m_Patterns.push_back(semPattern);
				engRel.m_Pattern = semPattern;
				// Следующий оператор  вставлен Сокиркой для анализа фразы "Индия  считается  космической державой"
				engRel.m_bInterpreted = true;
				
		
			}			
		}
		m_Relations.push_back(engRel);
	}
	return iEngCopulNode;
}

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::InterpretRusNodeAndItsChildren( CEnglishEquivMap& mapRNodeToENode, int iRusNode)
{
	int iSubjRel;
	vector<long> RusRelationsToValsOfNode;
	
	int iEngNode = InterpretOneNode(mapRNodeToENode, iRusNode, RusRelationsToValsOfNode, iSubjRel);		
	const CSemNode& RusNode = RusStr.GetNode(iRusNode);

	vector<long> vectorTargetRelationsNum;
	RusStr.GetOutcomingRelations(iRusNode,vectorTargetRelationsNum);	

	for( int i=0; i<vectorTargetRelationsNum.size(); i++ )
	{
		int iEngActantNode = -1;
		const CSemRelation& rusRel = *RusStr.GetRelation(vectorTargetRelationsNum[i]);
		if( RusStr.GetNode(rusRel.m_TargetNodeNo).m_NodeType == SJA )
			continue;

		if( !WasInterpreted(rusRel.m_TargetNodeNo))
		{
			SetInterpreted(rusRel.m_TargetNodeNo);
			if( RusStr.GetNode(rusRel.m_TargetNodeNo).m_NodeType == Copul )	
				iEngActantNode = InterpretCopulNodeAndChildren(mapRNodeToENode, rusRel.m_TargetNodeNo, iEngNode, RusRelationsToValsOfNode[i]);					
			else
				iEngActantNode = InterpretRusNodeAndItsChildren(mapRNodeToENode, rusRel.m_TargetNodeNo);
		}
		else
			iEngActantNode = GetEngNodeByRusNode(rusRel.m_TargetNodeNo);

		assert(iEngActantNode != -1);

		CEngSemRelation engRel = *RusStr.GetRelation(vectorTargetRelationsNum[i]);
		engRel.m_SynReal = TranslateRelization(rusRel.m_SynReal, RusStr.GetNode(rusRel.m_TargetNodeNo));

		// если  найдена LF, в которой записан каким предлогом должно управляться слово-ситуация,
		// то нужно удалить предлог с отношения, чтобы он не переводился по общим законам
		if( FindLexFunct(rusRel.m_SourceNodeNo, rusRel.m_TargetNodeNo) != -1 )
		{
			if( m_Nodes[iEngNode].m_MainWordNo != -1 )
				if(    !m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].m_PosleLog.m_PosleLog.empty() 
					|| (m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].m_PosleLog.m_PosleLogPrep.m_DictType != NoneRoss) 
				  ) 
					engRel.m_SynReal.m_Preps.clear();
		}

		engRel.m_SourceNodeNo = iEngNode;
		engRel.m_TargetNodeNo = iEngActantNode;				
		engRel.m_Valency.m_UnitNo = ErrUnitNo;
		engRel.m_Valency.m_RossHolder = NULL;

		// выясняем, кому принаделжит данная валентность
		int iValencyOwner = engRel.m_bReverseRel ? iEngActantNode : iEngNode;
		CEngSemNode& EngValencyOwner = m_Nodes[iValencyOwner];

		if(		EngValencyOwner.GetType()!=NoneRoss 
		  )
		{				
			int valNo = -1;
			
			if( engRel.m_bReverseRel )
				valNo = GetReverseRelValNo(rusRel, EngValencyOwner);
			else 
				if (i < RusRelationsToValsOfNode.size())
					valNo = RusRelationsToValsOfNode[i];

			if (	   ( valNo != -1 )
					&& ( valNo < EngValencyOwner.m_Vals.size() )
				)
			{	
				engRel.m_bInterpreted = true;

				engRel.m_Valency = EngValencyOwner.m_Vals[valNo];
				//assert (EngValencyOwner.m_Vals.size() == EngValencyOwner.m_Patterns.size());
				if (EngValencyOwner.m_Vals.size() == EngValencyOwner.m_Patterns.size())
				{
					SetPositionOfActantInColloc(engRel,EngValencyOwner.m_Patterns[valNo],iValencyOwner);
					engRel.m_Pattern = EngValencyOwner.m_Patterns[valNo];
					assert (engRel.m_Pattern.m_GramCorteges.size() > 0);
					engRel.m_SynReal.m_Cortege = engRel.m_Pattern.m_GramCorteges[0];
				}
			}
			else
			{
				engRel.m_bInterpreted = false;					
			}

			engRel.m_RusRel = vectorTargetRelationsNum[i];
			m_Relations.push_back(engRel);
		} 
		else
		{
			engRel.m_bInterpreted = false;					
			engRel.m_RusRel = vectorTargetRelationsNum[i];
			m_Relations.push_back(engRel);
		}
	}

	return iEngNode;
}





/////////////////////////////////////////////////////////////////////////////
// перевод предлогов на связи
//#pragma runtime_checks("", off)
// 

CSynRealization CEngSemStructure::TranslateRelization(const CSynRealization& RusSynReal,  const CSemNode& RusNode) const
{
	CSynRealization Result;
	
	Result= RusSynReal;
	Result.m_Preps.clear();
	
	// надо обнулить кортеж, а то будут считать, что это отношение пришло из англ. словарей,
	// а кортеж на самом деле пришел из русского словаря 
	
	Result.m_Cortege.m_DomItemNos[0] = -1;
	

	for( int i=0; i<RusSynReal.m_Preps.size(); i++ )
	{
		const CRossInterp& prep_int = RusSynReal.m_Preps[i];
		string debug;
		if (RusNode.IsWordContainer()) debug = RusNode.GetWord(0).m_Word;
		assert(prep_int.m_DictType == OborRoss);
		vector<SEngEquiv> vectorEngEquivs;
		
		vector<TCortege> vectorACX;
		
		string strACX;
		vector<string> SemFets;
		vector<int> goodIds;
		
		GetEngEquivsFromRusArticle(vectorEngEquivs,prep_int.m_UnitNo,prep_int.m_DictType);

		// ====== обработка поля ESF2: удаляем все переводы, которые не попали под ESF		  Например:
		// перед дверью -> in front of the door
		// перед началом -> before the door
	    GetAFieldVector("ESF", OborRoss, vectorACX, prep_int.m_UnitNo);

		for( int j=0; j<vectorACX.size(); j++ )
		{
			if( vectorACX[j].m_LeafId == 2 ) // если это АCХ2
			{
				strACX = GetCortegeStr(OborRoss,vectorACX[j]);
				SemFets.clear();
				SemFets.push_back(strACX);
				IncludeLowerInHierarchy(&m_pData->m_HierarchySemFetDoc,SemFets);
				int k=0;
				for( ; k<SemFets.size(); k++ )
					if( HasSemFetPro(RusNode,SemFets[k]) )
						break;
				int theId = vectorACX[j].m_BracketLeafId - 1;
				if( k<SemFets.size() )
					if (! _find (goodIds, theId) )
						goodIds.push_back(theId);
						
			};
		};
		
		if (!goodIds.empty())
		{
			vector<SEngEquiv> tempEngEquivs;
			for (int j=0; j < goodIds.size(); j++)
			{
				//assert ( goodIds[j] < vectorEngEquivs.size() ); 
                if (goodIds[j] < vectorEngEquivs.size())
				    tempEngEquivs.push_back( vectorEngEquivs[goodIds[j]] );
			};
			vectorEngEquivs = tempEngEquivs;
		};
// =============  конец обработки ESF2
		for( int k=0; k<vectorEngEquivs.size(); k++ )
		{
			if( vectorEngEquivs[k].m_iMeanNum==10 ) 
				vectorEngEquivs[k].m_iMeanNum = 1;
			WORD EngUnitNo = GetRoss(EngObor)->LocateUnit(vectorEngEquivs[k].m_StrEngWord.c_str(),vectorEngEquivs[k].m_iMeanNum);
			if( EngUnitNo != ErrUnitNo )
			{
				CDictUnitInterp I(EngObor,EngUnitNo);
				Result.m_Preps.push_back(I);
			};

		}
		
		
		if( vectorEngEquivs.size()==1 && vectorEngEquivs[0].m_StrLexFunc=="А1(НЕ)" )
			Result.m_AlgStr = vectorEngEquivs[0].m_StrLexFunc;
		if( vectorEngEquivs.size()==1 && vectorEngEquivs[0].m_StrLexFunc=="А2(НЕ)" )
			Result.m_AlgStr = vectorEngEquivs[0].m_StrLexFunc;
			
	}

	return Result;
}
//#pragma runtime_checks("", restore)
/////////////////////////////////////////////////////////////////////////////
