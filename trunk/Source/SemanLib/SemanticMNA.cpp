#include "stdafx.h"
#include "SemanticRusStructure.h"

const int  MaxMNANodesCount = 100;

// узел является  ПГ или наречием
bool CRusSemStructure::IsAdverbial (long NodeNo) const
{
	return m_Nodes[NodeNo].HasSomePrep() || HasRichPOS (NodeNo, ADV);
};


long GetMNAViolationsCountForChemOborot(const CRusSemStructure& R, long NodeNo, const long* SubordNodes, long SubordNodesCount)
{
	long Result = 0;;
	long IncomRels[100];
	long IncomRelsCount =	0;
	for (long	l=0; l < R.m_Nodes[NodeNo].m_InRels.size(); l++)
	{
		long RelNo = R.m_Nodes[NodeNo].m_InRels[l];
		if (!R.m_Relations[RelNo].m_bRelUse) continue;
		IncomRels[IncomRelsCount++]	= RelNo;
		if (IncomRelsCount == 99) break;
	};



	if (SubordNodesCount !=	2) 
	{
		// рассмотрим	случаи,	когда сравнительная	степень
		// может управлять узлами, число которых не равно	2 

		// Первый дизъюнкт "я буду умнее"
		if	(	(		(R.m_Clauses[R.m_Nodes[NodeNo].m_ClauseNo].m_ClauseType	== COMPARATIVE_T)
					&& !R.m_Clauses[R.m_Nodes[NodeNo].m_ClauseNo].m_bHasCHEM							
					&& (SubordNodesCount == 1)
					&& IncomRelsCount  == 0	
				) 
			// Второй дизъюнкт, пример: "я мечтал	побыстрее его женить"
			//							 "У меня нет больше надежды."
			||
				(		(SubordNodesCount	== 0)
					&&	!R.m_Clauses[R.m_Nodes[NodeNo].m_ClauseNo].m_bHasCHEM
					&&	(IncomRelsCount	 ==	1)
					&&	(			( R.m_Relations[IncomRels[0]].m_SyntacticRelation	== "чистое_обстоятельство")
							||		( R.m_Relations[IncomRels[0]].m_SyntacticRelation	== "НАР_ЧИСЛ_СУЩ")
						)
				)
			)
			;
		else
			Result +=20;
	};

	if (R.m_Clauses[R.m_Nodes[NodeNo].m_ClauseNo].m_bHasCHEM)
		if (SubordNodesCount == 2)	
		{
			if (R.IsAdverbial(SubordNodes[0]) !=	R.IsAdverbial(SubordNodes[1])) 
				Result +=20;
		};

	// во	фразе типа "я буду больше, чем ты",	нужно проверить,
	// что узел,который втыкается	в "больше" имеет граммему имен.	падежа
	if (IncomRelsCount ==	1)
		if (R.m_Clauses[R.m_Nodes[NodeNo].m_ClauseNo].m_ClauseType ==	COMPARATIVE_T)
		{
			long SubjNodeNo = R.m_Relations[IncomRels[0]].m_SourceNodeNo;
			if ( !R.m_Nodes[SubjNodeNo].HasOneGrammem(rNominativ) )
				Result +=	20;
		};

	// последний член CHEMOborot доожен	стоять после  сравнительной	степени
	// таким образолм, "ты меди	дороже"	- ошибка
	if	(SubordNodesCount	> 0)
		if	(SubordNodes[SubordNodesCount -	1] < NodeNo)
			Result	+= 20;

	/*
	только ограниченный класс	сравнительных наречий может	
	быть связна с	модельными словами,	например
	Например,	"я	хотел побыстрее	его	женить", здесь 
	"побыстрее" нельзя связать с модальным глалом	"хотеть"
	*/
	if (IncomRelsCount == 1)
		if	(		R.HasSemFetPro(R.m_Nodes[R.m_Relations[IncomRels[0]].m_SourceNodeNo].m_NodeSemFets, "MODL")
				||	R.m_Nodes[R.m_Relations[IncomRels[0]].m_SourceNodeNo].IsLemma("БЫТЬ")
			)
			if ( !R.m_Nodes[NodeNo].IsLemmaList ("МНОГО",	"МАЛО",	"ОСТРЫЙ", "СКОРЫЙ",	"СИЛЬНЫЙ", "РАННИЙ", "ОХОТНЫЙ",	 "") )
				Result += 20;
	/*
	Примеры на	модальные  глаголы со сравнительными степенями:
	Я больше/меньше/сильнее	хочу этого,	чем	ты
	Я острее нуждаюсь в	этом, чем ты
	Ты скорее мечтаешь обо мне,	чем	о нем
	Он разрешил	тебе это раньше	меня.
	Я охотнее разрешу ему уехать, чем уйти.
	он был больше учителем,	чем	сторожем.

	*/
	if	(IncomRelsCount	== 1)
		if ( R.m_Relations[IncomRels[0]].m_SyntacticRelation ==	"чистое_обстоятельство")
			if (SubordNodesCount > 0) 
				Result +=	20;


	// если	один членов	сравнительного оборота - тайм-группа, то другой	также 
	// должен быть тайм-группой
	if	(SubordNodesCount	== 2) 
		if	( R.IsTimeAdverbial(SubordNodes[0])	!= R.IsTimeAdverbial(SubordNodes[1]))
			Result	+=20;

	/*
	Алексей, нужно,	чтобы в	Сравнительных конструкциях "его" не	могло бы
	заполнять субъектную валентность. Например:	Побыстрее его повесить.	-
	Здесь "его"	не субъект.
	Олег.
	*/
	if	(SubordNodesCount	== 2) 
		if	(  (   R.m_Nodes[SubordNodes[0]].IsPrimitive() 
				&& (R.m_Nodes[SubordNodes[0]].m_Words[0].m_Word	== "ЕГО")
				)
			||	(		R.m_Nodes[SubordNodes[1]].IsPrimitive() 
					&&	(R.m_Nodes[SubordNodes[1]].m_Words[0].m_Word	== "ЕГО")
				)
			)
			if (	(IncomRelsCount == 1) 
					&& R.IsRusSubj(IncomRels[0])
				)
			Result +=30;

	return Result;

};

// используется при нахождении лучшего дерева клаузы.
// проверяет, что число членов MUA больше или равно двум,
// так же проверяет, что между двумя членами MUA  стоит запятая или союз.

long CRusSemStructure::GetMNAViolationsCount(long Tag)
{
	long Result =	0;
	for (size_t NodeNo=0;	NodeNo <m_Nodes.size();	NodeNo++)
		if	(		HasTag (NodeNo,	Tag)
				&&	(m_Nodes[NodeNo].m_NodeType ==  MNA)
			)
		{
			long SubordNodes[MaxMNANodesCount];
			long SubordNodesCount	= 0;
			bool HasCompAdj	= false;
			bool HasNotCompAdj = false;
			for	(long l=0; l < m_Nodes[NodeNo].m_OutRels.size(); l++)
			{
				long RelNo = m_Nodes[NodeNo].m_OutRels[l];

				if (!m_Relations[RelNo].m_bRelUse) continue;

				if (m_Relations[RelNo].m_Valency.m_RelationStr.empty() )
				{
					SubordNodes[SubordNodesCount++]	= m_Relations[RelNo].m_TargetNodeNo;
					assert (SubordNodesCount < MaxMNANodesCount);
					if (m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_bCompAdj)
						HasCompAdj = true;
					else
						HasNotCompAdj =	true;
				};
			};
			/*
				в однородном ряде не могут	быть одновременно сравнительные	прил.  и что-то
				от	них	отличное
			*/
			if (HasCompAdj)
				if (HasNotCompAdj)
					Result+=60;

			if (m_Nodes[NodeNo].m_MNAType == CHEMOborot) 
			{ 
				Result += GetMNAViolationsCountForChemOborot(*this, NodeNo, SubordNodes, SubordNodesCount);
				continue;	
			}; 

			if (SubordNodesCount	< 2)
			{ 
				Result ++;
				continue;
			};


			if (m_Nodes[NodeNo].m_MNAType  == RepeatMNA)  continue;


			const CRusSemClause& C = m_Clauses[m_Nodes[NodeNo].m_ClauseNo];

			// если	это	простой	однородный ряд,	то проверими наличие запятых между 
			// составляющими однородного ряда, кроме последнего
			// (последний разделитель должен быть союз)
			for	(long k=0; k <SubordNodesCount-2;	k++)
				if  (!(C.GetGramMatrix(SubordNodes[k],SubordNodes[k+1]) & HaveCommaBetween))	
					Result++;

		};

	return Result;
};

// используется при нахождении лучшего дерева клаузы.
// выдает число операторов однородностей (MUA), у которых все члены имеют хотя бы одну// общую  SF
long CRusSemStructure::GetSemFetAgreeMNACount(long Tag)
{
  long Result = 0;
  for (size_t i=0; i <m_Nodes.size(); i++)
  if (HasTag (i, Tag))
   if (m_Nodes[i].m_NodeType ==  MNA)
	if (m_Nodes[i].m_MNAType == SimpleMNA)
	{
			  long Nodes[MaxMNANodesCount];
			  long NodesCount = 0;
			  for (long l=0; l < m_Nodes[i].m_OutRels.size(); l++)
			  {
				long RelNo = m_Nodes[i].m_OutRels[l];
				if (!m_Relations[RelNo].m_bRelUse) continue;
				if (!m_Relations[RelNo].m_Valency.m_RelationStr.empty() ) continue;
				Nodes[NodesCount++] = m_Relations[RelNo].m_TargetNodeNo;
			  };

			bool  NeighboursAreGleicheSemFet = true;
			for (long k=1; k< NodesCount; k++)
		      NeighboursAreGleicheSemFet =  NeighboursAreGleicheSemFet 
		             && GleicheSemFet(m_Nodes[Nodes[k]].m_NodeSemFets, m_Nodes[Nodes[k-1]].m_NodeSemFets, false);
			if (NeighboursAreGleicheSemFet)
				Result ++;
	};

   return Result;
};


// создает узел с MUA по синтаксической структуре// Петр и Василий
void CRusSemStructure::InterpretSimilarNounGroups (long ClauseNo)
{
	for (size_t i = 0;  i<m_SynRelations.size(); i++)
		if (  (     ( m_SynRelations[i].m_SynRelName == "ОДНОР_ИГ" ) 
				||	( m_SynRelations[i].m_SynRelName == "ОДНОР_ИНФ" ) 
				||	( m_SynRelations[i].m_SynRelName == "ОДНОР_ЧИСЛ" ) 
				||  ( m_SynRelations[i].m_SynRelName == "РАЗРЫВ_СОЮЗ" ) 
				||	( m_SynRelations[i].m_SynRelName == "ОДНОР" ) 
			)
			&& IsInClause(m_SynRelations[i].m_SourceNodeNo,  ClauseNo)
			)
		{
			long MNANodeNo = m_SynRelations[i].m_SourceNodeNo;
			// не устанавливаем тип SimpleMNA для разрыв. союзов, которые были уже 
			// обработаны в CRusSemStructure::InterpretRepeatConj
			if (m_Nodes[MNANodeNo].m_NodeType != MNA)
			{
				m_Nodes[MNANodeNo].m_NodeType = MNA;
				m_Nodes[MNANodeNo].m_MNAType = SimpleMNA;
			};


			{
				long Target = m_SynRelations[i].m_TargetNodeNo;
				AddRelation(CRusSemRelation (CValency(), MNANodeNo, Target, string("") ));
				m_Relations.back().m_CannotHaveOnlyCommaBetween = ( m_SynRelations[i].m_SynRelName != "РАЗРЫВ_СОЮЗ");
				
			}

		};
};

// создает узел с MUA по сочинительному союзу и предложной группе
// или по сочинительному союзу  и беспредложной группе,  если этого не сделал синтаксис

void CRusSemStructure::ConvertFreeCoordConjToMNA (long ClauseNo)
{
	try {
		for (long i=m_Clauses[ClauseNo].m_BeginNodeNo+1; i < m_Clauses[ClauseNo].m_EndNodeNo - 1; i++)
			if  (    IsCoordConj (i) 
				&& (m_Nodes[i].m_NodeType != MNA)
				)
			{
				long NodeNo = GetDefiniteRightHostByNotWeakSynRelation(i+1);
				if (NodeNo == -1) continue;
				if (   !HasRichPOS (NodeNo , NOUN) 
					&& !HasRichPOS (NodeNo , ADV)
					&& !HasRichPOS (NodeNo , INFINITIVE)
					&& !m_Nodes[NodeNo].m_bCompAdj
					) 
					continue;

				m_Nodes[i].m_NodeType = MNA;
				m_Nodes[i].m_MNAType = SimpleMNA;
				AddRelation(CRusSemRelation(CValency(), i, NodeNo, ""));
				m_Relations[m_Relations.size() - 1].m_CannotHaveOnlyCommaBetween = true;
			};
	}
	catch (...)
	{
		ErrorMessage ("ConvertFreeCoordConjToMNA");
		throw;
	};


};

long RepeatConjCount = 2;
const string RepeatConj[2]  =
{
	"ТО", "ЛИБО"
};

long CRusSemStructure::GetRepeatConj (long NodeNo)
{
    if (m_Nodes[NodeNo].m_Words.size() == 0) return -1;
	for (size_t i=0; i <RepeatConjCount; i++)
	{

		string WordStr = m_Nodes[NodeNo].m_Words[0].m_Word;
	 	if (WordStr == string(RepeatConj[i]))
			return i;
	};
   return -1;
};



void CRusSemStructure::GetMNARelations(long NodeNo, vector<long>& Rels, bool UseUse) const 
{
 	GetOutcomingRelations(NodeNo, Rels, UseUse);

	//  удаляем все отношения, которые имеют названия
	for (size_t i=0; i < Rels.size(); i++)
		if (!m_Relations[Rels[i]].m_Valency.m_RelationStr.empty())
			{
				Rels.erase(Rels.begin() +i );
				i--;
			};
};

void CRusSemStructure::GetMNAMembers(long NodeNo, vector<long>& Nodes, bool UseUse) const 
{
	vector <long> Rels;
	GetMNARelations(NodeNo, Rels, UseUse);

	Nodes.clear();
	for (size_t i=0; i < Rels.size(); i++)
		Nodes.push_back(m_Relations[Rels[i]].m_TargetNodeNo);

};


long CRusSemStructure::GetFirstMNAMemberIfHas(long NodeNo) const 
{
    
	vector <long> Nodes;

	GetMNAMembers (NodeNo, Nodes, false);

	sort (Nodes.begin(), Nodes.end(), IsLessByMinWordNo(this));

    if (Nodes.size() == 0) return NodeNo;

	return Nodes[0];
};


// предлог "c"
bool CRusSemStructure::DealWithNodePrepS(long NodeNo)
{
	long  nd = NodeNo;
	if (m_Nodes[NodeNo].m_NodeType == MNA)
		nd = GetFirstMNAMemberIfHas(NodeNo);

	long PrepNo;
	if (!CheckGroupBeginAndCase("с+Т",nd, PrepNo)) return false;
	long SourceNodeNo = FindLeftClosestNode(nd);
	if (SourceNodeNo == -1) return false;
	if (!IsInClause (SourceNodeNo, m_Nodes[nd].m_ClauseNo)) return false;
	if (!HasRichPOS (SourceNodeNo, NOUN) && !HasPOS(SourceNodeNo, PRONOUN) ) return false;
	CValency V ("TOGETHER", A_C,0);	
	assert(PrepNo != -1);
    AddRelation(CRusSemRelation(V, SourceNodeNo, NodeNo, ""));
	m_Relations[m_Relations.size() - 1].m_SynReal.m_Preps.push_back(m_Nodes[NodeNo].m_SynReal.m_Preps[PrepNo]);
    m_Nodes[SourceNodeNo].m_Vals.push_back(V);
	return true;
}


// функция, которая дает ограничения на валентность для однородного ряда
// именных групп.
void CRusSemStructure::GetMNAPattern(size_t NodeNo, long& Poses, QWORD& Grammems) const
{
	Grammems = 0;
	Poses = 0;

	long LastNodeNo  = FindRightClosestNode(NodeNo);

	if ( LastNodeNo == -1)  return;

	Grammems = m_Nodes[LastNodeNo].GetGrammems() & rAllCases;

    if ( HasRichPOS  (LastNodeNo, ADV) || m_Nodes[LastNodeNo].HasSomePrep() )
	{
		// нужно, чтобы собрать "раз в году, по вечерам"
		Grammems = 0;
		Poses = (1<<ADV);
	}
    else
	 if ( HasRichPOS (LastNodeNo, INFINITIVE) )
		Poses = (1<<INFINITIVE);
	 else
		if (m_Nodes[LastNodeNo].m_bCompAdj)
		{
		  Poses = (1<<ADJ_FULL);
		  Grammems |= _QM(rComparative); 
		}
		else
		  Poses = (1<<NOUN);

	// нужно, чтобы собрать "раз в году, 9 мая"
	if ( m_Nodes[LastNodeNo].IsTimeRossNode() )
		Poses |= (1<<ADV);
};


void CRusSemStructure::BuildMNA_KAK(long ClauseNo)
{
	try {
		//  Все "как" без запятой перед ним объявляем узлами MUA 
		// Например, "Протокол обрабатывает датаграмму как независимую единицу"
		// например, "Она, как последняя дура, заплакала."	
		for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
			if (    
				m_Nodes[NodeNo].IsPrimitive()
				&& (m_Nodes[NodeNo].m_Words[0].m_Lemma == "КАК") 
				&& (m_Nodes[NodeNo].GetType() == OborRoss) 
				&& (m_Nodes[NodeNo].m_MNAType != RepeatMNA)
				)
			{

				// проходим все прилагательные и другие жестко синтаксические зависимые
				long l=NodeNo+1;
				for (; l < m_Clauses[ClauseNo].m_EndNodeNo; l++)
						if ( !HasIncomingNotWeakSynRelation (l))
							break;

				if (l == m_Clauses[ClauseNo].m_EndNodeNo) continue;
				if (!m_Nodes[l].IsWordContainer()) continue;
				
				m_Nodes[NodeNo].m_NodeType = MNA;
				m_Nodes[NodeNo].m_MNAType = KAK_MNA;
				if (HasCommaBefore (NodeNo))
				{
					/* 
					здесь  нужно перевести интерпретацию на как3, чтобы "как" переводился в "like"
					*/
					m_Nodes[NodeNo].GetInterp()->m_UnitNo = GetRossHolder(OborRoss)->LocateUnit("как",3);
					assert (m_Nodes[NodeNo].GetInterp()->m_UnitNo != ErrUnitNo);
				};


				AddRelation(CRusSemRelation(CValency(), NodeNo, l, ""));
			};
	}
	catch (...)
	{
		ErrorMessage ("BuildMNA_KAK Failed");
		throw;
	};


};

void CRusSemStructure::BuildMNAOutcoming(long ClauseNo)
{
 try {
  

  for (long NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
    if (   IsInClause(NodeNo, ClauseNo)
		&& ( m_Nodes[NodeNo].m_NodeType == MNA ) 
		&& ( m_Nodes[NodeNo].m_MNAType != CHEMOborot)
	   )
	{
		long  PatternPoses;
		QWORD Grammems;
		GetMNAPattern(NodeNo, PatternPoses, Grammems);
		bool bAdjMNA = HasOutcomingSynRelation(NodeNo,"ОДНОР_ПРИЛ");
		
        // мы начинаем поиск левых потомков от оператора однородности 
		// (единственный правый потомок уже был присоединен в процедуре, которая ищет оператор однородности )
	    for (long LeftChildNo=NodeNo; LeftChildNo >= m_Clauses[ClauseNo].m_BeginNodeNo; LeftChildNo--)
		{
			// ищем запятую, союз  или какой другой  оператор однородности
			for (; LeftChildNo >= m_Clauses[ClauseNo].m_BeginNodeNo; LeftChildNo--)
				if (IsFiniteVerb(LeftChildNo))
					  goto NoMoreLeftChilds;
				else
					if (   IsCoordConj(LeftChildNo) 
						|| m_Nodes[LeftChildNo].HaveCommaAfter()
						|| (m_Nodes[LeftChildNo].m_NodeType == MNA)
						)
					break;
			if (LeftChildNo <  m_Clauses[ClauseNo].m_BeginNodeNo) break; // не нашли

			// ищем слово, которое согласовано со словом, которое стоит сразу же  за оператором однородности, по падежу
			// или вообще не имеет граммем (ИЛЕ)
			for (; LeftChildNo >= m_Clauses[ClauseNo].m_BeginNodeNo; LeftChildNo--)
				if (IsFiniteVerb(LeftChildNo))
					goto NoMoreLeftChilds;
				else
				if (   (m_Nodes[LeftChildNo].m_MainWordNo != -1)
					&& (   !m_Nodes[LeftChildNo].IsTimeRossNode() 
						|| m_Nodes[LeftChildNo].IsMainTimeRossNode()
					   )
					&& (    ((m_Nodes[LeftChildNo].m_Words[m_Nodes[LeftChildNo].m_MainWordNo].m_Poses & PatternPoses) > 0)
					    ||  (     ( ( PatternPoses & (1<<ADV) ) > 0)
							  &&  (    m_Nodes[LeftChildNo].HasSomePrep() //  в лесу
								    || m_Nodes[LeftChildNo].IsMainTimeRossNode() // 9 мая	
								  )
							  &&  !HasIncomingNotWeakSynRelation(LeftChildNo)
						    )
					   )
					&& (  ((m_Nodes[LeftChildNo].GetGrammems() & Grammems & rAllCases) > 0)
						|| (Grammems == 0)
						|| (( PatternPoses & (1<<ADV) ) > 0)
						|| ( m_Nodes[LeftChildNo].GetGrammems() & Grammems & _QM(rComparative)) 
						|| ( HasSynRelation (LeftChildNo, "АНАТ_СРАВН") && (Grammems & _QM(rComparative)))
						)
					&& (!bAdjMNA || ( m_Nodes[LeftChildNo].HasPOS(ADJ_FULL) || m_Nodes[LeftChildNo].HasPOS(ADJ_SHORT)) )	
				   )
				break;
			if (LeftChildNo <  m_Clauses[ClauseNo].m_BeginNodeNo) break; 

			if (FindFirstRelation(NodeNo, LeftChildNo) == -1)
			{
				AddRelation(CRusSemRelation(CValency(), NodeNo, LeftChildNo,  ""));
				m_Relations[m_Relations.size() - 1].m_CannotHaveOnlyCommaBetween = true;
			};


		};
		NoMoreLeftChilds:;
	
	};

	}
 catch (...)
 {
	 ErrorMessage ("BuildMNAOutcoming Failed");
	 throw;
 };

};






vector<QWORD> CRusSemStructure::GetSemFetsOfFirstValency(long NodeNo)
{
	//получаем SF1 из РОСС  	 
	
	vector<QWORD> SemFets;
	if (   (m_Nodes[NodeNo].m_Vals.size() > 0)
		&& (m_Nodes[NodeNo].GetType() != NoneRoss)
		)
	{
		CSemPattern P;
		P.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(), m_Nodes[NodeNo].m_Vals[0].m_LeafId, m_Nodes[NodeNo].m_Vals[0].m_BracketLeafId);
		P.LoadSemFromDict();
		SemFets = GetIndexedSemFets(P, true, false);
	};
	return SemFets;
};





void CRusSemStructure::BuildMNAIncoming(long ClauseNo)
{
 UpdateBlockedRelations();
 try {
  for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
    if (m_Nodes[NodeNo].m_NodeType == MNA)
	  for (long i = m_Clauses[ClauseNo].m_BeginNodeNo;  i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
		   if  (i != NodeNo)
			 for (size_t j=0; j < m_Nodes[i].m_OutRels.size();j++)
			 {
				long Count = 0;
				/*
				   считаем кол-во отношений с одинаковым названием (это варианты заполнения одной валентности)
				   идущих из узла i (потенциальный хозяин однородного ряда) в
				   в члены однородного ряда.
				   Норамально, когда все члены однородного ряда подчиняются узлу i.
				   		
				*/
			    for (size_t l=j; l < m_Nodes[i].m_OutRels.size();l++)
				  if(m_Relations[m_Nodes[i].m_OutRels[j]].m_Valency == m_Relations[m_Nodes[i].m_OutRels[l]].m_Valency)
				  {
					  long k=0;
					  for (; k < m_Nodes[NodeNo].m_OutRels.size(); k++)
						if (m_Relations[m_Nodes[NodeNo].m_OutRels[k]].m_TargetNodeNo == m_Relations[m_Nodes[i].m_OutRels[l]].m_TargetNodeNo)
							break;

					  if (k <	m_Nodes[NodeNo].m_OutRels.size())
						Count++;
				  };

				// Для простых операторов однородности нужно, чтобы хотя бы один узел входил  
				// в член однородного ряда,чтобы подчинить весь однородный ряд. Это сделано из-за случая:
				// 'я жил в Америке и России'. Здесь пропущен прдлог 'в' после союза, поэтому
				// 'России'не подсоединится к 'жить'.
				// Для сравнительного оборота такой эллипсис не возможен, например:
				// Я жил в Америке дольше, чем в России.
				// но возможны обороты:
				// Я не знаю человека, больше чем Вася (здесь "Вася" не стоит в рд.  или  вн.) 
				// Я не отдаю тебе охотнее его (здесь "его" стоит в рд.) 
				if (    (   (Count > 0)
					//	&& (m_Nodes[NodeNo].m_MNAType != CHEMOborot)
					   )
				   /*||  (   (Count > 1) // 
						&& (m_Nodes[NodeNo].m_MNAType == CHEMOborot)
					   )*/
				   )
				{
				  // если валентность слова А - непредложная, а оператору однородности приписан предлог
				  // (такой предлог может придти из синтаксиса), тогда считаем, что слово А
				  // не может управлят этим однородным рядом
				   if (m_Relations[m_Nodes[i].m_OutRels[j]].m_Valency.IsFromDict())
				   {
					  const CRossHolder* RossHolder =  m_Relations[m_Nodes[i].m_OutRels[j]].m_Valency.m_RossHolder;
					  const TCortege& C = m_Relations[m_Nodes[i].m_OutRels[j]].m_SynReal.m_Cortege;
					  bool IsPrepCortege =     RossHolder->IsLemGroupBegining(RossHolder->GetSynFet(C)) 
											|| (RossHolder->GetSynRel(C) == RossHolder->AdverbialGrpNo);
					  if (!IsPrepCortege && m_Nodes[NodeNo].HasSomePrep()) continue;

				   };
				    

				  AddRelation(CRusSemRelation(m_Relations[m_Nodes[i].m_OutRels[j]].m_Valency, i, NodeNo,  ""));
		          m_Relations[m_Relations.size() - 1].m_CannotHaveOnlyCommaBetween = true;
				  m_Relations[m_Relations.size() - 1].m_SynReal = m_Relations[m_Nodes[i].m_OutRels[j]].m_SynReal;
				  m_Relations[m_Relations.size() - 1].m_SyntacticRelation = m_Relations[m_Nodes[i].m_OutRels[j]].m_SyntacticRelation;
				  if (m_Nodes[NodeNo].m_MNAType == CHEMOborot)
					  m_Relations[m_Relations.size() - 1].m_SemFets = GetSemFetsOfFirstValency(NodeNo);
				};

		   };


 }
 catch (...)
 {
	 ErrorMessage ("BuildMNAIncoming Failed");
	 throw;
 };
};



void CRusSemStructure::DeleteMNAWithOneChild(long ClauseNo)
{
 try {
  for (long NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
    if (   IsInClause(NodeNo, ClauseNo)
		&& (m_Nodes[NodeNo].m_NodeType == MNA)
	   )
	{
		vector<long> Rels;
		GetOutcomingRelations(NodeNo, Rels, false);
		if (Rels.size() == 1)
			// я буду умнее
		  if (	  (m_Clauses[ClauseNo].m_ClauseType != COMPARATIVE_T) 
			   || (m_Nodes[NodeNo].m_MNAType != CHEMOborot) 
			 )
		  {
		     EraseRelation(Rels[0]);
			 CRelSet R = GetIncomingRelations(NodeNo, false);
			 DeleteRelSet(R);
		  };

	};
 }
 catch (...)
 {
	 ErrorMessage ("DeleteMNAWithIOneChild Failed");
	 throw;
 };
};



