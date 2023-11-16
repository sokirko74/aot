
#include "stdafx.h"
#include "SemanticRusStructure.h"

long CRusSemStructure::GetAnaphoricRelationsCount(long Tag)
{
	size_t Result = 0;

	for (size_t i = 0; i < m_DopRelations.size(); i++)
		if (HasTag(m_DopRelations[i].m_SourceNodeNo, Tag))
			if (m_DopRelations[i].m_bRelUse)
				if (m_DopRelations[i].m_SyntacticRelation == "anaphora_relation")
					Result++;

	return Result;
};



bool CRusSemStructure::IsInLexVariant(long ClauseNo, long NodeNo)
{
	return    m_Nodes[NodeNo].IsWordContainer()
		&& IsInClause(NodeNo, ClauseNo)
		;
};


void CRusSemStructure::InitLexVariant(size_t ClauseNo)
{
	for (size_t NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (IsInLexVariant(ClauseNo, NodeNo))
			m_Nodes[NodeNo].ResetCurrInterp();

};

// процедура, которая выбирает все варианты, причем если где-это был выбран вариант Х,  
// то во всех последующих вариантах выбирается этот же Х, если это возможно
static void GetLexVariants(const std::vector<CDictUnitInterpVector>& Parents,
	CDictUnitInterpVector& V,
	std::vector<CDictUnitInterpVector>& Variants,
	long       Position)
{
	if (Variants.size() > 20000) return;
	if (Position < V.size())
	{

		size_t i = 0;
		for (; i < Parents[Position].size(); i++)
		{
			size_t k = 0;
			for (; k < Position; k++)
				if (V[k] == Parents[Position][i])
					break;
			if (k < Position) break;
		};
		if (i == Parents[Position].size())
			for (size_t i = 0; i < Parents[Position].size(); i++)
			{
				V[Position] = Parents[Position][i];
				GetLexVariants(Parents, V, Variants, Position + 1);
			}
		else
		{
			V[Position] = Parents[Position][i];
			GetLexVariants(Parents, V, Variants, Position + 1);
		}
	}
	else
		Variants.push_back(V);

};


bool CRusSemStructure::SetLexVariant(size_t ClauseNo, size_t& LexVariantInCurrSetCollocNo, size_t& CurrSetCollocHypNo)
{
StartPoint:

	CRusSemClause& C = m_Clauses[ClauseNo];
	std::vector<CRusSemNode> SaveNodes = m_Nodes;
	std::vector<CSynRelation> SaveSynRelations = m_SynRelations;
	SaveClausesNodeReferences();




	// строим текущее множество  словосочетаний
	BuildBlackBoxCollocs(ClauseNo, CurrSetCollocHypNo);

	std::vector<CDictUnitInterpVector> Parents;


	for (long NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (IsInLexVariant(ClauseNo, NodeNo))
			if (!m_Nodes[NodeNo].GetInterps().empty())
			{
				Parents.push_back(CDictUnitInterpVector());
				m_Nodes[NodeNo].ResetCurrInterp();
				for (size_t k = 0; k < m_Nodes[NodeNo].GetInterps().size(); k++)
					Parents[Parents.size() - 1].push_back(m_Nodes[NodeNo].GetInterps()[k]);

			}

	// получаем множество всех вариантов интерпретации
	CDictUnitInterpVector V_curr; // текущий вариант 
	V_curr.resize(Parents.size());
	std::vector<CDictUnitInterpVector> Variants; // все возможные варианты
	GetLexVariants(Parents, V_curr, Variants, 0);

	// если текущий вариант последний для текущего C.m_SetCollocHyps[m_CurrSetCollocHypNo],
	// перейти к следующему множеству словосочетаний
	if (LexVariantInCurrSetCollocNo >= Variants.size())
		if (CurrSetCollocHypNo + 1 == m_ClauseSetCollocHyps[ClauseNo].size())
		{
			PopClausesNodeReferences();
			return false;
		}
		else
		{
			CurrSetCollocHypNo++;
			LexVariantInCurrSetCollocNo = 0;
			m_Nodes = SaveNodes;
			m_SynRelations = SaveSynRelations;
			RestoreClausesNodeReferences();
			goto StartPoint;
		}

	PopClausesNodeReferences();


	// приписывание текущей словарной интерпретации
	size_t k = 0;
	CDictUnitInterpVector& V = Variants[LexVariantInCurrSetCollocNo];

	for (long NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (IsInLexVariant(ClauseNo, NodeNo))
			if (!m_Nodes[NodeNo].GetInterps().empty())
			{
				bool b = m_Nodes[NodeNo].SetCurrInterp(V[k]);
				assert(b);
				k++;
			}

	return true;
}

void CRusSemStructure::DeleteSAMNode(long ClauseNo, CRusSemNode& SamNode)
{
	SamNode.m_Words.clear();
	for (long i = 0; i < m_Nodes.size(); i++)
		if (IsInClause(i, ClauseNo))
			if (m_Nodes[i].IsPrimitive() && CanBeDeleted(i))
				if (HasGramFetAfterColon(i, "нар")
					&& HasRichPOS(i, PRONOUN_P)
					)

					if (GetSynRelationsCount(i) == 0)
					{
						SamNode = m_Nodes[i];
						DelNode(i);
						break;
					}
};

/*
  Эта функция проверяет, что  для слов "сам" и "один" выбрана правильная интерпретация.
  Если  интерпретация равна GF = *:нар, тогда она должна  применяться только для случая
  "Он сделал это сам.", когда "сам" не подчинено именной группе.
  Если интерпретация содеджит GF = *:c_опр, тогда она может быть использована только
  для случая "Сам директор сказал это".  Если  интерпретация для "сам" и "один" не соответствуется
  входному син. представлению, тогда данному лексическому варианту приписывается штраф.
*/
bool CRusSemStructure::CheckSAMNode(long ClauseNo) const
{
	for (long i = 0; i < m_Nodes.size(); i++)
		if (IsInClause(i, ClauseNo))
			if (m_Nodes[i].IsPrimitive())
			{
				if (HasGramFetAfterColon(i, "нар")
					&& HasRichPOS(i, PRONOUN_P)
					)
				{
					if (HasIncomingNotWeakSynRelation(i))  return false;
				};

				if (HasGramFetAfterColon(i, "с_опр"))
					if (m_Nodes[i].IsPrimitive())
						if (m_Nodes[i].IsLemma(_R("ОДИН"))
							|| m_Nodes[i].IsLemma(_R("САМ"))
							)
							if (!HasIncomingNotWeakSynRelation(i))
								return false;
			};

	return true;
};


/*
Функция вставляет узел _R("сам") или "один" и проводит отношения ASPECT к нему для всех потециальных подлежащих. Если ни одного отношения не удалось провести,
 тогда возвращаем ложь.
 Между подлежащим и узлом сам проверяется согласование.
*/
bool CRusSemStructure::InsertSAMNode(long ClauseNo, CRusSemNode& SamNode)
{
	/*
	  если ничего вставлять не надо, выйдем с истиной
	*/
	if (SamNode.m_Words.size() == 0) return true;

	long SamNodeNo = InsertNode(SamNode);
	bool bResult = false;

	for (long i = 0; i < m_Nodes.size(); i++)
		if (IsInClause(i, ClauseNo))
		{
			CRelSet R = GetIncomingRelations(i, false);
			for (long l = 0; l < R.m_RelsCount; l++)
				if (m_Relations[R.m_Rels[l]].m_SyntacticRelation == "подл")
					// проверяем согласование по числу/роду
					if ((((_QM(rPlural) | _QM(rSingular))
						& SamNode.GetGrammems()
						& m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems()
						) > 0
						)
						&& (((rAllGenders
							& m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems()
							& SamNode.GetGrammems()) > 0
							)
							|| (
								(rAllGenders
									& m_Nodes[m_Relations[R.m_Rels[l]].m_SourceNodeNo].GetGrammems()) == 0
								)
							)
						)
					{
						AddRelation(CRusSemRelation(CValency("ASPECT", A_C), m_Relations[R.m_Rels[l]].m_SourceNodeNo, SamNodeNo, ""));
						// оно должно быть реверсивным, чтобы не отличаться от простых 
						// наречий, чтобы занять правильное место в англ. предложении.
						m_Relations[m_Relations.size() - 1].m_bReverseRel = true;
						m_DopRelations.push_back(
							CRusSemRelation(
								CValency("THESAME", A_C),
								SamNodeNo,
								m_Relations[R.m_Rels[l]].m_TargetNodeNo,
								"anaphora_relation"));
						bResult = true;
					}
					else
					{
						// удаляем субъектную стрелку, которая не  согласовалась со словом "САМ"
						EraseRelation(R.m_Rels[l], "InsertSAMNode");
						// получаем новый набор выходящих отношений
						R = GetIncomingRelations(i, false);
						l = -1;
					};
		};

	return bResult;
};

long Count = 0;
void CRusSemStructure::ConnectClausesForLexVariantCombination()
{

	m_Relations.clear();
	m_LexFuncts.clear();
	m_ThesSemRelations.clear();
	m_DopRelations.clear();
	m_SynRelations.clear();

	m_ClausePropertiesProtocol = "";
	m_WeightProtocol = "";

	AssertValidGraph();

	for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{


		CSemClauseVariantResult I = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo];

		long CurrLexVariant = m_Clauses[ClauseNo].m_CurrLexVariantNo;
		CLexVariant& W = I.m_BestLexVariants[CurrLexVariant];

		// копируем узлы
		/*
		вообще говоря, в функции построения лексического варианта
		сейчас не строятся неабстрактные узлы (только Copul).
		На это мы опираемся. Если бы они строились, тогда мы имели проблему, что
		два узла имеют один WordNo, потому что сейчас мы просто
		копируем узлы, а если в копируемые узлы был бы вставлен  новый узел,
		тогда в последующей клаузе точно нашелся бы один узел, у которого
		WordNo совпадает с WordNo какого-то узла текущей клаузы.
		*/
		/*
		ClauseNo может отличаться от I.m_BuiltClauseNo, поскольку сохраненная
		клаузы могла быть построена в варианте, где произошло объединение клауз
		*/
		long SaveClauseNo = I.m_BuiltClauseNo;
		m_Nodes.erase(m_Nodes.begin() + m_Clauses[ClauseNo].m_BeginNodeNo, m_Nodes.begin() + m_Clauses[ClauseNo].m_EndNodeNo);

		m_Nodes.insert(m_Nodes.begin() + m_Clauses[ClauseNo].m_BeginNodeNo,
			W.m_Nodes.begin() + W.m_Clauses[SaveClauseNo].m_BeginNodeNo,
			W.m_Nodes.begin() + W.m_Clauses[SaveClauseNo].m_EndNodeNo);


		// копируем отношения
		long SaveStartNode = W.m_Clauses[SaveClauseNo].m_BeginNodeNo;
		long SaveLength = W.m_Clauses[SaveClauseNo].GetNodesCount();
		long StartNode = m_Clauses[ClauseNo].m_BeginNodeNo;
		long Length = m_Clauses[ClauseNo].GetNodesCount();
		long Delta = StartNode - SaveStartNode;

		//W.IncrementNodeRef(W.m_Clauses[SaveClauseNo].m_BeginNodeNo, Delta);
		/*
		добавим Delta ко всем узлам (не только к узлам клаузы и узлам идущим после них)
		Если мы изменим индексы отношений только для этйо клаузы,
		тогжа появится отношения, которое не принадлежит данной клаузы, но его
		могут указыать на узлы данной клаузы
		*/
		W.IncrementNodeRef(0, Delta);
		W.m_Clauses[SaveClauseNo].IncrementNodesReferences(Delta);
		m_Relations.insert(m_Relations.end(), W.m_Relations.begin(), W.m_Relations.end());
		m_DopRelations.insert(m_DopRelations.end(), W.m_DopRelations.begin(), W.m_DopRelations.end());
		m_LexFuncts.insert(m_LexFuncts.end(), W.m_LexFuncts.begin(), W.m_LexFuncts.end());
		m_Clauses[ClauseNo] = W.m_Clauses[SaveClauseNo];
		m_Clauses[ClauseNo].m_CurrLexVariantNo = CurrLexVariant;
		for (long i = 0; i < m_Clauses.size(); i++)
			if (m_Clauses[i].m_BeginNodeNo > m_Clauses[ClauseNo].m_BeginNodeNo)
				m_Clauses[i].IncrementNodesReferences(SaveLength - Length);



		/*
		В W.m_SynRelations слоты m_SourceNodeNo и слоты m_TargetNodeNo
		уже указывают на this->m_Nodes, а не на W.m_Nodes, поскольку
		была вызвана процедура W.IncrementNodeRef, поэтому корректно выражение
		m_Nodes[W.m_SynRelations[j].m_SourceNodeNo] и
		m_Nodes[W.m_SynRelations[j].m_TargetNodeNo]
		*/
		for (long j = 0; j < W.m_SynRelations.size(); j++)
			if ((W.m_SynRelations[j].m_SourceNodeNo < m_Clauses[ClauseNo].m_EndNodeNo)
				&& (W.m_SynRelations[j].m_TargetNodeNo < m_Clauses[ClauseNo].m_EndNodeNo)
				&& (m_Clauses[ClauseNo].m_BeginNodeNo <= W.m_SynRelations[j].m_SourceNodeNo)
				&& (m_Clauses[ClauseNo].m_BeginNodeNo <= W.m_SynRelations[j].m_TargetNodeNo)
				)
				m_SynRelations.push_back(W.m_SynRelations[j]);


		//*    

		/*
		см. выше замечание о слияние клауз и о неравенсте номеров клауз
		*/
		for (long i = m_Clauses[ClauseNo].m_BeginNodeNo; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
			m_Nodes[i].m_ClauseNo = ClauseNo;

		AssertValidGraph();
		//*	 
		std::string Q = Format
		("===== Clause %i ==== \n LexVariantCount = %i\n %s \n Weight = %i, %s \n",
			ClauseNo,
			I.m_BestLexVariants.size(),
			W.m_BestValue.IsPanic() ? "Panic" : "All checked",
			W.GetBestTreeWeight(),
			(W.m_BestValue.GetSingleWeight(ProjectnessViolation) == 1) ? "not projected" : "projected");
		m_ClausePropertiesProtocol += Q;
		if (m_Clauses[ClauseNo].m_BestPO != "")
			m_ClausePropertiesProtocol += Format("DOMAIN = %s\n", m_Clauses[ClauseNo].m_BestPO.c_str());


		Q = Format("\n === Clause %i === \n", ClauseNo);
		m_WeightProtocol += Q;
		m_WeightProtocol += W.m_BestValue.GetStrOfNotNull();
		//*
	};



	// пробуем связать все предложение 
	SetNodeToDeleteFalse();
	SetRelsToDeleteFalse();

	ProcessIsolatedParticipleAnd();

	CLexVariant InitialLexVariant(*this);

	// устаналиваем обычный режим построения межклаузных связей
	m_ClauseConnectionType = ProminentClauseRules;

	// три попытки соединить клаузы
	// 1 попытка
	ApplyClauseRules(1);

	// 2 попытка
	if (GetClauseRelationCount() != m_Clauses.size() - 1)
	{
		CopyLexVar(InitialLexVariant);
		ApplyClauseRules(m_Clauses.size());
	};

	// 3 попытка
	if (GetClauseRelationCount() != m_Clauses.size() - 1)
	{
		CopyLexVar(InitialLexVariant);
		m_ClauseConnectionType = AllPossibleClauseRules;
		ApplyClauseRules(m_Clauses.size());

	};


	/*
	нужно удалить отношения и узлы, которые помечены к удалению
	в функции ApplyClauseRules
	*/
	DelRelsToDelete("from ConnectClausesForLexVariantCombination");
	DelNodesToDelete();
	AssertValidGraph();


}



CLexVariant CRusSemStructure::BuildTheVariant(long ClauseNo)
{
	try
	{
		CLexVariant V;

		V.m_BestValue.SetCoefs(&m_SemCoefs);

		m_Relations.clear();

		// узел, содежащий лемму "САМ",удаленный на время построения дерева
		CRusSemNode SamNode;
		DeleteSAMNode(ClauseNo, SamNode);

		bool HypotGraphResult = BuildHypotRelationsGraph(ClauseNo);

		AssertValidGraph();

		PrintRelations();


		if (m_PanicTreeVariantCount > 0)
		{
			V.AllRelationsCount = m_Relations.size();
			V.TreeVariantCount = GetTreeByConnectedComponents(ClauseNo, V.m_BestValue);
		}

		if (!HypotGraphResult)
		{
			V.m_BestValue.SetPanic();
		};


		/*
			если  не удалось вставить узел "сам", значит мы получили еще один разрыв
		*/
		if (!InsertSAMNode(ClauseNo, SamNode))
			V.m_BestValue.SetWeight(ConnectedComponentsCount, V.m_BestValue.GetSingleWeight(ConnectedComponentsCount) + 1);

		V.m_BestValue.SetWeight(LexFunctsCount, CountLexFunctsInOneClause(ClauseNo));
		V.m_BestValue.SetWeight(GramRestrViolationsCount, 0);
		return V;
	}
	catch (...)
	{
		ErrorMessage("BuildTheVariant Failed");
		throw;
	};
};



//восстанавливаем леммы НИКТО, НИЧТО, НИСКОЛЬКО, НИКАКОЙ,
// если они были записаны разрывно, типа "ни о чем"

void UnitDisruptedConjunctions(CRusSemStructure& R, long ClauseNo)
{
	for (long i = R.m_Clauses[ClauseNo].m_BeginNodeNo; i < R.m_Clauses[ClauseNo].m_EndNodeNo; i++)
		if (R.m_Nodes[i].IsLemmaList({ _R("КТО"),_R("ЧТО"),_R("СКОЛЬКО"),_R("КАКОЙ") }))
		{
			//"НИ" может стоять контактно или быть разделено предлогом 
			if ((i > 0)
				&& R.m_Nodes[i - 1].IsWordForm(_R("НИ"))
				&& R.CanBeDeleted(i - 1)
				)
				R.DelNode(i - 1);
			else
				if ((i > 1)
					&& R.m_Nodes[i - 2].IsWordForm(_R("НИ"))
					&& R.CanBeDeleted(i - 2)
					)
					R.DelNode(i - 2);
				else
					continue;
			// только что удалили узел
			i--;


			CRusSemWord& W = R.m_Nodes[i].m_Words[0];
			W.m_Lemma = _R("НИ") + W.m_Lemma;
			W.m_ParadigmId = R.m_pData->GetFirstParadigmId(morphRussian, W.m_Lemma, 0);
			R.m_Nodes[i].DelAllInterps();

			std::string UnitStr = W.m_Lemma;
			EngRusMakeLower(UnitStr);
			uint16_t UnitNo = R.GetRossHolder(Ross)->LocateUnit(UnitStr.c_str(), 1);
			if (UnitNo != ErrUnitNo)
				for (long j = UnitNo; UnitStr == R.GetRoss(Ross)->GetEntryStr(j); j++)
					if (R.GramFetAgreeWithPoses(*R.GetRossHolder(Ross), j, W))
						R.m_Nodes[i].AddInterp(CDictUnitInterp(R.GetRossHolder(Ross), Ross, j, false, false));

		};


};

// проходит по макисмальным узлам, подсоединяя частицу "не", "только" к первому слева узлу.
void CRusSemStructure::InitRelationOperators(long ClauseNo)
{
	UnitDisruptedConjunctions(*this, ClauseNo);
	//if (m_Clauses[ClauseNo].GetNodesCount() == 0)
	//	return

	SetNodeToDeleteFalse();

	// префиксные операторы
	for (long i = m_Clauses[ClauseNo].m_BeginNodeNo; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
	{
		// префиксных операторов может быть много
		StringVector PrefixRelOperators;
		long k = i;
		for (; (k < m_Clauses[ClauseNo].m_EndNodeNo) && CanBePrefixRelationOperator(k); k++)
			if (!m_Nodes[k].IsTimeRossNode()
				&& (m_Nodes[k].m_MNAType != RepeatMNA)
				&& CanBeDeleted(k)
				// двойной союз "ни ..., ни" не является оператором, хотя одиночный "ни" - оператор
				)
			{
				m_Nodes[k].m_bToDelete = true;
				PrefixRelOperators.push_back(m_Nodes[k].m_Words[0].m_Lemma);
			};

		// добавляем префиксные операторы, если  они были найдены, и после них был  найден
		// хотя бы одно слово, не явл. преф. оператором ( к-й узел)
		if (k < m_Clauses[ClauseNo].m_EndNodeNo)
		{
			for (long l = 0; l < PrefixRelOperators.size(); l++)
				m_Nodes[k].m_RelOperators.push_back(PrefixRelOperators[l]);
		}
		else
		{

			for (long j = i; j < k; j++)
				m_Nodes[j].m_bToDelete = false;


		}

		i = k;
	};

	// постфиксные операторы
	long MainNodeNo = 0; // главное слово - к которому  будет добавляться операторы
	for (long i = m_Clauses[ClauseNo].m_BeginNodeNo + 1; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
		if (CanBePostfixRelationOperator(i))
		{
			m_Nodes[MainNodeNo].m_RelOperators.push_back(m_Nodes[i].m_Words[0].m_Lemma);
			m_Nodes[i].m_bToDelete = true;
		}
		else
			MainNodeNo = i;

	DelNodesToDelete();
};


static void GetIndexedVariants(const std::vector<CLexVariantWeightVector>& Parents,
	VectorLong& V,
	std::vector<VectorLong>& Variants,
	long       Position)
{
	if (Variants.size() > 10000) return;
	if (Position < V.size())
		for (long i = 0; i < Parents[Position].size(); i++)
		{
			V[Position] = i;
			GetIndexedVariants(Parents, V, Variants, Position + 1);
		}
	else
		Variants.push_back(V);

};

// returns the weight of the best tree variant

long CRusSemStructure::FindSituationsForClauseVariantCombination()
{
	const long constMinStructureWeight = 1000000;
	m_bHasConjBut = false;
	StartTimer("Syntax interpretation", 0);
	try {
		BuildSemNodesBySyntax();
	}
	catch (...)
	{
		ErrorMessage("Ошибка в интерпретации синтаксиса");
		throw;
	};


	if (m_Nodes.size() == 0) return constMinStructureWeight;

#ifdef _DEBUG
	PrintLemNodes();
	AssertValidGraph();
#endif
	EndTimer("Syntax interpretation");

	LOGV << "=================================================";
	LOGV << "===  FindSituationsForClauseVariantCombination " <<  m_ClauseVariantsCombinationNo;

	ProcessParticipleAndInTheFirstClause();

	m_ClauseSetCollocHyps.clear();
	m_ClauseSetCollocHyps.resize(m_Clauses.size());

	// создаем архивные места для морф. вариантов клауз
	for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo = FindAlreadyBuiltClauseVariant(ClauseNo);
		if (m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo == -1)
		{

			m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo = m_AlreadyBuiltClauseVariants.size();
			CSemClauseVariantResult ClauseVar(m_Clauses[ClauseNo].m_SyntaxClauseNo, m_Clauses[ClauseNo].m_ClauseVariantNo, m_Clauses[ClauseNo].m_AddedClauses);
			ClauseVar.m_BuiltClauseNo = ClauseNo;
			m_AlreadyBuiltClauseVariants.push_back(ClauseVar);
		};
	};

	for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
	{
		if (m_bShouldBeStopped) return constMinStructureWeight;
		CSemClauseVariantResult& ClauseVar = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo];
		if (ClauseVar.m_bNew)
		{
			BuildTimeNodes(ClauseNo);
			InterpretRepeatConj(ClauseNo);

			// InitRelationOperators должен стоять до  поиска словосочетаний, но
			// после поиска разрывных союзов, иначе не будет собираться группа типы
			// "не красивую, но новую  маму", где "не" неправильно подсоединится к "красивую"
			InitRelationOperators(ClauseNo);

			InterpretPrepNouns(ClauseNo);

			InterpretSelectiveRelations(ClauseNo);


			ReadDopFieldForClause(ClauseNo);

			ReadThesInterps(ClauseNo);

			FindConceptFetsFromArticles(ClauseNo);


			DealInvitatoryMoodBeforeTree(ClauseNo);

			FindCollocsHyps(ClauseNo);

			InitLexVariant(ClauseNo);

			CLexVariant InitialLexVariant;
			InitialLexVariant.CopyLexVar(*this);

			// здесь начинается инициализация морфологического варината
			ClauseVar.m_bNew = false;
			m_InterfaceClauseNo++;

			ZaplataCompar2(ClauseNo);


			size_t LexVariantInCurrSetCollocNo = 0;
			size_t CurrSetCollocHypNo = 0;


			// цикл по всем лексическим вариантам 
			for (;;)
			{
				if (m_bShouldBeStopped) return constMinStructureWeight;


				if (!SetLexVariant(ClauseNo, LexVariantInCurrSetCollocNo, CurrSetCollocHypNo))
				{
					CopyLexVar(InitialLexVariant);
					break;
				};

				bool bCheckSAMNode = CheckSAMNode(ClauseNo);
				CLexVariant V = BuildTheVariant(ClauseNo);
				PrintRelations();
				PrintNodes();
				ClauseVar.m_NodesCount += m_Clauses[ClauseNo].GetNodesCount();
				ClauseVar.m_PanicRelationsCount += V.AllRelationsCount;
				V.m_BestValue.SetWeight(SAMNodeViolation, !bCheckSAMNode);
				V.m_LexVariantNo = LexVariantInCurrSetCollocNo;
				V.m_SetCollocHypNo = CurrSetCollocHypNo;
				V.m_BestValue.SetWeight(CollocsCount, m_ClauseSetCollocHyps[ClauseNo][CurrSetCollocHypNo].size());
				V.CopyLexVar(*this);


				LOGV << "CLause " << ClauseNo;
				LOGV.printf("Lex variant(ClauseNo = %i) =  %i, Collocation set = %i(of %i)",
					ClauseNo, LexVariantInCurrSetCollocNo + 1,
					CurrSetCollocHypNo + 1, m_ClauseSetCollocHyps[ClauseNo].size());
				LOGV << "Best tree weight " << V.GetBestTreeWeight();
				LOGV << "Variant weight components: %s" << V.m_BestValue.GetStrOfNotNull();
				LOGV <<"Is clause connected: " << IsConnectedClause(ClauseNo);

				ClauseVar.m_BestLexVariants.push_back(V);
				CopyLexVar(InitialLexVariant);
				LexVariantInCurrSetCollocNo++;
				if (LexVariantInCurrSetCollocNo > 100) {
					LOGV << "too many lexical varints, panic exit";
					break;
				}


			} // конец цикла

		 // уничтожение плохих вариантов
			sort(ClauseVar.m_BestLexVariants.begin(), ClauseVar.m_BestLexVariants.end());
			for (long i = 0; i < ClauseVar.m_BestLexVariants.size(); )
				if (ClauseVar.m_BestLexVariants[i].GetBestTreeWeight() > ClauseVar.m_BestLexVariants[0].GetBestTreeWeight())
					ClauseVar.m_BestLexVariants.erase(ClauseVar.m_BestLexVariants.begin() + i);
				else
					i++;

		}



	}

	long BestCombNo = -1;
	std::vector<VectorLong> Variants;
	LOGV << "================================================";
	LOGV << "===  Connecting clauses  " << m_ClauseVariantsCombinationNo;
	m_InterfaceClauseNo++;

	// построение связного варианта
	VectorLong V;

	V.resize(m_Clauses.size());
	std::vector<CLexVariantWeightVector> Parents;
	for (long i = 0; i < m_Clauses.size(); i++)
	{
		long k = m_Clauses[i].m_AlreadyBuiltClauseVariantNo;
		CLexVariantWeightVector V;
		for (long j = 0; j < m_AlreadyBuiltClauseVariants[k].m_BestLexVariants.size(); j++)
			V.push_back(m_AlreadyBuiltClauseVariants[k].m_BestLexVariants[j]);
		Parents.push_back(V);
	};
	GetIndexedVariants(Parents, V, Variants, 0);


	long MinWeight = constMinStructureWeight;

	CLexVariant InitialLexVariant;
	InitialLexVariant.CopyLexVar(*this);

	for (long i = 0; i < Variants.size(); i++)
	{
		if (m_bShouldBeStopped) return constMinStructureWeight;

		for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
		{
			m_Clauses[ClauseNo].m_CurrLexVariantNo = Variants[i][ClauseNo];
		};


		ConnectClausesForLexVariantCombination();

		long Weight = GetStructureWeight();
		if (Weight < MinWeight)
		{
			MinWeight = Weight;
			BestCombNo = i;
		};

		CopyLexVar(InitialLexVariant);
	};


	AssertValidGraph();


	LOGV << "=================================================";
	LOGV << "===  Writing the best variant N " << m_ClauseVariantsCombinationNo;

	if (BestCombNo != -1)
	{
		for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
			m_Clauses[ClauseNo].m_CurrLexVariantNo = Variants[BestCombNo][ClauseNo];

		ConnectClausesForLexVariantCombination();

		BuildAnaphoricRels();

		auto w1 = GetStructureWeight();
		long w2 = Idealize();
		LOGV << "clause combination weight = " << w1 << "(main weight) + " << w2 << "(idealize weight)";
		return w1 + w2;


	}
	else
		return constMinStructureWeight;

};


long CRusSemStructure::GetStructureWeight()
{
	bool IsConn = IsConnected();

	long Weight = 0;

	for (size_t i = 0; i < m_Clauses.size(); i++)
	{
		/*
			когда дублируются морф. варианты, может возникнуть ситуация, когда
			m_Clauses[i].m_AlreadyBuiltClauseVariantNo не проинициализирован, тогда
			такой вариант надо пропускать (дать очень большой штраф)
		*/
		if (m_Clauses[i].m_AlreadyBuiltClauseVariantNo == -1)
		{
			return  100000;
		};

		if (m_AlreadyBuiltClauseVariants[m_Clauses[i].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants.empty())
			Weight += 10000;
		else
		{
			CTreeOfLexVariantWeight W = m_AlreadyBuiltClauseVariants[m_Clauses[i].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants[m_Clauses[i].m_CurrLexVariantNo];
			auto w = W.GetBestTreeWeight1(!IsConn, true);
			LOGV << "clause no " << i << " weight1 = " <<  w;
			LOGV << W.m_BestValue.GetStrOfNotNull();
			Weight += w;
		};
	};

	long Tag = SetTagToSentence();
	long SemanticVolume = GetSemanticVolume(Tag);
	long RelsCount = GetUseRelationsCount();
	TreeVariantValue Summa;
	Summa.SetCoefs(&m_SemCoefs);
	// перевычисляем те оценки, которые относятся к отношениям ( 5 оценок )
	Summa.SetWeight(SemFetDisagree, RelsCount ? GetSemFetDisagreeCount(Tag) * 1000 / RelsCount : 0);
	Summa.SetWeight(LexFetAgreeCount, RelsCount ? GetLexFetAgreeCount(Tag) * 1000 / RelsCount : 0);
	Summa.SetWeight(OptionalValencyPenalty, GetOptionalValenciesCount(Tag));
	Summa.SetWeight(InstrAgentRelsCount, SemanticVolume ? GetInstrAgentRelsCount(Tag) * 1000 / SemanticVolume : 0);
	Summa.SetWeight(CorporaGleiche, GetCorporaGleicheCount(Tag));
	if (Summa.GetSingleWeight(CorporaGleiche) == 1)
		Summa.SetWeight(CorporaGleiche, 0);

	// вообще не будем учитывать оценку по длине связей на межклаузном уровне 
	// точнее, ее надо учесть, но вычислять расстояние в клаузах, а не в словах. 
	// Пока просто не будем учитывать.
	Summa.SetWeight(RelationsLength, 0);
	Summa.SetWeight(ObligatoryValencyViolation, GetObligatoryValencyViolation(Tag));

	// нужно сделать, поскольку под лексические функции подводятся, например, разрывные союзы.
	Summa.SetWeight(LexFunctsCount, m_LexFuncts.size());
	UpdateBlockedRelations();
	if (!IsConn)
		if (RelsCount == 0)
			Summa.SetWeight(RelationsLength, 0);
		else
			Summa.SetWeight(RelationsLength, SemanticVolume ? GetRelationsLength(Tag) * 1000 / RelsCount : 0);

	auto w = Summa.GetTreeWeight();
	LOGV << "clause component weights:";
	LOGV << Summa.GetStrOfNotNull();
	LOGV << "add weight by components for cross clause relations " << w << " to the main weight(" << Weight << ")";
	Weight += w;

	w = GetAnaphoricRelationsCount(Tag) * 5;
	if (w != 0) {
		Weight -= w;
		LOGV << "decrease clause weight by anaphoric rels: -" << w;
	}

	/*
	 если были использованы недостоверные межклаузные  связи, тогда
	 нужно дать штраф этой структуре
	*/
	if (AllPossibleClauseRules == m_ClauseConnectionType)
		Weight += 60;

	return Weight;
};

struct CClauseComplexity
{
	long      m_ClauseNo;
	float     m_Complexity;
	CClauseComplexity(long      ClauseNo, float     Complexity)
	{
		m_ClauseNo = ClauseNo;
		m_Complexity = Complexity;
	};
	std::string GetStr()
	{
		return Format("Clause=%i, Complexity=%.2f",
			m_ClauseNo, m_Complexity);
	};
};

std::string CRusSemStructure::GetClauseComplexitiesStr()
{
	std::vector<CClauseComplexity> ComplexClauses;
	for (long j = 0; j < m_AlreadyBuiltClauseVariants.size(); j++)
	{

		long i = 0;
		for (; i < ComplexClauses.size(); i++)
			if (ComplexClauses[i].m_ClauseNo == m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo)
				break;

		if (i == ComplexClauses.size())
		{
			double Value = 0;
			long  Count = 0;
			for (long i = 0; i < m_AlreadyBuiltClauseVariants.size(); i++)
				if (m_AlreadyBuiltClauseVariants[i].m_SyntaxClauseNo == m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo)
				{
					if (m_AlreadyBuiltClauseVariants[i].m_NodesCount > 0)
						Value += (double)m_AlreadyBuiltClauseVariants[i].m_PanicRelationsCount / (double)m_AlreadyBuiltClauseVariants[i].m_NodesCount;
					Count++;
				};

			if (Count > 0)
				ComplexClauses.push_back(CClauseComplexity(m_AlreadyBuiltClauseVariants[j].m_SyntaxClauseNo, Value / Count));
		};

	};
	std::string Result;
	for (long j = 0; j < ComplexClauses.size(); j++)
		Result += ComplexClauses[j].GetStr() + std::string("\n");
	return Result;

};




bool  CRusSemStructure::ReadAuxiliaryArticles()
{
	if (GetRoss(Ross) == NULL) return false;

	uint16_t UnitNo = GetRossHolder(Ross)->LocateUnit("_semantic_weight_components", 1);
	if (UnitNo == ErrUnitNo) {
		throw CExpc("cannot find entry _semantic_weight_components in Ross");
	}

	if (!GetRoss(Ross)->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss(Ross)->GetUnitStartPos(UnitNo); i <= GetRoss(Ross)->GetUnitLastPos(UnitNo); i++)
		{
			TCortege C = GetRossHolder(Ross)->GetCortegeCopy(i);
			std::string s = GetRoss(Ross)->WriteToString(C);
			m_SemCoefs.ReadOneCoef(s);
		};
	LOGV << "Semantic coefs: " <<  m_SemCoefs.GetCoefsString();
	UnitNo = GetRossHolder(Ross)->LocateUnit("_weak_syn_rel", 1);
	if (UnitNo == ErrUnitNo) return false;
	m_WeakSynRels.clear();

	if (!GetRoss(Ross)->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss(Ross)->GetUnitStartPos(UnitNo); i <= GetRoss(Ross)->GetUnitLastPos(UnitNo); i++)
		{
			dom_item_id_t ItemNo = GetRossHolder(Ross)->GetCortegeCopy(i).GetItem(0);
			if (!is_null(ItemNo))
			{
				std::string OneSynRel = GetRossHolder(Ross)->GetDomItemStrWrapper(ItemNo);
				m_WeakSynRels.push_back(OneSynRel);
			};
		};

	sort(m_WeakSynRels.begin(), m_WeakSynRels.end());

	return true;

};

bool CRusSemStructure::GetClauseVariantCombination()
{
	std::vector<VectorLong> Parents;
	Parents.resize(m_piSent->GetPrimitiveClausesCount());
	m_AllClausesVariants = 0;
	for (long i = 0; i < Parents.size(); i++)
	{
		for (long k = 0; k < m_piSent->GetPrimitiveClause(i)->m_SynVariants.size(); k++)
		{
			Parents[i].push_back(k);
			m_AllClausesVariants++;
		}
	};
	VectorLong V; // текущий вариант 
	V.resize(Parents.size());
	std::vector<VectorLong> Variants;
	GetCommonVariants(Parents, V, Variants, 0);



	//GetClauseVariantCombinations(Variants);

	if (m_ClauseVariantsCombinationNo >= Variants.size()) return false;
	m_Clauses.clear();
	m_Clauses.resize(m_piSent->GetPrimitiveClausesCount());
	for (long i = 0; i < m_Clauses.size(); i++)
	{
		const CClause* pClause = m_piSent->GetPrimitiveClause(i);
		m_Clauses[i].m_ClauseVariantNo = Variants[m_ClauseVariantsCombinationNo][i];
		CSVI pSynVar = pClause->GetSynVariantByNo(m_Clauses[i].m_ClauseVariantNo);

		int RootNo = pSynVar->m_ClauseTypeNo;
		if (RootNo != -1)
		{
			m_Clauses[i].m_ClauseType = pClause->m_vectorTypes[RootNo].m_Type;
			m_Clauses[i].m_RootWordNo = pClause->m_vectorTypes[RootNo].m_Root.m_WordNo;
		}
		else
		{
			m_Clauses[i].m_ClauseType = UnknownSyntaxElement;
			m_Clauses[i].m_RootWordNo = -1;
		}
		m_Clauses[i].m_SyntaxClauseNo = i;

		m_Clauses[i].m_SubjectWordNo = -1;
		if (!pSynVar->m_Subjects.empty())
			m_Clauses[i].m_SubjectWordNo = pSynVar->m_Subjects[0];
		m_Clauses[i].m_PredicateWordNo = pSynVar->m_iPredk;

	};

	LOGV << "ClauseVariantsCombinationNo " << m_ClauseVariantsCombinationNo + 1 << " (out of " << Variants.size() << ")";
	m_ClauseCombinationVariantsCount = Variants.size();
	m_Nodes.clear();
	return true;
};



long CRusSemStructure::FindSituations(size_t SentNo)
{
	try {
		m_AllClausesVariants = 0;
		ClearTimers();
		StartTimer("All time", 0);

		if (!ReadAuxiliaryArticles())
		{
			ErrorMessage("Cannot read ross auxiliary articles");
			return  -1;
		};

		Queries.clear();
		m_MemoryCaseNo = 0;
		m_MemRelations.clear();
		m_MemNodes.clear();

		StartTimer("Syntax interpretation", 0);

		m_AlreadyBuiltClauseVariants.clear();

		m_IndexedSemFets.clear();
		m_IndexedSemFets.push_back("NEG");
		m_IndexedSemFets.push_back("CAUS");
		m_IndexedSemFets.push_back("FIN");
		m_IndexedSemFets.push_back("SOC");

		m_bLastTry = false;
		m_piSent = m_pData->GetSynan()->m_vectorSents[SentNo];
		m_ClauseCombinationVariantsCount = 0;
		m_bShouldBeStopped = false;





		m_pData->InitializeIndices();
		m_InterfaceClauseNo = 0;


		EndTimer("Syntax interpretation");
		long BestClauseVariantsCombinationNo = 0;
		long BestClauseVariantsCombinationWeight = 100000;

		bool  bTooSlow = false;
		double WordsPerSecond = 0;
		size_t ProcessedVariantsCount = 0;
		if (m_UserClauseVariantsCombinationNo != -1)
		{
			BestClauseVariantsCombinationNo = m_UserClauseVariantsCombinationNo;
			m_ClauseVariantsCombinationNo = -1;
		}
		else
			for (m_ClauseVariantsCombinationNo = 0; GetClauseVariantCombination(); m_ClauseVariantsCombinationNo++)
			{
				ProcessedVariantsCount++;
				m_ClausePropertiesProtocol = "";
				if (m_bShouldBeStopped) return -1;
				StartTimer("FindSituationsForClauseVariantCombination", 0);
				long Weight = FindSituationsForClauseVariantCombination();

				if (m_bShouldBeStopped) return -1;


				LOGV <<"VariantWeght " << m_ClauseVariantsCombinationNo << " = " << Weight;

				if (Weight < BestClauseVariantsCombinationWeight)
				{
					BestClauseVariantsCombinationNo = m_ClauseVariantsCombinationNo;
					BestClauseVariantsCombinationWeight = Weight;
				};

				double AllTicks = EndTimer("FindSituationsForClauseVariantCombination");
				WordsPerSecond = (AllTicks == 0) ? 0 : (double)(m_Nodes.size()) / (AllTicks / CLOCKS_PER_SEC);

#ifndef _DEBUG

				/*
				В релизной  версии, если  скорость меньше, чем полсекунды на слово,
				тогда надо выходить.
				*/
				if ((WordsPerSecond < 0.5) && (WordsPerSecond > 0))
				{
					bTooSlow = true;
					break;
				};

				if (ProcessedVariantsCount > 100)
				{
					bTooSlow = true;
					break;
				};

#endif

			};


		if (m_ClauseVariantsCombinationNo - 1 == BestClauseVariantsCombinationNo)
			m_ClauseVariantsCombinationNo = BestClauseVariantsCombinationNo;
		else
		{
			m_ClauseVariantsCombinationNo = BestClauseVariantsCombinationNo;
			GetClauseVariantCombination();

			m_bLastTry = true;
			BestClauseVariantsCombinationWeight = FindSituationsForClauseVariantCombination();

			AssertValidGraph();

		};

		ConvertParticipleTreeToClause();

		std::string S = Format("Sentence length: %i\n Dictionary request count: %i\n Best clause : %i (out of  %i)\n",
			m_piSent->m_Words.size(), Queries.size(), BestClauseVariantsCombinationNo + 1, m_ClauseCombinationVariantsCount);

		EndTimer("All time");



		m_TimeStatictics = std::string(GetStrRepresentation().c_str()) + S;

		m_TimeStatictics += GetClauseComplexitiesStr() + std::string("\n");

		S = Format("==========\n Sentence weight: %i\n", BestClauseVariantsCombinationWeight);
		m_ClausePropertiesProtocol += S;
		S = Format("UserProhibitedLexVars.size : %i\n", m_UserProhibitedLexVars.size());
		m_ClausePropertiesProtocol += S;
		S = Format("WordsPerSecond =  %10.0f\n", WordsPerSecond);
		if (bTooSlow)
		{
			S += "Выход по скорости !!!!\n";
		};

		m_ClausePropertiesProtocol += S;





		m_ClauseVariantsStatistics = "";
		for (long j = 0; j < m_AlreadyBuiltClauseVariants.size(); j++)
			m_ClauseVariantsStatistics += m_AlreadyBuiltClauseVariants[j].GetStr() + std::string("\n");

		return  BestClauseVariantsCombinationWeight;
	}
	catch (...)
	{
		ErrorMessage("long CRusSemStructure::FindSituations(size_t) Failed");
		throw;
	};
};



bool CRusSemStructure::GetSyntaxTreeByText(std::string utf8text, int ClauseVarNo, std::string& Graph)
{
	try
	{
		CMyTimeSpanHolder			GlobalSpan;
		m_pData->MakeSyntaxStr(utf8text.c_str(), GlobalSpan);
		if (m_pData->GetSynan()->m_vectorSents.empty())
			return false;
		m_piSent = m_pData->GetSynan()->m_vectorSents[0];
		if (ClauseVarNo != -1)
			m_ClauseVariantsCombinationNo = ClauseVarNo;
		else
			m_ClauseVariantsCombinationNo = 0;
		GetClauseVariantCombination();
		m_pData->InitializeIndices();
		BuildSemNodesBySyntax();
		m_TimeStatictics = Format("Sentence length: %i\n ClauseCombinationVariantsCount: %i\n",
			m_piSent->m_Words.size(), m_ClauseCombinationVariantsCount);

		Graph = GetTclSyntaxGraph();

		return true;
	}
	catch (...)
	{
		return false;
	};
}

bool CRusSemStructure::SetLemmasToReplace(std::string LemmasToReplace)
{
	m_SynthLemmaToReplace.clear();

	RmlMakeUpper(LemmasToReplace, morphRussian);
	StringTokenizer tok(LemmasToReplace.c_str(), ";");
	while (tok())
	{
		char lemma1[255];
		char lemma2[255];
		std::string OnePair = tok.val();
		if (sscanf(OnePair.c_str(), "%[^/]/%[^/]", lemma1, lemma2) != 2)
			return false;
		std::string lem1 = lemma1;
		Trim(lem1);
		if (lem1.empty()) return false;
		std::string lem2 = lemma2;
		Trim(lem2);
		if (lem2.empty()) return false;
		m_SynthLemmaToReplace[lem1] = lem2;
	};
	return true;
};



