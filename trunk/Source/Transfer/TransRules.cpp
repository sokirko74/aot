/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////

int CEngSemStructure::GetEngRelByRusRel(int iRusRel)
{
	for(int i = 0 ; i < m_Relations.size() ; i++ )
		if( m_Relations[i].m_RusRel == iRusRel)
			return i;

	return -1;
}


bool CEngSemStructure::CanBeRusVerb(const string& Lemma)  const
{
	string l = Lemma;
	const CLemmatizer* P = m_pData->GetRusLemmatizer(); 
	vector<CFormInfo> ParadigmCollection;
	P->CreateParadigmCollection(true, l, false, false, ParadigmCollection);
	for (int i=0; i < ParadigmCollection.size(); i++)
	{
		string GramCodes = ParadigmCollection[i].GetSrcAncode();
		if (m_pData->GetRusGramTab()->GetPartOfSpeech(GramCodes.c_str()) == INFINITIVE)
			return true;
	};
	return false;
};

SEngEquiv CEngSemStructure::GetV0Value(const CSemNode& rusActant) 
{
	SEngEquiv S0_value;	
	SLexFunIndexes LexFunIndexes;

	DictTypeEnum type = rusActant.GetType();
	if( type != Ross )
		return S0_value;

	long RusActUnitNo = rusActant.GetUnitNo();
	if( RusActUnitNo != ErrUnitNo )
		LexFunIndexes.m_LexFunValue.m_MeanNum = GetRoss(type)->GetUnitMeanNum(RusActUnitNo);

	LexFunIndexes.m_LexFunValue.m_UnitStr = rusActant.GetWord(rusActant.m_MainWordNo).m_Lemma;
	EngRusMakeLower(LexFunIndexes.m_LexFunValue.m_UnitStr);
	LexFunIndexes.m_LexFun = "S0";
	vector<SLexFunIndexes>::const_iterator it;
	
	it = lower_bound(m_pData->m_LexFunIndexesRusByValue.begin(),m_pData->m_LexFunIndexesRusByValue.end(),LexFunIndexes,LexFunValueComp);
	
	while(		(it != m_pData->m_LexFunIndexesRusByValue.end() )
			&&	( (*it).EqByLexFunValue(LexFunIndexes.m_LexFunValue)  ))			
		
	{
		if( (*it).m_LexFun == LexFunIndexes.m_LexFun ) 
		{
			if ( CanBeRusVerb(it->m_LexFunArg.m_UnitStr) )
			{			
				S0_value.m_StrEngWord = it->m_LexFunArg.m_UnitStr;
				S0_value.m_iMeanNum = it->m_LexFunArg.m_MeanNum;
				break;
			}
		};
		it++;
	}
	
	return S0_value;
}


bool CEngSemStructure::MakeDeverbative( int iRusActant, int iEngActant, CEngSemNode& engNode)
{
	PrintRelations();
	engNode.m_ClauseNo = m_Nodes[iEngActant].m_ClauseNo;
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	if( !rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(NOUN) )
		return false;

	SEngEquiv V0_value = GetV0Value(rusActant);
	if( V0_value.m_StrEngWord.empty() )
		return false;

	vector<CEngInterp> engEquivs;
	long UnitNo = GetRoss(Ross)->LocateUnit(V0_value.m_StrEngWord.c_str(), (V0_value.m_iMeanNum == 10 ) ? 1 : V0_value.m_iMeanNum);
	if( UnitNo != ErrUnitNo )
	{
		CEngInterp interp;
		interp.m_UnitNo = UnitNo;
		interp.m_DictType = Ross;
		FindEngEquivForRusArticle(interp,engEquivs,Aoss);
	}
	else
		FindEnglishEquiv(engEquivs, V0_value.m_StrEngWord, V0_value.m_iMeanNum, Aoss);

	if( engEquivs.size() == 0 )
		return false;

	int no;
	vector<long> rusRelsToEngRels;
	CEngInterp UnitInterp = FindBestEnglishNodes(iRusActant,engEquivs,rusRelsToEngRels,no);
	CreateEnglNode(UnitInterp,engNode,RusStr.GetNode(iRusActant));
	InitEngVals(engNode);
	engNode.RusNode = iRusActant;

	vector<long> outRels;
	GetOutcomingRelations(iEngActant,outRels);
	vector<long> rusRels;
	RusStr.GetOutcomingRelations(m_Nodes[iEngActant].RusNode,rusRels);
			
	for( int i=0; i<rusRels.size(); i++ )
	{
		const CSemRelation& semRel = *(RusStr.GetRelation(rusRels[i]));
		int iEngRel = GetEngRelByRusRel(rusRels[i]);
		if (iEngRel != -1 )
		{
			if( rusRelsToEngRels[i] != -1 )
			{
				int iNewRel = rusRelsToEngRels[i];
				
				CEngSemNode& EngValencyOwner = m_Relations[iEngRel].m_bReverseRel ? m_Nodes[iEngActant] : engNode;
				int valNo;
				if( m_Relations[iEngRel].m_bReverseRel )
					valNo = GetReverseRelValNo(semRel, EngValencyOwner);
				else 
					valNo = rusRelsToEngRels[i];
				assert (valNo < EngValencyOwner.m_Vals.size() );

				const CValency& V = EngValencyOwner.m_Vals[valNo];
				DictTypeEnum type = m_pData->GetTypeByRossHolder(V.m_RossHolder);
				CSemPattern semPattern;
				semPattern.InitSemPattern(GetRossHolder(type),V.m_UnitNo, V.m_LeafId, V.m_BracketLeafId); 
				semPattern.LoadGramFromDict();

				if( !semPattern.m_GramCorteges.empty() )						
				{
					m_Relations[iEngRel].m_Pattern = semPattern;
					m_Relations[iEngRel].m_bInterpreted = true;
					m_Relations[iEngRel].m_Valency = EngValencyOwner.m_Vals[valNo];
					m_Relations[iEngRel].m_SynReal.m_Cortege = semPattern.m_GramCorteges[0];
					m_Relations[iEngRel].m_SynReal.m_CortegeNo = 0;
				}
				else	
					m_Relations[iEngRel].m_bInterpreted = false;
			}
			else
			{
				if(m_Relations[iEngRel].m_bInterpreted)
				{
					m_Relations[iEngRel].m_Pattern = m_Relations[iEngRel].m_Pattern;
				}				
			}

		}
	}

	return true;
}


bool CEngSemStructure::HasItem(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type, string strItem)
{
	int ItemNo;
	return HasItem(GramCorteges, CortegeNo, ItemNo, type, strItem);
}

bool CEngSemStructure::HasItem(const vector<TCortege>& GramCorteges, long& CortegeNo, int& ItemNo, DictTypeEnum type, string strItem)
{
	for( int i = 0 ; i < GramCorteges.size() ; i++ )
	{
		const TCortege& cortege = GramCorteges[i];
		if( (GetItemStr(cortege.m_DomItemNos[0], type).find(strItem) != -1) && (cortege.m_DomItemNos[1] == -1) )
		{
			CortegeNo = i;
			ItemNo = 0;
			return true;	
		}
		if( (cortege.m_DomItemNos[1] != -1) && (GetItemStr(cortege.m_DomItemNos[1], type).find(strItem) != -1))
		{
			CortegeNo = i;
			ItemNo = 1;
			return true;	
		}
	}

	return false;
}

bool CEngSemStructure::HasGerund(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type)
{
	if( type == NoneRoss )
		return false;

	return HasItem(GramCorteges, CortegeNo, type, "gerund");
}

string CEngSemStructure::GetConj(const vector<TCortege>& GramCorteges, DictTypeEnum type)
{
	long iCortege;
	int iItem;
	if( HasItem(GramCorteges, iCortege, iItem, type, "+sent") || HasItem(GramCorteges, iCortege, iItem, type, "+subj_clause"))
	{
		const TCortege& cortege = GramCorteges[iCortege];
		string strItem = GetItemStr(cortege.m_DomItemNos[iItem], type);

		int ii = strItem.find("+sent"); 		

		if( ii != -1)		
			return strItem.substr(0, ii);

		ii = strItem.find("+subj_clause"); 		

		if( ii != -1)		
			return strItem.substr(0, ii);

		
	}
	return "";
}

bool CEngSemStructure::HasToPlusInf(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type)
{
	return HasItem(GramCorteges, CortegeNo, type, "to+inf");
}


bool CEngSemStructure::HasInf(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type)
{
	return HasItem(GramCorteges, CortegeNo, type, "+inf");
}

string CEngSemStructure::GetGerundPrep(const TCortege& cortege, DictTypeEnum type)
{
	string item = GetItemStr(cortege.m_DomItemNos[0], type);
	int i = item.find("+gerund");
	if( i == -1 )
	{
		if( cortege.m_DomItemNos[1] != -1 )
		{
			item = GetItemStr(cortege.m_DomItemNos[1], type);
			i = item.find("+gerund");
		}
	}
	if(i == -1)
		return "";
	else
		return item.substr(0, i);
}

bool CEngSemStructure::Rule_TranslateInfinitive( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode)
{
	CEngSemRelation& semEngRel = m_Relations[RelationNo];
	CEngSemNode& engNode = m_Nodes[iEngNode];
	int iEngActant = semEngRel.m_TargetNodeNo;
	CEngSemNode& engActant = m_Nodes[iEngActant];	
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);

	long GerundCortegeNo = -1;
	long InfCortegeNo = -1;
	bool bToPlusInf = HasToPlusInf(GramCorteges,InfCortegeNo,engNode.GetType());
	bool bGerund = HasGerund(GramCorteges,GerundCortegeNo,engNode.GetType());
	int iRusMainWord = rusActant.m_MainWordNo;
	if( !RusStr.IsVerbOrMNAVerb(iRusActant) )
	{
		if( iRusMainWord == -1 )
			return false;
		if( !rusActant.GetWord(iRusMainWord).HasPOS(NOUN) )
			return false;		
	}

	CEngSemNode newEngNode;

	//если есть что-нибудь типа in+gerund
	bool NewNodeWasCreated = false;
	if(		(bGerund || bToPlusInf) 
		&& (iRusMainWord != -1) 
		&&  rusActant.GetWord(iRusMainWord).HasPOS(NOUN) 
	  )
	{	
		//если нашли такой глагол, от которого отглагольное существительное будет равно 
		//главному слову рассматриваемого узла		
		// например, "механизация" -> "механизировать"
		if( !MakeDeverbative(iRusActant,iEngActant,newEngNode) )
			return false;

		engActant = newEngNode;

		NewNodeWasCreated = true;
	} 

	if( bToPlusInf )
	{
		if( NewNodeWasCreated )
		{			
			CEngSemWord& engWord = newEngNode.m_Words[newEngNode.m_MainWordNo];
			engWord.SetTense(present_inf_tn,"MakeDeverbative");
			engWord.m_bDoNotChangeForm = true;
		}
		
		
		semEngRel.m_SynReal.m_Cortege = GramCorteges[InfCortegeNo];		

		return true;
	}

	if( bGerund )
	{				

		CEngSemWord engWord;

		engWord = engActant.m_Words[engActant.m_MainWordNo];
		
		string verb = engWord.m_Lemma;
		engWord.SetTense(gerund_tn,"Rule_TranslateInfinitive");
	
		engActant.m_Words[engActant.m_MainWordNo] = engWord;

		string prep = GetGerundPrep(GramCorteges[GerundCortegeNo], engNode.GetType());
		// герундий может быть без предлога 
		// например, "стоит поместить" ->  "worth placing"
		if (prep != "")
		{
			SetSimpleEngPrep(prep, iEngActant, -1);
			SetSimpleEngPrep(prep, -1, RelationNo);
		};
		semEngRel.m_SynReal.m_Cortege =  GramCorteges[GerundCortegeNo];		

		return true;
	}

	
	if( HasInf(GramCorteges,InfCortegeNo,engNode.GetType()) )
	{
		semEngRel.m_SynReal.m_Cortege =  GramCorteges[InfCortegeNo];
		return true;	
	}

	if( HasGerund(GramCorteges, GerundCortegeNo, engNode.GetType()) )
	{
		string prep = GetGerundPrep(GramCorteges[GerundCortegeNo], engNode.GetType());
		SetSimpleEngPrep(prep, iEngActant, -1);
		SetSimpleEngPrep(prep, -1, RelationNo);
		semEngRel.m_SynReal.m_Cortege =  GramCorteges[GerundCortegeNo];
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::Rule_TranslatePoss(int iRusActant,long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode)
{
	CEngSemRelation& semEngRel = m_Relations[RelationNo];

	if( semEngRel.m_RusRel == -1 )
		return false;
	const CSemRelation& semRusRel = *RusStr.GetRelation(semEngRel.m_RusRel);
	if( semRusRel.HasSomePrep() )
		return false;
//
	CEngSemNode& engNode = m_Nodes[semEngRel.m_TargetNodeNo];
	if( !engNode.IsWordContainer() )
		return false;
//	
    if (RusStr.IsNounOrMNANoun(iRusActant) )
	{
		semEngRel.m_SynReal.m_Cortege = GramCorteges[0];
		if	(		HasSemFetOrLower(engNode, "ANIM")			
				&& ( (engNode.GetGrammemsRich() & _QM(eGeographics)) == 0)
				// если существительное управляет "тяжелыми" актантами, тогда его нельзя
				// трансформировать в притяжательную форму (Сокирко)
				&& !HasOutRelationByPoses(semEngRel.m_TargetNodeNo, (1<<eNOUN)|(1<<ePN)|(1<<eVERB)|(1<<eMOD)|(1<<eVBE))
			)
		{
			engNode.AddOneGrammemRich( ePossessive );
			semEngRel.m_Position = "<";
			semEngRel.m_PosType = FromAlgorithmPosType;
		}
		else
		{
			SetSimpleEngPrep ("of",-1, RelationNo);
			semEngRel.m_Position = ">>";
			semEngRel.m_PosType = FromAlgorithmPosType;
		}
		return true;
	}
//
	if(      (engNode.GetPos() != ePN) 
		||	!(engNode.GetGrammemsRich() & eAllPersons ) 
	  )
		return false;

	string lemma = GetPronounEnglishFormByGrammems(engNode.GetGrammemsRich(),false);
	if( lemma.empty() )
		return false;

	CEngSemNode newNode;
	CreateSimpleEnglNodeByOldNode(lemma.c_str(),newNode,0,false,engNode);
	assert( newNode.m_Words.size()==1 );
	newNode.m_Words[0].m_Poses = (1<<ePN_ADJ);
	newNode.m_Words[0].SetFormGrammems(newNode.m_Words[0].GetFormGrammems() & ~eAllNumbers);
	if( engNode.GetGrammemsRich() & _QM(rPlural) )
		newNode.m_Words[0].AddFormGrammem( ePlural);
	if( engNode.GetGrammemsRich() & _QM( rSingular) )
		newNode.m_Words[0].AddFormGrammem( eSingular);
	m_Nodes[semEngRel.m_TargetNodeNo] = newNode;

	semEngRel.m_Position = "<";
	semEngRel.m_PosType = FromAlgorithmPosType;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::Rule_TranslateObj(int iRusActant,long EngRelNo, const vector<TCortege>& GramCorteges, int iEngNode)
{
	CEngSemRelation& semEngRel  = m_Relations[EngRelNo];
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	int iMainWord = rusActant.m_MainWordNo;
	if( iMainWord == -1 )
		return false;
	
	if( !RusStr.IsNounOrMNANoun(iRusActant) )
	{
		if(	   !rusActant.GetWord(iMainWord).HasPOS(PRONOUN_PREDK) 
			&& !rusActant.GetWord(iMainWord).HasPOS(PRONOUN) 
			&& !rusActant.GetWord(rusActant.m_MainWordNo).IsRusSubstPronounP()  
		  )
			return false;
	}

	
	if( rusActant.GetWord(iMainWord).HasPOS(NOUN) || 
		rusActant.GetWord(iMainWord).HasPOS(PRONOUN) ||
		rusActant.GetWord(iMainWord).HasPOS(PRONOUN_P) )
	{
		semEngRel.m_SynReal.m_Preps.clear();			
		semEngRel.m_SynReal.m_Cortege = GramCorteges[0];			
		return true;		
	}

	if( rusActant.m_NodeType == MNA )
	{			
		semEngRel.m_SynReal.m_Preps.clear();		
		semEngRel.m_SynReal.m_Cortege = GramCorteges[0];		
		return true;
	}

	return true;	
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

bool CEngSemStructure::Rule_TranslateSubj(int iRusActant,long EngRelNo, const vector<TCortege>& GramCorteges, int iEngNode)
{
	CEngSemRelation& semEngRel  = m_Relations[EngRelNo];
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	int iMainWord = rusActant.m_MainWordNo;
	if( iMainWord == -1 )
		return false;
	
	if( !semEngRel.m_Pattern.IsEmpty() &&
		semEngRel.m_bInterpreted &&
		semEngRel.m_Pattern.m_GramCorteges.size() )
	{
		semEngRel.m_SynReal.m_Cortege = semEngRel.m_Pattern.m_GramCorteges[0];
	}

	// первую валентность пассивной итнтерпретации надо отконвертировать в by+NP
	//  (например, Я был съеден тобой -> I was eaten by you),
	// но фразу "я интересовался вопросом" нужно переводить "I was interested in the question",
	// a не "I was interested by the question", поскольку у interested стоит помета GF - pass !
	PrintNodes();
	PrintRelations();
	if(     m_Nodes[semEngRel.m_SourceNodeNo].IsPassiveVerb()
	    && (semEngRel.m_Valency.m_LeafId == 1)
		&& (		!semEngRel.m_bInterpreted
				||  !semEngRel.m_Valency.m_RossHolder->HasFieldValue(  string("RESTR") , string("pass"),  semEngRel.m_Valency.m_UnitNo ) 
		   )
	  )
	{
		  semEngRel.m_SynReal.m_Cortege.SetItem(0, -1);
		  SetSimpleEngPrep("by",-1,EngRelNo);
		  assert  (!IsSubj(semEngRel));
		  return true;
	}

	return true;
	
}

/////////////////////////////////////////////////////////////////////////////
