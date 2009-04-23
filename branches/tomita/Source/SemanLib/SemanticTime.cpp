#include "stdafx.h"
#include "SemanticRusStructure.h"



bool IsMonth(const CSemNode& N)
{
	if (!N.IsPrimitive()) return false;
	const CSemanticsHolder* pData = N.GetWord(0).m_pData;
	return  binary_search (pData->m_RusMonths.begin(), pData->m_RusMonths.end(), N.GetWord(0).m_Lemma);
};

bool IsWeekDay(const CSemNode& N)
{
	if (!N.IsPrimitive()) return false;
	const CSemanticsHolder* pData = N.GetWord(0).m_pData;
	return  binary_search (pData->m_RusWeekDays.begin(), pData->m_RusWeekDays.end(), N.GetWord(0).m_Lemma);
};

bool CRusSemStructure::IsIntervalNode(const CSemNode& N) const
{
	if ( IsMonth(N) || IsWeekDay(N)) return true;

	for (long i=0; i < N.GetInterps().size(); i++)
		if ( N.GetInterps()[i].m_DictType == TimeRoss )
				if (    GetRossHolder(TimeRoss)->HasCX (N.GetInterps()[i].m_UnitNo, "PERIO", "D_SF")				)
					return true;

	return false;
};


bool CRusSemStructure::HasTimeSemFet(const CSemNode& N, string SemFet) const
{
	for (long i=0; i < N.GetInterps().size(); i++)
		if ( N.GetInterps()[i].m_DictType == TimeRoss )
			if (GetRossHolder(TimeRoss)->HasCX (N.GetInterps()[i].m_UnitNo, SemFet, "D_SF"))
				return true;

	return false;
};



long GetFullForm (vector<CAbbrFunct> TimeAbbrPairs, string AbbrForm)
{
	for (long i=0; i < TimeAbbrPairs.size(); i++)
		if (AbbrForm == TimeAbbrPairs[i].m_AbbrForm)
			return i;
	return -1;
};


long HasAbbrFunct (vector<CAbbrFunct> TimeAbbrPairs, string FullForm, string FunctName)
{
	for (long i=0; i < TimeAbbrPairs.size(); i++)
		if  (    (FullForm == TimeAbbrPairs[i].m_FullForm)
			  && (FunctName == TimeAbbrPairs[i].m_FunctName)
			 )
			return i;
	return -1;
};



extern bool IsEqualWithPhonetics (const string& TextItem,  const string& FormatItem);

bool CRusSemStructure::IsEqualTimeWord(string DictLemma, CRusSemNode& N, long& AbbrFunctNo)
{
	if (DictLemma == "#MONTH")	  return IsMonth(N);
	if (DictLemma == "#DAY-OF-WEEK")	  return IsWeekDay(N);
	if (N.m_Words.empty()) return false;
	CRusSemWord& W = N.m_Words[0];
	EngRusMakeUpper(DictLemma);
	AbbrFunctNo = -1;
	if (DictLemma[DictLemma.length() -1] == '$') DictLemma.erase(DictLemma.length() -1);
	if  (		IsEqualWithPhonetics(W.m_Lemma, DictLemma)
			|| (W.m_Word == DictLemma)
		)
		return true;
	else
		if (W.m_PostPuncts.find('.') != -1)
		{
			AbbrFunctNo = GetFullForm(m_pData->m_TimeAbbrPairs, W.m_Word+".");
			if (AbbrFunctNo == -1) return false;
			return        
				IsEqualWithPhonetics(m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm, DictLemma)
				|| (W.m_Word == m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm);
		};

	return false;
};

bool CRusSemStructure::CheckOneTimeWord(CTimeUnit& TimeUnit,  CNodeHypotVector& Hypots, BYTE PlaceNo, long& AbbrFunctNo)
{
	long NodeNo = Hypots[PlaceNo].m_NodeNo;
	Hypots[PlaceNo].m_LexFetNo = -1;
	string& Lemma =  TimeUnit.m_Places[PlaceNo];
	AbbrFunctNo = -1;

	if (Lemma[0] != '_')
      return IsEqualTimeWord(Lemma, m_Nodes[NodeNo], AbbrFunctNo);

	if (Lemma == "_")
	{
	  bool LexRestrIsFound = false;;
	  for (long i=0; i < TimeUnit.m_LexicalFillings.size(); i++)
	  {
         BYTE LeafId = TimeUnit.m_LexicalFillings[i].m_LeafId;
		 BYTE BracketLeafId = TimeUnit.m_LexicalFillings[i].m_BracketLeafId;
		 // заполнение поля LEXi
	     if ( (LeafId == 0)  && (BracketLeafId == PlaceNo+1) )
            for  (long j=0; j < TimeUnit.m_LexicalFillings[i].m_LexFets.size(); j++)
			{
 		      if (IsEqualTimeWord(TimeUnit.m_LexicalFillings[i].m_LexFets[j].first, m_Nodes[NodeNo], AbbrFunctNo)) 
			  {
			    Hypots[PlaceNo].m_LexFetNo =  TimeUnit.m_LexicalFillings[i].m_LexFets[j].second - 1;
			    return true;
			  };
			  LexRestrIsFound = true;
			}

		 // заполнение поля PREPi(j)
		 if ( (LeafId == PlaceNo+1)  && (BracketLeafId >  0) )
		   for  (long j=0; j < TimeUnit.m_LexicalFillings[i].m_Preps.size(); j++)
		   {
			   if ( _find(m_Nodes[NodeNo].GetInterps(), CDictUnitInterp(OborRoss, TimeUnit.m_LexicalFillings[i].m_Preps[j].first) ) )
			   {
				  if (BracketLeafId  - 1 < Hypots.size())
				  {
				    Hypots[BracketLeafId -1].m_PrepNo  = TimeUnit.m_LexicalFillings[i].m_Preps[j].first;
				    Hypots[BracketLeafId -1].m_PrepNoInArticle =  TimeUnit.m_LexicalFillings[i].m_Preps[j].second - 1;
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



bool CRusSemStructure::CheckTimeNumeral(long NodeNo, string GramFet) const 
{
	 assert ( GramFet.substr(0, 2) == "ЦК");
	 
	 BYTE Length = 100;

	 if (isdigit ((unsigned char)GramFet[GramFet.length() - 1]))
		 Length = GramFet[GramFet.length() - 1] - '0';

	 return   (		 (m_Nodes[NodeNo].m_SynGroupTypeStr == SIMILAR_NUMERALS)
				  || (m_Nodes[NodeNo].m_SynGroupTypeStr == NUMERALS)
				  || (m_Nodes[NodeNo].m_SynGroupTypeStr == C_NUMERALS) // например, "через 2,5 месяца"
				  || (m_Nodes[NodeNo].IsPrimitive() && isdigit((unsigned char)m_Nodes[NodeNo].m_Words[0].m_Word[0]))
				  || (m_Nodes[NodeNo].IsPrimitive() && HasRichPOS (NodeNo, NUMERAL))
				  || (m_Nodes[NodeNo].IsPrimitive() && HasRichPOS (NodeNo, NUMERAL_P))
				 )
		      && (	 (Length == 100) 
				  || (m_Nodes[NodeNo].IsPrimitive() && (m_Nodes[NodeNo].m_Words[0].m_Word.length() == Length))
				 );
};

bool CRusSemStructure::CheckTimeGramFet(CNodeHypot&  Hypot, CTimeUnit& TimeUnit, BYTE PlaceNo, bool IsHole, long Numbers)
{
	long UnitNo = TimeUnit.m_UnitNo;
	CSemPattern P;
	P.InitSemPattern(GetRossHolder(TimeRoss), UnitNo, 0, PlaceNo+1);
	P.LoadGramFromDict();

	for (Hypot.m_GramCortegeNo=0; Hypot.m_GramCortegeNo< P.m_GramCorteges.size(); Hypot.m_GramCortegeNo++)
	{
		TCortege& C = P.m_GramCorteges[Hypot.m_GramCortegeNo];
		string GramFet = WriteToString(GetRoss(TimeRoss), (char*)(GetRoss(TimeRoss)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
		Trim(GramFet);

		if ( GramFet.substr(0, 2) == "ЦК")   
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
			DWORD Pose;
			QWORD  Grammems;
			m_pData->GetCustomGrammems(GramFet, Grammems,Pose);;

			// если это сокращенная форма, то нужно вернуть истину, если согласовано по числу
			if  (     m_Nodes[Hypot.m_NodeNo].IsPrimitive()
				&& (m_Nodes[Hypot.m_NodeNo].m_Words[0].m_PostPuncts.find(".") != -1)
				&& ( (Grammems & rAllNumbers) > 0)
				)
				if  ( (Grammems  & Numbers) == 0)
				{
				}
				else
					return true;
			else  // здесь нельзя использовать GramCodes, поскольку Grammems имеют более правильную информацию
				if  (    (m_Nodes[Hypot.m_NodeNo].m_MainWordNo != -1) 
					&& ((m_Nodes[Hypot.m_NodeNo].m_Words[m_Nodes[Hypot.m_NodeNo].m_MainWordNo].GetAllGrammems() & Grammems) ==  Grammems)
					&& ((m_Nodes[Hypot.m_NodeNo].m_Words[m_Nodes[Hypot.m_NodeNo].m_MainWordNo].m_Poses & Pose) >  0)
			  )
			  return true;

		};
	};

	return   (P.m_GramCorteges.size() == 0) 
		|| (
		(PlaceNo < TimeUnit.m_LexicalFillings.size())
		&& TimeUnit.m_LexicalFillings[PlaceNo].m_Preps.size() > 0
		);
};

bool CRusSemStructure::CheckTimeSemFet(long NodeNo, long UnitNo, BYTE PlaceNo)
{
	assert (NodeNo != -1);
	assert (NodeNo < m_Nodes.size());

if (HasItem(TimeRoss, UnitNo, "SF","TIME","D_SEM_REL", 0, PlaceNo + 1 ))			return IsIntervalNode(m_Nodes[NodeNo]);
	return true;
};


struct CTimeNodeHypot {
	long m_UnitNo;
	long m_CountOfHoles;
	long m_LengthInText;
	long m_LengthInTitle;
	long m_CountOfLexFet;

	vector <CNodeHypot> m_Periods;
	CTimeNodeHypot (long UnitNo, size_t CountOfHoles, const vector <CNodeHypot>& Periods, long LengthInText, long LengthInTitle)
	{
		m_UnitNo = UnitNo;
		m_CountOfHoles = CountOfHoles;
		m_Periods = Periods;
		m_LengthInText = LengthInText;
		m_LengthInTitle = LengthInTitle;
		m_CountOfLexFet = 0;
		for (size_t i=0; i < Periods.size();i++)
			if (Periods[i].m_LexFetNo != -1)
				m_CountOfLexFet++;
	};

	long GetWeightOfTimeNode() const 
	{ 
      // длина в тексте + (число недырок) + (кол-во элементов в заголовке)
	  return  m_LengthInText*1000 + (m_LengthInTitle - (m_CountOfHoles-m_CountOfLexFet))*100 + m_LengthInTitle;
	};

	bool operator < (const CTimeNodeHypot& X) const 
	{
		return GetWeightOfTimeNode() < X.GetWeightOfTimeNode();
	};

	bool operator == (const CTimeNodeHypot& X) const 
	{
		return GetWeightOfTimeNode() == X.GetWeightOfTimeNode();
	};

};

bool CRusSemStructure::TimeHypotIsSyntaxAgree (CNodeHypotVector& V, CTimeUnit& U)
{
 for (long i=0; i <  U.m_Rels.size(); i++)
 {
  if (U.m_Rels[i].m_SourceNodeNo >= V.size())
	  return false;

  if (U.m_Rels[i].m_TargetNodeNo >= V.size())
	  return false;

  // Отношение ПР_УПР не входит  перечень синтаксических отношений, поэтому мы не проверяем,
  // построено или нет это отношение синтаксисом. Для отношения ПР_УПР мы только проверяем,
  // что ИГ, в которое это отношение входит, стоит в правильном падеже.
  if (U.m_Rels[i].m_SynRelName == "ПР_УПР")
  {
	  long PrepNo = V[U.m_Rels[i].m_TargetNodeNo].m_PrepNo;
	  if (PrepNo == -1)
		  return false;
	  string S = GetRoss(OborRoss)->GetEntryStr(PrepNo);
	  long j = S.find("+");
	  if (j==-1) return false;
	  S = S.substr(j+1, S.length() -  j);
	  TrimLeft(S);
	  long ItemNo = GetRossHolder(Ross)->GetItemNoByItemStr(S.c_str(), "D_CASE");
	  QWORD Grammems = GetCaseGrammem(GetRossHolder(Ross), ItemNo);
	  QWORD Grm =  m_Nodes[V[U.m_Rels[i].m_TargetNodeNo].m_NodeNo].GetGrammems();

	  /*
	   для сокращений проверку на ПР_УПР проводить не нужно
	  */
	  if  (m_Nodes[V[U.m_Rels[i].m_TargetNodeNo].m_NodeNo].m_Words[0].m_PostPuncts.find('.') == -1)
	    if ((Grm & Grammems)  != Grammems)
		   return false;
  }
  else
  {
    vector<long> Rels;

    GetIncomingSynRelations(V[U.m_Rels[i].m_TargetNodeNo].m_NodeNo, Rels);

	long l=0;
    for (; l < Rels.size(); l++)
	  if (     (V[U.m_Rels[i].m_SourceNodeNo].m_NodeNo == m_SynRelations[Rels[l]].m_SourceNodeNo))
  	    if (U.m_Rels[i].m_SynRelName == m_SynRelations[Rels[l]].m_SynRelName)
	      break;

     if (l == Rels.size() ) return false;
  };
  
 };

 return true;
};


// инициализация словарных статей TimeRoss
void CRusSemStructure::GetTimeInterps(string Lemma, string WordStr,  string PostPuncts, CRusSemNode& N) const
{
   EngRusMakeLower(Lemma);
   if (PostPuncts.find('.') != -1)
   {
	  string WordUpper = WordStr;
	  EngRusMakeUpper(WordUpper);
	  long AbbrFunctNo  = GetFullForm(m_pData->m_TimeAbbrPairs, WordUpper+".");
	  if ( AbbrFunctNo != -1) 
	  {
	     Lemma =  m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm;
	     EngRusMakeLower(Lemma);
	  };
   };

   long UnitNo =  GetRossHolder(TimeRoss)->LocateUnit(Lemma.c_str(), 1);
   if (UnitNo != ErrUnitNo)
	   N.AddInterp( CDictUnitInterp(GetRossHolder(TimeRoss),TimeRoss,UnitNo, false,false));
};


void CRusSemStructure::BuildTimeNodes(long ClauseNo)
{
  StartTimer("Построение групп времени",0); 

 //поиск Time-групп
 for (long NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; )
 {
	 // здесь будут лежать гипотезы групп времени 
	 vector<CTimeNodeHypot> Hypots;

	 // идем по всем статьям  словаря групп времени 
	 for (long TimeUnitNo =0; TimeUnitNo < m_pData->m_TimeUnits.size(); TimeUnitNo++)
	 {
		 long UnitNo = m_pData->m_TimeUnits[TimeUnitNo].m_UnitNo;
		 long PlacesCount = m_pData->m_TimeUnits[TimeUnitNo].m_Places.size();

		 /*
		  в таймроссе есть обычные слова, т.е. статьи без поля CONTENT
		 */
		 if (PlacesCount == 0) continue;

		 if (NodeNo + PlacesCount > m_Clauses[ClauseNo].m_EndNodeNo ) continue;
		 CNodeHypotVector Period;
		 Period.resize(PlacesCount);


	 
		 
		 //rml_TRACE  ("TimeUnit %s%i\n",(const char*)GetRoss(TimeRoss)->GetEntryStr(UnitNo),GetRoss(TimeRoss)->GetUnitMeanNum(UnitNo));
	     long SimilarNumeralLength = 0;	
		 
		 BYTE PlaceNo = 0;
		 for (; PlaceNo < PlacesCount; PlaceNo++)
		 {
			long AbbrFunctNo;
			Period[PlaceNo].m_NodeNo = NodeNo+PlaceNo+SimilarNumeralLength;
			if (Period[PlaceNo].m_NodeNo >= m_Clauses[ClauseNo].m_EndNodeNo) break;
            if ( !CheckOneTimeWord(m_pData->m_TimeUnits[TimeUnitNo], Period, PlaceNo, AbbrFunctNo ) ) break;
			QWORD Numbers;
			if (AbbrFunctNo == -1)
				Numbers = rAllNumbers;
			else
				 if (m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FunctName  == "СОКР_мн")
					 Numbers = _QM(rPlural);
				 else
					if (HasAbbrFunct (m_pData->m_TimeAbbrPairs, m_pData->m_TimeAbbrPairs[AbbrFunctNo].m_FullForm, "СОКР_мн"))
						Numbers = _QM(rSingular);
					else
						Numbers = rAllNumbers;

			 if  ( !CheckTimeGramFet(Period[PlaceNo], 
				             m_pData->m_TimeUnits[TimeUnitNo], 
							 PlaceNo, 
							 (m_pData->m_TimeUnits[TimeUnitNo].m_Places[PlaceNo] == "_")  && (Period[PlaceNo].m_LexFetNo == -1),
							 Numbers)
				 )
			 break;
			 

			 if ( !CheckTimeSemFet(Period[PlaceNo].m_NodeNo, UnitNo, PlaceNo) )
				  break;

 			 if (HasSynRelation (Period[PlaceNo].m_NodeNo, "ОДНОР_ЧИСЛ"))
			 {
				 SimilarNumeralLength += GetOutcomingSynRelationsCount(GetSynHost(Period[PlaceNo].m_NodeNo));
			 };


		 };

          // берем только те гипотеза, которые собрались до конца и удовлетворяют
		 // синтаксическому представлению
		 if  (    (PlaceNo == PlacesCount)
			   && TimeHypotIsSyntaxAgree(Period, m_pData->m_TimeUnits[TimeUnitNo])
			  )
		 {	
			   size_t CountOfHoles = 0;
			   for (size_t j=0; j<m_pData->m_TimeUnits[TimeUnitNo].m_Places.size(); j++)
				 if (m_pData->m_TimeUnits[TimeUnitNo].m_Places[j] == "_")
					 CountOfHoles++;
               long LenInText =  m_Nodes[Period[Period.size() - 1].m_NodeNo].GetMaxWordNo() - m_Nodes[Period[0].m_NodeNo].GetMinWordNo();
			   Hypots.push_back(CTimeNodeHypot(UnitNo, CountOfHoles, Period, LenInText, m_pData->m_TimeUnits[TimeUnitNo].m_Places.size()));
		 };
	 }

	 sort(Hypots.begin(), Hypots.end());

	 if (Hypots.size() == 0)
		 NodeNo++;
	 else
	 {    
		 CTimeNodeHypot& BestHypot  = Hypots[Hypots.size()-1];
		 long MainWordNo = GetRossHolder(TimeRoss)->GetSemMainWordFromArticle(BestHypot.m_UnitNo);
		 if (MainWordNo != -1) MainWordNo--;
		 if (MainWordNo >= BestHypot.m_Periods.size())
			 MainWordNo = -1;
		 assert (BestHypot.m_Periods.size() > 0 );
		 assert (m_Nodes[BestHypot.m_Periods[0].m_NodeNo].m_Words.size() > 0);

		 long CollocId  = m_Nodes[BestHypot.m_Periods[0].m_NodeNo].m_Words[0].m_WordNo;

		 for (long PeriodNo=0; PeriodNo < BestHypot.m_Periods.size(); PeriodNo++)
		 {
			 long nd = BestHypot.m_Periods[PeriodNo].m_NodeNo;
			 if (HasSynRelation (nd, "ОДНОР_ЧИСЛ"))
				 nd = GetSynHost(nd);
			 m_Nodes[nd].m_Colloc.m_Type = RossType;
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_DictType = TimeRoss;
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_UnitNo = BestHypot.m_UnitNo;
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_ItemNo = PeriodNo;
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_bMainWord = (PeriodNo == MainWordNo);
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_LexFetNo  = BestHypot.m_Periods[PeriodNo].m_LexFetNo;
			 m_Nodes[nd].m_Colloc.GetRossInterp().m_PrepNoInArticle  = BestHypot.m_Periods[PeriodNo].m_PrepNoInArticle;
			 m_Nodes[nd].m_CollocId = CollocId;
			 if (BestHypot.m_Periods[PeriodNo].m_bShouldBeNumeral_P)
				 if (m_Nodes[nd].m_RichPoses == 0)
				 {
					 m_Nodes[nd].m_RichPoses = (1<<NUMERAL_P);
					 m_Nodes[nd].m_Words[0].m_Poses = (1<<NUMERAL_P);
					 /*
					  Синтаксис ошибочно считал эти ЦК числительными, а это порядковые числительные.
					  Нужно удалить син. связи, построенные от него как от существительного
					 */
					 DeleteSynRelationsByName(nd, "ЧИСЛ_СУЩ");
				 };

				
			 

     		 if (BestHypot.m_Periods[PeriodNo].m_PrepNo != -1)
			   {
				 m_Nodes[nd].m_SynReal.m_Preps.clear();
				 m_Nodes[nd].m_SynReal.m_Preps.push_back(CDictUnitInterp(OborRoss, BestHypot.m_Periods[PeriodNo].m_PrepNo));
			   };


			 
		     if ( (MainWordNo != -1) &&  (PeriodNo != MainWordNo) )
			 {
			  vector<long> Rels;
			  GetIncomingSynRelations(nd, Rels);
			  long l=0;
			  for (; l <Rels.size(); l++)
			   if (!IsWeakSynRel(m_SynRelations[Rels[l]].m_SynRelName))
				   break;

			  if (l == Rels.size())
				  m_SynRelations.push_back(CSynRelation(BestHypot.m_Periods[MainWordNo].m_NodeNo, nd, "врем_группа"));

			 };
		 };
		 NodeNo += BestHypot.m_Periods.size();
	 };
 };

 EndTimer("Построение групп времени");  
};

long CRusSemStructure::FindTimeHost (long NodeNo, long ClauseNo)
{
  long BestNodeNo  = -1;
  long MinDistance = 100;

  for (size_t i=0; i < m_Nodes.size(); i++)
   if (m_Nodes[i].IsWordContainer())
   if (IsInClause(i, ClauseNo))
   if (   (    m_Nodes[i].GetMaxWordNo() < m_Nodes[NodeNo].GetMinWordNo()
	        && (m_Nodes[i].GetType() == TimeRoss ) 
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
	for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
		if (		m_Nodes[NodeNo].IsMainTimeRossNode()
				&&	m_pData->m_TimeUnits[m_Nodes[NodeNo].m_Colloc.GetRossInterp().m_UnitNo].m_bCanFillNotTimeValency
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
			for(int i = 0; i < m_Nodes.size(); i++)
				if(m_Nodes[i].m_CollocId == CollocId)
				{
					m_Nodes[i].m_Colloc.GetRossInterp().m_DictType = NoneRoss;
					m_Nodes[i].m_Colloc.m_Type  = NoneType;
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
void CRusSemStructure::MovePrepNodeToRelationForMainTimeGroups()
{
  for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
   if (   m_Nodes[NodeNo].IsMainTimeRossNode()
	  )
   {
		 vector<long> Nodes;
	     GetOutcomingNodes(NodeNo, Nodes, false);
		 long i=0;
		 for (; i < Nodes.size(); i++)
			 if (    (m_Nodes[Nodes[i]].GetType() == OborRoss)
			     &&  m_pData->Oborottos[m_Nodes[Nodes[i]].GetUnitNo()].m_bRusOborPrep
				)
			 break;

		 if (i == Nodes.size()) continue;
		 long PrepNodeNo = Nodes[i];
		 if (!CanBeDeleted(PrepNodeNo)) continue;

	     CRelSet R = GetIncomingRelations(NodeNo,  false);
		 if (R.m_RelsCount == 1) 
		 {
			 m_Relations[R.m_Rels[0]].m_SynReal.m_Preps.clear();
			 m_Relations[R.m_Rels[0]].m_SynReal.m_Preps.push_back(*m_Nodes[PrepNodeNo].GetInterp());
		 };
		 DelNode(PrepNodeNo);
		 /*предлог стоял перед главным словом*/
		 NodeNo--;
   };
}
