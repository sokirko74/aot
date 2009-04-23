
/////////////////////////////////////////////////////////////////////////////
// Ïåðåâîä êëàóçû + íåêîòîðûå àíãëèéñêèå àëãîðèòìû

// IsSubConj()
// GetClauseRelType()
// TranslateClauseRels()

// ClauseRelRule_Participle()
// ClauseRelRule_Kakoj()
// ClauseRelRule_SubConj()
// ClauseRelRule_Kak()
// ClauseRelRule_ChtoKtoAsConjWords()
// ClauseRelRule_Kotoryj()

#include "StdAfx.h"

const int AntecWordsCount = 8;
const char g_strAntecWords[AntecWordsCount][MaxWordLen] = {"ÂÑÅ", "ÊÒÎ-ÍÈÁÓÄÜ", "ÊÒÎ-ÒÎ", "ÎÄÈÍ", "ÊÀÆÄÛÉ", "ÒÎÒ", "ÒÎ", "×ÒÎ-ÍÈÁÓÄÜ"};


#define _SWITCH(X) string __switch_val = X;
#define _CASE(Y) if (__switch_val == Y)

/////////////////////////////////////////////////////////////////////////////
// IsSubConj()

bool CEngSemStructure::IsSubConj(CRossInterp interp)
{
	if( interp.m_DictType != OborRoss)
		return false;
	bool bIf     = GetRossHolder(interp.m_DictType)->HasFieldValue(string("GF"),string("ÏÎÄ×_ÑÎÞÇ"),interp.m_UnitNo);
	bool bIfThen = GetRossHolder(interp.m_DictType)->HasFieldValue(string("GF"),string("ÏÎÄ×_ÐÀÇÐÛÂ_ÑÎÞÇ"),interp.m_UnitNo);
	return( bIf || bIfThen );
}

/////////////////////////////////////////////////////////////////////////////
// GetClauseRelType()

EClauseRelsType CEngSemStructure::GetClauseRelType(int iRel)
{
	CSemRelation& semRel = m_Relations[iRel];

	CEngSemNode& sourceNode = m_Nodes[semRel.m_SourceNodeNo];
	if( sourceNode.RusNode == -1 )
		return UndefType;
	const CSemNode& rusSourceNode = RusStr.GetNode(sourceNode.RusNode);
	if( !rusSourceNode.IsWordContainer() )
		return UndefType;

	if( IsSubConj(semRel.m_SynReal.m_Conj ))
		return SubConj;

	if (rusSourceNode.m_MainWordNo != -1)
	{

		if(			rusSourceNode.GetWord(rusSourceNode.m_MainWordNo).m_Lemma == "ÊÎÒÎÐÛÉ" 
				&&	semRel.m_Valency.m_RelationStr == "THESAME" )			
			return Kotoryj;

		if(		rusSourceNode.GetWord(rusSourceNode.m_MainWordNo).m_Lemma  == "ÊÀÊ" 
			&& semRel.m_Valency.m_RelationStr == "THESAME" )			
			return  Kak;

		if( rusSourceNode.GetWord(rusSourceNode.m_MainWordNo).m_Lemma == "ËÈ" )
			return Li;
	};

	CEngSemNode& targetNode = m_Nodes[semRel.m_TargetNodeNo];
	if( targetNode.RusNode == -1 )
		return UndefType;
	const CSemNode& rusTargetNode = RusStr.GetNode(targetNode.RusNode);

	if( rusTargetNode.m_MainWordNo!=-1 &&
		rusTargetNode.GetWord(rusTargetNode.m_MainWordNo).HasPOS(PARTICIPLE) )
		return Participle;

	if( rusTargetNode.m_MainWordNo!=-1 &&
		rusTargetNode.GetWord(rusTargetNode.m_MainWordNo).m_Lemma == "ÊÀÊÎÉ" )
		return Kakoj;

	if( rusTargetNode.m_MainWordNo!=-1 &&
		rusTargetNode.GetWord(rusTargetNode.m_MainWordNo).m_Lemma  == "ÊÀÊ")
		return  Kak;

	if( IsChtoOrKto(semRel.m_TargetNodeNo) || IsChtoOrKto(semRel.m_SourceNodeNo) )
		return ChtoKtoAsConjWords;

	string strAntecWord;
	if( m_Nodes[semRel.m_TargetNodeNo].RusNode != -1 )
	{
		int iRusNode = m_Nodes[semRel.m_TargetNodeNo].RusNode;
		if( RusStr.GetNode(iRusNode).GetWordsSize() != 0 )		
			strAntecWord =  RusStr.GetNode(iRusNode).GetWord(0).m_Lemma;		
	}

if( (semRel.m_Valency.m_RelationStr == "THESAME") &&		IsChtoOrKto(semRel.m_SourceNodeNo) &&
		WordInList((const char*)g_strAntecWords, AntecWordsCount, strAntecWord )
		)
		return ChtoKtoAsConjWords;
	
	return UndefType;
}

/////////////////////////////////////////////////////////////////////////////
// TranslateClauseRels()
void CEngSemStructure::TranslateClauseRels()
{

	size_t InitialRelationCount = m_Relations.size();

	for( int i=0; i< min(InitialRelationCount,m_Relations.size()); i++ )
	{		
		int iSrc = m_Nodes[m_Relations[i].m_SourceNodeNo].m_ClauseNo;
		int iTrg = m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo;
		if( iSrc == iTrg )
			continue;

		EClauseRelsType type = GetClauseRelType(i);
		m_Relations[i].m_Type = type;

		switch( type )
		{
			case SubConj:
				ClauseRelRule_SubConj(i);
			break;

			case Kotoryj:
				ClauseRelRule_Kotoryj(i);
			break;

			case Participle :
			{
				ClauseRelRule_Participle(i);
				break;
			}

			case ChtoKtoAsConjWords:
			{
				ClauseRelRule_ChtoKtoAsConjWords(i);
				break;
			}

			case Kak:
			{
				ClauseRelRule_Kak(i);
				break;				
			}
			case Kakoj:
			{
				ClauseRelRule_Kakoj(i);
					break;
			}

			default: 
			{
				CEngSemRelation& engSemRel = m_Relations[i];
				engSemRel.m_bInterpreted = false;
				if( engSemRel.m_RusRel == -1 )
					break;

				const CSemRelation& rusSemRel = *RusStr.GetRelation(engSemRel.m_RusRel);
				if( rusSemRel.m_SynReal.m_Conj.m_DictType == NoneRoss )
					break;

				vector<CEngInterp> equivs;
				FindEngEquivForRusArticle(rusSemRel.m_SynReal.m_Conj,equivs,EngObor);
				if( equivs.size() > 0 )	
				{
					engSemRel.m_SynReal.m_Conj = equivs[0];
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_Participle()
// Âîçüìè äîêóìåíò, ëåæàùèé íà ñòîëå

void CEngSemStructure::ClauseRelRule_Participle(int iRel)
{
	CEngSemRelation& semRel = m_Relations[iRel];
	if( semRel.m_RusRel == -1 )
		return;
	const CSemRelation& rusSemRel = *RusStr.GetRelation(semRel.m_RusRel);
	bool bAnim = HasSemFet(RusStr.GetNode(rusSemRel.m_SourceNodeNo),"ANIM");
	int iRusSrc = m_Nodes[semRel.m_SourceNodeNo].RusNode;
	int iRusTrg = m_Nodes[semRel.m_TargetNodeNo].RusNode;
	if( iRusSrc==-1 || iRusTrg==-1 )
		return;

	EngVerbTenseEnum newTense = (RusStr.GetNode(iRusTrg).GetGrammems() & _QM(rPastTense)) ? past_smp_tn: present_smp_tn;
	CEngSemNode& targetNode = m_Nodes[semRel.m_TargetNodeNo];
	targetNode.m_Words[targetNode.m_MainWordNo].SetTense(newTense,"ClauseRelRule_Participle");

	CEngSemNode newNode;
	CreateSimpleEnglNodeByOldNode(bAnim ? "who" : "which",newNode,0,true,targetNode);	
	newNode.SetGrammems(_QM(eNominative));
	newNode.m_ArticleStr = "-";

	long wNo = targetNode.GetMinWordNo();
	FreeWordNo(wNo);
	newNode.m_Words[0].m_WordNo = wNo;
	m_Nodes.push_back(newNode);
	int iNewNode = m_Nodes.size()-1;

	long saveSource  = semRel.m_SourceNodeNo;
	semRel.m_SourceNodeNo = m_Nodes.size()-1;
	ReverseRelation(iRel);
	semRel.m_Type = NotBetweenClauses;

	long DopRelNo = FindDopFirstRelation(semRel.m_SourceNodeNo,saveSource);
	if( DopRelNo != -1 )
	{
		m_Nodes[iNewNode].AddGrammemsRich( m_Nodes[m_DopRelations[DopRelNo].m_TargetNodeNo].GetGrammemsRich() & eAllNumbers );
		
		semRel.m_Valency = m_DopRelations[DopRelNo].m_Valency;
		semRel.m_SynReal = m_DopRelations[DopRelNo].m_SynReal;
		semRel.m_Pattern = m_DopRelations[DopRelNo].m_Pattern;
		semRel.m_bInterpreted = true;
	}

	CEngSemRelation newRel(CValency("THESAME",A_C,NULL), m_Nodes.size() - 1, saveSource, "");	
	newRel.m_Type = Kotoryj;
	newRel.m_bSourceClauseIsMain = false;
	
	m_Relations.push_back(newRel);
}

/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_Kakoj()

void CEngSemStructure::ClauseRelRule_Kakoj(int iRel)
{
	CEngSemRelation& semRel = m_Relations[iRel];	
	CEngSemNode& targetNode = m_Nodes[semRel.m_TargetNodeNo];
	
	CEngSemNode newNode;
	CreateSimpleEnglNode("which", newNode, 0, true);
	newNode.m_ClauseNo = m_Nodes[semRel.m_TargetNodeNo].m_ClauseNo;
	newNode.m_Words[0].m_WordNo = m_Nodes[semRel.m_TargetNodeNo].m_Words[0].m_WordNo;

	m_Nodes[semRel.m_TargetNodeNo] = newNode;
	semRel.m_bSourceClauseIsMain = true;

	vector<long> incomingRels;
	GetIncomingRelations(semRel.m_TargetNodeNo, incomingRels, false);

	if( incomingRels.size() == 2 )
	{
		int iRel1 = (incomingRels[0] == iRel ) ? incomingRels[1] : incomingRels[0];
		if( m_Relations[iRel1].m_SynReal.m_Preps.size()  )
		{
			m_Relations[iRel].m_SynReal = m_Relations[iRel1].m_SynReal;
			m_Relations[iRel1].m_SynReal.m_Preps.clear();
			m_Relations[iRel1].m_SynReal.m_Other.erase();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_SubConj()


void CEngSemStructure::ClauseRelRule_SubConj(int iRelNum)
{
	CEngSemRelation& engSemRel = m_Relations[iRelNum];
	CEngSemNode& engNode = m_Nodes[engSemRel.m_SourceNodeNo];
	if( engSemRel.m_RusRel == -1 )
		return;
	const CSemRelation& rusSemRel = *RusStr.GetRelation(engSemRel.m_RusRel);

	engSemRel.m_bSourceClauseIsMain = true;
// åñëè îòíîøåíèå ÿâëÿåòñÿ ÷üåé-òî âàëåòíîñòüþ 
	if( engSemRel.m_bInterpreted )
	{
		int iRusNode = engNode.RusNode;
		if( iRusNode == -1 )
			return;
		const CSemNode& rusNode = RusStr.GetNode(iRusNode);
		string rus_str = GetCortegeStr(rusNode.GetType(),rusSemRel.m_SynReal.m_Cortege);
		bool bRusInf = (rus_str.find("èíô")!=string::npos );

		vector<SGramCortegesAndType> GramCortegesAndTypeV;	
		GetGramCortegesAndTypeFromRel(GramCortegesAndTypeV, engSemRel);

		bool bEngInf = false;
		int  iEngInf = -1;
		for( int i=0; i<GramCortegesAndTypeV.size(); i++ )
		{
			if( GramCortegesAndTypeV[i].m_Type==InfinitiveOrGerundGram )
			{
				bEngInf = true;
				iEngInf = i;
			}
		}
		bool bEngSub = false;
		int  iEngSub = -1;
		for( int i=0; i<GramCortegesAndTypeV.size(); i++ )
		{
			if( GramCortegesAndTypeV[i].m_Type==SubClause )
			{
				bEngSub = true;
				iEngSub = i;
			}
		}

		if( bRusInf && bEngInf )
		{
			engSemRel.m_SynReal.m_Cortege = GramCortegesAndTypeV[iEngInf].m_WeightGramCorteges[0].m_GramCortege;
			return;
		}

		if( !bRusInf && bEngSub )
		{
			engSemRel.m_SynReal.m_Cortege = GramCortegesAndTypeV[iEngSub].m_WeightGramCorteges[0].m_GramCortege;
			CDictUnitInterp prep;
			string strConj = GetConj(GramCortegesAndTypeV[iEngSub].GetCorteges(),engNode.GetType());
			bool bRes = GetDictUnitInterpForObor(strConj,prep);
			assert(bRes);
			if( !bRes )
				return;
			engSemRel.m_SynReal.m_Conj = prep;					
			return;
		}
	}
//
	vector<CEngInterp> equivs;
	FindEngEquivForRusArticle(rusSemRel.m_SynReal.m_Conj,equivs,EngObor);
	if( equivs.size() > 0 )	
	{
		engSemRel.m_SynReal.m_Conj = equivs[0];
		CorrectTenseByDictConjFeatures(engSemRel.m_TargetNodeNo,equivs[0]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_Kak()

void CEngSemStructure::ClauseRelRule_Kak(int iRelNum)
{
	if( m_Relations[iRelNum].m_Valency.m_RelationStr == "THESAME")	
	{
		int iAntecEngNode = m_Relations[iRelNum].m_TargetNodeNo;
		vector<long> InRels;
		GetIncomingRelations(iAntecEngNode, InRels, false);
		assert(InRels.size() == 2);
		if( InRels.size() != 2 )
			return;
		int iClauseRel = (InRels[0] == iRelNum) ? InRels[1] : InRels[0];
		m_Relations[iClauseRel].m_TargetNodeNo = m_Relations[iRelNum].m_SourceNodeNo;
		m_Relations[iClauseRel].m_Type = Kak;
		m_Relations.erase(m_Relations.begin() + iRelNum);
		DelNode(iAntecEngNode);
	}
	else
		m_Relations[iRelNum].m_bSourceClauseIsMain = true;
}

/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_ChtoKtoAsConjWords()

void CEngSemStructure::ClauseRelRule_ChtoKtoAsConjWords(int iRelNum)
{
	CEngSemRelation& engSemRel = m_Relations[iRelNum];
	int iAntecEngNode = -1, iConjEngNode = -1;;
	if( engSemRel.m_Valency.m_RelationStr == "THESAME" )	
	{
		iAntecEngNode = engSemRel.m_TargetNodeNo;
		iConjEngNode = engSemRel.m_SourceNodeNo;
	}
	else
		iConjEngNode = engSemRel.m_TargetNodeNo;

	CEngSemNode engNode = m_Nodes[iConjEngNode];
	if( engNode.m_Words.size() != 1 )
		return;
	if( engSemRel.m_RusRel == -1 )
		return;

	const CSemRelation& rusSemRel = *RusStr.GetRelation(engSemRel.m_RusRel);

	int iAntecRusNode = -1, iConjRusNode = -1;;
	if( rusSemRel.m_Valency.m_RelationStr == "THESAME" )	
	{
		iAntecRusNode = rusSemRel.m_TargetNodeNo;
		iConjRusNode = rusSemRel.m_SourceNodeNo;
	}
	else
		iConjRusNode = rusSemRel.m_TargetNodeNo;

	const CSemNode& rusNode = RusStr.GetNode(iConjRusNode);	 

	if( rusNode.m_MainWordNo == -1 )
		return;
	if( rusNode.GetWordsSize() != 1 )
		return;

	int iRusActand = m_Nodes[engSemRel.m_TargetNodeNo].RusNode;
	int iRusRel = engSemRel.m_RusRel;
	bool bDelNode = false;
	
	if(	(iAntecRusNode != -1) &&
		(	( rusNode.GetWord(0).m_Lemma == "×ÒÎ") ||
			( rusNode.GetWord(0).m_Lemma == "ÊÒÎ") ))
	{
		const CSemNode& rusAntecNode = RusStr.GetNode(iAntecRusNode);
		string strAntecWord = rusAntecNode.GetWord(0).m_Lemma;
		if( rusNode.GetWord(0).m_Lemma == "×ÒÎ") 
		{
			if( strAntecWord == "ÒÎÒ" )
			{
				m_Relations.erase(m_Relations.begin() + iRelNum);
				vector<long> inRels;
				GetIncomingRelations(iAntecEngNode, inRels,false);
				for( int k=0; k< inRels.size(); k++ )
				{
					m_Relations[inRels[k]].m_Type = ChtoKtoAsConjWords;
					m_Relations[inRels[k]].m_bSourceClauseIsMain = true;
					break;
				}
				MoveRelations(iAntecEngNode, iConjEngNode);
				bDelNode = true;				
			} 
			else
				m_Relations[iRelNum].m_bSourceClauseIsMain = false;
		} 
		else
		{
			if( rusNode.GetWord(0).m_Lemma == "ÊÒÎ")
			{
				_SWITCH(strAntecWord)
				{
					_CASE("ÒÎÒ")
					{		
						
						CEngSemNode newNode;
						if( rusAntecNode.GetWord(0).HasOneGrammem(rPlural) )												
							CreateSimpleEnglNodeByOldNode("those", newNode, 0, true, m_Nodes[iAntecEngNode]); 						
						else						
							CreateSimpleEnglNodeByOldNode("the one", newNode, 0, true, m_Nodes[iAntecEngNode]); 						
						

						newNode.m_Words[0].m_WordNo = m_Nodes[iAntecEngNode].GetMinWordNo();
						m_Nodes[iAntecEngNode] = newNode;
					}

					_CASE("ÂÅÑÜ")
					{
						CEngSemNode newNode;
						CreateSimpleEnglNodeByOldNode("everybody", newNode, 0, true, m_Nodes[iAntecEngNode]);					

						newNode.m_Words[0].m_WordNo = m_Nodes[iAntecEngNode].GetMinWordNo();
						m_Nodes[iAntecEngNode] = newNode;
					}

					_CASE("ÊÀÆÄÛÉ")
					{
						CEngSemNode newNode;
						CreateSimpleEnglNodeByOldNode("everyone", newNode, 0, true, m_Nodes[iAntecEngNode]);					

						newNode.m_Words[0].m_WordNo = m_Nodes[iAntecEngNode].GetMinWordNo();
						m_Nodes[iAntecEngNode] = newNode;						
					}
				}
				engSemRel.m_bSourceClauseIsMain = false;
			}
		}
	}
	else
		engSemRel.m_bSourceClauseIsMain = true;
	
	engNode.m_Words[0].m_bDoNotChangeForm = true;

	_SWITCH(rusNode.GetWord(0).m_Lemma)
	{
		_CASE("×ÒÎ")
		{			
			engNode.m_Words[0].m_Lemma = "what";
			engNode.m_Words[0].m_Word = "what";
		}

		_CASE("ÊÒÎ")
		{
			engNode.m_Words[0].m_Lemma = "who";
			engNode.m_Words[0].m_Word = "who";			
		}
	}

	engSemRel.m_Position = "begin";
	engSemRel.m_PosType = FromAlgorithmPosType;
	
	vector<long> incomeRels;
	GetIncomingRelations(iConjEngNode, incomeRels, false);
	for( int i=0; i<incomeRels.size(); i++ )
	{
		if( (incomeRels[i] != iRelNum) || (iAntecRusNode != -1))
		{
			if( m_Nodes[m_Relations[incomeRels[i]].m_SourceNodeNo].m_ClauseNo == engNode.m_ClauseNo )
			{
				if( m_Relations[incomeRels[i]].m_SynReal.m_Preps.size() )
				{
					int mainWord = m_Nodes[m_Relations[incomeRels[i]].m_SourceNodeNo].m_MainWordNo;
					if( mainWord != -1)
					{
						m_Nodes[m_Relations[incomeRels[i]].m_SourceNodeNo].m_Words[mainWord].m_PosleLog.m_PosleLogPrep = m_Relations[incomeRels[i]].m_SynReal.m_Preps[0];
						m_Nodes[m_Relations[incomeRels[i]].m_SourceNodeNo].m_Words[mainWord].m_PosleLog.m_Position = "dir_obj _";
						m_Nodes[m_Relations[incomeRels[i]].m_TargetNodeNo].m_SynReal.m_Preps.clear();
						m_Relations[incomeRels[i]].m_SynReal.m_Preps.clear();
						m_Relations[incomeRels[i]].m_bInterpreted = true;
						break;
					}
				}
			}
		}
	}

	if( bDelNode )
		DelNode(iAntecEngNode);
}

/////////////////////////////////////////////////////////////////////////////
// ClauseRelRule_Kotoryj()

void CEngSemStructure::ClauseRelRule_Kotoryj(int iRelNum)
{
	CEngSemRelation& engSemRel = m_Relations[iRelNum];
	if( engSemRel.m_RusRel == -1 )
		return;
	const CSemRelation& rusSemRel = *RusStr.GetRelation(engSemRel.m_RusRel);
	assert( rusSemRel.m_Valency.m_RelationStr == "THESAME" );
	bool bAnim = HasSemFet(RusStr.GetNode(rusSemRel.m_TargetNodeNo),"ANIM");
	const CSemNode& sourceNode = RusStr.GetNode(rusSemRel.m_SourceNodeNo);	 
	const CSemNode& targetNode = RusStr.GetNode(rusSemRel.m_TargetNodeNo);	 

	vector<long> incomeRels;
	GetIncomingRelationsInThisClause(engSemRel.m_SourceNodeNo, incomeRels);
	if( incomeRels.size() != 1 )
		return;

	int iRelToKotoryj = incomeRels[0];
	engSemRel.m_bSourceClauseIsMain = false;

	if( IsSubj(m_Relations[iRelToKotoryj]) )
	{
		CEngSemNode newNode;
		CreateSimpleEnglNodeByOldNode(bAnim ? "who" : "which",newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);
		newNode.SetGrammemsRich(m_Nodes[engSemRel.m_SourceNodeNo].GetGrammemsRich());
		newNode.SetGrammems(m_Nodes[engSemRel.m_SourceNodeNo].GetGrammemsRich());

		newNode.m_Words[0].m_WordNo = m_Nodes[engSemRel.m_SourceNodeNo].GetMinWordNo();
		m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
		return;
	}

	if( m_Relations[iRelToKotoryj].m_bInterpreted )
	{
		switch(m_Relations[iRelToKotoryj].m_GramType)
		{
			case PossNp:
			{
				CEngSemNode newNode;
				CreateSimpleEnglNodeByOldNode("whose",newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);

				newNode.m_Words[0].m_WordNo = m_Nodes[engSemRel.m_SourceNodeNo].GetMinWordNo();
				m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
				return;
			}
			case PrepNp:
			{
				CEngSemNode newNode;
				CreateSimpleEnglNodeByOldNode(bAnim ? "whom" : "which",newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);

				newNode.m_Words[0].m_WordNo = m_Nodes[engSemRel.m_SourceNodeNo].GetMinWordNo();
				m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
				return;
			}

			case Obj:
			{
				CEngSemNode newNode;
				CreateSimpleEnglNodeByOldNode(bAnim ? "who" : "which", newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);

				newNode.m_Words[0].m_WordNo = m_Nodes[engSemRel.m_SourceNodeNo].GetMinWordNo();
				m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
				return;
			}
		}
	}

	if( m_Relations[iRelToKotoryj].m_RusRel != -1 )
	{
		const CSemRelation& relToKotoryj = *RusStr.GetRelation(m_Relations[iRelToKotoryj].m_RusRel);
		if( relToKotoryj.m_Valency.m_RossHolder )
		{
			if( (relToKotoryj.m_SynReal.m_Cortege.m_DomItemNos[0] == relToKotoryj.m_Valency.m_RossHolder->IndirObjSynONo) &&
				(relToKotoryj.m_SynReal.m_Cortege.m_DomItemNos[1] == relToKotoryj.m_Valency.m_RossHolder->GenitivNo) )
			{
				CEngSemNode newNode;
				CreateSimpleEnglNodeByOldNode("whose",newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);
				m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
				return;
			}
		}
	}

	CEngSemNode newNode;
	CreateSimpleEnglNodeByOldNode(bAnim ? "whom" : "which",newNode,0,true,m_Nodes[engSemRel.m_SourceNodeNo]);

	newNode.m_Words[0].m_WordNo = m_Nodes[engSemRel.m_SourceNodeNo].GetMinWordNo();
	m_Nodes[engSemRel.m_SourceNodeNo] = newNode;
}

/////////////////////////////////////////////////////////////////////////////
