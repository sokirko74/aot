#include "stdafx.h"
#include "SemanticStructure.h"
#include "HierarchyHolder.h"

//=====================================  CSemWord  =====================================
const CRossHolder* CSemWord::GetRossHolder (DictTypeEnum Type)   const  
{ 
	return m_pData->GetRossHolder(Type);
};
const CDictionary* CSemWord::GetRoss (DictTypeEnum Type) const     
{
	return m_pData->GetRossHolder(Type)->GetRoss();
};

void CSemWord::Init()
{
	m_CharCase = LowLow;
	m_FormGrammems = 0;
	m_TypeGrammems = 0;
	m_WordNo = -1;  
	m_ParadigmId = -1;
	m_WordWeight = 0;	
	m_AdditParadigmId = -1;
	m_bAdverbFromAdjective = false;
};

CSemWord::CSemWord()  { Init(); }

CSemWord::CSemWord (long WordNo, string Lemma )
{
	Init();
	m_WordNo = WordNo;
	m_Lemma = Lemma;
};
// является ли данное слово кавычкой
bool CSemWord::IsQuoteMark () const
{
	return (m_Word.length() == 1) && (m_Word[0] == '"');
};
// принадлежит ли граммема слову 
bool CSemWord::HasOneGrammem(int grammem) const
{
	return		((m_FormGrammems & _QM(grammem)) != 0)
			||	((m_TypeGrammems & _QM(grammem)) != 0);
}

bool CSemWord:: operator == ( const long& X ) const
{
	return (m_WordNo == X);
};


bool CSemWord::IsRusSubstPronounP () const
{
	if (m_Poses	 != (1<< PRONOUN_P)) return false;

	return     (m_Lemma == "КАЖДЫЙ" )
			||  (m_Lemma == "ЛЮБОЙ")			   
			||  (m_Lemma == "ТОТ")
			||  (m_Lemma == "КОТОРЫЙ")
			||  (m_Lemma == "ДРУГОЙ")
			||  (m_Lemma == "ОДИН");
};

QWORD		CSemWord::GetAllGrammems() const
{
	return m_FormGrammems|m_TypeGrammems;
}

QWORD		CSemWord::GetFormGrammems() const
{
	return m_FormGrammems;
}

void		CSemWord::SetFormGrammems(QWORD g)
{
	m_FormGrammems = g;
}

QWORD		CSemWord::GetTypeGrammems() const
{
	return m_TypeGrammems;
}

void		CSemWord::SetTypeGrammems(QWORD g)
{
	m_TypeGrammems = g;
}
void		CSemWord::AddFormGrammem(int g)
{
	m_FormGrammems |= _QM(g);
};



//=====================================  CSemNode  =====================================

CSemNode::CSemNode() 
{
	m_MainWordNo = -1;
	m_SynGroupTypeStr = "";
	m_Grammems = 0;
	m_bQuoteMarks = false;
	m_GramCodes = "";
	m_CurrInterp = -1;
	m_ClauseNo = -1;
	m_CollocId = -1;
	m_bToDelete = false;
	m_MNAType = NotMNA;
	m_NodeType = SimpleNode;
	m_ThesAbbrForm = false;
	m_bOrgName = false;
	m_bProper = false;
	m_SemCategory = scLabel;
};

// удаляет из оператор из  m_RelOperators
void CSemNode::DelRelOperator(const string& oper)
{
	for(int i = 0 ; i < m_RelOperators.size() ; i++ )
		if( m_RelOperators[i] == oper)
		{
			m_RelOperators.erase(m_RelOperators.begin() +i );
			i--;
		};
}


void      CSemRelation::Init() 
{
	m_bToDelete = false;
	m_bRelUse = true;
	m_SourceNodeNo  = -1;
	m_TargetNodeNo = -1;
	m_SyntacticRelation = "";
	m_bDopRelation = false;
	m_bReverseRel = false;
	m_bSituatRelation = false;

	m_Position = "";
	m_PosType = UnknownPosType;
};


 // дает тип словаря для текущей интерпретации
DictTypeEnum CSemNode::GetType()   const 
{
	return (m_CurrInterp == -1) ? NoneRoss : m_Interps[m_CurrInterp].m_DictType;
};

// дает  текущую интерпрецию
CDictUnitInterp*  CSemNode::GetInterp() 	
{
	return (m_CurrInterp == -1) ?  0 :&(m_Interps[m_CurrInterp]);	
};

void  CSemNode::DelAllInterps() 	
{
	m_CurrInterp = -1; m_Interps.clear();	
};
void  CSemNode::AddInterp(const CDictUnitInterp& X) 	
{ 
		m_Interps.push_back(X); 
		if (m_Interps.size() == 1)
			m_CurrInterp = 0;
};
void  CSemNode::SetInterp(const CDictUnitInterp& X) 	
{ 
		DelAllInterps();
		AddInterp(X);
};

void  CSemNode::CopyInterps(const CSemNode& X) 	
{ 
	m_Interps = X.m_Interps;
	m_CurrInterp = X.m_CurrInterp;
};

void  CSemNode::ResetCurrInterp() 	
{ 
	if (!m_Interps.empty())
		m_CurrInterp = 0;
	else
		m_CurrInterp = -1;
};

const CDictUnitInterpVector CSemNode::GetInterps () const 
{
	return m_Interps; 
}

bool  CSemNode::SetCurrInterp (const CDictUnitInterp& X) 	
{
	vector<CDictUnitInterp>::const_iterator it = find (m_Interps.begin(), m_Interps.end(), X);
	if (it == m_Interps.end()) return false;
	m_CurrInterp =  it - m_Interps.begin();	
	return true;
};

void  CSemNode::DeleteInterp (size_t i) 	
{	
	m_Interps.erase(m_Interps.begin() + i);	
	ResetCurrInterp();
};

// дает  константную текущую интерпрецию
const CDictUnitInterp*  CSemNode::GetInterp() const 	
{
	return (m_CurrInterp == -1) ?  0 :&(m_Interps[m_CurrInterp]);	
};

// дает  номер статьи для текущей интерпреции
WORD   CSemNode::GetUnitNo() const 
{
	return (m_CurrInterp == -1) ? ErrUnitNo : m_Interps[m_CurrInterp].m_UnitNo;
};

long  CSemNode::GetCurrInterpNo() const
{
	return m_CurrInterp;
};


// примитивный узел - это такой узел, который состоит только из одного слова
bool   CSemNode::IsPrimitive() const 
{
	return GetWordsSize() == 1;
};

// является ли данный узел абстрактным (не содержащий слов)
bool CSemNode::IsAbstract () const 	
{ 
	return    (m_NodeType == Copul) 
			|| (m_NodeType == SJA) 
			|| (m_NodeType == ModalCopul)
			|| (m_NodeType == Situat)
			;
};
// IsWordContainer = "содержит ли данный узел слова"
// версия функции IsWordContainer с GetWordsSize() 
// очень наглядна, но очень медленна,
// поэтому лучше использовать тип узла
bool CSemNode::IsWordContainer () const 		
{ 
	return (m_NodeType == SimpleNode) || (m_NodeType == MNA);
};


// выдает номер самого левого слова данного узла
long CSemNode::GetMaxWordNo () const 
{
	assert (IsWordContainer());
	return GetWord(GetWordsSize() - 1).m_WordNo;
};

// выдает номер самого правого слова данного узла
long CSemNode::GetMinWordNo () const 
{
	assert (IsWordContainer());
	return GetWord(0).m_WordNo;
};



// проверяет наличие оператора среди  RelOperators
bool CSemNode::HasRelOperator (string oper) const
{
	return  find(m_RelOperators.begin(),m_RelOperators.end(), oper) != m_RelOperators.end();
};


// проверяет, приписан ли узлу хотя бы один предлог
bool CSemNode::HasSomePrep () const 
{
	return m_SynReal.m_Preps.size() > 0; 
};

// проверяет, приписан ли узлу предлог PrepNo
bool CSemNode::HasThePrep (WORD UnitNo) const 
{ 
	return m_SynReal.HasThePrep(UnitNo);
};



bool  CSemNode::IsTimeRossNode() const
{
			return (m_Colloc.m_Type == RossType) 
			&& (m_Colloc.GetRossInterp().m_DictType == TimeRoss);
};

bool  CSemNode::IsMainTimeRossNode() const
{
		return    IsTimeRossNode() 
				&& m_Colloc.GetRossInterp().m_bMainWord;
};


bool CSemNode::IsThesNode() const
{
		return (m_Colloc.m_Type == ThesType);
};


bool  CSemNode::IsTrueLocNode() const 
{
	return     IsPrimitive() 
			&& (GetType() == LocRoss) 
			&& (   GetWord(0).HasPOS(NOUN) 
				|| GetWord(0).HasPOS(ADV)
				);
};
bool   CSemNode::HasPOS (size_t POS) const 
{ 
		if (m_MainWordNo == -1) return false;
		return GetWord(m_MainWordNo).HasPOS (POS);
}
bool   CSemNode::IsComma() const 
{
	return     IsPrimitive() 
	  		&& GetWord(0).m_Word[0] == ',';
};

bool CSemNode::IsLemma(string Lemma) const 
{
	return IsPrimitive() && (GetWord(0).m_Lemma == Lemma);
};

// проверяет, что данный узел является пассивным глаголом
bool	CSemNode::IsPassiveVerb () const
{	
	const CDictUnitInterp* I = GetInterp();
	if (I==0) return false; 
	return   I->m_bPassiveForm;
};

QWORD	CSemNode::GetGrammems() const
{
	return m_Grammems;
}

void	CSemNode::SetGrammems(QWORD g)
{
	m_Grammems = g;
}

void	CSemNode::AddOneGrammem(int g)
{
	m_Grammems |= _QM(g);

};
bool	CSemNode::HasOneGrammem(int g) const
{
	return (m_Grammems & _QM(g)) > 0;
};

long CSemNode::GetNodePoses() const 
{
	if (m_MainWordNo == -1) return 0;
	return  GetWord(m_MainWordNo).m_Poses;
};



void CSemNode::AddGrammems(QWORD  grammems)
{
	SetGrammems(GetGrammems() | grammems );
};

void CSemNode::DeleteGrammems(QWORD grammems)
{
	SetGrammems(GetGrammems() & ~grammems);
};

bool	CSemNode::HasGrammems(QWORD grammems) const
{
	return (grammems & m_Grammems) == grammems;
};


//=====================================  CSemRelation =====================================
CSemRelation::CSemRelation () 
{ 
	Init(); 
};

	
CSemRelation::CSemRelation (const CValency& Valency, 
	             long SourceNodeNo, 
				 long TargetNodeNo, 
				 string SyntacticRelation)
{
		Init();
		m_Valency    = Valency;
		m_SourceNodeNo  = SourceNodeNo;
		m_TargetNodeNo = TargetNodeNo;
		m_SyntacticRelation = SyntacticRelation;
};

bool  CSemRelation::HasSomePrep() const
{
	return m_SynReal.m_Preps.size() > 0;
}


//=====================================  CSemClause  =====================================

CSemClause::CSemClause() 
{
	m_HasParticleBY = false;
	m_BestPO = "";
	m_bQuestion = false;
	m_bHasParticleConjAnd = false;
	m_bQuoted = false;
	m_bBracketed = false;
};
void  CSemClause::Copy(const CSemClause& C) 
{
	m_HasParticleBY = C.m_HasParticleBY;
	m_BestPO = C.m_BestPO;
	m_bQuestion = C.m_bQuestion;
	m_bHasParticleConjAnd = C.m_bHasParticleConjAnd;
	m_bQuoted = C.m_bQuoted;
	m_bBracketed = C.m_bBracketed;

};




//=====================================  CSemanticStructure  =====================================


CRossHolder* CSemanticStructure::GetRossHolder (DictTypeEnum Type)   const  
{ 
	return m_pData->GetRossHolder(Type);
};
const CDictionary* CSemanticStructure::GetRoss (DictTypeEnum Type) const     
{
	return m_pData->GetRoss(Type);
};


// инициализация валентностей узла
void  CSemanticStructure::InitVals(CSemNode& Node)
{
	Node.m_Vals.clear();
	if (Node.GetType() == NoneRoss) return;

	long  UnitNo = Node.GetUnitNo();
	if( UnitNo==ErrUnitNo )
		return;

	const CRossHolder* pRossDoc= GetRossHolder(Node.GetType());

	if (UnitNo != ErrUnitNo) 
		if (!pRossDoc->GetRoss()->IsEmptyArticle(UnitNo))
			for (size_t i = pRossDoc->GetRoss()->GetUnitStartPos(UnitNo); i<= pRossDoc->GetRoss()->GetUnitEndPos(UnitNo); i++)
			{
				TCortege C =  GetCortege(pRossDoc->GetRoss(),i);
				if ( C.m_FieldNo ==  pRossDoc->ValFieldNo && Node.m_Vals.size() < MaxValsCount)
				{
					Node.m_Vals.push_back(CValency (C,pRossDoc->MainWordVarNo, pRossDoc, UnitNo));
				};
			};



}



inline long ConvertGramRestrToGrammems(string t)
{
	if ( t  == "ед" )
			  return _QM(rSingular);
    if ( t == "мн" )
			  return _QM(rPlural);
	if ( t == "1л" )
	          return _QM(rFirstPerson);
    if ( t == "2л" )
			  return _QM(rSecondPerson);
	if ( t == "3л" )
			  return _QM(rThirdPerson);
	return 0;
};


// возвращает множество наборов граммем, которые разрешено иметь данному узлу
vector<QWORD> CSemanticStructure::GetGramRestr(const CSemNode& W)
{
	vector<QWORD> GrammemsSet;

    if (W.GetUnitNo() != ErrUnitNo)
	 if (!GetRoss(W.GetType())->IsEmptyArticle(W.GetUnitNo()))
	  for (size_t i = GetRoss(W.GetType())->GetUnitStartPos(W.GetUnitNo()); i<= GetRoss(W.GetType())->GetUnitEndPos(W.GetUnitNo()); i++)
	   if (GetRoss(W.GetType())->GetCortegeFieldNo(i) == GetRossHolder(W.GetType())->GramRestrFieldNo) 
		  {
			const TCortege& C = GetCortege (GetRoss(W.GetType()), i);
			string GramFet = WriteToString(GetRoss(W.GetType()), (char*)(GetRoss(W.GetType())->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
            Trim(GramFet);
	        DWORD Pose;
			QWORD Grammems;
	        m_pData->GetCustomGrammems(GramFet, Grammems, Pose);
			GrammemsSet.push_back(Grammems);
		  };
     return GrammemsSet;
};





string GetStringVectorInOneStr(const StringVector& Vec, string Delimiter) 
{
	string Result;
	for (long i=0; i < Vec.size(); i++)
	{
		Result += Vec[i];
		Result += Delimiter;
	};
	return Result;
};





string CSemanticStructure::GetTclGraph(bool ShowUnusedValencies, bool UseIsTop)
{
 try { 
	// уничтожение графа 
	string Res ="";
	Res += "$GT($main,graph) delete nodes\1";

    Res += Format("$main.switches.resultText configure -text \"%s\"\1",m_ClausePropertiesProtocol.c_str());
	Res += Format("set GT($main,tree_variant_props) \"%s\"\1",m_WeightProtocol.c_str());
    Res += Format("set GT($main,statistics) \"%s\"\1",m_TimeStatictics.c_str());
	size_t debug = m_ClauseVariantsStatistics.length();
	Res += Format("set GT($main,variants_statistics) \"%s\"\1",m_ClauseVariantsStatistics.c_str());
	Res += "set values {\"Orthogonal\" \"Next middle\" \"Next corner\" \"Explicit\"}\1";
	

	for (size_t i = 0;  i < GetNodesSize(); i++)
	{
		Res += Format("set nds(%i) [$GT($main,graph) create node]\1", i);;
		Res += Format("$GT($main,graph) set $nds(%i) -label \"%s\" -type rectangle -x 0 -y 0\1", i,  GetNodeStr(i, 30).c_str());
		Res += Format("foreach p [$GT($main,graph) get $nds(%i) -ports] {lappend values [lindex $p 0]}\1", i);

		if (GetNode(i).m_SynGroupTypeStr != KEYB)
		{
			string Q =  GetMorphologyOfNode(i);
			if (GetNode(i).IsWordContainer())
				Q += Format("\nMinWordNo = %i", GetNode(i).GetMinWordNo());

			if (GetNode(i).IsPrimitive())
				if (!GetNode(i).GetWord(0).m_WordEquals.empty())
				{
					Q+= "\n Полные эквиваленты:";
					for (long j=0; j < GetNode(i).GetWord(0).m_WordEquals.size(); j++)
						Q+= Format("\n%s", GetNode(i).GetWord(0).m_WordEquals[j].c_str());

				};

			if (GetNode(i).m_SynGroupTypeStr != "")
			{
				Q += Format("\nЖесткая группа  = %s", GetNode(i).m_SynGroupTypeStr.c_str());
			};
			;
			Res += Format("$GT($main,graph) set $nds(%i)  .morphology \"%s\"\1",i, Q.c_str());
		};

		string ss = SynRealToStr(GetNode(i).m_SynReal, "\n");
		if( !ss.empty())
			Res += Format( "$GT($main,graph) set $nds(%i)  .preps \"%s\"\1",i, ss.c_str());

		if  (		   (GetNode(i).GetType() == LocRoss) 
				||  (		 (GetNode(i).m_Colloc.m_Type  == ThesType)
						&& (GetNode(i).m_Colloc.GetThesInterp().m_ThesaurusId  == LocThes)
					)
			)
		{
			
			Res += Format("$GT($main,graph) set $nds(%i)  -fill green\1",i);
		};

		if ((GetNode(i).GetType() == CollocRoss) || (GetNode(i).GetType() == EngCollocRoss))
		{
			Res += Format("$GT($main,graph) set $nds(%i)  -fill snow4\1",i);
		};

		if (GetNode(i).IsTimeRossNode())
		{
			Res += Format("$GT($main,graph) set $nds(%i)  -fill \"cyan\"\1",i);
		};


		if (GetNode(i).IsThesNode())
		{
			Res += Format("$GT($main,graph) set $nds(%i)  -fill \"bisque1\"\1",i);
		};

		if (GetNode(i).m_bProper)
		{
			Res += Format("$GT($main,graph) set $nds(%i)  -fill \"yellow\"\1",i);
		};



		string AllInterps;

		if  ( GetNode(i).m_Colloc.m_Type != NoneType  )
		{
			AllInterps += " + ";
			AllInterps += OpenCollocInterpToStr(GetNode(i).m_Colloc);
			AllInterps += "\n ";
		}


		for (long k=0; k < GetNode(i).GetInterps().size(); k++)
		{
			const CSemNode& N = GetNode(i);
			const CDictUnitInterpVector& Interps = N.GetInterps();
			vector<CDictUnitInterp>::const_iterator I = Interps.begin() + k;
			AllInterps += Format("%s %s\n", 
								(k == GetNode(i).GetCurrInterpNo()) ? " + ": " - ",
								InterpToStr(I).c_str());

		};
		Res += Format("$GT($main,graph) set $nds(%i)  .dict_interp \"%s\"\1",i,AllInterps.c_str());

		if (GetNode(i).IsPrimitive())
		{
			Res += Format ("$GT($main,graph) set $nds(%i)  .lemma \"%s\"\1",i,GetNode(i).GetWord(0).m_Lemma.c_str());
		};

		string SemFets = string("CAT = ") + GetStrByCategory(GetNode(i).m_SemCategory) + string("\n");
		SemFets += "SF = " + GetSemFetsInOneStr(GetNode(i).m_NodeSemFets);
		if (GetNode(i).m_POs.size() > 0)
		{
			SemFets += "\nDOMAIN = ";
			for (long j =0; j < GetNode(i).m_POs.size(); j++)
				SemFets += GetNode(i).m_POs[j] + "\n";
		};


		;
		Res += Format ("$GT($main,graph) set $nds(%i)  .sem_fets \"%s\"\1",i,SemFets.c_str());

		string Props;
		if (GetNode(i).m_bQuoteMarks)
			Props += "Quote marks\n";
		if (GetNode(i).m_ConnectedSits.size() > 0)
		{
			Props += "Connected Situations:";
			for (long j=0; j < GetNode(i).m_ConnectedSits.size();j++)
				Props += Format("%s\n", GetNode(i).m_ConnectedSits[j].m_UnitStr.c_str());
		};
		Res += Format ("$GT($main,graph) set $nds(%i)  .props \"%s\"\1",i,Props.c_str());
		Res += Format ("$GT($main,graph) set $nds(%i)  .type_dict \"%i\"\1",i,GetNode(i).GetType());
		Res += Format ("$GT($main,graph) set $nds(%i)  .unit_no \"%i\"\1",i,GetNode(i).GetUnitNo());
		Res += Format ("$GT($main,graph) set $nds(%i)  .higher_concepts \"%s\"\1",i,GetStringVectorInOneStr(GetNode(i).m_HigherConcepts, "\n").c_str());

		string AllVals;
		for (size_t k=0; k < GetNode(i).m_Vals.size(); k++)
		{
			if (GetNode(i).m_Vals[k].m_bObligatory)
				AllVals += " ! ";
			if (GetNode(i).m_Vals[k].m_bOptional)
				AllVals += " ? ";
			AllVals += GetNode(i).m_Vals[k].m_RelationStr;
			AllVals += " ";
			AllVals += (GetNode(i).m_Vals[k].m_Direction == A_C) ? "A,C": "C,A";
			if (GetNode(i).m_Vals[k].IsFromDict())
				if (GetNode(i).m_Vals[k].m_UnitNo != ErrUnitNo)
				{
					AllVals += " (from ";;
					AllVals += GetNode(i).m_Vals[k].m_RossHolder->GetRoss()->GetEntryStr(GetNode(i).m_Vals[k].m_UnitNo);
					AllVals += " )";
				};
			if (GetNode(i).m_Vals[k].m_IsAddition)
			{
				AllVals += " (added)";
			};
			AllVals += "\n";
		};
		Res += Format("$GT($main,graph) set $nds(%i)  .vals \"%s\"\1",i,AllVals.c_str());

	};

	

	for (size_t i = 0;  i < GetRelationsSize(); i++)
	{
		vector <long> Rels;
		FindRelations(GetRelation(i)->m_SourceNodeNo, GetRelation(i)->m_TargetNodeNo, Rels);
		if (Rels[0] < i) continue;

		size_t FirstNodeNo = GetRelation(i)->m_SourceNodeNo;
		size_t SecondNodeNo = GetRelation(i)->m_TargetNodeNo;
		Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", FirstNodeNo, SecondNodeNo);


		string Label;
		for (size_t k=0; k < Rels.size(); k++)
		{	
			Label += GetRelation(Rels[k])->m_Valency.m_RelationStr;
			Label += " ";	  

			Label += SynRealToStr(GetRelation(Rels[k])->m_SynReal, "\n");
			long g = FindLexFunctBySemRel(Rels[k]);
			if (g != -1)
				Label += string ("(") + GetLexFuncts()[g].m_LexFunctName + string (")");
		};

		//
		Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", Label.c_str());

		string Props; 
		if (GetRelation(i)->m_Valency.IsFromDict())
			if (GetRelation(i)->m_Valency.m_UnitNo != ErrUnitNo)
			{
				Props += "Статья = ";
				Props += GetRelation(i)->m_Valency.m_RossHolder->GetRoss()->GetEntryStr(GetRelation(i)->m_Valency.m_UnitNo);
				Props += "\n";
			};

		string LeafIdStr;
		BYTE LeafId = GetRelation(i)->m_Valency.m_LeafId;
		if (LeafId < 32)
			LeafIdStr = Format("%i", LeafId);
		else
			LeafIdStr = Format("%i(%i)", LeafId&31, LeafId >> 5);

		Props += "Актант = ";
		Props += "A";
		Props += LeafIdStr;
		Props += "\n";

		if (GetRelation(i)->m_Valency.IsFromDict() && ( GetRelation(i)->m_Valency.m_UnitNo != ErrUnitNo ) )
			if ( GetRelation(i)->m_SynReal.m_Cortege.m_DomItemNos[0] != -1)
			{
				TCortege C = GetRelation(i)->m_SynReal.m_Cortege;
				const CRossHolder* RossHolder = GetRelation(i)->m_Valency.m_RossHolder;
				string GramFet = WriteToString(RossHolder->GetRoss(), (char*)(RossHolder->GetRoss()->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
				Props += "GF";
				Props += LeafIdStr;
				Props += " = ";
				Props += GramFet;
				Props += "\n";
			};
		if (!GetRelation(i)->m_SyntacticRelation.empty())
		{
			Props += " СинО = "+ GetRelation(i)->m_SyntacticRelation;
			Props += "\n";
		};
		//
			
		if (GetRelation(i)->m_Position != "")
		{
			Props += " Позиция = "+ GetRelation(i)->m_Position;
			Props += "\n";

			Props += " Тип Позиции = ";

			if (GetRelation(i)->m_PosType ==  FromAlgorithmPosType)
				Props += "Алгоритм";
			else
				if (GetRelation(i)->m_PosType ==  FromArticlePosType)
					Props += "Статья";

			Props += "\n";
		};

		if (GetRelation(i)->m_bReverseRel)
		{
			Props += "Reverse Relation\n";
		};
		if (GetRelation(i)->m_Valency.m_bOptional)
		{
			Props += "Optional Valency\n";
		};


		Res += Format("$GT($main,graph) set $edge .props \"%s\"\1",Props.c_str());

		//
		Res += Format("$GT($main,graph) set $edge .sem_fets \"%s\"\1",GetSemFetsInOneStr( GetRelation(i)->m_SemFets).c_str());

		;
		Res += Format("$GT($main,graph) set $edge .lex_fets \"%s\"\1",GetStringVectorInOneStr( GetRelation(i)->m_LexFets, "\n").c_str());


		//
		string Color;
		float width;
		GetColorAndWidthOfRelation(i, width, Color);
		Res += Format("$GT($main,graph) configure $edge graphics -fill %s -width %f\1", Color.c_str(), width);
		
	};

   return Res;
  }
  catch (...)
  {
	 ErrorMessage ("GetTclGraph failed");
	 throw;
  };
};
   




string CSemanticStructure::GetTclSyntaxGraph()
{
	// уничтожение графа 
	string Res ="";
	Res += "$GT($main,graph) delete nodes\1";

    Res += Format("$main.switches.resultText configure -text \"%s\"\1",m_ClausePropertiesProtocol.c_str());
    Res += Format("set GT($main,tree_variant_props) \"%s\"\1",m_WeightProtocol.c_str());
    Res += Format("set GT($main,statistics) \"%s\"\1",m_TimeStatictics.c_str());
	Res += Format("set GT($main,variants_statistics) \"%s\"\1",m_ClauseVariantsStatistics.c_str());


    for (size_t i = 0;  i < GetNodesSize(); i++)
   	{
    	Res += Format ("set nds(%i) [$GT($main,graph) create node]\1", i);
	    Res += Format("$GT($main,graph) set $nds(%i) -label \"%s\" -type rectangle -x 0 -y 0\1", i,  GetNodeStr(i, 30).c_str());
		string Grammems = m_pData->GetRusGramTab()->GrammemsToStr(GetNode(i).GetGrammems());
		Res += Format("$GT($main,graph) set $nds(%i)  .grammems \"%s\"\1",i, Grammems.c_str());
		Res += Format("$GT($main,graph) set $nds(%i)  .lemmas \"%s\"\1",i, GetNodeLemStr(i).c_str());
	};

    for (size_t i = 0;  i < GetSynRels().size(); i++)
	 {
	  Res += Format ("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetSynRels()[i].m_SourceNodeNo, GetSynRels()[i].m_TargetNodeNo);
	  Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", GetSynRels()[i].m_SynRelName.c_str());

  	  string Color = !GetSynRels()[i].m_SynRelName.empty() ? "blue" : "darkgreen";
	  Res += Format("$GT($main,graph) configure $edge graphics -fill %s \1", Color.c_str());
	};

   return Res;

};



void GetCommonVariants(const vector<VectorLong>&  Parents, 
									   VectorLong&   V,
							  vector<VectorLong>&  Variants, 
								long       Position)
	{ 
     if (Variants.size() > 10000) return;
	 if (Position < V.size())
	  for (long i=0; i< Parents[Position].size(); i++)
	  {
			 V[Position] = Parents[Position][i];
			 GetCommonVariants(Parents, V, Variants, Position+1);
	  }
	 else
			 Variants.push_back(V);

	};


string   CSemanticStructure::InterpToStr(vector<CDictUnitInterp>::const_iterator I)  const
{
	 const CDictionary* Ross = GetRoss(I->m_DictType);

     string Result =  Format( "%s: %s %i", 
			Ross->m_DictName.c_str(),
			Ross->GetEntryStr(I->m_UnitNo).c_str(),
			Ross->GetUnitMeanNum(I->m_UnitNo));

	 if (I->m_bPassiveForm)
		 Result +=  "(passive)";

	 return Result;
};

string   CSemanticStructure::OpenCollocInterpToStr(const COpenCollocInterp& I)  const
{
	assert (I.m_Type != NoneType );
	string S;

	if (I.m_Type == RossType)
	{
		S = Format( "%s: %s%i", 
			GetRoss(I.GetRossInterp().m_DictType)->m_DictName.c_str(),
			GetRoss(I.GetRossInterp().m_DictType)->GetEntryStr(I.GetRossInterp().m_UnitNo).c_str(),
			GetRoss(I.GetRossInterp().m_DictType)->GetUnitMeanNum(I.GetRossInterp().m_UnitNo));

		if (I.GetRossInterp().m_ItemNo != -1) 
			S += Format(" (ItemNo=%i)",I.GetRossInterp().m_ItemNo+1);
		if (I.GetRossInterp().m_LexFetNo != -1)
			S += Format(" (LexFetNo=%i)",I.GetRossInterp().m_LexFetNo+1);

		if (I.GetRossInterp().m_PrepNoInArticle != -1)
			S += Format(" (m_PrepNoInArticle=%i)",I.GetRossInterp().m_PrepNoInArticle+1);
	}
	else
	{

		S = Format( "%s: %i", m_pData->GetThesStr(I.GetThesInterp().m_ThesaurusId).c_str(),I.GetThesInterp().m_TerminId);

		if (I.GetThesInterp().m_ItemNo != -1) S += Format(" (ItemNo=%i)",I.GetThesInterp().m_ItemNo+1);
	};

	return S;
};


string CSemanticStructure::GetTxtGraph()
{
	string Res; 
	Res += "Nodes\n";
    for (size_t i = 0;  i < GetNodesSize(); i++)
    if (GetNode(i).IsWordContainer())
   	{
		;
		Res += Format ("%i %-25s", i, GetNodeLemStr(i).c_str());
		if (GetNode(i).GetType() ==  NoneRoss)
		{
			Res += " -1 NoneRoss";
		}
		else
		{
			string RossName = m_pData->GetRoss(GetNode(i).GetType())->m_DictName;
			int MeanNum= -1;

			if (GetNode(i).GetUnitNo() != ErrUnitNo)
				GetRoss(GetNode(i).GetType())->GetUnitMeanNum(GetNode(i).GetUnitNo());

   	        Res += Format(" %i %s", MeanNum, RossName.c_str());
		};
		Res += '\n';
	};

	
    Res += "Relations\n";

    for (size_t i = 0;  i < GetRelationsSize(); i++)
	 {
	  string SynReal = SynRealToStr(GetRelation(i)->m_SynReal, "\n");
	  TrimRight(SynReal);
	  Res += Format("%-10s %-3i %-3i %s\n", 
		  GetRelation(i)->m_Valency.m_RelationStr.c_str(),
		  GetRelation(i)->m_TargetNodeNo,
		  GetRelation(i)->m_SourceNodeNo,
		  SynReal.c_str());
	  

	};

   return Res;

};







QWORD	CSemanticStructure::GetOneIndexedSemFet (const vector<string>& _SemFets, bool bInlcudeLowerHierarcy, bool	bInlcudeHigherHierarcy)
{
  QWORD Result = 0;
  vector<string> SemFets = _SemFets;
  
  if (bInlcudeLowerHierarcy)
	 IncludeLowerInHierarchy (&m_pData->m_HierarchySemFetDoc, SemFets);

  if (bInlcudeHigherHierarcy)
	 IncludeHigherInHierarchy (&m_pData->m_HierarchySemFetDoc, SemFets);

  

  for (int k=0; k < SemFets.size(); k++)
  {
		vector<string>::const_iterator It = find (m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFets[k]);
		QWORD Num = It - m_IndexedSemFets.begin();
		assert (Num < 64);
		if (It == m_IndexedSemFets.end())
			m_IndexedSemFets.push_back(SemFets[k]);
		Result |= _QM(Num);
  };

  return Result;
 
};

vector<QWORD> CSemanticStructure::GetIndexedSemFets (const CSemPattern& P, bool bInlcudeLowerHierarcy, bool	bInlcudeHigherHierarcy) 
{
	QWORD DomensSemFet = 0;
	/*
	в DomensSemFet находятся все SF, которые обозначают предметные области FIN, SOC  (пока их только две)
	*/
	//assert (m_IndexedSemFets[2] == "FIN");//assert (m_IndexedSemFets[3] == "SOC");  
	for (QWORD j=2; j < 4; j++)
		DomensSemFet |= _QM(j); 


	vector<QWORD> Result;

	for (int i=0; i < P.m_ActantSemFets.size(); i++)
	{
		QWORD OneSemFetsSet =   GetOneIndexedSemFet(P.m_ActantSemFets[i], bInlcudeLowerHierarcy,bInlcudeHigherHierarcy);

		/*
		надо добавить SF FIN,SOC (предметных областей), если предметных областей в SF не указано      Например, если SF=SITUAT и bInlcudeLowerHierarcy=true, тогда
		SF=SITUAT,FIN - разновидность  SF=SITUAT	*/
		if (bInlcudeLowerHierarcy)
			if ((OneSemFetsSet & DomensSemFet) == 0)
				OneSemFetsSet |= DomensSemFet;

		Result.push_back (OneSemFetsSet);
	};


	return Result;
};


bool CSemanticStructure::GleicheSemFet(const vector<QWORD>& SemFets1, const vector<QWORD>& SemFets2, bool bInclusion) const
{
	// если не приписана SF, то считаем, что у слова может быть любая SF
	// например, слову "большой"  SF1 не приписана	

	if (SemFets1.empty() || SemFets2.empty()) return true;

	string debug1 = GetSemFetsInOneStr(SemFets1);
	string debug2 = GetSemFetsInOneStr(SemFets2);

	for (long i=0; i < SemFets1.size(); i++)
		for (long k=0; k < SemFets2.size(); k++)
			if (bInclusion)
			{
				if ( (SemFets1[i] &  SemFets2[k])  == SemFets2[k])
					return true;
			}
			else
			{
				if ( (SemFets1[i] &  SemFets2[k])  > 0)
					return true; 
			};

	return false;
};



vector<string> CSemanticStructure::GetSemFetStr(QWORD SemFet) const 
{
	vector<string> Res;
	for (long k=0;k < 64; k++)
	{
			QWORD Value = _QM(k);
			if ( ( Value & SemFet) > 0)
			{
				Res.push_back(m_IndexedSemFets[k]);
			};
	};
    return Res;
};


string CSemanticStructure::GetSemFetsInOneStr(const vector<QWORD>& SemFets) const 
{
	string Result;
	for (long i=0; i < SemFets.size(); i++)
	{
		vector<string> SemFetStr = GetSemFetStr (SemFets[i]);
		assert (SemFetStr.size() <= m_IndexedSemFets.size());
		string Res = Format ("%i ", i+1);
		for (long k=0; k < SemFetStr.size(); k++)
				Res += string(SemFetStr[k]) + string (",");
		if (Res.length() > 0) Res.erase(Res.length() - 1);
		Result += Res;
		Result += "; ";
	};
	if (Result.length() == 0) Result = " ";
	return Result;
};



bool  CSemanticStructure::HasSemFet (const CSemNode& Node, const string& SemFet) const
{
	vector<string>::const_iterator It = find (m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);

	if (It == m_IndexedSemFets.end()) return false;

    QWORD Num = It - m_IndexedSemFets.begin();

	bool Result = Node.m_NodeSemFets.size() > 0;

	for (long i=0; i < Node.m_NodeSemFets.size(); i++)
	{
		Result =  Result && (  (_QM(Num) & Node.m_NodeSemFets[i])   > 0);
	};

	return Result;
};


bool  CSemanticStructure::HasSemFetPro (const vector<QWORD>& SemFets, const string& SemFet) const
{
	vector<string>::const_iterator It = find (m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);

	if (It == m_IndexedSemFets.end()) return false;

	QWORD Num = It - m_IndexedSemFets.begin();

	assert (m_IndexedSemFets[0] == "NEG");assert (m_IndexedSemFets[1] == "CAUS");
	for (long i=0; i < SemFets.size(); i++)
		if ( ( ((1<<0) | (1<<1)) & SemFets[i]) == 0)
			if ( _QM(Num) & SemFets[i] )
				return true;

   return false;

};
// проходит по всем дизъюнктам, если в дизъюнкте отстутствует CAUS и NEG,
// и присутствует SemFet, тогда выдает истину
bool  CSemanticStructure::HasSemFetPro (const CSemNode& Node, const string& SemFet) const
{
	return HasSemFetPro(Node.m_NodeSemFets, SemFet);
};

// проверяет, принадлежит ли узлу данная SF или какая-нибудь SF ниже по иерархии
bool  CSemanticStructure::HasSemFetOrLower (const CSemNode& Node, const string& SemFet) const
{
     for (long i=0; i < Node.m_NodeSemFets.size(); i++)
	 {
		vector<string> SemFetStr = GetSemFetStr (Node.m_NodeSemFets[i]);
		IncludeHigherInHierarchy (&m_pData->m_HierarchySemFetDoc, SemFetStr);
		for (long j =0; j < SemFetStr.size(); j++)
			if (SemFetStr[j] == SemFet)
				return  true;
	 };
   return false;
};




bool CSemanticStructure::IsVerbOrMNAVerb(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if( rusActant.m_MainWordNo != -1 )
		if(		rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(VERB) ||
				rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(INFINITIVE)  )
				return true;

	if(	rusActant.m_NodeType == MNA )
	{
		vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for(int i = 0 ; i < rels.size() ; i++ )
		{

			if( !IsVerbOrMNAVerb(GetRelation(rels[i])->m_TargetNodeNo ) )
				return false;
		}
		return true;
	}
	return false;
}

bool CSemanticStructure::IsInfinitiveOrMNAInfinitive(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if( rusActant.m_MainWordNo != -1 )
		if(		rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(INFINITIVE)  )
				return true;

	if(	rusActant.m_NodeType == MNA )
	{
		vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for(int i = 0 ; i < rels.size() ; i++ )
		{
			if( !IsInfinitiveOrMNAInfinitive(GetRelation(rels[i])->m_TargetNodeNo ) )
				return false;
		}
		return true;
	}
	return false;
}


bool CSemanticStructure::IsNounOrMNANoun(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if( rusActant.m_MainWordNo != -1 )
		if(		rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(NOUN) ||
				rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(PRONOUN)  )
				return true;

	if(	rusActant.m_NodeType == MNA )
	{
		vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for(int i = 0 ; i < rels.size() ; i++ )
		{
			if( !IsNounOrMNANoun(GetRelation(rels[i])->m_TargetNodeNo ) )
				return false;
		}
		return true;
	}
	return false;
}



  
bool CSemanticStructure::CheckGroupBeginAndCase(string ItemStr, size_t NodeNo, long& PrepNo) const 
{
   const CSemNode&  N = GetNode(NodeNo);
   if (ItemStr.length() > 3)
   if (ItemStr.substr (ItemStr.length() - 3) == "_мн")
   {
	   if (       !N.HasOneGrammem (rPlural)
		      && (N.m_NodeType != MNA)
		  )
		  return false;
   };

   WORD DictPrepNo = GetRossHolder(OborRoss)->LocateUnit(ItemStr.c_str(),1);
   for (PrepNo=0; PrepNo < N.m_SynReal.m_Preps.size(); PrepNo++)
	 if (DictPrepNo == N.m_SynReal.m_Preps[PrepNo].m_UnitNo) 
	   return true;
   PrepNo = -1;

   return false;
};



long    CSemanticStructure :: GetLocPrepFromBegining (size_t NodeNo) const 
{
	const CSemNode& N = GetNode(NodeNo);
	for (long i=0; i <N.m_SynReal.m_Preps.size(); i++)
	  if (IsLocSemRel(GetSemRelOfPrepOrConj(N.m_SynReal.m_Preps[i]).m_RelationStr)) 
		  return i;
	return -1;
};

bool    CSemanticStructure  :: HasLocPrepInBegining (size_t NodeNo) const 
{
	return GetLocPrepFromBegining(NodeNo)  != -1;
};




bool CSemanticStructure::IsRusSubj(int iRel) const
{
	if( GetRelation(iRel)->
		 m_SyntacticRelation == "подл")
		return true;
	long iRusSynRel = GetSynRelBySemRel(iRel);
	if( iRusSynRel != -1 )
		if( GetSynRels()[iRusSynRel].m_SynRelName == "ПОДЛ" )
			return true;
	return false;
}


SEngEquiv CSemanticStructure::GetAL1Value(int UnitNo) const 
{	
	SEngEquiv res;	
	res.m_iMeanNum = 1;
	if( UnitNo == ErrUnitNo )
		return res;
	
	vector<TCortege> corteges;
	GetRossHolder(Ross)->GetFieldValues(string("SF"), UnitNo, corteges);

	for(int i = 0 ; i < corteges.size() ; i++ )
	{
		int j = 0;
		if( corteges[i].m_DomItemNos[0] != -1 )
		{
			string str1 =  GetRossHolder(Ross)->GetDomItemStrInner( corteges[i].m_DomItemNos[0]); 
			if( str1 == "AL1" )
				if( corteges[i].m_DomItemNos[1] != -1 ) 
				{
					str1 =  GetRossHolder(Ross)->GetDomItemStrInner( corteges[i].m_DomItemNos[1]); 
					res.m_StrEngWord = str1;				
					if( corteges[i].m_DomItemNos[2] != -1)
					{
						string meanNum = GetRossHolder(Ross)->GetDomItemStrInner(corteges[i].m_DomItemNos[2]);
						res.m_iMeanNum = meanNum[0] - '0';
					}
				}
		}
	}

	return res;
}

void CSemanticStructure::AssertValidGraph()
{
	for(int i = 0 ; i < GetRelationsSize() ; i++ )
	{
		assert ( (GetRelation(i)->m_SourceNodeNo >= 0) && (GetRelation(i)->m_SourceNodeNo < GetNodesSize()));

		assert ( (GetRelation(i)->m_TargetNodeNo >= 0) && (GetRelation(i)->m_TargetNodeNo < GetNodesSize()));

	}

	for(int i = 0 ; i < GetSynRels().size() ; i++ )
	{
		assert ( (GetSynRels()[i].m_SourceNodeNo >= 0) && (GetSynRels()[i].m_SourceNodeNo < GetNodesSize()));

		assert ( (GetSynRels()[i].m_TargetNodeNo >= 0) && (GetSynRels()[i].m_TargetNodeNo < GetNodesSize()));

	}

}


string CSemanticStructure::SynRealToStr(const CSynRealization&  SynReal, string Delimiter) const
{
	string Preps;
    for (long l=0; l < SynReal.m_Preps.size(); l++)
	{		
		Preps += GetRoss(SynReal.m_Preps[l].m_DictType)->GetEntryStr(SynReal.m_Preps[l].m_UnitNo) + Delimiter;		
	}

    if (SynReal.m_Conj.m_UnitNo != ErrUnitNo)
	     Preps += GetRoss(SynReal.m_Conj.m_DictType)->GetEntryStr(SynReal.m_Conj.m_UnitNo)  + Delimiter;

	Preps += SynReal.m_Other;

	return Preps;
};



void CSemanticStructure::SetNodeToDeleteFalse () 
{
   for (long i=0; i<GetNodesSize(); i++)
	   GetNodePtr(i)->m_bToDelete = false;
};
void CSemanticStructure::DelNodesToDelete() 
{
  for (long i=0; i<GetNodesSize(); i++)
	   if (GetNode(i).m_bToDelete)
	   {
		   DelNode(i);
		   i = -1;
	   };
};

void CSemanticStructure::SetRelsToDeleteFalse () 
{
   for (long i=0; i<GetRelationsSize(); i++)
	   GetRelation(i)->m_bToDelete = false;
};
void CSemanticStructure::DelRelsToDelete() 
{
     for (long i=0; i<GetRelationsSize(); i++)
	   if (GetRelation(i)->m_bToDelete)
	   {
           EraseRelation(i);
		   i--;
	   };
};


string CSemanticStructure::GetOtherRelations()
{  
   string Res = "";
   for (long i=0; i < GetLexFuncts().size(); i++)
    if (GetLexFuncts()[i].m_SourceNodeNo != GetLexFuncts()[i].m_TargetNodeNo)
	if (    (FindFirstRelation(GetLexFuncts()[i].m_SourceNodeNo, GetLexFuncts()[i].m_TargetNodeNo) == -1)
		 && (FindFirstRelation(GetLexFuncts()[i].m_TargetNodeNo, GetLexFuncts()[i].m_SourceNodeNo) == -1)

	   )
   {
	  Res += Format ("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetLexFuncts()[i].m_SourceNodeNo, GetLexFuncts()[i].m_TargetNodeNo);
	  Res += Format ("$GT($main,graph) set $edge -label \"%s\"\1", GetLexFuncts()[i].m_LexFunctName.c_str());
	  Res += Format ("$GT($main,graph) configure $edge graphics -fill blue -width 1.0 -stipple gray50\1");
   };

   for (long i=0; i < GetDopRelationsSize(); i++)
    if (GetDopRelation(i)->m_SourceNodeNo != GetDopRelation(i)->m_TargetNodeNo)
	if (    (FindFirstRelation(GetDopRelation(i)->m_SourceNodeNo, GetDopRelation(i)->m_TargetNodeNo) == -1)
		 && (FindFirstRelation(GetDopRelation(i)->m_TargetNodeNo, GetDopRelation(i)->m_SourceNodeNo) == -1)

	   )
   {
	  Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetDopRelation(i)->m_SourceNodeNo, GetDopRelation(i)->m_TargetNodeNo);
	  Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", GetDopRelation(i)->m_Valency.m_RelationStr.c_str());
	  Res += Format("$GT($main,graph) configure $edge graphics -fill blue -width 1.0 -stipple gray50\1");
      Res += Format("$GT($main,graph) set $edge .sem_fets \"%s\"\1",GetSemFetsInOneStr( GetDopRelation(i)->m_SemFets).c_str());

   };



   return Res;

};


extern bool BuildGenitFormOfCardinal(const CLemmatizer* piRusLemmatizer, const CRusGramTab* Agramtab);


bool CSemanticsHolder::InitializeIndices()
{
	// проверка того, что хотя бы один словарь был изменен
	     if (   !GetRossHolder(Ross)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(TimeRoss)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(OborRoss)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(CollocRoss)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(Aoss)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(EngCollocRoss)->HasBeenModified(m_LastIndicesUpdateTime)
			 && !GetRossHolder(EngObor)->HasBeenModified(m_LastIndicesUpdateTime)
			)
		  return true;

		 m_LastIndicesUpdateTime = m_LastUpdateTime;
			 

		 
		 if (! GetRossHolder(Ross)->InitConsts() )
		 {
			 ErrorMessage ("GetRossHolder(Ross)->InitConsts() failed");
			 return false;
		 };
			 ;
         if (! GetRossHolder(TimeRoss)->InitConsts() )
		 {
			 ErrorMessage ("GetRossHolder(TimeRoss)->InitConsts() failed");
			 return false;
		 };

         if (!GetRossHolder(OborRoss)->InitConsts() )
		 {
			 ErrorMessage ("GetRossHolder(OborRoss)->InitConsts() failed");
			 return false;
		 };

         if (!GetRossHolder(CollocRoss)->InitConsts() )
		 {
			 ErrorMessage ("GetRossHolder(CollocRoss)->InitConsts() failed");
			 return false;
		 };

		 if ( !ReadAbstractArticles(Ross) )
		 {
			 ErrorMessage ("ReadAbstractArticles(Ross) failed");
			 return false;
		 };
		 if ( ! ReadAbstractArticles(Aoss) )
		 {
			 ErrorMessage ("ReadAbstractArticles(Aoss) failed");
			 return false;
		 };

		 if ( !InitTimeUnits() )
		 {
			 ErrorMessage ("InitTimeUnits failed");
			 return false;
		 };

		 if (!InitTimeThesLists() )
		 {
			 ErrorMessage ("InitTimeThesLists failed");
			 return false;
		 };

		 if ( !BuildOborottos() )
		 {
			 ErrorMessage ("BuildOborottos (russian) failed");
			 return false;
		 };

		 if ( !BuildEngOborStr() )
		 {
			 ErrorMessage ("BuildEngOborStr failed");
			 return false;
		 };
			 
		 if (!BuildEngCollocsStr() )
		 {
			 ErrorMessage ("BuildEngCollocsStr failed");
			 return false;
		 };

  		 if ( !CreateAROSSIndex() )
		 {
			 ErrorMessage ("CreateAROSSIndex failed");
			 return false;
		 };

		 if ( !CreateEngCollocsROSSIndex() )
		 {
			 ErrorMessage ("CreateEngCollocsROSSIndex failed");
			 return false;
		 };
		 if ( !CreateEngOborIndex() )
		 {
			 ErrorMessage ("CreateEngOborIndex failed");
			 return false;
		 };

		 if ( !CreateLexFunIndexes( GetRoss(Ross), m_LexFunIndexesRusByValue) )
		 {
			 ErrorMessage ("CreateLexFunIndexes( GetRoss(Ross), m_LexFunIndexesRusByValue) failed");
			 return false;
		 };

		 sort(m_LexFunIndexesRusByValue.begin(), m_LexFunIndexesRusByValue.end(), LexFunValueComp);

		 if ( !CreateLexFunIndexes( GetRoss(Aoss), m_LexFunIndexesEng) )
 		 {
			 ErrorMessage ("CreateLexFunIndexes( GetRoss(Aoss), m_LexFunIndexesEng) failed");
			 return false;
		 };


		 if (!BuildGenitFormOfCardinal (GetRusLemmatizer(), GetRusGramTab()) )
 		 {
			 ErrorMessage ("BuildGenitFormOfCardinal failed");
			 return false;
		 };

	     if ( ! BuildCollocs() )
 		 {
			 ErrorMessage ("BuildCollocs (russian) failed");
			 return false;
		 };
		 if ( !InitHierarchies() )
 		 {
			 ErrorMessage ("InitHierarchies  failed");
			 return false;
		 };
		 return true;

};

long CSemanticStructure::FindLeftClosestNode(size_t NodeNo) const
{
 long ClosestNodeNo = -1;
 size_t MinGap = 10;

 for (size_t i=0; i< GetNodesSize(); i++)
   if (i!=NodeNo)
     if (GetNode(i).IsWordContainer() )
	 {
	   long value = GetNode(NodeNo).GetMinWordNo() - GetNode(i).GetMaxWordNo();
       if (( value < MinGap) && (value > 0))
	   {
	     MinGap = value; 
		 ClosestNodeNo = i;
	   };
	 };

 return ClosestNodeNo; 
};


void  CSemanticStructure::AddSemFet (CSemNode& Node, const string& SemFet) 
{
	vector<string>::const_iterator It = find (m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);
    QWORD Num = It - m_IndexedSemFets.begin();
	assert (Num < 64);
	if (It == m_IndexedSemFets.end())
			m_IndexedSemFets.push_back(SemFet);
	if (Node.m_NodeSemFets.size() == 0)
		Node.m_NodeSemFets.push_back(_QM(Num) );
    else
	for (long i=0; i < Node.m_NodeSemFets.size(); i++)
	  Node.m_NodeSemFets[i] |= _QM(Num);
};


// SF из тезауруса нужно получить до чтения добавочных статей
void CSemanticStructure::InitThesSemFet (CSemNode& OutNode, const CSemNode& InNode)
{
  if ( !IsThesRoss(InNode.GetType()) ) return;
  assert (InNode.GetInterp());
  long TerminId = InNode.GetInterp()->m_TerminId;
  assert (TerminId != -1);

  const CThesaurus* Thes = m_pData->GetThes(GetThesIdByRossId(InNode.GetType()));
  vector<int> TopConcepts;
  Thes->QueryTopConcepts (TerminId, TopConcepts);
  long Count = TopConcepts.size();
  for (long k=0; k < Count; k++)
  {
		string ConceptStr = Thes->m_Concepts[TopConcepts[k]].m_ConceptStr;
		Trim(ConceptStr);
		if (  ( GetRossHolder(Ross)->GetItemNoByItemStr (ConceptStr.c_str(), "D_SF") != -1)
			|| ( GetRossHolder(Ross)->GetItemNoByItemStr (ConceptStr.c_str(), "D_SEM_REL") != -1)
		   )
			AddSemFet(OutNode,ConceptStr.c_str());
  };
};


bool CSemanticStructure::IsConnected()
{
	if (GetNodesSize() == 0) return true;

	size_t i=0;
	for (; i<GetNodesSize(); i++)
		GetNodePtr(i)->m_bReached = false;


	for (i=0; i<GetNodesSize(); i++)
		if (GetNodePtr(i)->IsWordContainer())
			break;

	if (i == GetNodesSize()) return true;

	dfs (i, false);

	for (i=0; i<GetNodesSize(); i++)
		if (GetNodePtr(i)->IsWordContainer())
			if (!GetNodePtr(i)->m_bReached) 
				return false;

	return true;
};



long CSemanticStructure::FindAbstractPlugArticle (DictTypeEnum type, QWORD Grammems, long Poses, long ClauseType) const
{

	const vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);

	for (long i=0; i< AbstractArticles.size(); i++)
	{
	  const CAbstractArticle& A = AbstractArticles[i];

      for (long k=0; k < A.m_GramInfos.size(); k++)
		  if (   (  ( (Poses > 0 ) && (Poses & A.m_GramInfos[k].m_PartOfSpeechMask) == Poses) )
			  && (     (A.m_GramInfos[k].m_Grammems == 0)
			        || (    (Grammems > 0 ) 
				         && (Grammems & A.m_GramInfos[k].m_Grammems) ==  A.m_GramInfos[k].m_Grammems
					   ) 
				 )
			  && (A.m_Type == atArticlePlug)
			  &&  ( 
			           (A.m_ClauseTypes.size() == 0)
			       ||  _find(A.m_ClauseTypes, ClauseType)
				  )
			 )
			 return i;
	};

    return -1;
};




void CSemanticStructure::FindAbstractAdditionArticle (DictTypeEnum type, const CSemNode& Node, vector<long>& Articles, bool IsClauseSyntaxRoot, long ClauseType) 
{
	Articles.clear();
	const vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);
	if (Node.m_MainWordNo == -1) return;
	const CSemWord& W = Node.GetWord(Node.m_MainWordNo);

	for (long i=0; i< AbstractArticles.size(); i++)
		if  (AbstractArticles[i].m_Type == atAdditionArticle)
		{
			const CAbstractArticle& A = AbstractArticles[i];

			// проверка GF
			long k=0;
			for (; k < A.m_GramInfos.size(); k++)
				if (		((W.m_Poses & A.m_GramInfos[k].m_PartOfSpeechMask) > 0)
					&&	Node.HasGrammems (A.m_GramInfos[k].m_Grammems) 
					)

					break;

			if  (   (A.m_GramInfos.size() > 0) 
				&& (k == A.m_GramInfos.size())
				)
				continue;

			// проверка КЛАУЗЫ
			if (A.m_ClauseTypes.size() > 0)
				if (!_find(A.m_ClauseTypes, ClauseType))
					continue;

			// Проверка CAT
			for (k=0; k < A.m_SemTypes.size(); k++)
			{
				// если узел был синтаксической вершиной, тогда  считаем, что это LABL.SIT		   
				if	(		IsClauseSyntaxRoot 
					&&	(A.m_SemTypes[k] == "LABL.SIT")			  
					)
					break;

				if (HasSemType(Node, A.m_SemTypes[k]))
					break;
			}

			if  (   (A.m_SemTypes.size() > 0) 
				&& (k == A.m_SemTypes.size())
				)
				continue;


			// Проверка LEX
			for (k=0; k < A.m_LexFets.size(); k++)
			{
				if (_find(Node.m_HigherConcepts, A.m_LexFets[k]))
					break;
			}

			if  (   (A.m_LexFets.size() > 0) 
				&& (k == A.m_LexFets.size())
				)
				continue;

			// Проверка SF
			for (k=0; k < A.m_SemFets.size(); k++)
				if	(    !HasSemFetPro(Node, A.m_SemFets[k].c_str()) 
					&& !HasSemFetOrLower(Node, A.m_SemFets[k].c_str() ) // чтобы +MEDINF добавлялся к тем, которые имеют SET-DEMAN			 
					)
					break;

			if  (    (A.m_SemFets.size() > 0) 
				&& (k < A.m_SemFets.size())
				)
				continue;


			Articles.push_back(i);
		};

};

void CSemanticStructure::AddAbstractAdditionVals (DictTypeEnum type, CSemNode& Node, const vector<long>& Articles)
{
	const vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);
	for  (long i=0; i <Articles.size(); i++)
    {
      long ArtNo = Articles[i];
	  for (long l=0; l < AbstractArticles[ArtNo].m_Vals.size(); l++)
	  if (Node.m_Vals.size() + 1 < MaxValsCount)
	  {
		CValency V = AbstractArticles[ArtNo].m_Vals[l];
		V.m_IsAddition = true;
		V.m_UnitNo = AbstractArticles[ArtNo].m_UnitNo;
		V.m_LeafId = AbstractArticles[ArtNo].m_Vals[l].m_LeafId;
		V.m_BracketLeafId = AbstractArticles[ArtNo].m_Vals[l].m_BracketLeafId;
		Node.m_Vals.push_back(V);
	  };

  	  // добавляем лексичексие фукнции	из добавочных словарных статей
      GetRossHolder(type)->GetLexFuncts(AbstractArticles[ArtNo].m_UnitNo, Node.m_LexFunctFields);
	};

};

bool CSemanticStructure::HasItem (DictTypeEnum DictTy,  WORD UnitNo, const string& FieldStr, const string& ItemStr, const string& DomStr, BYTE LeafId, BYTE BracketLeafId)  const
{
		CRossQuery Q(DictTy, UnitNo, FieldStr, ItemStr, DomStr, LeafId, BracketLeafId);
		vector<CRossQuery>::const_iterator It = find (Queries.begin(),Queries.end(),Q);
		if (It < Queries.end())
			return It->m_Result;
        Q.m_Result = GetRossHolder(DictTy)->HasItem(UnitNo, FieldStr, ItemStr, DomStr, LeafId, BracketLeafId);
		(const_cast<CSemanticStructure*>(this))->Queries.push_back(Q);
		return Q.m_Result;
};

// проверяет часть речи узла
bool   CSemanticStructure::HasPOS (const CSemNode& N, size_t POS) const
{
	return (N.m_MainWordNo != -1) && N.GetWord(N.m_MainWordNo).HasPOS(POS);
};
bool   CSemanticStructure::HasPOS (size_t NodeNo, size_t POS) const
{
	return HasPOS (GetNode(NodeNo), POS);
};

bool CSemanticStructure::IsVerbForm(const CSemNode& Node) const
{
	return  (   HasPOS(Node, VERB) 
			|| HasPOS(Node, ADVERB_PARTICIPLE) 
			|| HasPOS(Node, PARTICIPLE) 
   			|| HasPOS(Node, PARTICIPLE_SHORT) 
			|| HasPOS(Node, INFINITIVE) 
			);
};

void CSemanticStructure::SetLastSentencePunctuationMark(string& str) const
{
	switch (m_SentenceMood) {
		case Interrogative	: str += "?"; break;
		case Exclamative	: str += "!"; break;
		case Indicative		: str += "."; break;
	};
};

const char* GetStrByCategory (SemCategoryEnum t) 
{
	switch (t)  {
		case scSituation: return "LABL.SIT";
		case scObject: return "LABL.OBJ";
		case scLabel: return "LABL";
		case scRelation: return "RELAT";	
	};
	assert (false);
	return "LABL";
};


//==========================================

CRossQuery::CRossQuery (DictTypeEnum  TypeEnum,
		WORD UnitNo,	
		string FieldStr,
		string ItemStr,
		string DomStr,
		BYTE  LeafId,
		BYTE BracketLeafId
		)
{
	m_TypeEnum  = TypeEnum;
    m_UnitNo = UnitNo;
	m_FieldStr = FieldStr;
	m_ItemStr = ItemStr;
	m_LeafId = LeafId;
	m_BracketLeafId = BracketLeafId;
	m_DomStr = DomStr;

};
bool CRossQuery::operator == ( const CRossQuery& X ) const
{
	return   (X.m_TypeEnum == m_TypeEnum) 
			&& (X.m_UnitNo == m_UnitNo)
			&& (X.m_LeafId == m_LeafId)
			&& (X.m_BracketLeafId == m_BracketLeafId)
			&& (X.m_FieldStr == m_FieldStr)
			&& (X.m_ItemStr == m_ItemStr)
			&& (X.m_DomStr == m_DomStr);
};	

//=====================================


void CSynRealization::SetEmpty()
{
	m_Preps.clear();
	m_Conj.m_UnitNo = ErrUnitNo;
	m_Grammems = 0;
	m_Other = "";
	m_Cortege = TCortege();
};

// проверяет, приписан ли узлу предлог PrepNo
bool CSynRealization::HasThePrep (WORD UnitNo) const 
{ 
	for (long i=0; i < m_Preps.size(); i++)
		if (m_Preps[i].m_UnitNo == UnitNo)
			return true;

	return false;
};


//==============================

void SetSpacesAndRegisterInSentence (string& str, MorphLanguageEnum Langua) 
{
	/*
	 При выставлении запятых для вводных слов некоторые запятые могут оказать в самом 
	 конце или самом начале предложения. Их нужно удалить.
	*/
	Trim(str);
	// в начале 
	if (str.length() == 0) return;
	if (str[0] == ',') str.erase(0,1);
	// в конце 
	if (str.length() == 0) return;
	if (str[str.length()-1] == ',') str.erase(str.length()-1,1);


	// если два соседних слова стоят в кавычках, то нужно объединить эти кавычки
	// <Quote> X </Quote> <Quote> Y </Quote> -> <Quote> X Y </Quote>
	for (size_t i=0; i< str.size(); i++)
	{
		// открывающую кавычку пишм слитно с  текстом
		if (str.substr(i, 8) == "</Quote>")
		{
			long k=i+8;
			for (; k< str.size(); k++)
			  if (str[k] != ' ')
				  break;
			if (str.substr(k, 7) == "<Quote>")
				str.erase(i, k+7-i);
		};

	};

	// выставлем кавычки 
	for (size_t i=0; i< str.size(); i++)
	{
		// открывающую кавычку пишм слитно с  текстом
		if (str.substr(i, 7) == "<Quote>")
		{
			for (int k = i+7; k < str.size();)
				if (str[k] != ' ')
					break;
				else
					str.erase(k,1);
			str.replace	(i, 7, "\"");
		};
		// закрывающую  кавычку пишем слитно с  текстом
		if (str.substr(i, 8) == "</Quote>")
		{
			for (int k = i-1; k>=0;i--)
				if (str[k] != ' ')
					break;
				else
					str.erase(k,1);
			str.replace	(i, 8, "\"");
		};
	};


	// проходим по предложению и удаляем пробелы перед знаками препинания
	StringTokenizer tok(str.c_str(), " ");
	str.erase();
	bool last_is_punct = true;
	int tok_count = 0;
	while(tok())
	{
		string word = tok.val();
		bool is_punct =    (word.size() == 1) 
						&& ispunct((unsigned char)word[0]) != 0
						&& !strchr("(){}<>\"",word[0]);
		if(is_punct && last_is_punct) continue;
		last_is_punct = is_punct;
		// последний знак препинания не печатаем
		if(is_punct && !tok.has_next()) continue;
		// если слово, то перед словом должен быть пробел, а если знак 
		// препинания, то его нужно писать слитно с предыдущим словом, поэтому
		// пробел не добавляем.
		if(!is_punct && tok_count) str += " ";
		str += word;
		tok_count++;
	}

		// Первая буква предложения всегда должна быть прописной
	// (проходим начальные знаки препинания (типа кавычки)
	size_t k=0;
	for (; k < str.size(); k++)
		if (!ispunct ((BYTE)str[k]))
			break;

	if (k < str.size())
		if (is_lower_alpha((BYTE)str[k], Langua))
			str[k] = ReverseChar((BYTE)str[k], Langua);

}



bool CSemanticStructure::IsRelBetweenClauses(const CSemRelation& rel) const 
{
	return GetNode(rel.m_SourceNodeNo).m_ClauseNo != GetNode(rel.m_TargetNodeNo).m_ClauseNo;
}


CValency		CSemanticStructure::GetSemRelOfPrepOrConj(const CDictUnitInterp& Unit) const
{
	if (Unit.m_DictType == NoneRoss) 
			return CValency("ACT", A_C);		 

	return GetRossHolder(Unit.m_DictType)->GetSemRelOfPrepOrConj(Unit.m_UnitNo);
};

bool			CSemanticStructure::IsInClause (size_t NodeNo, size_t ClauseNo) const	
{
	return GetNode(NodeNo).m_ClauseNo == ClauseNo; 
};

bool			CSemanticStructure::HasSemType (const CSemNode& Node, string Type) const
{
       return (Node.GetType() != NoneRoss) && HasItem (Node.GetType(), Node.GetUnitNo(), "CAT", Type, "D_CAT", 0, 0);
};

bool			CSemanticStructure::GramFetAgreeWithPoses (CRossHolder& Ross,WORD UnitNo, const CSemWord& W ) const
{
	return (W.m_Poses & GetPosesFromRusArticle(Ross, UnitNo)) > 0; 
};
