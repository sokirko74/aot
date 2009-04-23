#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"



bool IsThesCorpora (const CRusSemNode& N, DictTypeEnum Dict)
{
   return (   (N.GetType() == Dict) 
	   || (     N.IsThesNode() 
	        &&  (N.m_Colloc.GetThesInterp().m_ItemNo == 0)
			&&  (N.m_Colloc.GetThesInterp().m_ThesaurusId == GetThesIdByRossId(Dict))
	      )
	  );
};


extern void get_all_subsets(vector<int>& the_set, vector<vector<int> >& subsets);

void CalculateValencyNoWithProhibited(CRusSemNode& N)
{
	vector<int> Vals;
	for (long i=0;i < N.m_Vals.size(); i++) Vals.push_back(i);
	N.m_NotProhibitedValSets.clear();
	if (N.ProhibitedPairOfVals.size() == 0)
	{
		N.m_NotProhibitedValSets.push_back(Vals);
		return;
	};


	vector<VectorInt> Subsets;
	get_all_subsets(Vals, Subsets);

	for (int i=0;i < Subsets.size(); i++)
	{
		/*
		   если валентности нет, но ее можно добавить, тогда нужно удалить 
		   этот вариант
		*/
		bool deleted = false;;
		for (long j=0; j < N.m_Vals.size(); j++) 
			if (!_find(Subsets[i], j))
			{
				long k=0;
				for (; k < N.ProhibitedPairOfVals.size(); k++)
					if (  (     (N.ProhibitedPairOfVals[k].first == j)
						    && _find(Subsets[i], N.ProhibitedPairOfVals[k].second)
					  	  )
						||(     (N.ProhibitedPairOfVals[k].second == j)
						    && _find(Subsets[i], N.ProhibitedPairOfVals[k].first)
					  	  )
						)
						  break;
				if ( k == N.ProhibitedPairOfVals.size() )
				{
					deleted = true;
					break;
				};
			};
		/*
		   если есть две несовметных валентности, тогда нужно удалить 
		   этот вариант
		*/
		if (!deleted)
		for (long l=0; l < N.ProhibitedPairOfVals.size(); l++)
			if (  _find(Subsets[i], N.ProhibitedPairOfVals[l].first)
				&&_find(Subsets[i], N.ProhibitedPairOfVals[l].second) 
			   )
			{
					deleted = true;
					break;
			};

		if (deleted)
		{
			Subsets.erase(Subsets.begin() +i);
			i--;
		};
	};
	N.m_NotProhibitedValSets = Subsets;
};



/*
  копируем информацию из словарных статей
*/

template <class T1, class T2>
void CopyInterpNode(T1& To, const T2& From)
{	
	To.m_NodeSemFets = From.m_NodeSemFets;
	To.m_SemCategory = From.m_SemCategory;
	To.m_POs = To.m_POs;
	To.m_ThesAbbrForm = From.m_ThesAbbrForm;
	To.m_bOrgName = From.m_bOrgName;
	To.m_bProper  = From.m_bProper;
	To.m_ConnectedSits  = From.m_ConnectedSits;
	To.m_Vals  = From.m_Vals;
	To.m_bSaveIsParenthesis  = From.m_bSaveIsParenthesis;
	To.m_bConjWord = From.m_bConjWord;
	To.m_LexFunctFields  = From.m_LexFunctFields;
	To.m_ValsLayout  = From.m_ValsLayout;
	To.ProhibitedPairOfVals  = From.ProhibitedPairOfVals;
	To.m_NotProhibitedValSets  = From.m_NotProhibitedValSets;
	To.m_bFullGleiche  = From.m_bFullGleiche;
	To.m_VerbsOfS0  = From.m_VerbsOfS0;
	To.m_MinActantWordNo = From.m_MinActantWordNo;
	To.m_MaxActantWordNo = From.m_MaxActantWordNo;

};

// инициализация валентностей узла из русской структуры
bool CRusSemStructure::InitValsRussian(long NodeNo)
{
	if (m_Nodes[NodeNo].m_MemNodeNo != -1)
	{
		CopyInterpNode(m_Nodes[NodeNo], m_MemNodes[m_Nodes[NodeNo].m_MemNodeNo]);
		return true;
	};

	InitVals(m_Nodes[NodeNo]);

	if (m_Nodes[NodeNo].IsPassiveVerb() )
	{
		if ( m_Nodes[NodeNo].m_Vals.size()>=2 )
		{
			m_Nodes[NodeNo].m_ValsLayout[1] = 0;
			m_Nodes[NodeNo].m_ValsLayout[0] = 1;
		}
		else
		{
			#ifdef _DEBUG
				// у пассивного глагола должно быть хотя бы две валентности
				assert (m_Nodes[NodeNo].m_Vals.size()>=2);
			#endif
		}
	};



	if (m_Nodes[NodeNo].GetType() != NoneRoss) 
	{
		const CRossHolder* RossDoc  =  GetRossHolder(m_Nodes[NodeNo].GetType());
		const CDictionary* pRoss = RossDoc->GetRoss();
		long  UnitNo = m_Nodes[NodeNo].GetUnitNo();


		if (UnitNo != ErrUnitNo) 
			if (!pRoss->IsEmptyArticle(UnitNo))
				for (size_t i = pRoss->GetUnitStartPos(UnitNo); i<= pRoss->GetUnitEndPos(UnitNo); i++)
					if ( pRoss->GetCortegeFieldNo(i) == RossDoc->NESOVMFieldNo )
					{
						TCortege C  = GetCortege(pRoss,i);
						if (C.m_DomItemNos[0] == -1) continue;
						string S = RossDoc->GetDomItemStrInner(C.m_DomItemNos[0]);
						if (S.length() != 2) continue;
						if (S[0] != 'A') continue;
						long ValNo1 = S[1] - '0';
						if (ValNo1 == 0)  continue;
						ValNo1--;
						if (ValNo1 >= m_Nodes[NodeNo].m_Vals.size() ) continue;
						if (C.m_DomItemNos[1] == -1) continue;
						S = RossDoc->GetDomItemStrInner(C.m_DomItemNos[1]);
						if (S.length() != 2) continue;
						if (S[0] != 'A') continue;
						long ValNo2 = S[1] - '0';
						if (ValNo2 == 0)  continue;
						ValNo2--;
						if (ValNo2 >= m_Nodes[NodeNo].m_Vals.size() ) continue;
						m_Nodes[NodeNo].ProhibitedPairOfVals.push_back(PairOfLong(ValNo1, ValNo2));
					};

		// читаем лексические функции из основной статьи
		if (UnitNo != ErrUnitNo)
			GetRossHolder(m_Nodes[NodeNo].GetType())->GetLexFuncts(UnitNo, m_Nodes[NodeNo].m_LexFunctFields, OborRoss, m_pData->GetRossHolder(OborRoss));


	};


	// читаем SF  из  статьи или из корневых концепотв, если узел интерпретирован по тезаурусу 
	


	if (m_Nodes[NodeNo].GetUnitNo() != ErrUnitNo)
	{
		CSemPattern P;
		P.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),0, 0);
		if (P.LoadSemFromDict ())
		{
			// удаляем все LF из значений поля SF, кроме Copul и  ЛФ-прилагательных
			for (int i=0; i < P.m_ActantSemFets.size(); i++)
				for (int k=0; k < P.m_ActantSemFets[i].size(); k++)
					if	(		(		(string(P.m_ActantSemFets[i][k]) != "Copul")
									&&	!IsParameterOfAdjOrAdv(string(P.m_ActantSemFets[i][k]))
									&&	(GetRossHolder(Ross)->GetItemNoByItemStr (P.m_ActantSemFets[i][k].c_str(), "D_LF") != -1)
								)
							||	(P.m_ActantSemFets[i][k] == "REL") // SF=REL только мешает			 
						)

					{
						P.m_ActantSemFets[i].erase( P.m_ActantSemFets[i].begin() + k);
						k--;
					};

			m_Nodes[NodeNo].m_NodeSemFets = GetIndexedSemFets(P, false, false);

		};
	};

	if (m_Nodes[NodeNo].m_NodeSemFets.size() == 0)
		InitThesSemFet(m_Nodes[NodeNo], m_Nodes[NodeNo]);



	// добавляем абстрактные добавочные статьи
	bool IsClauseSyntaxRoot =  GetClauseSyntaxTop(m_Nodes[NodeNo].m_ClauseNo) == NodeNo; 
	vector<long> Articles;
	FindAbstractAdditionArticle (Ross, m_Nodes[NodeNo], Articles, IsClauseSyntaxRoot, m_Clauses[ m_Nodes[NodeNo].m_ClauseNo ].m_ClauseType);
	AddAbstractAdditionVals(Ross, m_Nodes[NodeNo], Articles);



	if ( m_Nodes[NodeNo].GetGrammems() & ( _QM( rSurName) |  _QM( rName) | _QM( rPatronymic)) )
		AddSemFet (m_Nodes[NodeNo], "ANIM");

	if (IsLocNode(NodeNo))
		AddSemFet (m_Nodes[NodeNo], "LOK");
	if (m_Nodes[NodeNo].IsMainTimeRossNode())
		AddSemFet (m_Nodes[NodeNo], "TIME");

	if (m_Nodes[NodeNo].GetType() == TimeRoss)
		if (HasRichPOS(NodeNo, ADV))
			AddSemFet (m_Nodes[NodeNo], "TIME");


	m_Nodes[NodeNo].m_SemCategory = scLabel;

	/*
	в локативном тезаурусе все термины - LABL.OBJ   */
	if (m_Nodes[NodeNo].GetType() == LocRoss)
		m_Nodes[NodeNo].m_SemCategory = scObject;

	for (int  t = scSituation;  t <= scRelation; t++)
		if (HasSemType(m_Nodes[NodeNo], GetStrByCategory((SemCategoryEnum)t)) )
			m_Nodes[NodeNo].m_SemCategory = (SemCategoryEnum)t;

	if ( m_Nodes[NodeNo].m_SemCategory == scSituation )
		AddSemFet (m_Nodes[NodeNo], "SITUAT");


	m_Nodes[NodeNo].m_bFullGleiche =    (HasRichPOS(NodeNo,ADJ_FULL)&& !IsCompAdj(NodeNo))
		|| HasRichPOS(NodeNo,PRONOUN_P)
		|| HasRichPOS(NodeNo,NUMERAL_P)
		|| HasRichPOS(NodeNo,PARTICIPLE);

	m_Nodes[NodeNo].m_bSaveIsParenthesis =    IsParenthesis(NodeNo);

	m_Nodes[NodeNo].m_bConjWord = IsConjWord(NodeNo);



	CalculateValencyNoWithProhibited(m_Nodes[NodeNo]);


	/*
	1. считаем, что первая валентность  всегда обязательна
	для  глаголов и  кратких форм, кроме тех случаев, когда
	она объявлена факультативной (знак  "?"  в словарной статье)
	2. Для предикативов первую валентность  не будем объявлять обязательной, 
	поскольку нормальны фразы
	"Интересно, что ты скажешь"
	"Надо что-то сделать" 
	и т.д.
	где первая валентность не выражена.
	*/
	if (m_Nodes[NodeNo].m_Vals.size() > 0)
		if  (  HasRichPOS (NodeNo, VERB)
			|| HasRichPOS(NodeNo, PARTICIPLE_SHORT) 
			|| HasRichPOS (NodeNo, ADJ_SHORT)
			)
			if (m_Nodes[NodeNo].m_ValsLayout[0] !=  0) // это означает, что есть диатеза
			{
				/*
				Если сработала хотя какая-нибудь диатеза, тогда объявляем, 
				что первая валентность по диатезе - обязательная. 
				Поле INCONS в таком случае будет проигнорировано.
				*/
				if (!m_Nodes[NodeNo].m_Vals[m_Nodes[NodeNo].m_ValsLayout[0]].m_bOptional)
					m_Nodes[NodeNo].m_Vals[m_Nodes[NodeNo].m_ValsLayout[0]].m_bObligatory = true;
			}
			else
			{
				/*
				Если нет диатез (не пассив), тогда объявляем обязательными все первые валентности
				совместных множеств, что означает, что 
				поле INCONS в таком случае будет использоваться.
				*/
				for (long i=0; i < m_Nodes[NodeNo].m_NotProhibitedValSets.size(); i++)
					if (!m_Nodes[NodeNo].m_Vals[m_Nodes[NodeNo].m_NotProhibitedValSets[i][0]].m_bOptional)
					{
						m_Nodes[NodeNo].m_Vals[m_Nodes[NodeNo].m_NotProhibitedValSets[i][0]].m_bObligatory = true;
						m_Nodes[NodeNo].m_Vals[m_Nodes[NodeNo].m_NotProhibitedValSets[i][0]].m_bObligatoryBecauseFirst = true;
					};

			};

	FindVerbsOfS0(NodeNo);	

	FindConnectedSituations(NodeNo);

	if(    HasSemFetPro (m_Nodes[NodeNo],"ORG")&& HasSemFetPro (m_Nodes[NodeNo],"IDENT")	 )
		m_Nodes[NodeNo].m_bOrgName = true;

	if (HasSemFetPro (m_Nodes[NodeNo],"IDENT"))	   m_Nodes[NodeNo].m_bProper = true;

	if (m_Nodes[NodeNo].IsILENode())
		m_Nodes[NodeNo].m_bProper = true;

	/*
	удаляем SF IDENT из всех кортежей. Поскольку это SF обозначает лишь то, что	слово является именем собственным.
	Например, SF (Microsoft) = IDENT, ORGSF (Сокирко) = IDENT, ANIMПо существу IDENT ничего не обозначает. Для того чтоыбы пометить имена собственные	есть слот m_bProper.
	*/
	vector<string>::const_iterator It = find (m_IndexedSemFets.begin(), m_IndexedSemFets.end(), string("IDENT"));   
	if (It != m_IndexedSemFets.end())
	{
		QWORD Num = It - m_IndexedSemFets.begin();
		for (long i=0; i <m_Nodes[NodeNo].m_NodeSemFets.size(); i++)
			if ( ( _QM(Num) & m_Nodes[NodeNo].m_NodeSemFets[i]) > 0)
				m_Nodes[NodeNo].m_NodeSemFets[i] &=  ~_QM(Num);
	};



	return true;

}


// инициализация DOMAIN
void CRusSemStructure::InitPOs(long ClauseNo)
{
 for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
 if (IsInClause(NodeNo, ClauseNo))
 {

  if (m_Nodes[NodeNo].GetType() != NoneRoss) 
  {
   const CRossHolder* RossDoc  =  GetRossHolder(m_Nodes[NodeNo].GetType());
   const CDictionary* pRoss = RossDoc->GetRoss();
   long  UnitNo = m_Nodes[NodeNo].GetUnitNo();

// читаем поле DOMAIN
    if (UnitNo != ErrUnitNo) 
     if (!pRoss->IsEmptyArticle(UnitNo))
      for (size_t i = pRoss->GetUnitStartPos(UnitNo); i<= pRoss->GetUnitEndPos(UnitNo); i++)
	  if ( (const char*)pRoss->Fields[pRoss->GetCortegeFieldNo(i)].FieldStr ==  string("DOMAIN") )
	  {
  		  TCortege C  = GetCortege(pRoss,i);
		  if (C.m_DomItemNos[0] == -1) continue;
		  string S = RossDoc->GetDomItemStrInner(C.m_DomItemNos[0]);
		  if (S != "общ")
			  m_Nodes[NodeNo].m_POs.push_back(S);
	  };
   
	};

   if (   IsThesCorpora(m_Nodes[NodeNo], LocRoss)  )
	  m_Nodes[NodeNo].m_POs.push_back("геогр");

   if (   IsThesCorpora(m_Nodes[NodeNo], FinRoss)  )
   	   m_Nodes[NodeNo].m_POs.push_back("фин");

   if (   IsThesCorpora(m_Nodes[NodeNo], CompRoss)  )
	   m_Nodes[NodeNo].m_POs.push_back("информ");

   

 }

};



void CRusSemStructure::DeleteAllGramCodesWithoutTheGrammems (CRusSemNode& N, QWORD Grammems)
{
  N.SetGrammems(0 );
  for (long l=0; l <N.m_GramCodes.length(); l+=2)
	  {
		string S = N.m_GramCodes.substr(l, 2);
		QWORD grams;
		m_pData->GetRusGramTab()->GetGrammems(S.c_str(), grams);
		if ( (grams & Grammems) == 0 )  
		{
			N.m_GramCodes.erase(l, 2);
			l-=2;
		}
		else
		 N.AddGrammems ( grams );
	  };

	if (!N.m_TypeAncode.empty())
		N.AddGrammems(m_pData->GetRusGramTab()->GetAllGrammems(N.m_TypeAncode.c_str()));
};

/*
  когда пользователь  вводит "актив" должен выбираться актив1 (RESTR = ед), а когда 
  пользователь вводит "активы" должен выбираться актив2 (RESTR = мн)
*/

bool CRusSemStructure::ApplyGramRestr (size_t ClauseNo)
{
  for (size_t i=0; i < m_Nodes.size(); i++)
   if ( IsInClause(i, ClauseNo) )
	if (m_Nodes[i].IsWordContainer())
		if (m_Nodes[i].m_MainWordNo != -1)
   {
	  QWORD OldGrammems 	 = m_Nodes[i].GetGrammems();
	  if (ErrUnitNo == m_Nodes[i].GetUnitNo()) continue;
	  vector<QWORD>  GrammemsSet = GetGramRestr(m_Nodes[i]);
      if ( GrammemsSet.empty() ) continue;
	  // пока проверяем только первый кортеж, поскольку пока поле RESTR одиночное 

	  if ((m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].GetTypeGrammems() & GrammemsSet[0]) == GrammemsSet[0])
		  return true;

	  // если внешние граммемы и внутренние различаются, тогда в грамкодах  хранятся внутренние граммемы,
	  // которые менять бессмысленно, поскольку везде в программе используются внешние 
  	  if (m_Nodes[i].m_Words[m_Nodes[i].m_MainWordNo].GetAllGrammems() == m_Nodes[i].GetGrammems())
			DeleteAllGramCodesWithoutTheGrammems(m_Nodes[i], GrammemsSet[0]);

	  // ни один анкод не подходит к текущей словарной интерпретации 
	  
	  if (OldGrammems > 0) // у союзов нет граммем
		  if ( (m_Nodes[i].GetGrammems() & GrammemsSet[0]) == 0) 
			  return false;

   };

   return true;


};

bool CRusSemStructure::BuildAnalyticalSupelative() 
{
   // обрабатываем наречия, которые превращают прилагательные в превосходную степень
   // В этот список входят наречия "наиболее", "самый", "наименее".
   // удаляем наречия,  а прилагательным приписываем оператор "_превос". 
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	if ( m_Nodes[NodeNo].IsLemmaList("НАИБОЛЕЕ","НАИМЕНЕЕ","САМЫЙ", "")  )
	  {
		  long SynHost = GetSynHost (NodeNo);
		  if (SynHost == -1) continue;
			// отметаем случай "на самом деле"
		  if (   !HasRichPOS(SynHost,ADJ_FULL) 
			  && !HasRichPOS(SynHost,PARTICIPLE) 
			  && !HasRichPOS(SynHost,ADJ_SHORT) 
			  && !HasRichPOS(SynHost,PARTICIPLE_SHORT) 
			 )
			 continue;

		  if (m_Nodes[NodeNo].IsLemmaList("НАИБОЛЕЕ","САМЫЙ", "") )	
			  m_Nodes[SynHost].AddGrammems(_QM(rSuperlative)); //m_RelOperators.push_back("Magn");
		  else
			  m_Nodes[SynHost].m_RelOperators.push_back("AntiMagn"); 

		  if (CanBeDeleted(NodeNo))
		  {
			  DelNode(NodeNo);
			  NodeNo--;
		  }
	  };
	return true;
};

void CRusSemStructure::CalculateBestCorporaLabel(size_t ClauseNo)
{
  // все предметные области, которые упомянуты в узлах СемП
  typedef pair<string,int> StringIntPair;
  vector<StringIntPair>	 POs;
  /*
    если  пользователь выбрал  предметную область (=/= "общ"), тогда это приравниваем
	тому  случаю, когда одно слово с этой предметной областью вошли в предложение
  */
  if (m_PO != "общ")
	  POs.push_back(pair<string,int>(m_PO, 1));

  for (size_t NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
	if (IsInClause(NodeNo, ClauseNo))
	  for (long i=0; i <m_Nodes[NodeNo].m_POs.size(); i++)
	  {
		  for (long j=0; j <POs.size(); j++)
			if (POs[j].first == m_Nodes[NodeNo].m_POs[i])
			{
				POs[j].second++;
				break;
			};
		   POs.push_back(StringIntPair(m_Nodes[NodeNo].m_POs[i], 1));
	  };

  m_Clauses[ClauseNo].m_BestPONodesCount = 0;
  for (long i=0; i <POs.size(); i++)
	if (POs[i].second > m_Clauses[ClauseNo].m_BestPONodesCount)
	{
		m_Clauses[ClauseNo].m_BestPONodesCount =  POs[i].second;
		m_Clauses[ClauseNo].m_BestPO =  POs[i].first;
	};
  if (m_Clauses[ClauseNo].m_BestPO == "общ")
  {
	  m_Clauses[ClauseNo].m_BestPO = "";
	  m_Clauses[ClauseNo].m_BestPONodesCount = 0;
  };

};

void CRusSemStructure::ProcessEveryIdents(long ClauseNo)
{
	/*
	 разбираем случай типа типа
	"Дом N 9 по Карлтон-Террас - здание немецкого посольства."
	 лицо, подготовившее описание перевала в соответствии с приложением N 2.
	 ошибки представлены в таблице N 6
	 ошибки представлены в таблице N% 6
	 Полеты авиалайнеров по маршруту N 666 
	 кэбмене номер две тысячи семьсот четыре
	 Итак, проверив заднюю стену дома номер тринадцать на 
	 */

  for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo+1;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo - 1; NodeNo++)
    if (    HasOutcomingSynRelation(NodeNo, "СУЩ_ЧИСЛ") 
		 && HasRichPOS(NodeNo-1,NOUN) 
       )
	{
AddRelation(CRusSemRelation(CValency("IDENT",A_C), NodeNo-1, NodeNo, ""));	};


}


void CRusSemStructure::BuildIZCHORelation(long ClauseNo) 
{
  for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
   if ( IsInClause(NodeNo, ClauseNo) )
	 if (m_Nodes[NodeNo].IsPrimitive())
	   if (m_Nodes[NodeNo].m_Words[0].m_Lemma == "ЕЩЕ")
	   {
		   long q = FindRightClosestNode(NodeNo);
		   if (q == -1) continue;
		   if (m_Nodes[q].m_Words.size() == 0) continue;
		   if (   (m_Nodes[q].m_Words[0].m_Lemma == "ОДИН")
			   || (m_Nodes[q].m_Words[0].m_Lemma == "ЧТО-НИБУДЬ")
			   || (m_Nodes[q].m_Words[0].m_Lemma == "КТО-НИБУДЬ")
			   || (     (m_Nodes[q].m_Words[0].HasPOS(ADJ_FULL))
					&&	 m_Nodes[q].m_Words[0].HasOneGrammem(rComparative)
				  )
			   || (     m_Nodes[q].m_Words[0].HasPOS(ADV)
			         && (m_Nodes[q].m_Words[0].m_Lemma.substr(m_Nodes[q].m_Words[0].m_Lemma.length()-2) == "ШЕ")
				  )
			   || m_Nodes[q].IsMainTimeRossNode()
			  )
		   {
				CValency V("ASPECT", C_A);  		       
				AddRelation (CRusSemRelation(V, q, NodeNo,  ""));
				m_Nodes[NodeNo].m_Vals.push_back(V);
		   };

	   };
};



/*
функция ищет идущие подряд локативные или темпопоральны группы  и строит цепочку 
 из SPECIF (вторая группа уточняет первую,  третяя уточняет вторую и т.д.)
 Например, 
   К семи часам утра в пятницу он уже уехал.
   На воле в лесу он пел.
*/


void CRusSemStructure::BuildCircumsChains(long ClauseNo) 
{
	for (long NodeNo=m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo<m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
	{
		int NextNodeNo;
		if (CheckAllIncomingRelations (NodeNo, "TIME"))	 
		{
			for (NextNodeNo=NodeNo+1;  NextNodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NextNodeNo++)
				if (m_Nodes[NextNodeNo].IsTimeRossNode())
				 {
					 if ( CheckAllIncomingRelations (NextNodeNo, "TIME") )					
						 break;
				 }
				else
					break;

			if (NextNodeNo == m_Clauses[ClauseNo].m_EndNodeNo) continue;

			if ( !CheckAllIncomingRelations (NextNodeNo, "TIME") )		  
			{
				NodeNo = NextNodeNo-1;
				continue;
			};

		}
		else
			if (CheckAllIncomingRelations(NodeNo, "LOK"))	   
			{
				NextNodeNo = GetDefiniteRightHostByNotWeakSynRelation(NodeNo+1);	
				if (NextNodeNo == -1) continue;;	   
				if (!CheckAllIncomingRelations(NextNodeNo, "LOK")) continue;;	   
			}
			else 
				continue;

		if (NextNodeNo == m_Clauses[ClauseNo].m_EndNodeNo) break;

		vector<long> Rels;
		CSemanticStructure::GetIncomingRelations(NextNodeNo, Rels, false);
		if (Rels.size() == 0) continue;
		CSynRealization	SynReal = m_Relations[Rels[0]].m_SynReal;
		DeleteRelations(Rels);
		AddRelation(CRusSemRelation(CValency("SPECIF", A_C), NodeNo,    NextNodeNo,   ""));
		m_Relations.back().m_SynReal = SynReal;
	};
};



void CRusSemStructure::CopyToMemory(long ClauseNo)
{
	const CRusSemClause& C = m_Clauses[ClauseNo];
	/* копирую узлы*/
	for (long NodeNo=C.m_BeginNodeNo; NodeNo < C.m_EndNodeNo; NodeNo++)
	if (m_Nodes[NodeNo].IsWordContainer())
	if (m_Nodes[NodeNo].m_MemNodeNo == -1)
	{
		m_Nodes[NodeNo].m_MemNodeNo = m_MemNodes.size();
		CMemNode N;
		if (m_Nodes[NodeNo].GetInterp() != 0)
			N.m_Interp = *m_Nodes[NodeNo].GetInterp();
		N.m_PrepsCount = m_Nodes[NodeNo].m_SynReal.m_Preps.size();
		N.m_Grammems =  m_Nodes[NodeNo].GetGrammems();
		if (N.m_PrepsCount > 0)
			N.m_FirstPrepUnitNo = m_Nodes[NodeNo].m_SynReal.m_Preps[0].m_UnitNo;
		else
			N.m_FirstPrepUnitNo = ErrUnitNo;
		for (long i=0;i < m_Nodes[NodeNo].m_Words.size(); i++)
			N.m_Words.push_back((CRusMorphHomonym)m_Nodes[NodeNo].m_Words[i]);
		CopyInterpNode(N, m_Nodes[NodeNo]);
		m_MemNodes.push_back (N);
	};

	/*
	 запоминаю, что узлы были вместе в процедуре FindActants
	*/
	for (long NodeNo=C.m_BeginNodeNo; NodeNo < C.m_EndNodeNo; NodeNo++)
	if (m_Nodes[NodeNo].m_MemNodeNo != -1)
	{
		m_MemNodes[m_Nodes[NodeNo].m_MemNodeNo].m_Cases.insert(m_MemoryCaseNo);
	};

	/* копирую отношения*/
	/*
		не все новые отношения будем копировать, например,
		отношения от копула или к копулу не будем.
		для этих отношение узлы не были запомнены, поэтому 
		m_MemNodeNo == -1
	*/

	for (long i=m_MemSaveRelationCount; i < m_Relations.size(); i++)
		if (   (m_Nodes[m_Relations[i].m_SourceNodeNo].m_MemNodeNo != -1) 
		    && (m_Nodes[m_Relations[i].m_TargetNodeNo].m_MemNodeNo != -1) 
		   )
	{
		m_MemRelations.push_back(m_Relations[i]);
		CRusSemRelation& R = m_MemRelations[m_MemRelations.size()-1];
		R.m_SourceNodeNo = m_Nodes[R.m_SourceNodeNo].m_MemNodeNo;
		R.m_TargetNodeNo = m_Nodes[R.m_TargetNodeNo].m_MemNodeNo;
	};

	m_MemoryCaseNo++;
};

bool EqualNodeAndMemNode (const CMemNode& N1,const CRusSemNode& N2)
{
	if (N1.m_Interp.m_DictType != N2.GetType())  return false;
	if (N1.m_Interp.m_UnitNo != N2.GetUnitNo())  return false;
	if (N1.m_Words.size() !=  N2.m_Words.size()) return false;
	if (N1.m_PrepsCount != N2.m_SynReal.m_Preps.size()) return false;
	if (N1.m_PrepsCount > 0)
		if (N1.m_FirstPrepUnitNo != N2.m_SynReal.m_Preps[0].m_UnitNo) return false;

	if (N1.m_Grammems != N2.GetGrammems()) return false;

	for (long i=0; i < N1.m_Words.size(); i++)
	  if (   (N1.m_Words[i].m_SynWordNo != N2.m_Words[i].m_SynWordNo)
		  || (N1.m_Words[i].m_ParadigmId != N2.m_Words[i].m_ParadigmId)
		 )
		 return false;
		
	return true;
}


/*
	 устанавливаем соответствие между старыми узлами и новыми 
*/
void CRusSemStructure::FindOldNodes(long ClauseNo)
{
	const CRusSemClause& C = m_Clauses[ClauseNo];
	for (long NodeNo=C.m_BeginNodeNo; NodeNo < C.m_EndNodeNo; NodeNo++)
		m_Nodes[NodeNo].m_MemNodeNo = -1;

	for (long NodeNo1=0; NodeNo1 < m_MemNodes.size(); NodeNo1++)
		for (long NodeNo2=C.m_BeginNodeNo; NodeNo2 < C.m_EndNodeNo; NodeNo2++)
			if (m_Nodes[NodeNo2].m_MemNodeNo == -1)
				if (EqualNodeAndMemNode(m_MemNodes[NodeNo1], m_Nodes[NodeNo2]))
				{
					m_Nodes[NodeNo2].m_MemNodeNo = NodeNo1;
					break;
				}


};

void CRusSemStructure::RestoreMemoryRelations (long ClauseNo)
{
	vector<long> Nodes;
	Nodes.resize(m_MemNodes.size());

	for (long i=0;  i< Nodes.size(); i++)  
		Nodes[i] = -1;

    // строим соответствие от старых узлов к новым
	const CRusSemClause& C = m_Clauses[ClauseNo];
    for (long NodeNo=C.m_BeginNodeNo; NodeNo < C.m_EndNodeNo; NodeNo++)
		  if (m_Nodes[NodeNo].m_MemNodeNo != -1)
		  {
			  Nodes[m_Nodes[NodeNo].m_MemNodeNo] = NodeNo;
		  }

	/*
	 копируем отношения
	*/
    for (long RelNo=0; RelNo < m_MemRelations.size(); RelNo++)
	{
	  
	  if (Nodes[m_MemRelations[RelNo].m_SourceNodeNo] == -1) continue;
	  if (Nodes[m_MemRelations[RelNo].m_TargetNodeNo] == -1) continue;
	  CRusSemRelation R = m_MemRelations[RelNo];
	  R.m_SourceNodeNo = Nodes[R.m_SourceNodeNo];
	  R.m_TargetNodeNo = Nodes[R.m_TargetNodeNo];
	  AddRelation(R);
	};


};



void CRusSemStructure::PrintMemRelations() const 
{ 
 rml_TRACE ("\n\nMemRelations : \n");

 for (size_t i=0; i<m_MemRelations.size(); i++)
 {
	 string Label = m_MemRelations[i].m_Valency.m_RelationStr;
	 string Q = Format ("%i %s ",  m_MemRelations[i].m_SourceNodeNo, Label.c_str());
	 {
		 string  W = Format(" %i ", m_MemRelations[i].m_TargetNodeNo);
		 Q += W;
	 };
	 Q += string("\n");

	 rml_TRACE (Q.c_str());
 };


};


bool CRusSemStructure::BuildHypotRelationsGraph(size_t ClauseNo)
{
	size_t NodeNo;

	StartTimer("Панический граф",0);
	StartTimer("До построения всех отношений",1);

	try {
		//for (size_t i=0; i<  m_Nodes.size(); 

		try {

			FindOldNodes(ClauseNo);

			m_Clauses[ClauseNo].m_bSaveClauseHasNegationInVerb =  ClauseHasNegationInVerb(ClauseNo);


			for (size_t NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
				if (IsInClause(NodeNo, ClauseNo))
					InitValsRussian(NodeNo);

			
		}
		catch (...)
		{
			string Mess = Format ("InitValsRussian  Failed. ClauseNo = %i ClauseVariantsCombinationNo=%i", ClauseNo, m_ClauseVariantsCombinationNo);
			ErrorMessage (Mess);
			throw;
		};

		try {


			bool Result = ApplyGramRestr (ClauseNo);
			if (!m_bLastTry)
			{
				if (!Result) return false;
				if (!FindComparWithNoActantAndCheck(ClauseNo)) return false;
			};

		}
		catch (...)
		{
			ErrorMessage ("ApplyGramRestr  Failed");
			throw;
		};



		try {
			InitGrammarMatrix1(ClauseNo);
			BuildVerbLexFunctParameter(ClauseNo);
			BuildAdjOrAdvLexFunctParamNodes(ClauseNo);
		}
		catch (...)
		{
			ErrorMessage ("BuildVerbLexFunctParameter or BuildAdjOrAdvLexFunctParamNodes Failed");
			throw;
		};

		// Эта процедура создает отношения по локативным признакам
		try {

			PutLocRelations(ClauseNo);

		}
		catch (...)
		{
			ErrorMessage ("PutLocRelations Failed");
			throw;
		};



		try {

			InterpretOrganisations(ClauseNo);

		}
		catch (...)
		{
			ErrorMessage ("InitThesSemFet or InterpretOrganisations Failed");
			throw;
		};

		try {

			BuildIZCHORelation(ClauseNo);

		}
		catch (...)
		{
			ErrorMessage ("BuildIZCHORelationFailed");
			throw;
		};
		try {

			BuildPustychaWithETO(ClauseNo);
		}
		catch (...)
		{
			ErrorMessage ("BuildPustychaWithETO");
			throw;
		};

		try {


			InterpretSimilarNounGroups(ClauseNo);
		}
		catch (...)
		{
			ErrorMessage ("FindParticleBY or InterpretSimilarNounGroups");
			throw;
		}


		EndTimer("До построения всех отношений");
		StartTimer("Построение всех отношений",1);

		RestoreMemoryRelations(ClauseNo);

		/* функция InterpretSimilarNounGroups строит отношения, 
		которые уже могут быть в старом варианте, поэтому нужно 
		вызвать функцию DeleteDubleRelations, чтобы удалить 
		дубли
		*/
		DeleteDubleRelations();

		m_MemSaveRelationCount = m_Relations.size();

		try {
			InitGrammarMatrix2(ClauseNo);


			for (NodeNo = 0;  NodeNo < m_Nodes.size(); NodeNo++)
				if (   IsInClause(NodeNo, ClauseNo)
					&& m_Nodes[NodeNo].IsWordContainer()
					&& (m_Nodes[NodeNo].m_NodeType != MNA)
					)
				{
					
					FindActants(NodeNo);   
					
				}



			FindReverseActantForPreps(ClauseNo);


		}
		catch (...)
		{
			ErrorMessage (Format("FindActants Failed: %i", NodeNo));
			throw;
		};
		

		CopyToMemory(ClauseNo);

		EndTimer("Построение всех отношений");

		try 
		{

			ApplySynStr (ClauseNo);

			ConvertFreeCoordConjToMNA(ClauseNo);  

			BuildOutcomingOf_CHEM_Obor(ClauseNo);
			BuildOutcomingOf_COMP_Obor(ClauseNo);

			BuildMNA_KAK(ClauseNo);
			BuildMNAOutcoming(ClauseNo);
			BuildMNAIncoming(ClauseNo);
			DeleteMNAWithOneChild(ClauseNo);
			PrintRelations();

			/*
			заново прогоняем ApplySynStr, а перед этим помечает те син. отношения,
			которым можно доверять
			*/
			FindPragmaticallyStrongSynRel();	
			ApplySynStr (ClauseNo);

			/*
			восстанавливаем значение слота СSynRelation::m_bPragmaticallyStrong
			*/
			for (long i=0;i <m_SynRelations.size();i++) 
				m_SynRelations[i].m_bPragmaticallyStrong = false;

			ApplyTerminSemStr(ClauseNo);

			PrintRelations();

			DeleteDubleRelations();


			BuildRelationLeapsCount(ClauseNo);
			InitPOs(ClauseNo);
			CalculateBestCorporaLabel (ClauseNo);	
			HeuristicsLocative(ClauseNo); 
			DealUnboundModalCopul(ClauseNo);
			ProcessEveryIdents(ClauseNo);
			BuildCircumsChains(ClauseNo);



			// уничтожаю длинные отношения,  длина которых превышает указанную пользователем (MaxRelationLeapsCount)
			DeleteLongRelations(ClauseNo, m_SemCoefs.MaxRelationLeapsCount,true);


			// если мы разбираем очень сложную (Complexity(K) > 1.9) клаузу, то 
			// уничтожаем длинные отношения, у которых (MaxRelationLeapsCount > 0)  
			if (   (m_Clauses[ClauseNo].GetNodesCount()  > 10) 
				&& (((float)m_Relations.size() / (float) m_Clauses[ClauseNo].GetNodesCount()) > 1.9)
				)
				DeleteLongRelations(ClauseNo, 0, true);


			MarkSemRelPOSViolations();


		}
		catch (...)
		{
			ErrorMessage ("BuildHypotRelationsGraph (after FindActants) Failed");
			throw;
		};


	}
	catch (...)
	{
		ErrorMessage ("BuildHypotRelationsGraph Failed");
		throw;
	};

	EndTimer("Панический граф");
	return true;

};





