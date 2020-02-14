/////////////////////////////////////////////////////////////////////////////
// Английские алгоритмы 4 

// AddLexFuncNode()
// ConvertClosedCollocToOpen()
// CheckQuestionClause()
// HandleQuestionClause()
// HandleQuestionNode()
// AddFixedGrammemsToNode()

#include "StdAfx.h"
#include "../SemanLib/LexFuncts.h"
#include "../StructDictLib/TempArticle.h"

/////////////////////////////////////////////////////////////////////////////
// AddLexFuncNode()
// мне грустно

void CEngSemStructure::AddLexFuncNode(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	CEngSemNode& engNode = m_Nodes[iEngNode];
	DictTypeEnum Etype = engNode.GetType();
	long         Eunit = engNode.GetUnitNo();
	if( Etype==NoneRoss || Eunit==ErrUnitNo )
		return;

	int iRusNode = engNode.RusNode;
	if( iRusNode == -1 )
		return;
	const CSemNode& RusNode = RusStr.GetNode(iRusNode);
	if( RusNode.IsTimeRossNode() )
		return;
	if( IsThesRoss(RusNode.GetType()) )
		return;

// смотрим, было ли указание на лексическую функцию
	vector< SEngEquiv > vectorEngEquivs;
	GetEngEquivsFromRusArticle(vectorEngEquivs,RusNode.GetUnitNo(),RusNode.GetType(),iRusNode);

	string StrLexFunc = "";
	for( int i=0; i<vectorEngEquivs.size(); i++ )
	{
		if( vectorEngEquivs[i].m_StrLexFunc.empty() )
			continue;
		long Tunit = GetRoss(Etype)->LocateUnit(vectorEngEquivs[i].m_StrEngWord.c_str(),vectorEngEquivs[i].m_iMeanNum);
		if( Tunit != Eunit )
			continue;
		StrLexFunc = vectorEngEquivs[i].m_StrLexFunc;
	}

	if( StrLexFunc.empty() )
		return;

// достаем из статьи значение лексической функции
	string EngWord = "";
	int    MeanNum = -1;
	TCortege cortege;
	for( int i=GetRoss(Etype)->GetUnitStartPos(Eunit); i<=GetRoss(Etype)->GetUnitEndPos(Eunit); i++ )
	{
		long FieldNo = GetRoss(Etype)->GetCortegeFieldNo(i);
		if(	string("LF") != GetRoss(Etype)->Fields[FieldNo].FieldStr )
			continue;
		cortege = GetCortege(GetRoss(Etype),i);
		if( cortege.m_DomItemNos[0]==-1 )
			continue;
		string ident = GetRoss(Etype)->GetDomItemStr(cortege.m_DomItemNos[0]);
		if( ident != StrLexFunc )
			continue;
		if( cortege.m_DomItemNos[1]==-1 || cortege.m_DomItemNos[2]==-1 )
			continue;
		EngWord = (const char*)GetRoss(Etype)->GetDomItemStr(cortege.m_DomItemNos[1]);
		string strMean = (const char*)GetRoss(Etype)->GetDomItemStr(cortege.m_DomItemNos[2]);
		MeanNum = strMean[0] - '0';
		break;
	}

	//assert( !EngWord.empty() && MeanNum!=-1 );
	if( EngWord.empty() || MeanNum==-1 )
		return;

// Ok
	CEngSemNode newNode;
	CreateSimpleEnglNode(EngWord.c_str(),newNode,0,false,Aoss,MeanNum);
	assert( newNode.m_Words.size()==1 );

	newNode.m_RelOperators = engNode.m_RelOperators;
	engNode.m_RelOperators.clear();

	newNode.RusNode = engNode.RusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	
	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode);
	int iNewNode = m_Nodes.size()-1;

// типа AntiBon
	if( IsParameterOfAdjOrAdv(StrLexFunc) )
	{
		CEngSemRelation newRel(CValency(StrLexFunc.c_str(),A_C),iEngNode,iNewNode,"");
		m_Relations.push_back(newRel);
		return;
	}

// типа PermFunc0
	assert( IsParameterOfVerb(StrLexFunc) );

	CEngSemRelation newRel(CValency(StrLexFunc.c_str(),A_C),iNewNode,iEngNode,"");
	m_Relations.push_back(newRel);

	TransferTimeGrammems(RusStr.GetNode(iRusNode),m_Nodes[iNewNode],"AddLexFuncNode");
	if( m_Nodes[iNewNode].m_Words[0].GetTense() == zero_tn )
		m_Nodes[iNewNode].m_Words[0].SetTense(present_smp_tn,"AddLexFuncNode");

// can+see
	if( m_Nodes[iEngNode].HasPOS(eVERB) )
	{
		m_Nodes[iEngNode].m_Words[0].SetTense(present_inf_tn,"AddLexFuncNode");
		if( m_Nodes[iNewNode].HasPOS(eMOD) &&
			m_Nodes[iNewNode].m_Words[0].GetTense() == present_smp_tn )
		{
			m_Nodes[iEngNode].m_bNotUseTo = true;
		}
	}
	else
		m_Nodes[iEngNode].m_Words[0].SetTense(zero_tn,"AddLexFuncNode");

// обработка Subj + число
	bool bHasSub = false;
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if(		!IsSubj(m_Relations[outRels[i]]) 
			&& !(		EngWord=="feel" 
					&&	m_Relations[outRels[i]].m_Valency.m_LeafId ==1
				) 
		  )
		continue;

		int iRusSubj = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].RusNode;
		if( iRusSubj == -1 )
			continue;
		int iSemSubjWord = RusStr.GetNode(iRusSubj).m_MainWordNo;
		if( iSemSubjWord == -1 )
			continue;
		if( RusStr.GetNode(iRusSubj).m_NodeType == MNA )
			m_Nodes[iNewNode].m_Words[0].AddFormGrammem(ePlural);
		else
			TransferNumberGrammems(RusStr.GetNode(iRusSubj),m_Nodes[iNewNode].m_Words[0]);

		m_Relations[outRels[i]].m_SourceNodeNo = iNewNode;

		CEngSemNode& VerbNode = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo];
		if (HasSubjAsFirstValency(VerbNode))
		{
			m_Relations[outRels[i]].m_Valency = VerbNode.m_Vals[0];
			m_Relations[outRels[i]].m_SynReal.m_Cortege = VerbNode.m_Patterns[0].m_GramCorteges[0];
			bHasSub = true;
		};
		break;
	}
//
	if( !bHasSub )
	{
		m_Nodes[iNewNode].m_Words[0].AddFormGrammem(eSingular);
		m_Nodes[iNewNode].m_Words[0].AddFormGrammem(eThirdPerson);
	}

// связи
	MoveMeaningRelsToNewNode(iEngNode,iNewNode);
}

/////////////////////////////////////////////////////////////////////////////
// служебные функции для ConvertClosedCollocToOpen()

bool CEngSemStructure::GetSINO(DictTypeEnum type,long UnitNo,vector<TCortege>& vecSINO) const
{
	assert( type == EngCollocRoss );
	assert( UnitNo != ErrUnitNo );

	for( int i=GetRoss(type)->GetUnitStartPos(UnitNo); i<=GetRoss(type)->GetUnitEndPos(UnitNo); i++ )
	{
		long FieldNo = GetRoss(type)->GetCortegeFieldNo(i);
		if(	string("SYNR") == GetRoss(type)->Fields[FieldNo].FieldStr )
			vecSINO.push_back(GetCortege(GetRoss(type),i));
	}

	return( vecSINO.size()>0 );
}

void CEngSemStructure::GetSPrp(DictTypeEnum type,long UnitNo,vector<TCortege>& vecSPrp) const
{
	assert( type == EngCollocRoss );
	assert( UnitNo != ErrUnitNo );

	for( int i=GetRoss(type)->GetUnitStartPos(UnitNo); i<=GetRoss(type)->GetUnitEndPos(UnitNo); i++ )
	{
		long FieldNo = GetRoss(type)->GetCortegeFieldNo(i);
		if(	string("PREP") == GetRoss(type)->Fields[FieldNo].FieldStr )
			vecSPrp.push_back(GetCortege(GetRoss(type),i));
	}
}

void CEngSemStructure::GetSArt(DictTypeEnum type,long UnitNo,vector<TCortege>& vecSArt) const
{
	assert( type == EngCollocRoss );
	assert( UnitNo != ErrUnitNo );

	for( int i=GetRoss(type)->GetUnitStartPos(UnitNo); i<=GetRoss(type)->GetUnitEndPos(UnitNo); i++ )
	{
		long FieldNo = GetRoss(type)->GetCortegeFieldNo(i);
		if(	string("DETERM") == GetRoss(type)->Fields[FieldNo].FieldStr )
			vecSArt.push_back(GetCortege(GetRoss(type),i));
	}
}
long CEngSemStructure::FindNodeByLeafId (long NodeNo, int LeafId) const
{
	vector<long> outRels;
	GetOutcomingRelations(NodeNo,outRels);
	for( int m=0; m<outRels.size(); m++ )
	{
		if( m_Relations[outRels[m]].m_bReverseRel) continue;
		
		if( m_Relations[outRels[m]].m_Valency.m_LeafId  == LeafId)
			return m_Relations[outRels[m]].m_TargetNodeNo;
	}
	return -1;
};

void CEngSemStructure::GetSGxi(DictTypeEnum type,long unit,vector<TCortege>& vecSGxi) const
{
	assert( type == EngCollocRoss );
	assert( unit != ErrUnitNo );

	for( int i=GetRoss(type)->GetUnitStartPos(unit); i<=GetRoss(type)->GetUnitEndPos(unit); i++ )
	{
		long FieldNo = GetRoss(type)->GetCortegeFieldNo(i);
		string tmp = GetRoss(type)->Fields[FieldNo].FieldStr;
		if(	string("GF") == GetRoss(type)->Fields[FieldNo].FieldStr )
			vecSGxi.push_back(GetCortege(GetRoss(type),i));
	}
}

/////////////////////////////////////////////////////////////////////////////
// ConvertClosedCollocToOpen()
// у него часто бывает плохо на душе

void CEngSemStructure::ConvertClosedCollocToOpen()
{	
	for( int iEngNode=0; iEngNode<m_Nodes.size(); iEngNode++ )
	{
		int iMainWord = m_Nodes[iEngNode].m_MainWordNo;
		if( iMainWord == -1 ) 
			continue;
		// Ищем закрытые словосочетания
		if( m_Nodes[iEngNode].GetType()!=EngCollocRoss )
			continue;
			
		CEngSemNode& engNode = m_Nodes[iEngNode];
		DictTypeEnum type = EngCollocRoss;
		long UnitNo = m_Nodes[iEngNode].GetUnitNo();
		assert( UnitNo != ErrUnitNo );

		int nWords = engNode.m_Words.size();
		if( nWords==1 )
			continue;

		vector<TCortege> vecSINO;
		if( !GetSINO(type,UnitNo,vecSINO) )
			continue;

// сразу зачистим WordNo		
		int curWNo = engNode.GetMinWordNo();
		for( int j=1; j<nWords; j++ )
			FreeWordNo(curWNo+j);

// загрузим артикли
		vector<TCortege> vecSArt;
		GetSArt(type,UnitNo,vecSArt);
		StringVector articles;
		for( int j=0; j<nWords; j++ )
			articles.push_back("");

		for( int k=0; k<vecSArt.size(); k++ )
		{
			int no = vecSArt[k].m_BracketLeafId - 1;
			if( no>=0 && no<nWords )
				articles[no] = GetCortegeStr(type,vecSArt[k]);
		}

// загрузим части речи
		vector<TCortege> vecSGxi;
		GetSGxi(type,UnitNo,vecSGxi);
		StringVector poses;
		for( int j=0; j<nWords; j++ )
			poses.push_back("");
		for( int k=0; k<vecSGxi.size(); k++ )
		{
			if( vecSGxi[k].m_LeafId>0 )
				continue;
			int no = vecSGxi[k].m_BracketLeafId - 1;
			if( no<0 || no>=nWords )
				continue;
			int t = 0;
			string res = "";
			while( vecSGxi[k].m_DomItemNos[t] != -1 )
			{
				if( t==1 )
					res += " ";
				else if( t>1 )
					res += ",";
				res += (const char*)GetRoss(type)->GetDomItemStr(vecSGxi[k].m_DomItemNos[t]);
				t++;
			}	
			poses[no] = res;
		}

// создадим новые узлы
		vector<CEngSemNode> newNodes;
		for( int j=0; j<nWords; j++ )
		{
			CEngSemNode newNode;
			
			if( j==iMainWord )
			{
				CreateSimpleEnglNodeByOldNode(engNode.m_Words[j].m_Lemma.c_str(),newNode,0,false,m_Nodes[iEngNode]);
				newNode.m_Words[0].m_TenseHistory = engNode.m_Words[j].m_TenseHistory;
				newNode.m_Words[0].SetTense(m_Nodes[iEngNode].GetTense(),"ConvertClosedCollocToOpen");
				newNode.SetGrammemsRich( m_Nodes[iEngNode].GetGrammemsRich());
				newNode.m_HigherConcepts = m_Nodes[iEngNode].m_HigherConcepts;
				InitEngVals(newNode);
			}
			else
			{
				CreateSimpleEnglNode(engNode.m_Words[j].m_Lemma.c_str(),newNode,0,false);
				newNode.m_Words[0].SetFormGrammems(engNode.m_Words[j].GetFormGrammems());
			};
			
			newNode.m_Words[0].m_Poses = engNode.m_Words[j].m_Poses;
			newNode.m_Words[0].m_WordNo = curWNo+j;

			newNode.RusNode = engNode.RusNode;
			newNode.m_ClauseNo = engNode.m_ClauseNo;

			if( !articles[j].empty() )
			{
				newNode.m_ArticleStr = articles[j];
				newNode.m_ArticleCauseHistory.push_back(ArticleFromDict);
			};

			if( !poses[j].empty() )
			{
				BYTE pos;
				QWORD grm;
				if( m_pData->GetEngGramTab()->ProcessPOSAndGrammemsIfCan(poses[j].c_str(),&pos,&grm) )
				{
					newNode.m_Words[0].m_Poses = _QM(pos);
					newNode.m_Words[0].SetFormGrammems(grm);
				}
			}

			newNodes.push_back(newNode);
		}

		// выделим связи и сразу их зачистим
		vector<long> outRelsRaw;
		// в outRels загрузим все нереверсивные связи
		vector<long> outRels;
		GetOutcomingRelations(iEngNode,outRelsRaw);
		for( int m=0; m<outRelsRaw.size(); m++ )
		{
			if( !m_Relations[outRelsRaw[m]].m_bReverseRel )
				outRels.push_back(outRelsRaw[m]);
		}

		for( int m=0; m<outRels.size(); m++ )
		{
			if( m_Relations[outRels[m]].m_Valency.m_LeafId == 1 )
			{
				/*
					Закомментировано Сокиркой. Не понимаю, зачем это здесь
				*/
				//m_Relations[outRels[m]].m_bIsSubj = true;


				m_Relations[outRels[m]].m_SyntacticRelation = "subj";
				m_Relations[outRels[m]].m_SynReal.m_Preps.clear();
			}

			if( IsObjPattern(m_Relations[outRels[m]]) )
				m_Relations[outRels[m]].m_SyntacticRelation = "obj";

			m_Relations[outRels[m]].m_bInterpreted = false;
		}

// просчитаем их будущие места в m_Nodes[]
		vector<int> index;
		index.resize(newNodes.size());
		int curval = m_Nodes.size();
		for( int j=0; j<newNodes.size(); j++ )
		{
			if( j==iMainWord )
			{
				index[j] = iEngNode;
			}
			else
			{
				index[j] = curval;
				curval++;
			}
		}

// загрузим предлоги
		vector<TCortege> vecSPrp;
		GetSPrp(type,UnitNo,vecSPrp);
		StringVector preps;
		for( int j=0; j<nWords; j++ )
			preps.push_back("");
		for( int k=0; k<vecSPrp.size(); k++ )
		{
			int no = vecSPrp[k].m_BracketLeafId - 1;
			if( no>=0 && no<nWords )
				preps[no] = GetCortegeStr(type,vecSPrp[k]);
		}


// создадим новые связи
		SetRelsToDeleteFalse();
		vector<CEngSemRelation> newRels;
		for( int k=0; k<vecSINO.size(); k++ )
		{
			assert( vecSINO[k].m_DomItemNos[0]!=-1 );
			string ident = GetRoss(type)->GetDomItemStr(vecSINO[k].m_DomItemNos[0]);
			string value = GetCortegeStr(type,vecSINO[k]);
			bool bAuxRelation  = ident == "numeral_transfer";
		//
			assert( vecSINO[k].m_DomItemNos[1]!=-1 );
			string e1 = GetRoss(type)->GetDomItemStr(vecSINO[k].m_DomItemNos[1]);
			string e1lt = e1.substr(0,1);
			assert(e1lt=="A" || e1lt=="C");
			string e1ns = e1.substr(1,2);
			int    e1no = atoi(e1ns.c_str());
			assert(e1no>0 && (e1lt=="A" || e1no<=nWords) );
		//
			assert( vecSINO[k].m_DomItemNos[2]!=-1 );
			string e2 = GetRoss(type)->GetDomItemStr(vecSINO[k].m_DomItemNos[2]);
			string e2lt = e2.substr(0,1);
			assert(e2lt=="A" || e2lt=="C");
			string e2ns = e2.substr(1,2);
			int    e2no = atoi(e2ns.c_str());
			assert(e2no>0 && (e2lt=="A" || e2no<=nWords) );
			
		// C-C связь
			if( e1lt=="C" && e2lt=="C" )
			{
				CEngSemRelation newRel(CValency(ident.c_str(),A_C),index[e1no-1],index[e2no-1],ident.c_str());
				newRel.m_bInterpreted = false;
				if( !preps[e2no-1].empty() )
				{
					CDictUnitInterp prep;
					prep.m_DictType = EngObor;
					prep.m_UnitNo = GetRoss(EngObor)->LocateUnit(preps[e2no-1].c_str(),1);
					assert(prep.m_UnitNo != ErrUnitNo);
					newRel.m_SynReal.m_Preps.push_back(prep);
				}
				newRels.push_back(newRel);
			}
		
		// C-A связь
			else if( e1lt=="C" && e2lt=="A" )
			{
				vector<long> outRelsRaw;
				vector<long> outRels;
				GetOutcomingRelations(iEngNode,outRelsRaw);
				for( int m=0; m<outRelsRaw.size(); m++ )
				{
					if( !m_Relations[outRelsRaw[m]].m_bReverseRel )
						outRels.push_back(outRelsRaw[m]);
				}
				for( int m=0; m<outRels.size(); m++ )
				{
					if( m_Relations[outRels[m]].m_Valency.m_LeafId != e2no )
						continue;
					int Anode = m_Relations[outRels[m]].m_TargetNodeNo;
					int Cnode = index[e1no-1];
					CEngSemRelation newRel(CValency(ident.c_str(),A_C),Cnode,Anode,ident.c_str());
					newRel.m_bInterpreted = false;
					newRel.m_SynReal.m_Preps = m_Relations[outRels[m]].m_SynReal.m_Preps;
					newRel.m_Position = m_Relations[outRels[m]].m_Position;
					newRel.m_PosType  = m_Relations[outRels[m]].m_PosType;
					newRels.push_back(newRel);
					if (!bAuxRelation)
					m_Relations[outRels[m]].m_bToDelete = true;
					break;
				}
			}
		
		// A-C связь
			else if( e1lt=="A" && e2lt=="C" )
			{
				vector<long> outRelsRaw;
				vector<long> outRels;
				GetOutcomingRelations(iEngNode,outRelsRaw);
				for( int m=0; m<outRelsRaw.size(); m++ )
				{
					if( !m_Relations[outRelsRaw[m]].m_bReverseRel )
						outRels.push_back(outRelsRaw[m]);
				}
				for( int m=0; m<outRels.size(); m++ )
				{
					if( m_Relations[outRels[m]].m_Valency.m_LeafId != e1no )
						continue;
					int Anode = m_Relations[outRels[m]].m_TargetNodeNo;
					int Cnode = index[e2no-1];
					CEngSemRelation newRel(CValency(ident.c_str(),A_C),Anode,Cnode,ident.c_str());
					newRel.m_bInterpreted = false;
					newRels.push_back(newRel);
					if (!bAuxRelation)
						m_Relations[outRels[m]].m_bToDelete = true;
					break;
				}
			}
		}

	   DelRelsToDelete();

	// вставим узлы 
	    CEngSemNode OldCollocNode = m_Nodes[iEngNode];
		for( int j=0; j<newNodes.size(); j++ )
		{
			if( j==iMainWord )
				m_Nodes[iEngNode] = newNodes[j];
			else
				m_Nodes.push_back(newNodes[j]);
		}
	// вставим отношения
		for( int k=0; k<newRels.size(); k++ )
		{
			if (newRels[k].m_SyntacticRelation == "poss")
			{
				try{
				  CTempArticle A;
				  A.m_pRoss = const_cast<CDictionary*> (GetRoss(Aoss));
				  A.SetArticleStr("GF*     = *  NP_poss");
				  TCortege C = A.GetCortege(0);	
				  newRels[k].m_SynReal.m_Cortege = C;		
				}
				catch (...) 
				{
				};

			};
			if (newRels[k].m_SyntacticRelation == "subj")
			{

				assert ( HasSubjAsFirstValency(OldCollocNode) );

				newRels[k].m_SynReal.m_Cortege = OldCollocNode.m_Patterns[0].m_GramCorteges[0];
			};

			
			if (newRels[k].m_SyntacticRelation == "numeral_transfer")
			{
				QWORD  TargetGrammems  = m_Nodes[newRels[k].m_TargetNodeNo].GetGrammemsRich();
				TargetGrammems &=  ~eAllNumbers;
				TargetGrammems |=  eAllNumbers & m_Nodes[newRels[k].m_SourceNodeNo].GetGrammemsRich();
				continue;
				
			};


	
			m_Relations.push_back(newRels[k]);
			
		}

		// вставим дополнительные отношения (только для THESAME)	    
		vector<CDopField> DopFields;
		long MainItemNo = GetRossHolder(type)->GetDopFields(UnitNo, DopFields);
		for (int i=0;  i < DopFields.size(); i++)
			if (DopFields[i].m_RelationStr == "THESAME")			
			{
				const CDopField& F = DopFields[i];
				int Nd1 = F.m_IsActant1 ? FindNodeByLeafId(iEngNode, F.m_Word1) : index[F.m_Word1-1];
				int Nd2 = F.m_IsActant2 ? FindNodeByLeafId(iEngNode, F.m_Word2) : index[F.m_Word2-1];
				if ( (Nd1 == -1)  || (Nd2 == -1) ) continue;
					m_DopRelations.push_back(CEngSemRelation(CValency("THESAME", A_C),  Nd2,  Nd1,  ""));			
			};

	}
}

/////////////////////////////////////////////////////////////////////////////
// CheckQuestionClause()

bool CEngSemStructure::CheckQuestionClause(int iEngRoot,int &iQueRel,int &iSubRel)
{
// ищем ВС
	iQueRel = -1;
	vector<long> outChilds;
	GetChildNodes(iEngRoot,outChilds);
	for( int i=0; i<outChilds.size(); i++ )	
	{
		CEngSemNode& engNode = m_Nodes[outChilds[i]];
		if( engNode.m_MainWordNo!=-1 &&
			HasQuest(engNode.GetUnitNo(),engNode.GetType()) &&
			engNode.m_Words[engNode.m_MainWordNo].m_Lemma != "whether" )
		{
			vector<long> inRels;
			GetIncomingRelations(outChilds[i],inRels,false);
			assert( inRels.size()==1 );
			iQueRel = inRels[0];
			break;
		}
	}
	
// ищем subj
	iSubRel = -1;
	vector<long> outRels;
	GetOutcomingRelations(iEngRoot,outRels);
	for( int i=0; i<outRels.size(); i++ )	
	{
		if( IsSubj(m_Relations[outRels[i]]) )
		{
			iSubRel = outRels[i];
			break;
		}
	}

// если ВС "кто" или "что" (являющееся подлежащим)
	if( iQueRel!=-1 && iSubRel!=-1 && iQueRel==iSubRel )
	{
		CEngSemNode& engNode = m_Nodes[m_Relations[iQueRel].m_TargetNodeNo];
		if(engNode.RusNode!=-1 )
		{
			const CSemNode& rusNode = RusStr.GetNode(engNode.RusNode);
			if( rusNode.m_MainWordNo!=-1 )
			{
				string qwrd = rusNode.GetWord(rusNode.m_MainWordNo).m_Lemma;
				if( (qwrd=="КТО" || qwrd=="ЧТО") )
					return true;
			}
		}
	}

// если ВС "какой" или "чей" (отнсящееся к подлежащему)
	if( iQueRel!=-1 && iSubRel!=-1 &&
		m_Relations[iQueRel].m_SourceNodeNo==m_Relations[iSubRel].m_TargetNodeNo )
	{
		CEngSemNode& engNode = m_Nodes[m_Relations[iQueRel].m_TargetNodeNo];
		if(engNode.RusNode!=-1 )
		{
			const CSemNode& rusNode = RusStr.GetNode(engNode.RusNode);
			if( rusNode.m_MainWordNo!=-1 )
			{
				string qwrd = rusNode.GetWord(rusNode.m_MainWordNo).m_Lemma;
				if( qwrd=="КАКОЙ" || qwrd=="ЧЕЙ" )
					return true;
			}
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// HandleQuestionClause()

void CEngSemStructure::HandleQuestionClause(int iEngRoot)
{
	if( m_Nodes[iEngRoot].m_MainWordNo==-1 )
		return;
	int iRusRoot = m_Nodes[iEngRoot].RusNode;
	if( iRusRoot == -1 )
		return;

	int iQueRel,iSubRel;
	bool bRet = CheckQuestionClause(iEngRoot,iQueRel,iSubRel);

// Quest
	int iQueGrp = iQueRel;
	if( iQueRel!=-1 )
	{
		if( m_Relations[iQueRel].m_bReverseRel )
			m_Relations[iQueRel].m_Position = ">>";
		else
			m_Relations[iQueRel].m_Position = "<<";
		m_Relations[iQueRel].m_PosType = FromAlgorithmPosType;
	
		if( m_Relations[iQueRel].m_SourceNodeNo==iEngRoot )
		{	
			m_Relations[iQueRel].m_Valency.m_RelationStr = "Q?";
		}
		else
		{
			while( true )
			{
				vector<long> inRels;
				GetIncomingRelations(m_Relations[iQueGrp].m_SourceNodeNo,inRels);
				if( inRels.size()!=1 )
					break;
		 		iQueGrp = inRels[0];
				if( m_Relations[iQueGrp].m_SourceNodeNo==iEngRoot )
					break;
			}
		}
	}
	

// если существует ВС 
	if( iQueGrp!=-1 )
	{
// "кто", "что", "какой", "чей"
		if( bRet )
		{
		}

// если английский глагол-сказуемое - 'be'
		else if( m_Nodes[iEngRoot].m_Words[0].m_Lemma == "be" )
		{
			m_Relations[iQueGrp].m_Position = "begin";
			m_Relations[iQueGrp].m_PosType = FromAlgorithmPosType;
			if( iSubRel!=-1 )
			{
				m_Relations[iSubRel].m_Position = ">>";
				m_Relations[iSubRel].m_PosType = FromAlgorithmPosType;
			}
		}
// если английский глагол-сказуемое - не 'be' 
		else
		{
			m_Relations[iQueGrp].m_Position = "begin";
			m_Relations[iQueGrp].m_PosType = FromAlgorithmPosType;
			if( iSubRel!=-1 )
			{
				m_Relations[iSubRel].m_Position = ">>";
				m_Relations[iSubRel].m_PosType = FromAlgorithmPosType;
			}
		}
	}

// если вопросительного слова нет
	else
	{
// если английский глагол-сказуемое - 'be'
		if( m_Nodes[iEngRoot].m_Words[0].m_Lemma == "be" )
		{
			if( iSubRel!=-1 )
			{
				m_Relations[iSubRel].m_Position = ">>";
				m_Relations[iSubRel].m_PosType = FromAlgorithmPosType;
			}
		}
// если английский глагол-сказуемое - не 'be' 
		else
		{
			if( iSubRel!=-1 )
			{
				m_Relations[iSubRel].m_Position = ">>";
				m_Relations[iSubRel].m_PosType = FromAlgorithmPosType;
			}
		}
	}

// Если в клаузе после вопросительного слова (кроме "how") идет инфинитив,
//  его нужно переводить без to
	int iSemMainWord = RusStr.GetNode(iRusRoot).m_MainWordNo;
	if( iSemMainWord!=-1 &&
		RusStr.GetNode(iRusRoot).GetWord(iSemMainWord).HasPOS(INFINITIVE) &&
		iQueRel!=-1 &&
		m_Nodes[m_Relations[iQueRel].m_TargetNodeNo].m_Words[0].m_Lemma != "how" )
	{
		m_Nodes[iEngRoot].m_bNotUseTo = true;
	}

// "ли" и "же" игнорируются
NEXTSTEP:;
	vector<long> outChilds;
	GetChildNodes(iEngRoot,outChilds);
	for( int i=0; i<outChilds.size(); i++ )
	{
		CEngSemNode& engNode = m_Nodes[outChilds[i]];
		if( engNode.RusNode==-1 )
			continue;
		const CSemNode& rusNode = RusStr.GetNode(engNode.RusNode);
		if( rusNode.m_MainWordNo==-1 )
			continue;
		if( (rusNode.GetWord(rusNode.m_MainWordNo).m_Lemma == "ЛИ" ||
			 rusNode.GetWord(rusNode.m_MainWordNo).m_Lemma == "ЖЕ") )
		{
			DelNode(outChilds[i]);
			goto NEXTSTEP;
		}
	}

// no matter
	if( m_Nodes[iEngRoot].HasRelOperator("no_matter") &&
		iQueRel!=-1 )
	{
		CEngSemNode newNode;
		CreateSimpleEnglNode("no matter",newNode,0,true);
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = m_Nodes[iEngRoot].RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngRoot].m_ClauseNo;
	
		long wNo = m_Nodes[m_Relations[iQueRel].m_TargetNodeNo].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);

		CEngSemRelation newRel(CValency("no_matter",A_C),
			m_Relations[iQueRel].m_TargetNodeNo,m_Nodes.size()-1,"");
		newRel.m_Position = "<<";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel);

		m_Nodes[iEngRoot].DelRelOperator("no_matter");
	}		
}

/////////////////////////////////////////////////////////////////////////////
// HandleQuestionNode()

bool CEngSemStructure::HandleQuestionNode()
{
	bool bRet = false;
	
	for( int iEngNode=0; iEngNode<m_Nodes.size(); iEngNode++ )
	{
		int iMainWord = m_Nodes[iEngNode].m_MainWordNo;
		if( iMainWord == -1 ) 
			continue;
		if( m_Nodes[iEngNode].GetType() != Aoss )
			continue;
		string dbg_str = GetRoss(Aoss)->GetEntryStr(m_Nodes[iEngNode].GetUnitNo());
		int    dbg_num = GetRoss(Aoss)->GetUnitMeanNum(m_Nodes[iEngNode].GetUnitNo());
		if( dbg_str != "__quest" || dbg_num != 1 )
			continue;

		CEngSemNode newNode;
		CreateSimpleEnglNode(", right",newNode,0,true);
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = m_Nodes[iEngNode].RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	
		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes[iEngNode] = newNode;
		
		vector<long> inRels;
		GetIncomingRelations(iEngNode,inRels,false);
		if( inRels.size() > 0 )
		{
			m_Relations[inRels[0]].m_bReverseRel = false;
			m_Relations[inRels[0]].m_Position = "end";
			m_Relations[inRels[0]].m_PosType = FromAlgorithmPosType;
		}

		bRet = true;
		break;
	}

	return bRet;
}
	
/////////////////////////////////////////////////////////////////////////////
// AddFixedGrammemsToNode()
// добавление словообразовательных граммем

void CEngSemStructure::AddFixedGrammemsToNode()
{

	for( long iEngNode=0; iEngNode<m_Nodes.size(); iEngNode++ )
	{
		if( m_Nodes[iEngNode].m_MainWordNo==-1 )
			continue;
		CEngSemNode& engNode = m_Nodes[iEngNode];
		CEngSemWord& engWord = engNode.m_Words[engNode.m_MainWordNo];
		bool bProper = false;
		if (engNode.RusNode != -1)
			bProper = RusStr.GetNode(engNode.RusNode).m_bProper;

		string lemma = engWord.m_Lemma;
		BYTE pos = GetOnePOS(engWord.m_Poses);
		if (lemma.length() == 0) continue;
		QWORD grammems = helper.GetFixedGrammemsByLemma(morphEnglish, lemma, pos,bProper);

		/*
		  число не может быть словообразовательной граммемой
		*/
		grammems &= ~eAllNumbers;

		engWord.SetFormGrammems(engWord.GetFormGrammems() | grammems) ;
	};
}

/////////////////////////////////////////////////////////////////////////////
