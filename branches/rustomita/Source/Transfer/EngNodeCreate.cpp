#include "StdAfx.h"
#include "EngSemStructure.h"



void CEngSemStructure::InitEngWordAndLemma(CEngSemWord& semWord, string strEngLemma) const
{
	int ii;
	string strPosleLog;
	string strLemma;
	if (strEngLemma != "#oneself")
	if (( ii = strEngLemma.find("#oneself")) != -1)
	{
		semWord.m_OneselfStr = "#oneself";
		strEngLemma.erase (ii, 8);
	};

	if (strEngLemma != "#one's")
	if (( ii = strEngLemma.find("#one's")) != -1)
	{
		semWord.m_OneselfStr = "#one's";
		strEngLemma.erase (ii, 6);
	};

	if( (ii = strEngLemma.find("_")) != -1 )
	{
		int len  = strEngLemma.length();
		strPosleLog = strEngLemma.substr(ii+1);
		strLemma	= strEngLemma.substr(0, ii);

		Trim(strLemma);
		Trim(strPosleLog);

		semWord.m_Lemma = strLemma;
		semWord.m_Word = strLemma;	
		semWord.m_PosleLog.m_PosleLog = strPosleLog;
	}
	else
	{
		semWord.m_Lemma = strEngLemma;
		semWord.m_Word =  strEngLemma;		
	}
}

void CEngSemStructure::CreateEnglNodeForEngObor(long UnitNo, CEngSemNode& engNode) const
{
	CEngSemWord newWord;
	newWord.m_Lemma = m_pData->GetEngOborStr(UnitNo);
	newWord.m_Word = newWord.m_Lemma;
	newWord.m_bDoNotChangeForm = true;	
	newWord.m_pData = m_pData;	
	engNode.m_Words.push_back(newWord);
	engNode.AddInterp(CDictUnitInterp(EngObor, UnitNo));
}

void CEngSemStructure::CreateEnglNodeForEngColloc(long UnitNo, CEngSemNode& engNode) const
{
	string strUnitStr = m_pData->GetEngCollocStr(UnitNo);

	StringTokenizer	strTok(strUnitStr.c_str(), " ()");

	CDictUnitInterp UnitInterp;
	UnitInterp.m_DictType = EngCollocRoss;
	UnitInterp.m_UnitNo = UnitNo;	
	engNode.SetInterp(UnitInterp);
	int No = 0;
	vector<long> word_nums;
	while( strTok() )
	{
		CEngSemWord newWord;
		CCollocItem CollocItem;
		CollocItem.InitCollocItem(strTok.val());
		
		if( CollocItem.IsHole() )
		{
			assert (No > 0);
			word_nums.push_back(-1);			
		}
		else
		{
			InitEngWordAndLemma(newWord, CollocItem.Item);	

			if (!CollocItem.RelOperator1.empty())
				if (engNode.HasRelOperator(CollocItem.RelOperator1))
					engNode.m_RelOperators.push_back(CollocItem.RelOperator1);

			if (!CollocItem.RelOperator2.empty())
				if (engNode.HasRelOperator(CollocItem.RelOperator2))
					engNode.m_RelOperators.push_back(CollocItem.RelOperator2);

			newWord.m_bDoNotChangeForm = true;			
			newWord.m_pData = m_pData;	
			engNode.m_Words.push_back(newWord);
			word_nums.push_back(No);
			No++;
		}
	}

	vector<TCortege> corteges;
	
	corteges.clear();
	GetRossHolder(EngCollocRoss)->GetFieldValues("ROOT", UnitNo, corteges);
	engNode.m_MainWordNo = 0;
	if( corteges.size() > 0 )
	{
		int SemMainWord =   GetItemStr(corteges[0].m_DomItemNos[0], EngCollocRoss)[0] - '0';
		assert( word_nums.size() >= SemMainWord);
		assert( word_nums[SemMainWord - 1] != -1 );
		engNode.m_MainWordNo = word_nums[SemMainWord - 1];

		ReadMorphFromMainGF(UnitNo, EngCollocRoss, engNode.m_Words[engNode.m_MainWordNo]);

		engNode.m_Words[engNode.m_MainWordNo].m_bDoNotChangeForm = false;
	}
}

void CEngSemStructure::CreateEnglNode(CEngInterp UnitInterp, CEngSemNode& engNode, const CSemNode& rusNode)
{	
	engNode.m_NodeType = rusNode.m_NodeType;
	engNode.m_MNAType = rusNode.m_MNAType;
	engNode.m_SynGroupTypeStr = rusNode.m_SynGroupTypeStr;
	engNode.m_bQuoteMarks = rusNode.m_bQuoteMarks;
	engNode.m_ThesAbbrForm = rusNode.m_ThesAbbrForm;
	if (engNode.m_ThesAbbrForm)
	{
		engNode.m_TerminId = rusNode.m_TerminId;
		engNode.m_ThesaurusId= rusNode.m_ThesaurusId;
	}
	else
	{
		engNode.m_TerminId = UnitInterp.m_TerminId;
		engNode.m_ThesaurusId= UnitInterp.m_ThesaurusId;
	};
	engNode.m_bOrgName = rusNode.m_bOrgName;
	engNode.m_bProper = rusNode.m_bProper;
	engNode.m_HigherConcepts = rusNode.m_HigherConcepts;



	
	engNode.m_RelOperators = rusNode.m_RelOperators;

	if( UnitInterp.m_DictType == EngCollocRoss )
	{
		CreateEnglNodeForEngColloc(UnitInterp.m_UnitNo, engNode);
		assert (engNode.IsWordContainer());
		engNode.m_Words[0].m_WordNo = rusNode.GetMinWordNo();
		engNode.m_Words[engNode.m_Words.size() -1].m_WordNo = rusNode.GetMaxWordNo();
		return;
	}

	if( UnitInterp.m_DictType == EngObor )
	{
		
		CreateEnglNodeForEngObor(UnitInterp.m_UnitNo, engNode);
		assert (engNode.IsWordContainer());
		engNode.m_Words[0].m_WordNo = rusNode.GetMinWordNo();
		engNode.m_Words[engNode.m_Words.size() -1].m_WordNo = rusNode.GetMaxWordNo();
		return;
	}

	CEngSemWord semWord;
	string strEngLemma = GetRoss(UnitInterp.m_DictType)->GetEntryStr(UnitInterp.m_UnitNo);
    // если узел пришел из словосочетаний или оборотов, но не является английским словосочетанием 
	// или английским оборотом, тогда он был переведен словарной статьей из АОССа, значит 
	// такой узел должен стать примитивным, а  в русской структуре он им не был.
	// Например, русское словосочетание "сам себя" переводится в примитивный узел "#oneself"
	bool bRusWasCoplex = (rusNode.GetType() == CollocRoss) ||  (rusNode.GetType() == OborRoss);
			    
	for(int i = 0 ; i < rusNode.GetWordsSize() ; i++ )
	if  ( (      ( i == rusNode.m_MainWordNo )
		     &&  !bRusWasCoplex
		  )
		  ||(   bRusWasCoplex
			  &&  (i == 0)
			)
		)
		{
			if ( !rusNode.IsThesNode() )
				InitEngWordAndLemma(semWord, strEngLemma.c_str());
			else
			{
				semWord = rusNode.GetWord(i);
				
			}

			// копируем значение слота пассивной интерпретации
			CDictUnitInterp I(UnitInterp);
			if (rusNode.IsPassiveVerb())
					I.m_bPassiveForm = true;

			engNode.SetInterp(I);
			engNode.m_MainWordNo = i;
			semWord.m_WordNo = rusNode.GetWord(i).m_WordNo;
			semWord.m_CharCase =  rusNode.GetWord(i).m_CharCase;

			ReadMorphFromMainGF(UnitInterp.m_UnitNo, Aoss, semWord);
			semWord.m_NumeralPrefix =  rusNode.GetWord(i).m_NumeralPrefix;

			
			semWord.m_pData = m_pData;	
			if( semWord.m_Poses == 0)
			{
				BYTE rus_pos =  GetOnePOS(rusNode.GetWord(i).m_Poses);
				semWord.m_Poses = EngPOSByRusPOS(rus_pos,rusNode.GetWord(i).m_Lemma);
			}
			engNode.m_Words.push_back(semWord);
		}		
		else
			if (!bRusWasCoplex)
			{
  			   engNode.m_Words.push_back(rusNode.GetWord(i));
			   engNode.m_Words[engNode.m_Words.size()  - 1].m_pData = m_pData;	
			};

	engNode.m_Colloc = rusNode.m_Colloc;
	engNode.m_CollocId = rusNode.m_CollocId;
}





void CEngSemStructure::CreateSimpleEnglNodeByOldNode(string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm , CEngSemNode& oldNode)
{
	CreateSimpleEnglNode(lemma, engNode, POS, bDontChangeForm );
	engNode.m_ClauseNo = oldNode.m_ClauseNo;
	engNode.RusNode = oldNode.RusNode;
	engNode.m_HigherConcepts = oldNode.m_HigherConcepts;
	engNode.m_Words[0].m_WordNo = oldNode.GetMinWordNo();
}

void CEngSemStructure::CreateSimpleEnglNode(string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm, DictTypeEnum DictType/*= Aoss*/, int iMean/*= 1*/ )
{

	long UnitNo = GetRoss(DictType)->LocateUnit(lemma.c_str(), iMean);
	if( UnitNo != ErrUnitNo)
	{
		CDictUnitInterp UnitInterp;
		UnitInterp.m_DictType = DictType;
		UnitInterp.m_UnitNo = UnitNo;
		CreateSimpleEnglNodeWithInterp( UnitInterp, engNode);
		return;
	}

	CEngSemWord semWord;	
	semWord.m_Lemma = lemma;
	semWord.m_Word = lemma;	
	semWord.m_Poses = 1<<POS;
	if (lemma == "be")
		semWord.m_Poses = (1 << eVBE);
	semWord.m_bDoNotChangeForm = bDontChangeForm;
	semWord.m_pData = m_pData;	
	if( semWord.m_bDoNotChangeForm )
	{
		semWord.m_Poses = 0;
	}
	engNode.m_MainWordNo = 0;
	engNode.m_Words.push_back(semWord);
	
}

void CEngSemStructure::CreateSimpleEnglNodeWithInterp(CRossInterp UnitInterp, CEngSemNode& engNode)
{

	engNode.m_Words.clear();
	CEngSemWord semWord;
	string strEngLemma = GetRoss(UnitInterp.m_DictType)->GetEntryStr(UnitInterp.m_UnitNo);
	semWord.m_Lemma = strEngLemma.c_str();
	semWord.m_Word = strEngLemma.c_str();	
	ReadMorphFromMainGF( UnitInterp.m_UnitNo, UnitInterp.m_DictType, semWord);
	semWord.m_pData = m_pData;	
	engNode.SetInterp(UnitInterp);
	engNode.m_MainWordNo = 0;
	engNode.m_Words.push_back(semWord);
	InitEngVals(engNode);
	
}

void CEngSemStructure::CreateBeSemNode(CEngSemNode& newNode, int& iSubjPattern , int& iObjPattern )
{
	newNode.m_Words.clear();
	long UnitNo = GetRoss(Aoss)->LocateUnit("be", 1);

	if( UnitNo == ErrUnitNo )
	{
		CreateSimpleEnglNode("be", newNode, eVBE, false);
		return;		
	}
	
	CDictUnitInterp UnitInterp;
	UnitInterp.m_UnitNo = UnitNo;
	UnitInterp.m_DictType = Aoss;
	CreateSimpleEnglNodeWithInterp(UnitInterp, newNode);
	newNode.m_Words[0].m_Poses = 1<<eVBE;
	
	
	iObjPattern = -1;
	iSubjPattern = -1;
	for(int i = 0 ; i < newNode.m_Vals.size() ; i++ )
		if( HasThisGX(newNode.m_Patterns[i].m_GramCorteges, "obj", Aoss) )
			iObjPattern = i;
		else
			if( HasThisGX(newNode.m_Patterns[i].m_GramCorteges, "subj", Aoss) )
				iSubjPattern = i;

	assert (iObjPattern != -1);
	assert (iSubjPattern != -1);
}


void CEngSemStructure::CreateHaveSemNode(CEngSemNode& newNode, int& iSubjPattern , int& iObjPattern, int iMeanNum )
{
	newNode.m_Words.clear();
	long UnitNo = GetRoss(Aoss)->LocateUnit("have", iMeanNum);

	if( UnitNo == ErrUnitNo )
	{
		CreateSimpleEnglNode("have", newNode, eVERB, false, Aoss, iMeanNum);
		return;		
	}
	
	CDictUnitInterp UnitInterp;
	UnitInterp.m_UnitNo = UnitNo;
	UnitInterp.m_DictType = Aoss;
	CreateSimpleEnglNodeWithInterp(UnitInterp, newNode);
	newNode.m_Words[0].m_Poses = 1<<eVERB;

	iObjPattern = -1;
	iSubjPattern = -1;
	
	for(int i = 0 ; i < newNode.m_Vals.size() ; i++ )
	{
		if( HasThisGX(newNode.m_Patterns[i].m_GramCorteges, "obj", Aoss) )
			iObjPattern = i;
		else
			if( HasThisGX(newNode.m_Patterns[i].m_GramCorteges, "subj", Aoss) )
				iSubjPattern = i;
	}
	#ifdef _DEBUG
		assert (iObjPattern != -1);
		assert (iSubjPattern != -1);
	#endif
}




// Функция SetPositionOfActantInColloc устанавливает позицию актанта словосочетания.
void CEngSemStructure::SetPositionOfActantInColloc(CEngSemRelation& engRel, CSemPattern& semPattern, int iEngNode)
{
	CEngSemNode& engNode = m_Nodes[iEngNode];
	DictTypeEnum type = engNode.GetType();
	if( type != EngCollocRoss )
		return;
	
	if( !semPattern.m_GramCorteges.empty() )
	  if( GetRossHolder(EngCollocRoss)->IsPosition(semPattern.m_GramCorteges[0].m_DomItemNos[0]) )
	     engRel.m_Position = GetItemStr(semPattern.m_GramCorteges[0].m_DomItemNos[0], type);
		
}
