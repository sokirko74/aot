#include "stdafx.h"
#include "SemanticStructure.h"
#include "HierarchyHolder.h"

//=====================================  CSemWord  =====================================
const CStructDictHolder* CSemWord::GetRossHolder(DictTypeEnum Type)   const
{
	return m_pData->GetRossHolder(Type);
};
const CDictionary* CSemWord::GetRoss(DictTypeEnum Type) const
{
	return m_pData->GetRossHolder(Type)->GetRoss();
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
	m_bReached = false;
	m_Tag = -1;
	m_TerminId = -1;
	m_ThesaurusId = -1;
	m_AbstractNodeId = -1;
};

// удаляет из оператор из  m_RelOperators
void CSemNode::DelRelOperator(const std::string& oper)
{
	for (int i = 0; i < m_RelOperators.size(); i++)
		if (m_RelOperators[i] == oper)
		{
			m_RelOperators.erase(m_RelOperators.begin() + i);
			i--;
		};
}


void      CSemRelation::Init()
{
	m_bToDelete = false;
	m_bRelUse = true;
	m_SourceNodeNo = -1;
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
CDictUnitInterp* CSemNode::GetInterp()
{
	return (m_CurrInterp == -1) ? 0 : &(m_Interps[m_CurrInterp]);
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

const CDictUnitInterpVector CSemNode::GetInterps() const
{
	return m_Interps;
}

bool  CSemNode::SetCurrInterp(const CDictUnitInterp& X)
{
	std::vector<CDictUnitInterp>::const_iterator it = find(m_Interps.begin(), m_Interps.end(), X);
	if (it == m_Interps.end()) return false;
	m_CurrInterp = it - m_Interps.begin();
	return true;
};

void  CSemNode::DeleteInterp(size_t i)
{
	m_Interps.erase(m_Interps.begin() + i);
	ResetCurrInterp();
};

// дает  константную текущую интерпрецию
const CDictUnitInterp* CSemNode::GetInterp() const
{
	return (m_CurrInterp == -1) ? 0 : &(m_Interps[m_CurrInterp]);
};

// дает  номер статьи для текущей интерпреции
uint16_t   CSemNode::GetUnitNo() const
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
bool CSemNode::IsAbstract() const
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
bool CSemNode::IsWordContainer() const
{
	return (m_NodeType == SimpleNode) || (m_NodeType == MNA);
};


// выдает номер самого левого слова данного узла
long CSemNode::GetMaxWordNo() const
{
	assert(IsWordContainer());
	return GetWord(GetWordsSize() - 1).m_WordNo;
};

// выдает номер самого правого слова данного узла
long CSemNode::GetMinWordNo() const
{
	assert(IsWordContainer());
	return GetWord(0).m_WordNo;
};



// проверяет наличие оператора среди  RelOperators
bool CSemNode::HasRelOperator(const std::string& oper) const
{
	return  find(m_RelOperators.begin(), m_RelOperators.end(), oper) != m_RelOperators.end();
};


// проверяет, приписан ли узлу хотя бы один предлог
bool CSemNode::HasSomePrep() const
{
	return m_SynReal.m_Preps.size() > 0;
};

// проверяет, приписан ли узлу предлог PrepNo
bool CSemNode::HasThePrep(uint16_t UnitNo) const
{
	return m_SynReal.HasThePrep(UnitNo);
};



bool  CSemNode::IsTimeRossNode() const
{
	return (m_Colloc.m_CollocSource == RossType)
		&& (m_Colloc.GetRossInterp().m_DictType == TimeRoss);
};

bool  CSemNode::IsMainTimeRossNode() const
{
	return    IsTimeRossNode()
		&& m_Colloc.GetRossInterp().m_bMainWord;
};


bool CSemNode::IsThesNode() const
{
	return (m_Colloc.m_CollocSource == ThesType);
};


bool  CSemNode::IsTrueLocNode() const
{
	return     IsPrimitive()
		&& (GetType() == LocRoss)
		&& (GetWord(0).HasPOS(NOUN)
			|| GetWord(0).HasPOS(ADV)
			);
};
bool   CSemNode::HasPOS(part_of_speech_t POS) const
{
	if (m_MainWordNo == -1) return false;
	return GetWord(m_MainWordNo).HasPOS(POS);
}

bool CSemNode::IsLemma(std::string Lemma) const
{
	return IsPrimitive() && (GetWord(0).m_Lemma == Lemma);
};

// проверяет, что данный узел является пассивным глаголом
bool	CSemNode::IsPassiveVerb() const
{
	const CDictUnitInterp* I = GetInterp();
	if (I == 0) return false;
	return   I->m_bPassiveForm;
};

uint64_t	CSemNode::GetGrammems() const
{
	return m_Grammems;
}

void	CSemNode::SetGrammems(uint64_t g)
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

part_of_speech_mask_t CSemNode::GetNodePoses() const
{
	if (m_MainWordNo == -1) return 0;
	return  GetWord(m_MainWordNo).m_Poses;
};



void CSemNode::AddGrammems(uint64_t  grammems)
{
	SetGrammems(GetGrammems() | grammems);
};

void CSemNode::DeleteGrammems(uint64_t grammems)
{
	SetGrammems(GetGrammems() & ~grammems);
};

bool	CSemNode::HasGrammems(uint64_t grammems) const
{
	return (grammems & m_Grammems) == grammems;
};


//=====================================  CSemRelation =====================================
CSemRelation::CSemRelation()
{
	Init();
};


CSemRelation::CSemRelation(const CValency& Valency,
	long SourceNodeNo,
	long TargetNodeNo,
	std::string SyntacticRelation)
{
	Init();
	m_Valency = Valency;
	m_SourceNodeNo = SourceNodeNo;
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


CStructDictHolder* CSemanticStructure::GetRossHolder(DictTypeEnum Type)   const
{
	return m_pData->GetRossHolder(Type);
};
const CDictionary* CSemanticStructure::GetRoss(DictTypeEnum Type) const
{
	return m_pData->GetRoss(Type);
};


// инициализация валентностей узла
void  CSemanticStructure::InitVals(CSemNode& Node)
{
	Node.m_Vals.clear();
	if (Node.GetType() == NoneRoss) return;

	long  UnitNo = Node.GetUnitNo();
	if (UnitNo == ErrUnitNo)
		return;

	const CStructDictHolder* pRossDoc = GetRossHolder(Node.GetType());

	if (UnitNo != ErrUnitNo)
		if (!pRossDoc->GetRoss()->IsEmptyArticle(UnitNo))
			for (size_t i = pRossDoc->GetRoss()->GetUnitStartPos(UnitNo); i <= pRossDoc->GetRoss()->GetUnitLastPos(UnitNo); i++)
			{
				TCortege C = pRossDoc->GetCortegeCopy(i);
				if (C.m_FieldNo == pRossDoc->ValFieldNo && Node.m_Vals.size() < MaxValsCount)
				{
					Node.m_Vals.push_back(CValency(C, pRossDoc, UnitNo));
				};
			};



}



inline long ConvertGramRestrToGrammems(std::string t)
{
	if (t == "ед")
		return _QM(rSingular);
	if (t == "мн")
		return _QM(rPlural);
	if (t == "1л")
		return _QM(rFirstPerson);
	if (t == "2л")
		return _QM(rSecondPerson);
	if (t == "3л")
		return _QM(rThirdPerson);
	return 0;
};


// возвращает множество наборов граммем, которые разрешено иметь данному узлу
std::vector<uint64_t> CSemanticStructure::GetGramRestr(const CSemNode& W)
{
	std::vector<uint64_t> GrammemsSet;

	if (W.GetUnitNo() != ErrUnitNo)
		if (!GetRoss(W.GetType())->IsEmptyArticle(W.GetUnitNo()))
			for (size_t i = GetRoss(W.GetType())->GetUnitStartPos(W.GetUnitNo()); i <= GetRoss(W.GetType())->GetUnitLastPos(W.GetUnitNo()); i++)
				if (GetRoss(W.GetType())->GetCortegeFieldNo(i) == GetRossHolder(W.GetType())->GramRestrFieldNo)
				{
					const TCortege& C = GetRossHolder(W.GetType())->GetCortegeCopy(i);
					std::string GramFet = GetRoss(W.GetType())->WriteToString(C);
					Trim(GramFet);
					uint32_t Pose;
					uint64_t Grammems;
					m_pData->GetCustomGrammems(GramFet, Grammems, Pose);
					GrammemsSet.push_back(Grammems);
				};
	return GrammemsSet;
};




std::vector<std::string> CSemanticStructure::GetNodeDictInterps(size_t nodeIndex) const {
	std::vector<std::string> result;
	if (GetNode(nodeIndex).m_Colloc.m_CollocSource != NoneType)
	{
		result.push_back(" + " + OpenCollocInterpToStr(GetNode(nodeIndex).m_Colloc));
	}

	const CSemNode& N = GetNode(nodeIndex);
	for (long k = 0; k < N.GetInterps().size(); k++)
	{
		result.push_back(Format("%s %s",
			(k == N.GetCurrInterpNo()) ? " + " : " - ",
			InterpToStr(N.GetInterps()[k]).c_str()));
	};
	return result;
}







std::string CSemanticStructure::GetTclSyntaxGraph()
{
	// уничтожение графа 
	std::string Res = "";
	Res += "$GT($main,graph) delete nodes\1";

	Res += Format("$main.switches.resultText configure -text \"%s\"\1", m_ClausePropertiesProtocol.c_str());
	Res += Format("set GT($main,tree_variant_props) \"%s\"\1", m_WeightProtocol.c_str());
	Res += Format("set GT($main,statistics) \"%s\"\1", m_TimeStatictics.c_str());
	Res += Format("set GT($main,variants_statistics) \"%s\"\1", m_ClauseVariantsStatistics.c_str());


	for (size_t i = 0; i < GetNodesSize(); i++)
	{
		Res += Format("set nds(%i) [$GT($main,graph) create node]\1", i);
		Res += Format("$GT($main,graph) set $nds(%i) -label \"%s\" -type rectangle -x 0 -y 0\1", i, GetNodeStr(i, 30).c_str());
		std::string Grammems = m_pData->GetRusGramTab()->GrammemsToStr(GetNode(i).GetGrammems());
		Res += Format("$GT($main,graph) set $nds(%i)  .grammems \"%s\"\1", i, Grammems.c_str());
		Res += Format("$GT($main,graph) set $nds(%i)  .lemmas \"%s\"\1", i, GetNodeLemStr(i).c_str());
	};

	for (size_t i = 0; i < GetSynRels().size(); i++)
	{
		Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetSynRels()[i].m_SourceNodeNo, GetSynRels()[i].m_TargetNodeNo);
		Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", GetSynRels()[i].m_SynRelName.c_str());

		std::string Color = !GetSynRels()[i].m_SynRelName.empty() ? "blue" : "darkgreen";
		Res += Format("$GT($main,graph) configure $edge graphics -fill %s \1", Color.c_str());
	};

	return Res;

};



void GetCommonVariants(const std::vector<VectorLong>& Parents,
	VectorLong& V,
	std::vector<VectorLong>& Variants,
	long       Position)
{
	if (Variants.size() > 10000) return;
	if (Position < V.size())
		for (long i = 0; i < Parents[Position].size(); i++)
		{
			V[Position] = Parents[Position][i];
			GetCommonVariants(Parents, V, Variants, Position + 1);
		}
	else
		Variants.push_back(V);

};


std::string   CSemanticStructure::InterpToStr(const CDictUnitInterp& i)  const
{
	const CDictionary* Ross = GetRoss(i.m_DictType);

	std::string Result = Format("%s: %s %i",
		Ross->GetDictName().c_str(),
		Ross->GetEntryStr(i.m_UnitNo).c_str(),
		Ross->GetUnitMeanNum(i.m_UnitNo));

	if (i.m_bPassiveForm)
		Result += "(passive)";

	return Result;
};

std::string   CSemanticStructure::OpenCollocInterpToStr(const COpenCollocInterp& I)  const
{
	assert(I.m_CollocSource != NoneType);
	std::string S;

	if (I.m_CollocSource == RossType)
	{
		S = Format("%s: %s%i",
			GetRoss(I.GetRossInterp().m_DictType)->GetDictName().c_str(),
			GetRoss(I.GetRossInterp().m_DictType)->GetEntryStr(I.GetRossInterp().m_UnitNo).c_str(),
			GetRoss(I.GetRossInterp().m_DictType)->GetUnitMeanNum(I.GetRossInterp().m_UnitNo));

		if (I.GetRossInterp().m_ItemNo != -1)
			S += Format(" (ItemNo=%i)", I.GetRossInterp().m_ItemNo + 1);
		if (I.GetRossInterp().m_LexFetNo != -1)
			S += Format(" (LexFetNo=%i)", I.GetRossInterp().m_LexFetNo + 1);

		if (I.GetRossInterp().m_PrepNoInArticle != -1)
			S += Format(" (m_PrepNoInArticle=%i)", I.GetRossInterp().m_PrepNoInArticle + 1);
	}
	else
	{

		S = Format("%s: %i", m_pData->GetThesStr(I.GetThesInterp().m_ThesaurusId).c_str(), I.GetThesInterp().m_TerminId);

		if (I.GetThesInterp().m_ItemNo != -1) S += Format(" (ItemNo=%i)", I.GetThesInterp().m_ItemNo + 1);
	};

	return S;
};


std::string CSemanticStructure::GetTxtGraph()
{
	std::string Res;
	Res += "Nodes\n";
	for (size_t i = 0; i < GetNodesSize(); i++)
		if (GetNode(i).IsWordContainer())
		{
			;
			Res += Format("%i %-25s", i, GetNodeLemStr(i).c_str());
			if (GetNode(i).GetType() == NoneRoss)
			{
				Res += " -1 NoneRoss";
			}
			else
			{
				std::string RossName = m_pData->GetRoss(GetNode(i).GetType())->GetDictName();
				int MeanNum = -1;

				if (GetNode(i).GetUnitNo() != ErrUnitNo)
					GetRoss(GetNode(i).GetType())->GetUnitMeanNum(GetNode(i).GetUnitNo());

				Res += Format(" %i %s", MeanNum, RossName.c_str());
			};
			Res += '\n';
		};


	Res += "Relations\n";

	for (size_t i = 0; i < GetRelationsSize(); i++)
	{
		std::string SynReal = SynRealToStr(GetRelation(i)->m_SynReal, "\n");
		TrimRight(SynReal);
		Res += Format("%-10s %-3i %-3i %s\n",
			GetRelation(i)->m_Valency.m_RelationStr.c_str(),
			GetRelation(i)->m_TargetNodeNo,
			GetRelation(i)->m_SourceNodeNo,
			SynReal.c_str());


	};

	return Res;

};







uint64_t	CSemanticStructure::GetOneIndexedSemFet(const std::vector<std::string>& _SemFets, bool bInlcudeLowerHierarcy, bool	bInlcudeHigherHierarcy)
{
	uint64_t Result = 0;
	std::vector<std::string> SemFets = _SemFets;

	if (bInlcudeLowerHierarcy)
		IncludeLowerInHierarchy(&m_pData->m_HierarchySemFetDoc, SemFets);

	if (bInlcudeHigherHierarcy)
		IncludeHigherInHierarchy(&m_pData->m_HierarchySemFetDoc, SemFets);



	for (int k = 0; k < SemFets.size(); k++)
	{
		std::vector<std::string>::const_iterator It = find(m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFets[k]);
		uint64_t Num = It - m_IndexedSemFets.begin();
		assert(Num < 64);
		if (It == m_IndexedSemFets.end())
			m_IndexedSemFets.push_back(SemFets[k]);
		Result |= _QM(Num);
	};

	return Result;

};

std::vector<uint64_t> CSemanticStructure::GetIndexedSemFets(const CSemPattern& P, bool bInlcudeLowerHierarcy, bool	bInlcudeHigherHierarcy)
{
	uint64_t DomensSemFet = 0;
	/*
	в DomensSemFet находятся все SF, которые обозначают предметные области FIN, SOC  (пока их только две)
	*/
	//assert (m_IndexedSemFets[2] == "FIN");//assert (m_IndexedSemFets[3] == "SOC");  
	for (uint64_t j = 2; j < 4; j++)
		DomensSemFet |= _QM(j);


	std::vector<uint64_t> Result;

	for (int i = 0; i < P.m_ActantSemFets.size(); i++)
	{
		uint64_t OneSemFetsSet = GetOneIndexedSemFet(P.m_ActantSemFets[i], bInlcudeLowerHierarcy, bInlcudeHigherHierarcy);

		/*
		надо добавить SF FIN,SOC (предметных областей), если предметных областей в SF не указано      Например, если SF=SITUAT и bInlcudeLowerHierarcy=true, тогда
		SF=SITUAT,FIN - разновидность  SF=SITUAT	*/
		if (bInlcudeLowerHierarcy)
			if ((OneSemFetsSet & DomensSemFet) == 0)
				OneSemFetsSet |= DomensSemFet;

		Result.push_back(OneSemFetsSet);
	};


	return Result;
};


bool CSemanticStructure::GleicheSemFet(const std::vector<uint64_t>& SemFets1, const std::vector<uint64_t>& SemFets2, bool bInclusion) const
{
	// если не приписана SF, то считаем, что у слова может быть любая SF
	// например, слову "большой"  SF1 не приписана	

	if (SemFets1.empty() || SemFets2.empty()) return true;

	std::string debug1 = GetSemFetsInOneStr(SemFets1);
	std::string debug2 = GetSemFetsInOneStr(SemFets2);

	for (long i = 0; i < SemFets1.size(); i++)
		for (long k = 0; k < SemFets2.size(); k++)
			if (bInclusion)
			{
				if ((SemFets1[i] & SemFets2[k]) == SemFets2[k])
					return true;
			}
			else
			{
				if ((SemFets1[i] & SemFets2[k]) > 0)
					return true;
			};

	return false;
};



std::vector<std::string> CSemanticStructure::GetSemFetStr(uint64_t SemFet) const
{
	std::vector<std::string> Res;
	for (long k = 0; k < 64; k++)
	{
		uint64_t Value = _QM(k);
		if ((Value & SemFet) > 0)
		{
			Res.push_back(m_IndexedSemFets[k]);
		};
	};
	return Res;
};


std::string CSemanticStructure::GetSemFetsInOneStr(const std::vector<uint64_t>& SemFets) const
{
	std::string Result;
	for (long i = 0; i < SemFets.size(); i++)
	{
		std::vector<std::string> SemFetStr = GetSemFetStr(SemFets[i]);
		assert(SemFetStr.size() <= m_IndexedSemFets.size());
		std::string Res = Format("%i ", i + 1);
		for (long k = 0; k < SemFetStr.size(); k++)
			Res += std::string(SemFetStr[k]) + std::string(",");
		if (Res.length() > 0) Res.erase(Res.length() - 1);
		Result += Res;
		Result += "; ";
	};
	if (Result.length() == 0) Result = " ";
	return Result;
};



bool  CSemanticStructure::HasSemFet(const CSemNode& Node, const std::string& SemFet) const
{
	std::vector<std::string>::const_iterator It = find(m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);

	if (It == m_IndexedSemFets.end()) return false;

	uint64_t Num = It - m_IndexedSemFets.begin();

	bool Result = Node.m_NodeSemFets.size() > 0;

	for (long i = 0; i < Node.m_NodeSemFets.size(); i++)
	{
		Result = Result && ((_QM(Num) & Node.m_NodeSemFets[i]) > 0);
	};

	return Result;
};


bool  CSemanticStructure::HasSemFetPro(const std::vector<uint64_t>& SemFets, const std::string& SemFet) const
{
	std::vector<std::string>::const_iterator It = find(m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);

	if (It == m_IndexedSemFets.end()) return false;

	uint64_t Num = It - m_IndexedSemFets.begin();

	assert(m_IndexedSemFets[0] == "NEG"); assert(m_IndexedSemFets[1] == "CAUS");
	for (long i = 0; i < SemFets.size(); i++)
		if ((((1 << 0) | (1 << 1)) & SemFets[i]) == 0)
			if (_QM(Num) & SemFets[i])
				return true;

	return false;

};
// проходит по всем дизъюнктам, если в дизъюнкте отстутствует CAUS и NEG,
// и присутствует SemFet, тогда выдает истину
bool  CSemanticStructure::HasSemFetPro(const CSemNode& Node, const std::string& SemFet) const
{
	return HasSemFetPro(Node.m_NodeSemFets, SemFet);
};

// проверяет, принадлежит ли узлу данная SF или какая-нибудь SF ниже по иерархии
bool  CSemanticStructure::HasSemFetOrLower(const CSemNode& Node, const std::string& SemFet) const
{
	for (long i = 0; i < Node.m_NodeSemFets.size(); i++)
	{
		std::vector<std::string> SemFetStr = GetSemFetStr(Node.m_NodeSemFets[i]);
		IncludeHigherInHierarchy(&m_pData->m_HierarchySemFetDoc, SemFetStr);
		for (long j = 0; j < SemFetStr.size(); j++)
			if (SemFetStr[j] == SemFet)
				return  true;
	};
	return false;
};




bool CSemanticStructure::IsVerbOrMNAVerb(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if (rusActant.m_MainWordNo != -1)
		if (rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(VERB) ||
			rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(INFINITIVE))
			return true;

	if (rusActant.m_NodeType == MNA)
	{
		std::vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for (int i = 0; i < rels.size(); i++)
		{

			if (!IsVerbOrMNAVerb(GetRelation(rels[i])->m_TargetNodeNo))
				return false;
		}
		return true;
	}
	return false;
}

bool CSemanticStructure::IsInfinitiveOrMNAInfinitive(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if (rusActant.m_MainWordNo != -1)
		if (rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(INFINITIVE))
			return true;

	if (rusActant.m_NodeType == MNA)
	{
		std::vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for (int i = 0; i < rels.size(); i++)
		{
			if (!IsInfinitiveOrMNAInfinitive(GetRelation(rels[i])->m_TargetNodeNo))
				return false;
		}
		return true;
	}
	return false;
}


bool CSemanticStructure::IsNounOrMNANoun(int iNode) const
{
	const CSemNode& rusActant = GetNode(iNode);

	if (rusActant.m_MainWordNo != -1)
		if (rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(NOUN) ||
			rusActant.GetWord(rusActant.m_MainWordNo).HasPOS(PRONOUN))
			return true;

	if (rusActant.m_NodeType == MNA)
	{
		std::vector<long> rels;
		GetOutcomingRelations(iNode, rels, false);
		for (int i = 0; i < rels.size(); i++)
		{
			if (!IsNounOrMNANoun(GetRelation(rels[i])->m_TargetNodeNo))
				return false;
		}
		return true;
	}
	return false;
}




bool CSemanticStructure::CheckGroupBeginAndCase(std::string ItemStr, size_t NodeNo, long& PrepNo) const
{
	const CSemNode& N = GetNode(NodeNo);
	if (ItemStr.length() > 3)
		if (ItemStr.substr(ItemStr.length() - 3) == "_мн")
		{
			if (!N.HasOneGrammem(rPlural)
				&& (N.m_NodeType != MNA)
				)
				return false;
		};

	uint16_t DictPrepNo = GetRossHolder(OborRoss)->LocateUnit(ItemStr.c_str(), 1);
	for (PrepNo = 0; PrepNo < N.m_SynReal.m_Preps.size(); PrepNo++)
		if (DictPrepNo == N.m_SynReal.m_Preps[PrepNo].m_UnitNo)
			return true;
	PrepNo = -1;

	return false;
};



long    CSemanticStructure::GetLocPrepFromBegining(size_t NodeNo) const
{
	const CSemNode& N = GetNode(NodeNo);
	for (long i = 0; i < N.m_SynReal.m_Preps.size(); i++)
		if (IsLocSemRel(GetSemRelOfPrepOrConj(N.m_SynReal.m_Preps[i]).m_RelationStr))
			return i;
	return -1;
};

bool    CSemanticStructure::HasLocPrepInBegining(size_t NodeNo) const
{
	return GetLocPrepFromBegining(NodeNo) != -1;
};




bool CSemanticStructure::IsRusSubj(int iRel) const
{
	if (GetRelation(iRel)->
		m_SyntacticRelation == "подл")
		return true;
	long iRusSynRel = GetSynRelBySemRel(iRel);
	if (iRusSynRel != -1) {
		auto& synrel = GetSynRels()[iRusSynRel];
		if (synrel.m_SynRelName == "ПОДЛ")
			return true;
	}
	return false;
}


SEngEquiv CSemanticStructure::GetAL1Value(int UnitNo) const
{
	SEngEquiv res;
	res.m_iMeanNum = 1;
	if (UnitNo == ErrUnitNo)
		return res;

	std::vector<TCortege> corteges;
	GetRossHolder(Ross)->GetFieldValues(std::string("SF"), UnitNo, corteges);

	for (auto& c: corteges)
	{
		if (!c.is_null(0))
		{
			std::string str1 = GetRossHolder(Ross)->GetDomItemStrWrapper(c.GetItem(0));
			if (str1 == "AL1")
				if (!c.is_null(1))
				{
					str1 = GetRossHolder(Ross)->GetDomItemStrWrapper(c.GetItem(1));
					res.m_StrEngWord = str1;
					if (!c.is_null(2))
					{
						std::string meanNum = GetRossHolder(Ross)->GetDomItemStrWrapper(c.GetItem(2));
						res.m_iMeanNum = meanNum[0] - '0';
					}
				}
		}
	}

	return res;
}

void CSemanticStructure::AssertValidGraph()
{
	for (int i = 0; i < GetRelationsSize(); i++)
	{
		assert((GetRelation(i)->m_SourceNodeNo >= 0) && (GetRelation(i)->m_SourceNodeNo < GetNodesSize()));

		assert((GetRelation(i)->m_TargetNodeNo >= 0) && (GetRelation(i)->m_TargetNodeNo < GetNodesSize()));

	}

	for (int i = 0; i < GetSynRels().size(); i++)
	{
		assert((GetSynRels()[i].m_SourceNodeNo >= 0) && (GetSynRels()[i].m_SourceNodeNo < GetNodesSize()));

		assert((GetSynRels()[i].m_TargetNodeNo >= 0) && (GetSynRels()[i].m_TargetNodeNo < GetNodesSize()));

	}

}


std::string CSemanticStructure::SynRealToStr(const CSynRealization& SynReal, std::string Delimiter) const
{
	std::string Preps;
	for (long l = 0; l < SynReal.m_Preps.size(); l++)
	{
		Preps += GetRoss(SynReal.m_Preps[l].m_DictType)->GetEntryStr(SynReal.m_Preps[l].m_UnitNo) + Delimiter;
	}

	if (SynReal.m_Conj.m_UnitNo != ErrUnitNo)
		Preps += GetRoss(SynReal.m_Conj.m_DictType)->GetEntryStr(SynReal.m_Conj.m_UnitNo) + Delimiter;

	Preps += SynReal.m_Other;

	return Preps;
};



void CSemanticStructure::SetNodeToDeleteFalse()
{
	for (long i = 0; i < GetNodesSize(); i++)
		GetNodePtr(i)->m_bToDelete = false;
};
void CSemanticStructure::DelNodesToDelete()
{
	for (long i = 0; i < GetNodesSize(); i++)
		if (GetNode(i).m_bToDelete)
		{
			DelNode(i);
			i = -1;
		};
};

void CSemanticStructure::SetRelsToDeleteFalse()
{
	for (long i = 0; i < GetRelationsSize(); i++)
		GetRelation(i)->m_bToDelete = false;
};

void CSemanticStructure::DelRelsToDelete(const char* cause)
{
	for (long i = 0; i < GetRelationsSize(); i++)
		if (GetRelation(i)->m_bToDelete)
		{
			EraseRelation(i, cause);
			i--;
		};
};


std::string CSemanticStructure::GetOtherRelations() const
{
	std::string Res = "";
	for (long i = 0; i < GetLexFuncts().size(); i++)
		if (GetLexFuncts()[i].m_SourceNodeNo != GetLexFuncts()[i].m_TargetNodeNo)
			if ((FindFirstRelation(GetLexFuncts()[i].m_SourceNodeNo, GetLexFuncts()[i].m_TargetNodeNo) == -1)
				&& (FindFirstRelation(GetLexFuncts()[i].m_TargetNodeNo, GetLexFuncts()[i].m_SourceNodeNo) == -1)

				)
			{
				Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetLexFuncts()[i].m_SourceNodeNo, GetLexFuncts()[i].m_TargetNodeNo);
				Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", GetLexFuncts()[i].m_LexFunctName.c_str());
				Res += Format("$GT($main,graph) configure $edge graphics -fill blue -width 1.0 -stipple gray50\1");
			};

	for (long i = 0; i < GetDopRelationsSize(); i++)
		if (GetDopRelation(i)->m_SourceNodeNo != GetDopRelation(i)->m_TargetNodeNo)
			if ((FindFirstRelation(GetDopRelation(i)->m_SourceNodeNo, GetDopRelation(i)->m_TargetNodeNo) == -1)
				&& (FindFirstRelation(GetDopRelation(i)->m_TargetNodeNo, GetDopRelation(i)->m_SourceNodeNo) == -1)

				)
			{
				Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", GetDopRelation(i)->m_SourceNodeNo, GetDopRelation(i)->m_TargetNodeNo);
				Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", GetDopRelation(i)->m_Valency.m_RelationStr.c_str());
				Res += Format("$GT($main,graph) configure $edge graphics -fill blue -width 1.0 -stipple gray50\1");
				Res += Format("$GT($main,graph) set $edge .sem_fets \"%s\"\1", GetSemFetsInOneStr(GetDopRelation(i)->m_SemFets).c_str());

			};



	return Res;

};


bool CSemanticsHolder::InitializeIndices()
{
	// проверка того, что хотя бы один словарь был изменен
	if (!GetRossHolder(Ross)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(TimeRoss)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(OborRoss)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(CollocRoss)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(Aoss)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(EngCollocRoss)->HasBeenModified(m_LastIndicesUpdateTime)
		&& !GetRossHolder(EngObor)->HasBeenModified(m_LastIndicesUpdateTime)
		)
		return true;

	m_LastIndicesUpdateTime = m_LastUpdateTime;



	if (!GetRossHolder(Ross)->InitConsts())
	{
		ErrorMessage("GetRossHolder(Ross)->InitConsts() failed");
		return false;
	};
	;
	if (!GetRossHolder(TimeRoss)->InitConsts())
	{
		ErrorMessage("GetRossHolder(TimeRoss)->InitConsts() failed");
		return false;
	};

	if (!GetRossHolder(OborRoss)->InitConsts())
	{
		ErrorMessage("GetRossHolder(OborRoss)->InitConsts() failed");
		return false;
	};

	if (!GetRossHolder(CollocRoss)->InitConsts())
	{
		ErrorMessage("GetRossHolder(CollocRoss)->InitConsts() failed");
		return false;
	};

	if (!ReadAbstractArticles(Ross))
	{
		ErrorMessage("ReadAbstractArticles(Ross) failed");
		return false;
	};
	if (!ReadAbstractArticles(Aoss))
	{
		ErrorMessage("ReadAbstractArticles(Aoss) failed");
		return false;
	};

	if (!InitTimeUnits())
	{
		ErrorMessage("InitTimeUnits failed");
		return false;
	};

	if (!InitTimeThesLists())
	{
		ErrorMessage("InitTimeThesLists failed");
		return false;
	};

	if (!BuildOborottos())
	{
		ErrorMessage("BuildOborottos (russian) failed");
		return false;
	};

	if (!BuildEngOborStr())
	{
		ErrorMessage("BuildEngOborStr failed");
		return false;
	};

	if (!BuildEngCollocsStr())
	{
		ErrorMessage("BuildEngCollocsStr failed");
		return false;
	};

	if (!CreateAROSSIndex())
	{
		ErrorMessage("CreateAROSSIndex failed");
		return false;
	};

	if (!CreateEngCollocsROSSIndex())
	{
		ErrorMessage("CreateEngCollocsROSSIndex failed");
		return false;
	};
	if (!CreateEngOborIndex())
	{
		ErrorMessage("CreateEngOborIndex failed");
		return false;
	};

	if (!CreateLexFunIndexes(GetRoss(Ross), m_LexFunIndexesRusByValue))
	{
		ErrorMessage("CreateLexFunIndexes( GetRoss(Ross), m_LexFunIndexesRusByValue) failed");
		return false;
	};

	sort(m_LexFunIndexesRusByValue.begin(), m_LexFunIndexesRusByValue.end(), LexFunValueComp);

	if (!CreateLexFunIndexes(GetRoss(Aoss), m_LexFunIndexesEng))
	{
		ErrorMessage("CreateLexFunIndexes( GetRoss(Aoss), m_LexFunIndexesEng) failed");
		return false;
	};


	if (!BuildCollocs())
	{
		ErrorMessage("BuildCollocs (russian) failed");
		return false;
	};
	if (!InitHierarchies())
	{
		ErrorMessage("InitHierarchies  failed");
		return false;
	};
	return true;

};

long CSemanticStructure::FindLeftClosestNode(size_t NodeNo) const
{
	long ClosestNodeNo = -1;
	size_t MinGap = 10;

	for (size_t i = 0; i < GetNodesSize(); i++)
		if (i != NodeNo)
			if (GetNode(i).IsWordContainer())
			{
				long value = GetNode(NodeNo).GetMinWordNo() - GetNode(i).GetMaxWordNo();
				if ((value < MinGap) && (value > 0))
				{
					MinGap = value;
					ClosestNodeNo = i;
				};
			};

	return ClosestNodeNo;
};


void  CSemanticStructure::AddSemFet(CSemNode& Node, const std::string& SemFet)
{
	std::vector<std::string>::const_iterator It = find(m_IndexedSemFets.begin(), m_IndexedSemFets.end(), SemFet);
	uint64_t Num = It - m_IndexedSemFets.begin();
	assert(Num < 64);
	if (It == m_IndexedSemFets.end())
		m_IndexedSemFets.push_back(SemFet);
	if (Node.m_NodeSemFets.size() == 0)
		Node.m_NodeSemFets.push_back(_QM(Num));
	else
		for (long i = 0; i < Node.m_NodeSemFets.size(); i++)
			Node.m_NodeSemFets[i] |= _QM(Num);
};


// SF из тезауруса нужно получить до чтения добавочных статей
void CSemanticStructure::InitThesSemFet(CSemNode& OutNode, const CSemNode& InNode)
{
	if (!IsThesRoss(InNode.GetType())) return;
	assert(InNode.GetInterp());
	long TerminId = InNode.GetInterp()->m_TerminId;
	assert(TerminId != -1);

	const CThesaurus* Thes = m_pData->GetThes(GetThesIdByRossId(InNode.GetType()));
	for (auto i : Thes->QueryTopConcepts(TerminId))
	{
		std::string conceptStr = Thes->GetConceptStrById(i);
		Trim(conceptStr);
		if (   !is_null(GetRossHolder(Ross)->GetItemNoByItemStr(conceptStr, "D_SF", false))
			|| !is_null(GetRossHolder(Ross)->GetItemNoByItemStr(conceptStr, "D_SEM_REL", false))
			)
			AddSemFet(OutNode, conceptStr.c_str());
	};
};


bool CSemanticStructure::IsConnected()
{
	if (GetNodesSize() == 0) return true;

	size_t i = 0;
	for (; i < GetNodesSize(); i++)
		GetNodePtr(i)->m_bReached = false;


	for (i = 0; i < GetNodesSize(); i++)
		if (GetNodePtr(i)->IsWordContainer())
			break;

	if (i == GetNodesSize()) return true;

	dfs(i, false);

	for (i = 0; i < GetNodesSize(); i++)
		if (GetNodePtr(i)->IsWordContainer())
			if (!GetNodePtr(i)->m_bReached)
				return false;

	return true;
};



long CSemanticStructure::FindAbstractPlugArticle(DictTypeEnum type, uint64_t Grammems, part_of_speech_mask_t Poses, long ClauseType) const
{

	const std::vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);

	for (long i = 0; i < AbstractArticles.size(); i++)
	{
		const CAbstractArticle& A = AbstractArticles[i];

		for (long k = 0; k < A.m_GramInfos.size(); k++)
			if ((((Poses > 0) && (Poses & A.m_GramInfos[k].m_PartOfSpeechMask) == Poses))
				&& ((A.m_GramInfos[k].m_Grammems == 0)
					|| ((Grammems > 0)
						&& (Grammems & A.m_GramInfos[k].m_Grammems) == A.m_GramInfos[k].m_Grammems
						)
					)
				&& (A.m_Type == atArticlePlug)
				&& (
					(A.m_ClauseTypes.size() == 0)
					|| _find(A.m_ClauseTypes, ClauseType)
					)
				)
				return i;
	};

	return -1;
};




void CSemanticStructure::FindAbstractAdditionArticle(DictTypeEnum type, const CSemNode& Node, std::vector<long>& Articles, bool IsClauseSyntaxRoot, long ClauseType)
{
	Articles.clear();
	const std::vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);
	if (Node.m_MainWordNo == -1) return;
	const CSemWord& W = Node.GetWord(Node.m_MainWordNo);

	for (long i = 0; i < AbstractArticles.size(); i++)
		if (AbstractArticles[i].m_Type == atAdditionArticle)
		{
			const CAbstractArticle& A = AbstractArticles[i];

			// проверка GF
			long k = 0;
			for (; k < A.m_GramInfos.size(); k++)
				if (((W.m_Poses & A.m_GramInfos[k].m_PartOfSpeechMask) > 0)
					&& Node.HasGrammems(A.m_GramInfos[k].m_Grammems)
					)

					break;

			if ((A.m_GramInfos.size() > 0)
				&& (k == A.m_GramInfos.size())
				)
				continue;

			// проверка КЛАУЗЫ
			if (A.m_ClauseTypes.size() > 0)
				if (!_find(A.m_ClauseTypes, ClauseType))
					continue;

			// Проверка CAT
			for (k = 0; k < A.m_SemTypes.size(); k++)
			{
				// если узел был синтаксической вершиной, тогда  считаем, что это LABL.SIT		   
				if (IsClauseSyntaxRoot
					&& (A.m_SemTypes[k] == "LABL.SIT")
					)
					break;

				if (HasSemType(Node, A.m_SemTypes[k]))
					break;
			}

			if ((A.m_SemTypes.size() > 0)
				&& (k == A.m_SemTypes.size())
				)
				continue;


			// Проверка LEX
			for (k = 0; k < A.m_LexFets.size(); k++)
			{
				if (_find(Node.m_HigherConcepts, A.m_LexFets[k]))
					break;
			}

			if ((A.m_LexFets.size() > 0)
				&& (k == A.m_LexFets.size())
				)
				continue;

			// Проверка SF
			for (k = 0; k < A.m_SemFets.size(); k++)
				if (!HasSemFetPro(Node, A.m_SemFets[k].c_str())
					&& !HasSemFetOrLower(Node, A.m_SemFets[k].c_str()) // чтобы +MEDINF добавлялся к тем, которые имеют SET-DEMAN			 
					)
					break;

			if ((A.m_SemFets.size() > 0)
				&& (k < A.m_SemFets.size())
				)
				continue;


			Articles.push_back(i);
		};

};

void CSemanticStructure::AddAbstractAdditionVals(DictTypeEnum type, CSemNode& Node, const std::vector<long>& Articles)
{
	const std::vector<CAbstractArticle>& AbstractArticles = *m_pData->GetAbstractArticles(type);
	for (long i = 0; i < Articles.size(); i++)
	{
		long ArtNo = Articles[i];
		for (long l = 0; l < AbstractArticles[ArtNo].m_Vals.size(); l++)
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

bool CSemanticStructure::HasItem(DictTypeEnum DictTy, uint16_t UnitNo, const std::string& FieldStr, const std::string& ItemStr, const std::string& DomStr, BYTE LeafId, BYTE BracketLeafId)  const
{
	CRossQuery Q(DictTy, UnitNo, FieldStr, ItemStr, DomStr, LeafId, BracketLeafId);
	std::vector<CRossQuery>::const_iterator It = find(Queries.begin(), Queries.end(), Q);
	if (It < Queries.end())
		return It->m_Result;
	Q.m_Result = GetRossHolder(DictTy)->HasItem(UnitNo, FieldStr, ItemStr, DomStr, LeafId, BracketLeafId);
	(const_cast<CSemanticStructure*>(this))->Queries.push_back(Q);
	return Q.m_Result;
};

// проверяет часть речи узла
bool   CSemanticStructure::HasPOS(const CSemNode& N, part_of_speech_t POS) const
{
	return (N.m_MainWordNo != -1) && N.GetWord(N.m_MainWordNo).HasPOS(POS);
};
bool   CSemanticStructure::HasPOS(size_t NodeNo, part_of_speech_t POS) const
{
	return HasPOS(GetNode(NodeNo), POS);
};

bool CSemanticStructure::IsVerbForm(const CSemNode& Node) const
{
	return  (HasPOS(Node, VERB)
		|| HasPOS(Node, ADVERB_PARTICIPLE)
		|| HasPOS(Node, PARTICIPLE)
		|| HasPOS(Node, PARTICIPLE_SHORT)
		|| HasPOS(Node, INFINITIVE)
		);
};

void CSemanticStructure::SetLastSentencePunctuationMark(std::string& str) const
{
	switch (m_SentenceMood) {
	case Interrogative: str += "?"; break;
	case Exclamative: str += "!"; break;
	case Indicative: str += "."; break;
	};
};

const char* GetStrByCategory(SemCategoryEnum t)
{
	switch (t) {
	case scSituation: return "LABL.SIT";
	case scObject: return "LABL.OBJ";
	case scLabel: return "LABL";
	case scRelation: return "RELAT";
	};
	assert(false);
	return "LABL";
};


//==========================================

CRossQuery::CRossQuery(DictTypeEnum  TypeEnum,
	uint16_t UnitNo,
	std::string FieldStr,
	std::string ItemStr,
	std::string DomStr,
	BYTE  LeafId,
	BYTE BracketLeafId
)
{
	m_TypeEnum = TypeEnum;
	m_UnitNo = UnitNo;
	m_FieldStr = FieldStr;
	m_ItemStr = ItemStr;
	m_LeafId = LeafId;
	m_BracketLeafId = BracketLeafId;
	m_DomStr = DomStr;

};
bool CRossQuery::operator == (const CRossQuery& X) const
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



//==============================

void SetSpacesAndRegisterInSentence(std::string& str)
{
	/*
	 При выставлении запятых для вводных слов некоторые запятые могут оказать в самом
	 конце или самом начале предложения. Их нужно удалить.
	*/
	Trim(str);
	// в начале 
	if (str.length() == 0) return;
	if (str[0] == ',') str.erase(0, 1);
	// в конце 
	if (str.length() == 0) return;
	if (str[str.length() - 1] == ',') str.erase(str.length() - 1, 1);


	// если два соседних слова стоят в кавычках, то нужно объединить эти кавычки
	// <Quote> X </Quote> <Quote> Y </Quote> -> <Quote> X Y </Quote>
	for (size_t i = 0; i < str.size(); i++)
	{
		// открывающую кавычку пишм слитно с  текстом
		if (str.substr(i, 8) == "</Quote>")
		{
			long k = i + 8;
			for (; k < str.size(); k++)
				if (str[k] != ' ')
					break;
			if (str.substr(k, 7) == "<Quote>")
				str.erase(i, k + 7 - i);
		};

	};

	// выставлем кавычки 
	for (size_t i = 0; i < str.size(); i++)
	{
		// открывающую кавычку пишм слитно с  текстом
		if (str.substr(i, 7) == "<Quote>")
		{
			for (int k = i + 7; k < str.size();)
				if (str[k] != ' ')
					break;
				else
					str.erase(k, 1);
			str.replace(i, 7, "\"");
		};
		// закрывающую  кавычку пишем слитно с  текстом
		if (str.substr(i, 8) == "</Quote>")
		{
			for (int k = i - 1; k >= 0; i--)
				if (str[k] != ' ')
					break;
				else
					str.erase(k, 1);
			str.replace(i, 8, "\"");
		};
	};


	// проходим по предложению и удаляем пробелы перед знаками препинания
	StringTokenizer tok(str.c_str(), " ");
	str.erase();
	bool last_is_punct = true;
	int tok_count = 0;
	while (tok())
	{
		std::string word = tok.val();
		bool is_punct = (word.size() == 1)
			&& std::iswpunct(word[0]) != 0
			&& !strchr("(){}<>\"", word[0]);
		if (is_punct && last_is_punct) continue;
		last_is_punct = is_punct;
		// последний знак препинания не печатаем
		if (is_punct && !tok.has_next()) continue;
		// если слово, то перед словом должен быть пробел, а если знак 
		// препинания, то его нужно писать слитно с предыдущим словом, поэтому
		// пробел не добавляем.
		if (!is_punct && tok_count) str += " ";
		str += word;
		tok_count++;
	}

// Первая буква предложения всегда должна быть прописной
// (проходим начальные знаки препинания (типа кавычки)
	auto wstr = utf8_to_wstring(str);
	for (size_t k = 0; k < wstr.size(); k++)
		if (!std::iswpunct(wstr[k]))
			if (IsUnicodeAlpha(wstr[k])) {
				wstr[k] = toupper_utf32(wstr[k]);
				break;
			}
	str = wstring_to_utf8(wstr);
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

bool			CSemanticStructure::IsInClause(size_t NodeNo, size_t ClauseNo) const
{
	return GetNode(NodeNo).m_ClauseNo == ClauseNo;
};

bool			CSemanticStructure::HasSemType(const CSemNode& Node, std::string Type) const
{
	return (Node.GetType() != NoneRoss) && HasItem(Node.GetType(), Node.GetUnitNo(), "CAT", Type, "D_CAT", 0, 0);
};

bool			CSemanticStructure::GramFetAgreeWithPoses(CStructDictHolder& Ross, uint16_t UnitNo, const CSemWord& W) const
{
	return (W.m_Poses & GetPosesFromRusArticle(Ross, UnitNo)) > 0;
};
