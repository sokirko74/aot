/////////////////////////////////////////////////////////////////////////////
// Английские алгоритмы 5

// MoveMeaningRelsToNewNode()
// MakeBeNodeForEngNode()
// ApplyVerbToAdjRule()
// ApplyALG_AL3()
// ApplyInvitatoryRule()
// RefineByNumeralMarks()
// RefineSynRealMarks()

#include "StdAfx.h"
#include "EngBinaryTranslate.h"

/////////////////////////////////////////////////////////////////////////////
// MoveMeaningRelsToNewNode()


void  CEngSemStructure::MoveMeaningRelsToNewNode(int iOldNode,int iNewNode)
{
// перевешиваем подлежащее на вспомогательный глагол
	vector<long> outRels;
	GetOutcomingRelations(iOldNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		if( IsSubj(m_Relations[outRels[i]]) )
		{
			m_Relations[outRels[i]].m_SourceNodeNo = iNewNode;
		}
	}

// входящие связи
	vector<long> inRels;
	GetIncomingRelations(iOldNode,inRels,false);
	for( int i=0; i<inRels.size(); i++ )
	{
		if( m_Relations[inRels[i]].m_SourceNodeNo != iNewNode )
			m_Relations[inRels[i]].m_TargetNodeNo = iNewNode;
	}

// выходящие межклаузные связи
	outRels.clear();
	GetOutcomingRelations(iOldNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
		m_Relations[outRels[i]].m_SourceNodeNo = iNewNode;
	}

// позиционные связи
	outRels.clear();
	GetOutcomingRelations(iOldNode,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
		int iSrc = m_Nodes[m_Relations[i].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo;
		if( iSrc != iTrg )
			continue;
		if( m_Relations[outRels[i]].m_Position == "begin" ||
			m_Relations[outRels[i]].m_Position == "<<"  ||
			m_Relations[outRels[i]].m_Position == "<" )
//			m_Relations[outRels[i]].m_Position == "end" )
		{
			m_Relations[outRels[i]].m_SourceNodeNo = iNewNode;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// MakeBeNodeForEngNode()

int CEngSemStructure::MakeBeNodeForEngNode(int iEngNode,string sTenseHistory,bool bCheckOtherVerb)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return -1;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return -1;
	int iSemMainWord = RusStr.GetNode(iRusNode).m_MainWordNo;
	if( iSemMainWord == -1 )
		return -1;
//
	if( bCheckOtherVerb )
	{
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
				return -1;
		}
	}
//
	CEngSemNode newNode;
	int iSubjPattern = -1,iObjPattern = -1;
	CreateBeSemNode(newNode,iSubjPattern,iObjPattern);
	assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
	assert( newNode.m_Words.size()==1 );

	newNode.RusNode = m_Nodes[iEngNode].RusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

	TransferTimeGrammems(RusStr.GetNode(iRusNode),newNode,sTenseHistory.c_str());
	if( newNode.m_Words[0].GetTense() == zero_tn )
		newNode.m_Words[0].SetTense(present_smp_tn,sTenseHistory.c_str());
	m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].SetTense(zero_tn,sTenseHistory.c_str());

	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode); // be
	int iBeNode = m_Nodes.size()-1;
	
CEngSemRelation newRel(CValency("S-ACT",A_C),m_Nodes.size()-1,iEngNode,"");	newRel.m_bInterpreted = true;
	newRel.m_Valency.m_RossHolder = GetRossHolder(m_Nodes[iBeNode].GetType());
	newRel.m_Valency.m_UnitNo = m_Nodes[iBeNode].GetUnitNo();
	newRel.m_Valency.m_LeafId = 2;
	newRel.m_Pattern = m_Nodes[iBeNode].m_Patterns[iObjPattern];
	newRel.m_SynReal.m_Cortege = m_Nodes[iBeNode].m_Patterns[iObjPattern].m_GramCorteges[0];
	m_Relations.push_back(newRel); // be->Node

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
		m_Relations[iSubRel].m_SourceNodeNo = iBeNode;
		m_Relations[iSubRel].m_bInterpreted = true;

		m_Relations[iSubRel].m_Pattern = m_Nodes[iBeNode].m_Patterns[iSubjPattern];
		m_Relations[iSubRel].m_SynReal.m_Cortege = m_Nodes[iBeNode].m_Patterns[iSubjPattern].m_GramCorteges[0];
		m_Relations[iSubRel].m_Valency = m_Nodes[iBeNode].m_Vals[0];

		CEngSemNode& subNode = m_Nodes[m_Relations[iSubRel].m_TargetNodeNo];
		if( subNode.m_NodeType == MNA )
			m_Nodes[iBeNode].AddOneGrammemRich(ePlural);
		else
			m_Nodes[iBeNode].AddGrammemsRich( subNode.GetGrammemsRich() & eAllNumbers);
	}
	else
		m_Nodes[iBeNode].AddOneGrammemRich( eSingular);

// связи
	MoveMeaningRelsToNewNode(iEngNode,iBeNode);
	return iBeNode;
}

/////////////////////////////////////////////////////////////////////////////
// ApplyVerbToAdjRule()
// "он отсутствовал"

void CEngSemStructure::ApplyVerbToAdjRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode == -1 )
		return;

	if( !RusStr.GetNode(iRusNode).HasPOS(VERB) )
		return;
	if( !m_Nodes[iEngNode].HasPOS(eADJ) )
		return;

	MakeBeNodeForEngNode(iEngNode,"ApplyVerbToAdjRule");
}

/////////////////////////////////////////////////////////////////////////////
// ApplyALG_AL3()
// обработка транзитивных глаголов
// он дал мне яблоко
// он дал яблоко мне
// он дал большому толстому зеленому крокодилу яблоко

void CEngSemStructure::ApplyALG_AL3(int iEngNode)
{
	int iRusNode = m_Nodes[iEngNode].RusNode;
	if( iRusNode==-1 )
		return;

	if( !HasALG(m_Nodes[iEngNode].GetType(),m_Nodes[iEngNode].GetUnitNo(),"AL3") )
		return;
//
	vector<long> outRels;
	GetOutcomingRelations(iEngNode,outRels);

	int iObjRel = -1, iDirRel = -1;
	for( int i=0; i<outRels.size(); i++ )
	{
		CSemRelation& engRel = m_Relations[outRels[i]];
if( engRel.m_Valency.m_RelationStr == "OBJ" ||engRel.m_Valency.m_RelationStr == "CONTEN" )		{
			iObjRel = outRels[i];
			continue;
		}
if( engRel.m_Valency.m_RelationStr == "ADR")		{
			iDirRel = outRels[i];
			continue;
		}
	}

	if( iObjRel==-1 || iDirRel==-1 )
		return;

	int iObjNode = m_Relations[iObjRel].m_TargetNodeNo;
	int iDirNode = m_Relations[iDirRel].m_TargetNodeNo;
//
	int iObjPos = m_Nodes[iObjNode].GetMinWordNo();
	int iDirPos = m_Nodes[iDirNode].GetMinWordNo();

	bool bDirLong = (m_Nodes[iDirNode].GetPos() != ePN);

	if( bDirLong || iDirPos>iObjPos )
	{
		SetSimpleEngPrep("to",-1,iDirRel);
		m_Relations[iDirRel].m_SyntacticRelation = "indir_obj";
	}
}

/////////////////////////////////////////////////////////////////////////////
// ApplyInvitatoryRule()
// Выслушай меня и давай иди на работу
// Давай пойдем на концерт завтра, потому что сегодня у меня есть срочные дела
// Пошли домой
// Давай мы сначала поедим, а потом пойдем гулять
// Может быть, давайте мы ему не скажем, что случилось?

void CEngSemStructure::ApplyInvitatoryRule(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;

	string strSoft = "_мягк_пригласит_наклонение";
	string strHard = "_пригласит_наклонение";
	
	bool bSoft = m_Nodes[iEngNode].HasRelOperator(strSoft);
	bool bHard = m_Nodes[iEngNode].HasRelOperator(strHard);
	
	if( !( bSoft || bHard ) )
		return;
//
	if( bHard && m_Nodes[iEngNode].HasGrammemRich(eSecondPerson) )
	{
		CEngSemNode newNode;
		CreateSimpleEnglNode("come on,",newNode,0,true);
		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = m_Nodes[iEngNode].RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	
		long wNo = m_Nodes[iEngNode].GetMinWordNo();
		FreeWordNo(wNo);
		newNode.m_Words[0].m_WordNo = wNo;
		m_Nodes.push_back(newNode);
		int iNewNode = m_Nodes.size()-1;

		if( bSoft )
			m_Nodes[iEngNode].DelRelOperator(strSoft);
		else
			m_Nodes[iEngNode].DelRelOperator(strHard);
		m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].SetTense(present_inf_tn,"ApplyInvitatoryRule");
		m_Nodes[iEngNode].m_bNotUseTo = true;

		CEngSemRelation newRel(CValency("Invitatory",A_C),iEngNode,iNewNode,"");
		newRel.m_Position = "begin";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel);
		return;
	}
//
	CEngSemNode newNode;
	CreateSimpleEnglNode(bSoft ? "should better" : "let's",newNode,0,true);
	assert( newNode.m_Words.size()==1 );

	newNode.RusNode = m_Nodes[iEngNode].RusNode;
	newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;
	
	long wNo = m_Nodes[iEngNode].GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode);
	int iNewNode = m_Nodes.size()-1;

	if( bSoft )
		m_Nodes[iEngNode].DelRelOperator(strSoft);
	else
		m_Nodes[iEngNode].DelRelOperator(strHard);
	m_Nodes[iEngNode].m_Words[m_Nodes[iEngNode].m_MainWordNo].SetTense(present_inf_tn,"ApplyInvitatoryRule");
	m_Nodes[iEngNode].m_bNotUseTo = true;

	CEngSemRelation newRel(CValency("Invitatory",A_C),iNewNode,iEngNode,"");
	m_Relations.push_back(newRel);

	MoveMeaningRelsToNewNode(iEngNode,iNewNode);
}

/////////////////////////////////////////////////////////////////////////////
// RefineByNumeralMarks()

string fix_case(const CEngSemWord& EngWord);

void CEngSemStructure::RefineByNumeralMarks(int iEngNode)
{
	if( m_Nodes[iEngNode].m_MainWordNo==-1 )
		return;
	CEngSemNode& engNode = m_Nodes[iEngNode];
	DictTypeEnum Etype = engNode.GetType();
	long         Eunit = engNode.GetUnitNo();
	if( Etype==NoneRoss || Eunit==ErrUnitNo )
		return;
	CEngSemWord& engWord = engNode.m_Words[engNode.m_MainWordNo];

	int iRusNode = engNode.RusNode;
	if( iRusNode == -1 )
		return;
	const CSemNode& RusNode = RusStr.GetNode(iRusNode);
	if( RusNode.IsTimeRossNode() )
		return;
	if( IsThesRoss(RusNode.GetType()) )
		return;

// смотрим, было ли указание на эту тему
	vector< SEngEquiv > vectorEngEquivs;
	GetEngEquivsFromRusArticle(vectorEngEquivs,RusNode.GetUnitNo(),RusNode.GetType(),iRusNode);

	string StrNumeral = "";
	for( int i=0; i<vectorEngEquivs.size(); i++ )
	{
		if( vectorEngEquivs[i].m_StrNumeral.empty() )
			continue;
		long Tunit = GetRoss(Etype)->LocateUnit(vectorEngEquivs[i].m_StrEngWord.c_str(),vectorEngEquivs[i].m_iMeanNum);
		if( Tunit != Eunit )
			continue;
		StrNumeral = vectorEngEquivs[i].m_StrNumeral;
	}

	if( StrNumeral.empty() )
		return;
//
	if( StrNumeral=="#delete_numeral_prefix" )
	{
		engWord.m_NumeralPrefix = "";
		return;
	}
//
	if( StrNumeral!="#ed" )
		return; // неизвестная помета
//
	vector<CFormInfo> piPC;
	helper.GetEngLemmatizer()->CreateParadigmCollection(true, engWord.m_Lemma, false, false, piPC);
	
	int pid = -1;
	for( int k=0; k<piPC.size(); k++ )
	{
		const CFormInfo& piPagadigm = piPC[k];
		string anc = piPagadigm.GetAncode(0);
		anc = anc.substr(0,2);
		string pos = helper.GetEngGramTab()->GetPartOfSpeechStr(helper.GetEngGramTab()->GetPartOfSpeech(anc.c_str()));
		if( pos != "VERB" )
			continue;
		pid = piPagadigm.GetParadigmId();
		break;
	}
	if( pid<0 )
		return;
//
	engWord.m_Word = helper.create_form_by_id(pid,_QM(ePastParticiple));
	engWord.m_Word = fix_case(engWord);
	engNode.m_Words[engNode.m_MainWordNo].m_Poses = _QM(eVERB);
	engNode.m_Words[engNode.m_MainWordNo].SetFormGrammems(_QM(ePastParticiple));
	engNode.m_Words[engNode.m_MainWordNo].SetTense(pp_tn,"RefineByNumeralMarks");
	engNode.m_Words[engNode.m_MainWordNo].m_bDoNotChangeForm = true;
}

/////////////////////////////////////////////////////////////////////////////
// RefineSynRealMarks()

void CEngSemStructure::RefineSynRealMarks(int iEngNode)
{
	CEngSemNode& engNode = m_Nodes[iEngNode];

	if( engNode.m_SynReal.m_AlgStr != "А1(НЕ)" )
		return;

	vector<long> inRels;
	GetIncomingRelations(iEngNode,inRels,false);
	for( int i=0; i<inRels.size(); i++ )
	{
		m_Nodes[m_Relations[inRels[i]].m_SourceNodeNo].m_RelOperators.push_back("НЕ");
	}
}

/////////////////////////////////////////////////////////////////////////////
/*
	TranslateAdverbAdjectiveRule ищет русские наречия  типа "по-английски", "по-ковбойски" (CSemWord::m_bAdverbFromAdjective),
	устанавливает им часть речи прилагательное.
	далее три случая:
1. если управляющее слово этого наречия  было COMMUNIC или  INTEL (писать, говорить думать)	и у этого слова не выражена  валентность  содержания, тогда  выходим из обработки.
	// "I speak English"

2. если управляющее слово этого наречия  было COMMUNIC или  INTEL (писать, говорить думать)	и у этого слова выражена  валентность  содержания, тогда  добавляем предлог "in" к прилагательному.
	// "I wrote a letter in English"

	2. иначе добавляем предлог "in" и слово "fashion"
	// "he was dressed in the Eastern fashion"
	
*/

void CEngSemStructure::TranslateAdverbAdjectiveRule(int EngNodeNo)
{

	CEngSemNode& engNode = m_Nodes[EngNodeNo];
	if (engNode.RusNode == -1) return;
	const CSemNode& RusNode = RusStr.GetNode(engNode.RusNode);
	if (!RusNode.IsWordContainer() ) return;
	if (!RusNode.GetWord(0).m_bAdverbFromAdjective) return;
	if (!engNode.IsWordContainer()) return;
	engNode.m_Words[0].m_Poses = _QM(eADJ);
	vector<long> inRels;
	GetIncomingRelations(EngNodeNo,inRels,false);
	// проходим мимо MUA	
	if (!inRels.empty())
		if (m_Nodes[m_Relations[inRels[0]].m_SourceNodeNo].m_NodeType == MNA)
			GetIncomingRelations(m_Relations[inRels[0]].m_SourceNodeNo, inRels,false);

	if (inRels.empty())
	{
		SetSimpleEngPrep("in", EngNodeNo, -1);
		return;
	}
		
	
	
	int TerminId = helper.GetThes(OmniThes)->GetTerminIdBySingleWord(RusNode.GetWord(0).m_Lemma.c_str());
	int ParentNodeNo = m_Relations[inRels[0]].m_SourceNodeNo;
	const CEngSemNode& ParentNode = m_Nodes[ParentNodeNo];
	if	(	(		HasSemFet(ParentNode, "INTEL")||	HasSemFet(ParentNode, "COMMUNIC")			)
				&&	m_pData->GetThes(OmniThes)->IsA(TerminId, "_ЯЗЫК")
			)
	{
			if (		!HasOutRelationByName(ParentNodeNo, "CONTEN")&&	!HasOutRelationByName(ParentNodeNo, "OBJ")				)
				return; // "I speak English"
			else
			{
				// "I wrote a letter in English"
				SetSimpleEngPrep("in", -1, inRels[0]);
			};
	}
	else
	{
		//he was dressed in the Eastern fashion - он был одет по-восточному
		CEngSemWord W;
		W.m_Word = "fashion";
		W.m_Lemma = "FASHION";
		W.m_Poses = _QM(eNOUN);
		engNode.m_Words.push_back(W);
		SetSimpleEngPrep("in", -1, inRels[0]);

	};

	

};
