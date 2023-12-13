#include "StdAfx.h"
#include "EngSemStructure.h"
#include "seman/SemanLib/LexFuncts.h"

const int g_PersPronounsCount = 5;
std::string g_PersPronouns[g_PersPronounsCount] = {"Я","ТЫ","ОН","ОНА", "ОНО"};


bool CEngSemStructure::CheckDomensForCortege(StringVector& domens, TCortege& cortege, DictTypeEnum   DictType ) const
{
	const CSignat& Signat = GetRoss(DictType)->GetSignat(cortege);
	if( Signat.GetDomsWithDelims().size() != domens.size() )
		return false;

	for(int i = 0 ; i < Signat.GetDomsWithDelims().size() ; i++ )
	{
		const TSignatItem& SignatItem = Signat.GetDomsWithDelims()[i];
		std::string strDom = GetRoss(DictType)->m_Domens[SignatItem.m_DomNo].GetDomStr();
		if( strDom != domens[i] )
			return false;
	}
	return true;
}

BYTE CEngSemStructure::GetPosFromPosBit(size_t Pos) const
{
	for(int i = 0 ; i < 32 ; i++ )
		if( (1 << i) & Pos )
			return i;

	return 0;
}

void CEngSemStructure::ReadMorphFromMainGF(uint16_t UnitNo, DictTypeEnum type, CSemWord& SemWord) const
{
	BYTE pos;
	uint64_t Grammems;

    std::string UnitStr = GetRoss(type)->GetEntryStr(UnitNo);
	assert (UnitStr.length() > 0);
	
	if( UnitStr[0] == '+' )// Добавочная статья
	{
		UnitStr.erase(0,1);
		if( m_pData->GetEngGramTab()->ProcessPOSAndGrammemsIfCan(UnitStr.c_str(),&pos,&Grammems) )
		{
			SemWord.m_Poses = 1<<pos;
			SemWord.SetFormGrammems(Grammems);

		}
		return;
	};

	if	(		(UnitStr == "be")
			||	(SemWord.m_Lemma == "be")
		)
	{
			SemWord.m_Poses = 1<<eVBE;
			return;
	};

	std::vector<TCortege> corteges;
	GetRossHolder(type)->GetFieldValues(std::string("GF"),UnitNo,corteges);
	if( corteges.size() == 0 )
		return;
	if( corteges[0].is_null(0))  
		return;
	bool bFound = false;
	std::string strVal = GetCortegeStr(type, corteges[0]).c_str();

	if( strVal=="VERB_MODL" )
	{
		SemWord.m_Poses = 1<<eMOD;
		return;
	};
	
	int ii;
	if( (ii = strVal.find(":")) != string::npos )
		strVal.erase(ii);

	if(!strVal.empty())
		if (m_pData->GetEngGramTab()->ProcessPOSAndGrammemsIfCan(strVal.c_str(), &pos, &Grammems) )
		{
			SemWord.m_Poses = 1<<pos;
			SemWord.SetFormGrammems( Grammems );
		};
}

bool CEngSemStructure::HasThisLexFunc(std::string LexFunctName, CRossInterp UnitInterp, CLexicalFunctionField& LexicalFunct)
{	
	if( UnitInterp.m_UnitNo == ErrUnitNo )
		return false;
	assert (UnitInterp.m_DictType != NoneRoss);
	std::vector<CLexicalFunctionField> LexFuncts;
	GetRossHolder(UnitInterp.m_DictType)->GetLexFuncts(UnitInterp.m_UnitNo, LexFuncts, EngObor, GetRossHolder(EngObor));
	for(int i = 0 ; i < LexFuncts.size() ; i++ )
		{
		if( LexFunctName == LexFuncts[i].m_LexFunct )
		{
			LexicalFunct = LexFuncts[i];
			if( LexicalFunct.m_MeanNum == -1 )
				LexicalFunct .m_MeanNum = 1;
			return true;
		}
	}
	return false;
}


bool CEngSemStructure::HasStringInCortege(DictTypeEnum type, TCortege cortege, const std::string& str) const {
	for (int i = 0; i < MaxNumDom; ++i) {
		if (cortege.is_null(i)) {
			break;
		}
		if (GetItemStr(cortege.GetItem(i), type) == str) {
			return true;
		}
	}
	return false;
}

bool CEngSemStructure::HasCopul(std::vector<CValency>& Vals, std::string strRusVal)
{
	for(int i = 0 ; i < Vals.size() ; i++ )
	{
		if( ValencyEq(strRusVal,Vals[i].m_RelationStr) )
		{
			DictTypeEnum t = m_pData->GetTypeByRossHolder(Vals[i].m_RossHolder);
			if (t == NoneRoss) continue;
			CSemPattern semPattern;
			semPattern.InitSemPattern(GetRossHolder(t), Vals[i].m_UnitNo, Vals[i].m_LeafId, Vals[i].m_BracketLeafId);
			semPattern.LoadGramFromDict();
			for( auto& c: semPattern.GetGramCorteges())
			{
				if (HasStringInCortege(t, c, "Copul")) {
					return true;
				}
			}
		}
	}
	return false;
}

bool CEngSemStructure::HasALG(DictTypeEnum type, long UnitNo, std::string strAlg)
{
	if( type == NoneRoss )
		return false;
	return GetRossHolder(type)->HasFieldValue(std::string("ALG"), strAlg,  UnitNo);
}

std::string CEngSemStructure::GetItemStr(dom_item_id_t item_id, DictTypeEnum type /* = Aoss*/) const
{
	if( type == NoneRoss )
		return std::string();

	if( is_null(item_id) )
		return std::string();
	return GetRoss(type)->GetDomItemStr(item_id);
}

part_of_speech_mask_t CEngSemStructure::EngPOSesByRusPOS(part_of_speech_t rus_pos, std::string lemma)
{
	switch(rus_pos)
	{
		case NOUN :				{	return 1<<eNOUN;	};
		case ADJ_FULL :			{	return 1<<eADJ;	};
		case VERB:				{	return 1<<eVERB;	};
		case PRONOUN:			
			{	
				MakeUpperUtf8(lemma);
				if( WordInList((const char*)g_PersPronouns, g_PersPronounsCount, lemma) )
					return 1<<ePN;
				return	1<<ePRON;   
			};
		case PRONOUN_P:			{	return 1<<ePN_ADJ;};
		case PRONOUN_PREDK:		{	return 1<<eNUMERAL;};
		case ADV:				{	return 1<<eADV;	};
		case PREP:				{	return 1<<ePREP;	};
		case CONJ:				{	return 1<<eCONJ;	};
		case INTERJ:			{	return 1<<eINTERJ;	};
		case ADJ_SHORT:			{	return 1<<eADJ;	};
		case PARTICIPLE:		{	return 1<<eVERB;	};
		case ADVERB_PARTICIPLE: {	return 1<<eVERB;	};
		case PARTICIPLE_SHORT:	{	return 1<<eVERB;	};			
		case NUMERAL:			{	return 1<<eNUMERAL;};			
		case NUMERAL_P:			{	return 1<<eORDNUM;};			
	}

	return 0;
}



bool CEngSemStructure::GetDictUnitInterpForObor(std::string oborStr, CRossInterp& interp)
{
	long UnitNo = GetRoss(EngObor)->LocateUnit(oborStr.c_str(), 1);
	if( UnitNo == ErrUnitNo )
		return false;
	interp.m_DictType = EngObor;
	interp.m_UnitNo = UnitNo;
	return true;
}

bool CEngSemStructure::IsChtoOrKto(int iNode)
{
	if( m_Nodes[iNode].RusNode == -1)
		return false;
	const CSemNode& rusNode = RusStr.GetNode(m_Nodes[iNode].RusNode);
	if(	rusNode.GetWordsSize() != 1)		
		return false;

	if( (rusNode.GetWord(0).m_Lemma == "КТО") ||
		(rusNode.GetWord(0).m_Lemma == "ЧТО") )
		return true;
	
	return false;
}


void CEngSemStructure::GetIncomingRelationsInThisClause(int iNode, std::vector<long>& IncomeRels) const
{
	GetIncomingRelations(iNode, IncomeRels, false);
	for(int i = IncomeRels.size() - 1 ; i >= 0  ; i-- )
	{
		int iRel = IncomeRels[i];
		if (m_Nodes[m_Relations[iRel].m_SourceNodeNo].m_ClauseNo  != m_Nodes[iNode].m_ClauseNo)
			IncomeRels.erase(IncomeRels.begin() + i);
	}
}

bool CEngSemStructure::HasThisGX(const std::vector<TCortege>& gramCorteges, std::string value, DictTypeEnum type) const
{
	if( type == NoneRoss )
		return false;
	for(auto c: gramCorteges)
	{
		if (HasStringInCortege(type, c, value)) {
			return true;
		}
	}

	return false;	
}

std::string CEngSemStructure::GetCortegeStr(DictTypeEnum type, const TCortege& cortege) const
{
	if( type == NoneRoss )
		return "";
	std::string strRes;
	for (int i = 0; i < MaxNumDom; ++i) {
		if (cortege.is_null(i)) {
			break;
		}
		strRes += GetItemStr(cortege.GetItem(i), type);
		strRes += " ";
		i++;
	}
	Trim(strRes);
	return strRes;
}


bool CEngSemStructure::IsSubjPattern(const CEngSemRelation& Rel ) const
{
	return		IsGramFet(Rel,"subj")  
			||	IsGramFet(Rel,"subj_pl")
			||	IsGramFet(Rel,"subj_sg")
			;
}


bool CEngSemStructure::IsObjPattern(const CEngSemRelation& Rel ) const
{
	return IsGramFet(Rel,"obj")  || IsGramFet(Rel,"obj_pl");
}


/*
	проверяет,  что один из кортежей в Rel.m_Pattern.m_GramCorteges содержит константу strPattern
*/
bool CEngSemStructure::HasGramFet(const CEngSemRelation& Rel, const std::string strPattern) const
{
	if( !Rel.m_bInterpreted )
		return false;

	const CSemPattern& semPattern = Rel.m_Pattern;

	DictTypeEnum type = m_pData->GetTypeByRossHolder(Rel.m_Valency.m_RossHolder);
	for( auto& c: semPattern.GetGramCorteges())
	{
		if (HasStringInCortege(type, c, strPattern)) {
			return true;
		}
	}
	return false;
};

/*
	проверяет,  что Rel.m_SynReal.m_Cortege содержит константу strPattern
*/
bool CEngSemStructure::IsGramFet(const CEngSemRelation& Rel, const std::string strPattern) const
{
	if (Rel.m_Valency.m_RossHolder == 0) return false;

	DictTypeEnum type = m_pData->GetTypeByRossHolder(Rel.m_Valency.m_RossHolder);

	return HasStringInCortege(type, Rel.m_SynReal.m_Cortege, strPattern);
}


bool CEngSemStructure::IsSubj(const CEngSemRelation& Rel) const 
{
	if( IsSubjPattern(Rel) )
		return true;

	std::vector<long> outRels;
	GetOutcomingRelations(Rel.m_SourceNodeNo,outRels);

	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubjPattern(m_Relations[outRels[i]]) )
			return false;
	}

	if( Rel.m_RusRel != -1 && RusStr.IsRusSubj(Rel.m_RusRel) )
		return true;

	// копул прозрачен для подлежащего
	if( m_Nodes[Rel.m_TargetNodeNo].m_NodeType == Copul )
	{
		std::vector<long> rels;
		GetOutcomingRelations(Rel.m_TargetNodeNo,rels);
		for( int i=0; i<rels.size(); i++ )
		{
			if( IsSubj(m_Relations[rels[i]]) )
				return true;
		}
	}

	return false;
}

long  CEngSemStructure::GetSubj(long NodeNo) const 
{
	std::vector<long> Rels;
	GetOutcomingRelations(NodeNo,Rels);
	for (long i=0; i <Rels.size(); i++)
      if ( !IsRelBetweenClauses(m_Relations[Rels[i]]) )
		if (IsSubj(m_Relations[Rels[i]]))
			return m_Relations[Rels[i]].m_TargetNodeNo;
	return -1;
};

void CEngSemStructure::FillSemRelEquivs()
{	
m_SemRelEquivs.push_back(StringPair("PACIEN","OBJ"));m_SemRelEquivs.push_back(StringPair("CONTEN","OBJ"));}


bool CEngSemStructure::SetSimpleEngPrep(std::string strPrep, int iNode, int iRel)
{
	CDictUnitInterp prep;
	prep.m_UnitNo = GetRoss(EngObor)->LocateUnit(strPrep.c_str(), 1);
	//assert(prep.m_UnitNo != ErrUnitNo);
	if( prep.m_UnitNo != ErrUnitNo )
	{
		prep.m_DictType = EngObor;
		if( iNode != -1)
		{
			m_Nodes[iNode].m_SynReal.m_Preps.clear();
			m_Nodes[iNode].m_SynReal.m_Preps.push_back(prep);
		}
		if( iRel != -1 )
		{
			m_Relations[iRel].m_SynReal.m_Conj.m_UnitNo = ErrUnitNo;
			m_Relations[iRel].m_SynReal.m_Conj.m_DictType = NoneRoss;
			m_Relations[iRel].m_SynReal.m_Preps.clear();
			m_Relations[iRel].m_SynReal.m_Preps.push_back(prep);
		}
	}
	else
		return false;
	return true;
}


bool CEngSemStructure::HasSubjAsFirstValency(const CEngSemNode& N)
{
	if (N.m_Patterns.empty()) return false;
	if (N.m_Patterns[0].GetGramCorteges().empty()) return false;
	return HasStringInCortege(N.GetType(), N.m_Patterns[0].GetGramCorteges()[0], "subj");
};

/*
	В некоторых глаголах subj приписан не самой первой  валнтности, например, "seem", 
	поэтому приходится  его искать
*/
bool CEngSemStructure::FindSubjPattern(const CEngSemNode& N, CSemPattern& Result)
{
	DictTypeEnum type = N.GetType();
	if (type == NoneRoss) return false;

	for (auto& p: N.m_Patterns)
	{
		if (p.GetGramCorteges().empty()) continue;
		if (HasStringInCortege(type, p.GetGramCorteges()[0], "subj")) {
			Result = p;
			return true;
		}
	};

	for (auto& p : N.m_CopulPatterns)
	{
		if (p.GetGramCorteges().empty()) continue;
		if (HasStringInCortege(type, p.GetGramCorteges()[0], "subj")) {
			Result = p;
			return true;
		}
	};

	return false;
};




bool CEngSemStructure::SetSubjPattern(CEngSemRelation& semRel)
{	
	const CStructDictHolder* R = semRel.m_Valency.m_RossHolder;
	if (R == 0)  return false;

	TCortege T;
	T.SetItem(0, R->EnglSubjSynONo);
	T.m_FieldNo = R->GramFetFieldNo;
	T.m_LeafId = semRel.m_Valency.m_LeafId;
	semRel.m_SynReal.m_Cortege = T;
	return true;
}

void CEngSemStructure::HideCopul()
{
	PrintNodes();
	for(int i = 0 ; i < m_Nodes.size() ; i++ )
	{
		if( m_Nodes[i].m_NodeType == Copul )
		{
			std::vector<long> incomRels;
			GetIncomingRelations(i,incomRels, false);
			assert(incomRels.size() <= 1);
			if( incomRels.size() )
			{
				CEngSemNode& parentOfCopul = m_Nodes[m_Relations[incomRels[0]].m_SourceNodeNo];
				std::vector<long> outcomRels;
				GetOutcomingRelations(i,outcomRels);

				for(int j = 0 ; j < outcomRels.size() ; j++ )
				{
					CEngSemRelation& semRel = m_Relations[outcomRels[j]];
					if( !semRel.m_Pattern.IsEmpty() )
					{
						parentOfCopul.m_Patterns.push_back(semRel.m_Pattern) ;
					}
				}

				MoveRelations(i, m_Relations[incomRels[0]].m_SourceNodeNo);
				m_Relations.erase(m_Relations.begin() + incomRels[0]);
				DelNode(i);
				i--;
			}
			else
			{
				// Иначе просто удалим Copul (хотя мы этот случай не понимаем, 
				// но он был)
				DelNode(i);
				i--;

			}
		}
	}
}



// опираясь на слот m_bSourceClauseMain, переворачиваем межклаузные стрелки,
//  так чтобы они всегда втыкались в подчинемую клаузу
void CEngSemStructure::ReverseClauseRel()
{
	for(int i = 0 ; i < m_Relations.size() ; i++ )
	{
		CEngSemRelation& rel = m_Relations[i];
		if( m_Nodes[rel.m_SourceNodeNo].m_ClauseNo != m_Nodes[rel.m_TargetNodeNo].m_ClauseNo &&
			!rel.m_bSourceClauseIsMain )
			ReverseRelation(i);
	}
}

void CEngSemStructure::MakeEngInfinitive(int iNode)
{
	CEngSemNode& child = m_Nodes[iNode];
	if( child.m_Words[child.m_MainWordNo].m_Poses != 1<<eVERB )
		return;
	if( child.m_MainWordNo == -1 )
		return;
	child.m_Words[child.m_MainWordNo].SetTense(present_inf_tn, "MakeEngInfinitive");
}

long CEngSemStructure::GetEngNodeByRusNode(long RusNodeNo) const
{
	for( long i=0; i<m_Nodes.size(); i++ )
	{
		if( m_Nodes[i].RusNode == RusNodeNo )
			return i;
	}

    return -1;
}

void CEngSemStructure::FilLexFunctRel()
{
	for(int i = 0 ; i < m_LexFuncts.size(); i++ )	
	{
		CLexFunctRel& LexFunctRel = m_LexFuncts[i];
		long SourceEngNode = GetEngNodeByRusNode(LexFunctRel.m_SourceNodeNo);
		long TargetEngNode = GetEngNodeByRusNode(LexFunctRel.m_TargetNodeNo);
		if( ( SourceEngNode != -1 ) && 
			( TargetEngNode != -1 ) )
		{
			LexFunctRel.m_SourceNodeNo = SourceEngNode;
			LexFunctRel.m_TargetNodeNo = TargetEngNode;
		}	
		else		
			m_LexFuncts.erase(m_LexFuncts.begin() + i--);
	}
}


bool CEngSemStructure::IsPlugArticle( const CStructDictHolder* RossHolder, uint16_t UnitNo) const
{
    if (!RossHolder || (UnitNo == ErrUnitNo)) return false;
	std::vector<TCortege> corteges;
	RossHolder->GetFieldValues(std::string("TYP"), UnitNo, corteges);
	for(auto& c: corteges)
	{
		if( !c.is_null(0) )
			if( std::string("ЗАГЛУШКА") == RossHolder->GetRoss()->GetDomItemStr(c.GetItem(0))  )
				return true;
	}
	return false;
}

bool CEngSemStructure::IsValFromRossArticle(const CSemRelation& semRel) const
{
	return (     (semRel.m_Valency.m_RossHolder) 
			&&   (semRel.m_Valency.m_UnitNo != ErrUnitNo) );
}

bool CEngSemStructure::TryToAddNewSemPatternFromLexFunWord(int iRel)
{
	CEngSemRelation& semRel = m_Relations[iRel];
	std::vector<long> InRels;
	GetIncomingRelations(semRel.m_SourceNodeNo, InRels, false);
	if( InRels.size() != 1 )
		return false;

	
	int iLexFun = FindLexFunct(m_Relations[InRels[0]].m_SourceNodeNo, m_Relations[InRels[0]].m_TargetNodeNo);
	if( iLexFun == -1 )
		return false;
		
	CLexFunctRel& LexFunctRel = m_LexFuncts[iLexFun];

	int iOperNum;
	CSemPattern semPattern1;
	if( IsOper(LexFunctRel.m_LexFunctName, iOperNum) )
	{		
		if( semRel.m_PatternNoInArticle >= 1 )
			if( AddGXiFromLexFunOperWord(m_Nodes[LexFunctRel.m_TargetNodeNo].GetUnitNo(), semPattern1, semRel.m_PatternNoInArticle, iOperNum) )
				{					
					semRel.m_Pattern.CopyGramCorteges(semPattern1);
					return true;
				}
	}

	int iFuncNum;
	if( IsFunc(LexFunctRel.m_LexFunctName, iFuncNum) )
	{
		
		if( semRel.m_PatternNoInArticle >= 1 )
			if( AddGXiFromLexFunFuncWord(m_Nodes[LexFunctRel.m_TargetNodeNo].GetUnitNo(), semPattern1, semRel.m_PatternNoInArticle, iFuncNum) )
			{
				semRel.m_Pattern.CopyGramCorteges(semPattern1);
				return true;
			}
	}

	return false;
}

void CEngSemStructure::ChangeGrammemsForDirObj(int iNode)
{
	m_Nodes[iNode].DeleteGrammemsRich( _QM( eNominative ) );
	m_Nodes[iNode].AddOneGrammemRich(eObjectCase);
}

void CEngSemStructure::FreeWordNo(long WordNo)
{
	for( int i=0; i<m_Nodes.size(); i++ )
		::FreeWordNo(m_Nodes[i],WordNo);
}


// узел является самой левой вершиной клаузы	
bool CEngSemStructure::IsLeftClauseTop(long NodeNo) const
{
   return GetLeftClauseTop(m_Nodes[NodeNo].m_ClauseNo) == NodeNo;
}


// самая левая вершина клаузы	
long  CEngSemStructure::GetLeftClauseTop(long ClauseNo) const
{
   std::vector<long> roots;	
   GetClauseRoots(ClauseNo,roots);
   if( roots.size() == 0 )
	   return -1;
   sort(roots.begin(),roots.end(),IsLessByMinWordNo(this));
   return roots[0];
}

void CEngSemStructure::dfs_out(size_t NodeNo, std::vector<long>& Nodes) const
{
  Nodes.push_back(NodeNo);
  for (size_t i=0; i<m_Relations.size(); i++)
	if (m_Relations[i].m_SourceNodeNo == NodeNo)
     if ( !_find(Nodes, m_Relations[i].m_TargetNodeNo) )
    	  dfs_out(m_Relations[i].m_TargetNodeNo, Nodes);
};

struct CClauseRootProps {
	long m_RootNodeNo;
	bool m_bHasPredicate;
	long m_NodesCount;
	CClauseRootProps (long RootNodeNo)
	{
		m_RootNodeNo = RootNodeNo;
	    m_bHasPredicate = false;
	    m_NodesCount = 0;
	};
	CClauseRootProps ()
	{
		m_RootNodeNo = -1;
	    m_bHasPredicate = false;
	    m_NodesCount = 0;
	};
	bool operator < (const CClauseRootProps& X) const 
	{
		if (m_bHasPredicate != X.m_bHasPredicate)
			return m_bHasPredicate < X.m_bHasPredicate;
		if (m_NodesCount != X.m_NodesCount)
			return m_NodesCount < X.m_NodesCount;

		// вернем что-нибудь, чтобы не зациклив.
		return m_RootNodeNo < X.m_RootNodeNo;
	};

};

bool  CEngSemStructure::IsTheVeryLeftNodeOfClause(long NodeNo) const
{
	if(m_Nodes[NodeNo].RusNode == -1) return false;

	long w_pos =  RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetMinWordNo();
	long cl_no =  RusStr.GetNode(m_Nodes[NodeNo].RusNode).m_ClauseNo;

	// если найдется узел, который стоял раньше узла node, то э
	// то не opener
	for (long i=0; i < m_Nodes.size();i++)
	  if( m_Nodes[i].RusNode != -1)
	  	 if (RusStr.GetNode(m_Nodes[i].RusNode).m_ClauseNo == cl_no) 
			if ( RusStr.GetNode(m_Nodes[i].RusNode).IsWordContainer())
				if ( RusStr.GetNode(m_Nodes[i].RusNode).GetMinWordNo() < w_pos )
					return false;

    return true;
};

long  CEngSemStructure::GetMainClauseRoot(long ClauseNo) const
{
   std::vector<long> roots;	
   GetClauseRoots(ClauseNo,roots);
   if( roots.size() == 0 )
	   return -1;
   if (roots.size() == 1) return roots[0];
   
   std::vector<CClauseRootProps >  RootProps;
   for (long i=0; i < roots.size(); i++)
   {
	   std::vector<long> Nodes;
	   dfs_out(roots[i], Nodes);
	   CClauseRootProps P (roots[i]);;
	   for (long j=0; j < Nodes.size(); j++)
		  {
			 P.m_NodesCount ++;
			 if (m_Nodes[Nodes[j]].GetTense())
				 P.m_bHasPredicate = true;
		 };
	   RootProps.push_back(P);
   };
   sort(RootProps.begin(),RootProps.end());
   return RootProps[RootProps.size() - 1].m_RootNodeNo;
};



void CEngSemStructure::dfs(size_t NodeNo, bool ConsiderUseOfNodes, long Tag)
{
  m_Nodes[NodeNo].m_bReached = true;
  if (Tag != -1) m_Nodes[NodeNo].m_Tag = Tag;
  long Count = m_Relations.size();
  for (size_t i=0; i<Count; i++)
	  {
		  if(m_Relations[i].m_SourceNodeNo == NodeNo)
			  	  if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_bReached == false)
					  if (!ConsiderUseOfNodes || m_Relations[i].m_bRelUse)
						  if (Tag == -1)
					        dfs(m_Relations[i].m_TargetNodeNo, ConsiderUseOfNodes, Tag);

		   if (m_Nodes[m_Relations[i].m_SourceNodeNo].m_bReached == false)
			    if ( NodeNo == m_Relations[i].m_TargetNodeNo)
					if (!ConsiderUseOfNodes || m_Relations[i].m_bRelUse)
						if (Tag == -1)
				  	       dfs(m_Relations[i].m_SourceNodeNo, ConsiderUseOfNodes, Tag);

	  };
};


bool CEngSemStructure::has_plural_rel(long NodeNo) const
{
	
	std::vector<long> rels;
	GetOutcomingRelations(NodeNo, rels);
	for(int i = 0; i < rels.size(); i++){
		if(m_Relations[rels[i]].m_Valency.m_RelationStr == "QUANTIT"){			
			const CEngSemNode& node = m_Nodes[m_Relations[rels[i]].m_TargetNodeNo];

			// случай how_much и how_many, для которых написан специальный алгоритм
			if (node.m_Words.size() != 1) return false;
			float Number = atof (node.m_Words[0].GetWord().c_str());
			if( Number==0 )
				return false; 
			if(    ! ( (Number != 0) && (Number <2))
				&&   node.m_Words[0].m_Lemma != "ОДИН"
			  )
			return true;

		}
	}
	//!!!
	return false;
}




