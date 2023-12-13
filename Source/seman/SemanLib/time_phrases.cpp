#include "stdafx.h"
#include "SemanticRusStructure.h"

CNodeHypot::CNodeHypot(long NodeNo, long LexFetNo, long GramCortegeNo, long PrepNo, long PrepNoInArticle,
	bool bShouldBeNumeral_P) {
	m_NodeNo = NodeNo;
	m_LexFetNo = LexFetNo;
	m_GramCortegeNo = GramCortegeNo;
	m_PrepNo = PrepNo;
	m_PrepNoInArticle = PrepNoInArticle;
	m_bShouldBeNumeral_P = bShouldBeNumeral_P;
};


CNodeHypot::CNodeHypot(long NodeNo, long GramCortegeNo) {
	m_NodeNo = NodeNo;
	m_GramCortegeNo = GramCortegeNo;
	m_LexFetNo = -1;
	m_PrepNo = -1;
	m_PrepNoInArticle = -1;
	m_bShouldBeNumeral_P = false;
};


CNodeHypot::CNodeHypot() {
	m_NodeNo = -1;
	m_GramCortegeNo = -1;
	m_LexFetNo = -1;
	m_PrepNo = -1;
	m_PrepNoInArticle = -1;
	m_bShouldBeNumeral_P = false;
};

//================

CTimeNodeHypot::CTimeNodeHypot(long UnitNo, size_t CountOfHoles, const std::vector <CNodeHypot>& Periods, long LengthInText, long LengthInTitle)
{
	m_UnitNo = UnitNo;
	m_CountOfHoles = CountOfHoles;
	m_Periods = Periods;
	m_LengthInText = LengthInText;
	m_LengthInTitle = LengthInTitle;
	m_CountOfLexFet = 0;
	for (size_t i = 0; i < Periods.size(); i++)
		if (Periods[i].m_LexFetNo != -1)
			m_CountOfLexFet++;
};

long CTimeNodeHypot::GetWeightOfTimeNode() const
{
	// длина в тексте + (число недырок) + (кол-во элементов в заголовке)
	return  m_LengthInText * 1000 + (m_LengthInTitle - (m_CountOfHoles - m_CountOfLexFet)) * 100 + m_LengthInTitle;
};

bool CTimeNodeHypot::operator < (const CTimeNodeHypot& X) const
{
	return GetWeightOfTimeNode() < X.GetWeightOfTimeNode();
};

bool CTimeNodeHypot::operator == (const CTimeNodeHypot& X) const
{
	return GetWeightOfTimeNode() == X.GetWeightOfTimeNode();
};


bool IsMonth(const CSemNode& N)
{
	if (!N.IsPrimitive()) return false;
	const CSemanticsHolder* pData = N.GetWord(0).m_pData;
	return  binary_search(pData->m_RusMonths.begin(), pData->m_RusMonths.end(), N.GetWord(0).m_Lemma);
};

bool IsWeekDay(const CSemNode& N)
{
	if (!N.IsPrimitive()) return false;
	const CSemanticsHolder* pData = N.GetWord(0).m_pData;
	return  binary_search(pData->m_RusWeekDays.begin(), pData->m_RusWeekDays.end(), N.GetWord(0).m_Lemma);
};

bool CRusSemStructure::IsIntervalNode(const CSemNode& N) const
{
	if (IsMonth(N) || IsWeekDay(N)) return true;

	for (long i = 0; i < N.GetInterps().size(); i++)
		if (N.GetInterps()[i].m_DictType == TimeRoss)
			if (GetRossHolder(TimeRoss)->HasCX(N.GetInterps()[i].m_UnitNo, "PERIO", "D_SF"))
				return true;

	return false;
};


bool CRusSemStructure::HasTimeSemFet(const CSemNode& N, std::string SemFet) const
{
	for (auto& i: N.GetInterps())
		if (i.m_DictType == TimeRoss)
			if (GetRossHolder(TimeRoss)->HasCX(i.m_UnitNo, SemFet, "D_SF"))
				return true;

	return false;
};



long GetFullForm(std::vector<CAbbrFunct> TimeAbbrPairs, std::string AbbrForm)
{
	for (long i = 0; i < TimeAbbrPairs.size(); i++)
		if (AbbrForm == TimeAbbrPairs[i].m_AbbrForm)
			return i;
	return -1;
};


long HasAbbrFunct(std::vector<CAbbrFunct> TimeAbbrPairs, std::string FullForm, std::string FunctName)
{
	for (long i = 0; i < TimeAbbrPairs.size(); i++)
		if ((FullForm == TimeAbbrPairs[i].m_FullForm)
			&& (FunctName == TimeAbbrPairs[i].m_FunctName)
			)
			return i;
	return -1;
};


extern bool IsEqualWithPhonetics(const std::string& TextItem, const std::string& FormatItem);

bool CRusSemStructure::IsEqualTimeWord(std::string DictLemma, const CRusSemNode& N, long& AbbrFunctNo) const
{
	if (DictLemma == "#MONTH")	  return IsMonth(N);
	if (DictLemma == "#DAY-OF-WEEK")	  return IsWeekDay(N);
	if (N.m_Words.empty()) return false;
	const CRusSemWord& W = N.m_Words[0];
	MakeUpperUtf8(DictLemma);
	AbbrFunctNo = -1;
	if (DictLemma.back() == '$') DictLemma.pop_back();
	if (   IsEqualWithPhonetics(W.m_Lemma, DictLemma)
		|| (W.GetWord() == DictLemma)
		)
		return true;
	else
		if (W.m_PostPuncts.find('.') != -1)
		{
			AbbrFunctNo = GetFullForm(m_pData->m_TimeAbbrPairs, W.GetWord() + ".");
			if (AbbrFunctNo == -1) return false;
			return
				   IsEqualWithPhonetics(m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm, DictLemma)
				|| (W.GetWord() == m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm);
		};

	return false;
};

bool CRusSemStructure::CheckOneTimeWord(const CTimeUnit& TimeUnit, CNodeHypotVector& Hypots, BYTE PlaceNo, long& AbbrFunctNo) const
{
	long NodeNo = Hypots[PlaceNo].m_NodeNo;
	Hypots[PlaceNo].m_LexFetNo = -1;
	const std::string& Lemma = TimeUnit.m_Places[PlaceNo];
	AbbrFunctNo = -1;

	if (Lemma[0] != '_')
		return IsEqualTimeWord(Lemma, m_Nodes[NodeNo], AbbrFunctNo);

	if (Lemma == "_")
	{
		bool LexRestrIsFound = false;;
		for (long i = 0; i < TimeUnit.m_LexicalFillings.size(); i++)
		{
			BYTE LeafId = TimeUnit.m_LexicalFillings[i].m_LeafId;
			BYTE BracketLeafId = TimeUnit.m_LexicalFillings[i].m_BracketLeafId;
			// заполнение поля LEXi
			if ((LeafId == 0) && (BracketLeafId == PlaceNo + 1))
				for (long j = 0; j < TimeUnit.m_LexicalFillings[i].m_LexFets.size(); j++)
				{
					if (IsEqualTimeWord(TimeUnit.m_LexicalFillings[i].m_LexFets[j].first, m_Nodes[NodeNo], AbbrFunctNo))
					{
						Hypots[PlaceNo].m_LexFetNo = TimeUnit.m_LexicalFillings[i].m_LexFets[j].second - 1;
						return true;
					};
					LexRestrIsFound = true;
				}

			// заполнение поля PREPi(j)
			if ((LeafId == PlaceNo + 1) && (BracketLeafId > 0))
				for (long j = 0; j < TimeUnit.m_LexicalFillings[i].m_Preps.size(); j++)
				{
					if (_find(m_Nodes[NodeNo].GetInterps(), CDictUnitInterp(OborRoss, TimeUnit.m_LexicalFillings[i].m_Preps[j].first)))
					{
						if (BracketLeafId - 1 < Hypots.size())
						{
							Hypots[BracketLeafId - 1].m_PrepNo = TimeUnit.m_LexicalFillings[i].m_Preps[j].first;
							Hypots[BracketLeafId - 1].m_PrepNoInArticle = TimeUnit.m_LexicalFillings[i].m_Preps[j].second - 1;
						};
						return true;
					};
					LexRestrIsFound = true;
				};
		};

		return !LexRestrIsFound;
	};
	return false;
};



bool CRusSemStructure::CheckTimeNumeral(long NodeNo, std::string GramFet) const
{
	assert(startswith(GramFet, "ЦК"));
	
	int Length = -1;

	if (isdigit((unsigned char)GramFet.back()))
		Length = GramFet.back() - '0';
	auto& n = m_Nodes[NodeNo];
	return   ((n.m_SynGroupTypeStr == SIMILAR_NUMERALS_STR)
		|| (n.m_SynGroupTypeStr == NUMERALS_STR)
		|| (n.m_SynGroupTypeStr == C_NUMERALS_STR) // например, "через 2,5 месяца"
		|| (n.IsPrimitive() && isdigit((unsigned char)n.m_Words[0].GetWord()[0]))
		|| (n.IsPrimitive() && HasRichPOS(NodeNo, NUMERAL))
		|| (n.IsPrimitive() && HasRichPOS(NodeNo, NUMERAL_P))
		)
		&& (   (Length == -1)
			|| (n.IsPrimitive() && (n.m_Words[0].GetWord().length() == Length))
			);
};

bool CRusSemStructure::CheckTimeGramFet(CNodeHypot& Hypot, const CTimeUnit& TimeUnit, BYTE PlaceNo, bool IsHole, long Numbers) const
{
	long UnitNo = TimeUnit.m_UnitNo;
	CSemPattern P;
	P.InitSemPattern(GetRossHolder(TimeRoss), UnitNo, 0, PlaceNo + 1);
	P.LoadGramFromDict();

	for (Hypot.m_GramCortegeNo = 0; Hypot.m_GramCortegeNo < P.GetGramCorteges().size(); Hypot.m_GramCortegeNo++)
	{
		const TCortege& C = P.GetGramCorteges()[Hypot.m_GramCortegeNo];
		std::string GramFet = GetRoss(TimeRoss)->WriteToString( C);
		Trim(GramFet);

		if (startswith(GramFet, "ЦК"))
		{
			if (CheckTimeNumeral(Hypot.m_NodeNo, GramFet))
			{
				if (GramFet == "ЦК_порядк")
					Hypot.m_bShouldBeNumeral_P = true;
				return true;
			};
		}
		else
		{
			uint32_t Pose;
			uint64_t  Grammems;
			m_pData->GetCustomGrammems(GramFet, Grammems, Pose);;

			// если это сокращенная форма, то нужно вернуть истину, если согласовано по числу
			if (m_Nodes[Hypot.m_NodeNo].IsPrimitive()
				&& (m_Nodes[Hypot.m_NodeNo].m_Words[0].m_PostPuncts.find(".") != string::npos)
				&& ((Grammems & rAllNumbers) > 0)
				)
				if ((Grammems & Numbers) == 0)
				{
				}
				else
					return true;
			else  // здесь нельзя использовать GramCodes, поскольку Grammems имеют более правильную информацию
				if ((m_Nodes[Hypot.m_NodeNo].m_MainWordNo != -1)
					&& ((m_Nodes[Hypot.m_NodeNo].m_Words[m_Nodes[Hypot.m_NodeNo].m_MainWordNo].GetAllGrammems() & Grammems) == Grammems)
					&& ((m_Nodes[Hypot.m_NodeNo].m_Words[m_Nodes[Hypot.m_NodeNo].m_MainWordNo].m_Poses & Pose) > 0)
					)
					return true;

		};
	};

	return   (P.GetGramCorteges().size() == 0)
		|| (
			(PlaceNo < TimeUnit.m_LexicalFillings.size())
			&& TimeUnit.m_LexicalFillings[PlaceNo].m_Preps.size() > 0
			);
};

bool CRusSemStructure::CheckTimeSemFet(long NodeNo, long UnitNo, BYTE PlaceNo) const
{
	assert(NodeNo != -1);
	assert(NodeNo < m_Nodes.size());

	if (HasItem(TimeRoss, UnitNo, "SF", "TIME", "D_SEM_REL", 0, PlaceNo + 1))			return IsIntervalNode(m_Nodes[NodeNo]);
	return true;
};



bool CRusSemStructure::TimeHypotIsSyntaxAgree(CNodeHypotVector& V, const CTimeUnit& U) const
{
	for (auto& r: U.m_Rels)
	{
		if (r.m_SourceNodeNo >= V.size())
			return false;

		if (r.m_TargetNodeNo >= V.size())
			return false;

		// Отношение ПР_УПР не входит  перечень синтаксических отношений, поэтому мы не проверяем,
		// построено или нет это отношение синтаксисом. Для отношения ПР_УПР мы только проверяем,
		// что ИГ, в которое это отношение входит, стоит в правильном падеже.
		if (r.m_SynRelName == "ПР_УПР")
		{
			long PrepNo = V[r.m_TargetNodeNo].m_PrepNo;
			if (PrepNo == -1)
				return false;
			std::string S = GetRoss(OborRoss)->GetEntryStr(PrepNo);
			long j = S.find("+");
			if (j == string::npos) return false;
			S = S.substr(j + 1, S.length() - j);
			TrimLeft(S);
			dom_item_id_t ItemNo = GetRossHolder(Ross)->GetItemNoByItemStr1(S, GetRossHolder(Ross)->CaseDomNo);
			uint64_t Grammems = GetCaseGrammem(GetRossHolder(Ross), ItemNo);
			uint64_t Grm = m_Nodes[V[r.m_TargetNodeNo].m_NodeNo].GetGrammems();

			for (long j = 0; j < V.size(); j++)
				if (V[j].m_bShouldBeNumeral_P && !m_Nodes[V[j].m_NodeNo].HasPOS(NUMERAL_P))
					return false;

			/*
			 для сокращений проверку на ПР_УПР проводить не нужно
			*/
			if (m_Nodes[V[r.m_TargetNodeNo].m_NodeNo].m_Words[0].m_PostPuncts.find('.') == string::npos)
				if ((Grm & Grammems) != Grammems)
					return false;
		}
		else
		{
			std::vector<long> Rels;

			GetIncomingSynRelations(V[r.m_TargetNodeNo].m_NodeNo, Rels);

			long l = 0;
			for (; l < Rels.size(); l++)
				if ((V[r.m_SourceNodeNo].m_NodeNo == m_SynRelations[Rels[l]].m_SourceNodeNo))
					if (r.m_SynRelName == m_SynRelations[Rels[l]].m_SynRelName)
						break;

			if (l == Rels.size()) return false;
		};

	};

	return true;
};


// инициализация словарных статей TimeRoss
void CRusSemStructure::GetTimeInterps(std::string Lemma, std::string WordStr, std::string PostPuncts, CRusSemNode& N) const
{
	MakeLowerUtf8(Lemma);
	if (PostPuncts.find('.') != string::npos)
	{
		std::string WordUpper = WordStr;
		MakeUpperUtf8(WordUpper);
		long AbbrFunctNo = GetFullForm(m_pData->m_TimeAbbrPairs, WordUpper + ".");
		if (AbbrFunctNo != -1)
		{
			Lemma = m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm;
			MakeLowerUtf8(Lemma);
		};
	};

	long UnitNo = GetRossHolder(TimeRoss)->LocateUnit(Lemma.c_str(), 1);
	if (UnitNo != ErrUnitNo)
		N.AddInterp(CDictUnitInterp(GetRossHolder(TimeRoss), TimeRoss, UnitNo, false, false));
};

std::vector<CTimeNodeHypot> CRusSemStructure::BuildTimeHypots(long node_no) const {
	std::vector<CTimeNodeHypot> hypots;
	auto clause_no = m_Nodes[node_no].m_ClauseNo;
	// идем по всем статьям  словаря групп времени 
	for (const auto& e : m_pData->m_TimeUnits)
	{
		long UnitNo = e.m_UnitNo;
		long PlacesCount = e.m_Places.size();

		/*ыыыыя
		 в таймроссе есть обычные слова, т.е. статьи без поля CONTENT
		*/
		if (PlacesCount == 0) continue;

		if (node_no + PlacesCount > m_Clauses[clause_no].m_EndNodeNo) continue;
		CNodeHypotVector Period;
		Period.resize(PlacesCount);

		//rml_TRACE  ("TimeUnit %s%i\n",GetRoss(TimeRoss)->GetEntryStr(UnitNo).c_str(),GetRoss(TimeRoss)->GetUnitMeanNum(UnitNo));
		long SimilarNumeralLength = 0;

		BYTE PlaceNo = 0;
		for (; PlaceNo < PlacesCount; PlaceNo++)
		{
			long AbbrFunctNo;
			Period[PlaceNo].m_NodeNo = node_no + PlaceNo + SimilarNumeralLength;
			if (Period[PlaceNo].m_NodeNo >= m_Clauses[clause_no].m_EndNodeNo) break;
			if (!CheckOneTimeWord(e, Period, PlaceNo, AbbrFunctNo)) break;
			uint64_t Numbers;
			if (AbbrFunctNo == -1)
				Numbers = rAllNumbers;
			else
				if (m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FunctName == "СОКР_мн")
					Numbers = _QM(rPlural);
				else
					if (HasAbbrFunct(m_pData->m_TimeAbbrPairs, m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm, "СОКР_мн"))
						Numbers = _QM(rSingular);
					else
						Numbers = rAllNumbers;

			if (!CheckTimeGramFet(Period[PlaceNo],
				e,
				PlaceNo,
				(e.m_Places[PlaceNo] == "_") && (Period[PlaceNo].m_LexFetNo == -1),
				Numbers)
				)
				break;


			if (!CheckTimeSemFet(Period[PlaceNo].m_NodeNo, UnitNo, PlaceNo))
				break;

			if (HasSynRelation(Period[PlaceNo].m_NodeNo, "ОДНОР_ЧИСЛ"))
			{
				SimilarNumeralLength += GetOutcomingSynRelationsCount(GetSynHost(Period[PlaceNo].m_NodeNo));
			};


		};

		// берем только те гипотеза, которые собрались до конца и удовлетворяют
	   // синтаксическому представлению
		if ((PlaceNo == PlacesCount)
			&& TimeHypotIsSyntaxAgree(Period, e)
			)
		{
			size_t CountOfHoles = 0;
			for (auto& p : e.m_Places)
				if (p == "_")
					CountOfHoles++;
			long LenInText = m_Nodes[Period.back().m_NodeNo].GetMaxWordNo() - m_Nodes[Period[0].m_NodeNo].GetMinWordNo();
			hypots.push_back(CTimeNodeHypot(UnitNo, CountOfHoles, Period, LenInText, e.m_Places.size()));
		};
	}
	sort(hypots.begin(), hypots.end());
	return hypots;
}

void CRusSemStructure::CreateOneTimeNode(const CTimeNodeHypot& h) {
	long MainWordNo = GetRossHolder(TimeRoss)->GetSemMainWordFromArticle(h.m_UnitNo);
	if (MainWordNo != -1) MainWordNo--;
	if (MainWordNo >= h.m_Periods.size())
		MainWordNo = -1;
	assert(h.m_Periods.size() > 0);
	assert(m_Nodes[h.m_Periods[0].m_NodeNo].m_Words.size() > 0);

	long CollocId = m_Nodes[h.m_Periods[0].m_NodeNo].m_Words[0].m_WordNo;

	for (long PeriodNo = 0; PeriodNo < h.m_Periods.size(); PeriodNo++)
	{
		long nd = h.m_Periods[PeriodNo].m_NodeNo;
		if (HasSynRelation(nd, "ОДНОР_ЧИСЛ"))
			nd = GetSynHost(nd);


		// пока не понимаю, что это такое
		/*if (      m_pData->m_TimeUnits[BestHypot.m_UnitNo].m_Rels.empty() //не работал TimeHypotIsSyntaxAgree
			   && (
					   !BestHypot.m_Periods[PeriodNo].m_bShouldBeNumeral_P
					|| !HasSynRelation(nd, "ЧИСЛ_СУЩ")
				   )
			)
			continue;*/
		auto& c = m_Nodes[nd].m_Colloc;
		c.m_CollocSource = RossType;
		c.GetRossInterp().m_DictType = TimeRoss;
		c.GetRossInterp().m_UnitNo = h.m_UnitNo;
		c.GetRossInterp().m_ItemNo = PeriodNo;
		c.GetRossInterp().m_bMainWord = (PeriodNo == MainWordNo);
		c.GetRossInterp().m_LexFetNo = h.m_Periods[PeriodNo].m_LexFetNo;
		c.GetRossInterp().m_PrepNoInArticle = h.m_Periods[PeriodNo].m_PrepNoInArticle;
		m_Nodes[nd].m_CollocId = CollocId;
		if (h.m_Periods[PeriodNo].m_bShouldBeNumeral_P)
			if (m_Nodes[nd].m_RichPoses == 0)
			{
				m_Nodes[nd].m_RichPoses = (1 << NUMERAL_P);
				m_Nodes[nd].m_Words[0].m_Poses = (1 << NUMERAL_P);
				/*
				 Синтаксис ошибочно считал эти ЦК числительными, а это порядковые числительные.
				 Нужно удалить син. связи, построенные от него как от существительного
				*/
				DeleteSynRelationsByName(nd, "ЧИСЛ_СУЩ");
			};

		if (h.m_Periods[PeriodNo].m_PrepNo != -1)
		{
			m_Nodes[nd].m_SynReal.m_Preps.clear();
			m_Nodes[nd].m_SynReal.m_Preps.push_back(CDictUnitInterp(OborRoss, h.m_Periods[PeriodNo].m_PrepNo));
		};



		if ((MainWordNo != -1) && (PeriodNo != MainWordNo))
		{
			std::vector<long> Rels;
			GetIncomingSynRelations(nd, Rels);
			long l = 0;
			for (; l < Rels.size(); l++)
				if (!IsWeakSynRel(m_SynRelations[Rels[l]].m_SynRelName))
					break;

			if (l == Rels.size())
				m_SynRelations.push_back(CSynRelation(h.m_Periods[MainWordNo].m_NodeNo, nd, "врем_группа"));

		};
	};

}

void CRusSemStructure::BuildTimeNodes(long ClauseNo)
{
	StartTimer("Time groups", 0);

	for (long node_no = m_Clauses[ClauseNo].m_BeginNodeNo; node_no < m_Clauses[ClauseNo].m_EndNodeNo; )
	{
		// здесь будут лежать гипотезы групп времени 
		auto hypots = BuildTimeHypots(node_no);
		if (hypots.empty()) {
			node_no++;
		}
		else
		{
			PrintNodes();
			auto& best = hypots.back();
			LOGV << "create time node " << GetRoss(TimeRoss)->GetEntryStr(m_pData->m_TimeUnits[best.m_UnitNo].m_UnitNo) <<
				" at node " << node_no;
			CreateOneTimeNode(best);
			node_no += best.m_Periods.size();
		};
	};

	EndTimer("Time groups");
};

long CRusSemStructure::FindTimeHost(long NodeNo, long ClauseNo)
{
	long BestNodeNo = -1;
	long MinDistance = 100;

	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (m_Nodes[i].IsWordContainer())
			if (IsInClause(i, ClauseNo))
				if ((m_Nodes[i].GetMaxWordNo() < m_Nodes[NodeNo].GetMinWordNo()
					&& (m_Nodes[i].GetType() == TimeRoss)
					)
					)
				{
					long Dist = GetDistance(i, NodeNo);
					if (Dist < MinDistance)
					{
						BestNodeNo = i;
						MinDistance = Dist;

					};
				};

	return BestNodeNo;
};



/*
	=====================================
	Идеализации для таймгрупп
	======================================
*/





/*
  1. стирает интерпретацию таймгруппы, если таймгруппа заполнила
  не временную валентность и это свободная тайм-группа
*/

void CRusSemStructure::ClearInterpsForFreeTimeGroups()
{
	for (long NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (m_Nodes[NodeNo].IsMainTimeRossNode()
			&& m_pData->m_TimeUnits[m_Nodes[NodeNo].m_Colloc.GetRossInterp().m_UnitNo].m_bCanFillNotTimeValency
			)
		{
			CRelSet R = GetIncomingRelations(NodeNo, false);
			if (R.m_RelsCount != 1) continue;
			if (m_Relations[R.m_Rels[0]].m_Valency.m_RelationStr == "TIME") continue;
			if (m_Nodes[m_Relations[R.m_Rels[0]].m_SourceNodeNo].m_NodeType == MNA)
			{
				R = GetIncomingRelations(m_Relations[R.m_Rels[0]].m_SourceNodeNo, false);
				if (R.m_RelsCount != 1) continue;
				if (m_Relations[R.m_Rels[0]].m_Valency.m_RelationStr == "TIME") continue;
			};
			long CollocId = m_Nodes[NodeNo].m_CollocId;
			for (int i = 0; i < m_Nodes.size(); i++)
				if (m_Nodes[i].m_CollocId == CollocId)
				{
					m_Nodes[i].m_Colloc.GetRossInterp().m_DictType = NoneRoss;
					m_Nodes[i].m_Colloc.m_CollocSource = NoneType;
				};
		}
};

/*
  если от главного  слова тайм-группы зависит узел, который является предлогом,
  тогда, если в главный узел идет отношение, нужно прописать этот предлог в отношение,
  а предложный узел  в любом случае удалить. Нужно это для перевода конструкции
  "не ранее вторника", где предлог "не ранее" должен оказаться на стрелке, поскольку
  его перевод равен "till + A1(НЕ)", т.е. при переводе "не ранее" у его отца
  должно будет появиться отрицание, а сам предлог должен будет перевестись в "till".
*/
long CRusSemStructure::MovePrepNodeToRelationForMainTimeGroups()
{
	long res = 0;
	for (long NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (m_Nodes[NodeNo].IsMainTimeRossNode()
			)
		{
			std::vector<long> Nodes;
			GetOutcomingNodes(NodeNo, Nodes, false);
			long i = 0;
			for (; i < Nodes.size(); i++)
				if ((m_Nodes[Nodes[i]].GetType() == OborRoss)
					&& m_pData->Oborottos[m_Nodes[Nodes[i]].GetUnitNo()].m_bRusOborPrep
					)
					break;

			if (i == Nodes.size()) continue;
			long PrepNodeNo = Nodes[i];
			if (!CanBeDeleted(PrepNodeNo)) continue;

			CRelSet R = GetIncomingRelations(NodeNo, false);
			if (R.m_RelsCount == 1)
			{
				m_Relations[R.m_Rels[0]].m_SynReal.m_Preps.clear();
				m_Relations[R.m_Rels[0]].m_SynReal.m_Preps.push_back(*m_Nodes[PrepNodeNo].GetInterp());
			}
			else
				if (HasSynRelation(NodeNo, "врем_группа"))
				{
					//res +=300; //штраф за удаленный узел и связи, "В конце 2 года"
					//удаляем не состоявшуюся группу и пересчитываем лучший вариант
					DeleteSynRelationsByName(NodeNo, "врем_группа");
					m_Nodes[PrepNodeNo].DelAllInterps();
					for (long ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++)
					{
						CLexVariant& V = m_AlreadyBuiltClauseVariants[m_Clauses[ClauseNo].m_AlreadyBuiltClauseVariantNo].m_BestLexVariants[m_Clauses[ClauseNo].m_CurrLexVariantNo];
						bool bCheckSAMNode = CheckSAMNode(ClauseNo);
						size_t LexVariantInCurrSetCollocNo = 0;
						size_t CurrSetCollocHypNo = 0;
						SetLexVariant(ClauseNo, LexVariantInCurrSetCollocNo, CurrSetCollocHypNo);

						V = BuildTheVariant(ClauseNo);
						//PrintRelations();
						//PrintNodes();
						V.m_BestValue.SetWeight(SAMNodeViolation, !bCheckSAMNode);
						V.m_LexVariantNo = LexVariantInCurrSetCollocNo;
						V.m_SetCollocHypNo = CurrSetCollocHypNo;
						V.m_BestValue.SetWeight(CollocsCount, m_ClauseSetCollocHyps[ClauseNo][V.m_SetCollocHypNo].size());
						V.CopyLexVar(*this);

						
					}
					continue;
				}

			DelNode(PrepNodeNo);

			/*предлог стоял перед главным словом*/
			NodeNo--;
		};
	return res;
}
