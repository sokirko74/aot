#include "stdafx.h"
#include "SemanticRusStructure.h"

/*
Правила для третьего класса
Будем называть существительные SF = 1 SUBST 2 ABSTR 3 PARAM 4 ESTIM 5 INTEL неисчисляемыми. 1) Если мы встречаем  больше/меньше + С рд ед и С не является неисчисялемым, 
тогда  здесь сравнительная степень образована от прилагательного "большой", а не от числитльного
"много":
Например:
  Это больше меня  
  Слот больше суслика. 
2) Если мы встречаем оборот "все(НАР) больше" или "все(НАР) меньше" + С рд  
(переводить такую конструкцию нужно с помощью оборота "more and more" ("less and less"))
  +Все больше землян утверждало 
  + ему приходилось производить все больше и больше обуви (с редупликацией )
3) Если справа от сравнительного слова идет числительное 
            +Больше двух тысяч людей  пришло на митинг

*/

bool CRusSemStructure::FindComparWithNoActantAndCheck(long ClauseNo)
{
 /*
  первое правило: после слов "больше" или "меньше" как числительных не может стоять
  исчисляемое существительное в единственном числе, исчисляемость 
  определяется  по SF.
   Таким образом во фразе "Стол больше стула" мы уничтожаем    
   интерпретацию слова больше как числительного.

 */
  vector<bool> UncounNodes;
  UncounNodes.resize(m_Nodes.size());
  for (long i=0; i < m_Nodes.size(); i++)
	 if (IsInClause(i, ClauseNo))
		UncounNodes[i] = 
				HasRichPOS(i, NOUN)  
			&& m_Nodes[i].HasOneGrammem (rSingular)
			&& (   HasSemFetPro(m_Nodes[i].m_NodeSemFets, "SUBST") // "воды"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "ABSTR") // "влюбчивочть"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "PARAM") // "мощность"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "ESTIM") // "красота"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "INTEL") // "ум"
				|| HasSemFetPro(m_Nodes[i].m_NodeSemFets, "EMOT") // "надежда"			 
				);


  for (long i=0; i+1 < m_Nodes.size(); i++)
	  if (IsInClause(i, ClauseNo))
		  if (   HasRichPOS(i, NUMERAL) 
			  && m_Nodes[i].HasOneGrammem (rComparative)
			 )
		  {
			  if  ( m_Nodes[i+1].HasOneGrammem (rPlural) ) continue;
			  if  (UncounNodes[i+1]) continue;

			  /*
				он был больше учителем, чем сторожем.
			  */
			  if  ( !m_Nodes[i+1].HasOneGrammem (rGenitiv)) continue;
			  return false;
		  };
 /*
  конец первого правила
 */

 /* 
 второе  правило: сравнительная степень от прилагательного "большой"- "big"
 ("маленький" - "small") используется только в сравнительных клаузах типа "Слон больше суслика" 

 А во фразе "он работает больше тебя" слово "больше" - это наречие "много".
*/

 if (m_Clauses[ClauseNo].m_ClauseType != COMPARATIVE_T)
 	 for (long i = m_Clauses[ClauseNo].m_BeginNodeNo+1;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
		 if (    m_Nodes[i].IsLemmaList ("БОЛЬШОЙ", "МАЛЕНЬКИЙ", "") 
		    &&	m_Nodes[i].HasOneGrammem(rComparative)
		 )
	  return false;



 /*
  конец второго правила
 */


 /*
  если перед сравнительным числительным стоит "все", нужно приписать 
  оператор "ПРОДОЛЖ" этому числительному, а само  "все" удалить
 */
  for (long i = m_Clauses[ClauseNo].m_BeginNodeNo+1;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
  	  if (			HasRichPOS(i, NUMERAL) 
			  &&	m_Nodes[i].HasOneGrammem (rComparative)
			  &&	CanBeDeleted(i)
		 )
	  {
			  if ( !m_Nodes[i-1].IsWordForm("ВСЕ") ) continue;
	  		  m_Nodes[i].m_RelOperators.push_back("ПРОДОЛЖ");
			  DelNode(i-1);
			  i--;
	  }

  return true;
};



// он был больше учителем, чем сторожем.
// находит слова  больше ...,чем
// я не знаю  человека добрее, чем Вася
/// === ОБРАБАТЫВАЕМ ТОЛЬКО ПОСЛНДНИЙ КОМПАРАТИВ В КЛАУЗЕ 
void CRusSemStructure::BuildOutcomingOf_CHEM_Obor(long ClauseNo)
{
 try {
  SetNodeToDeleteFalse();
  int EndNodeNo = m_Clauses[ClauseNo].m_EndNodeNo;
  
  long CompNodeNo = EndNodeNo-1;
  for (;  CompNodeNo >=  m_Clauses[ClauseNo].m_BeginNodeNo; CompNodeNo--)
    if ( m_Nodes[CompNodeNo].m_bCompAdj)
	{
		
		long  k=CompNodeNo+1;
		for (; k < EndNodeNo; k++)
        if  (     m_Nodes[k].IsPrimitive()
			   && (m_Nodes[k].m_Words[0].m_Lemma == "ЧЕМ")
			 )
		break;
        if (k == EndNodeNo) continue;
        long ChemNodeNo = k;
		//получаем SF1 компаратива из РОСС		
		vector<QWORD> SemFets = GetSemFetsOfFirstValency(CompNodeNo);

		for (k=ChemNodeNo+1; k < EndNodeNo; k++)
           if (     IsAdverbial(k)
				 || HasRichPOS (k, NOUN) 
			     || HasRichPOS (k, PRONOUN)
			  )
		   {
			 // удаляем отношение, которое было построено по валенностям
			 long RelNo = FindFirstRelation(CompNodeNo, k);
			 if (RelNo != -1)
				 EraseRelation(RelNo);
			 AddRelation(CRusSemRelation(CValency(), CompNodeNo, k, ""));
			 m_Relations[m_Relations.size() - 1].m_SemFets = SemFets;
			 break;
		   };
		if (k == EndNodeNo) continue;
		if (!CanBeDeleted(ChemNodeNo)) continue;
		long AfterChemNodeNo = k;




		for (k=m_Clauses[ClauseNo].m_BeginNodeNo; k < EndNodeNo; k++)
         if ( k!= ChemNodeNo)
		  if ( k!= CompNodeNo )
			if ( k!= AfterChemNodeNo )
           if (    (    IsAdverbial(AfterChemNodeNo)
			         && IsAdverbial(k)
				   )
				 ||(
				        !IsAdverbial(AfterChemNodeNo)
					 && (    HasRichPOS (k, NOUN) 
						  || HasRichPOS (k, PRONOUN)
						)
				   )
			  )
		   {
			 AddRelation(CRusSemRelation(CValency(), CompNodeNo, k, ""));
			 m_Relations[m_Relations.size() - 1].m_SemFets = SemFets;
		   };
		
          
         

		// уничтожаю "чем"
        m_Nodes[ChemNodeNo].m_bToDelete = true;

		// делаем компаратив оператором однородности
		m_Nodes[CompNodeNo].m_NodeType = MNA;
		m_Nodes[CompNodeNo].m_MNAType = CHEMOborot;
		
		break;

	};

  DelNodesToDelete();
  // обрабатываем случай "Налоги будут в два раза больше, чем в этом году"

  if (m_Clauses[ClauseNo].m_ClauseType == COMPARATIVE_T)
  if (m_Clauses[ClauseNo].m_bHasCHEM)
  if (CompNodeNo != -1)
  {
    for (long i = m_Clauses[ClauseNo].m_BeginNodeNo;  i < m_Clauses[ClauseNo].m_EndNodeNo ; i++)
	 if (    (GetIncomingRelationsCount (i) == 0)
		  && (   (HasRichPOS (i, NOUN))
		      || (HasRichPOS (i, PRONOUN))
			 )
		  && m_Nodes[i].HasOneGrammem(rNominativ)
	    )
	 {
		 if(!m_Nodes[CompNodeNo].m_Vals.empty())
			AddRelation(CRusSemRelation(m_Nodes[CompNodeNo].m_Vals[0], i, CompNodeNo, ""));
	 };
  };


 }
 catch (...)
 {
	 ErrorMessage ("BuildOutcomingOf_CHEM_Obor Failed");
	 throw;
 };

};



/*
  Клаузы со сравнительным "больше" ("меньше") и с "чем":
 "он был больше учителм, чем сторожем"
 здесь нужно переделать "большой"  в "много", чтобы переводилось в 
  "he was more a teacher, than a watchman", а не в
  "he was bigger a teacher, than a watchman"

   
*/
void CRusSemStructure::ZaplataCompar1(long ClauseNo)
{
  if (m_Clauses[ClauseNo].m_bHasCHEM) 
  if (m_Clauses[ClauseNo].m_ClauseType == COMPARATIVE_T) 
  {

	  for (long CompNodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  CompNodeNo <   m_Clauses[ClauseNo].m_EndNodeNo; CompNodeNo++)
		if (    m_Nodes[CompNodeNo].HasOneGrammem(rComparative)
			&&  (   m_Nodes[CompNodeNo].IsLemma("БОЛЬШОЙ") 		
				 || m_Nodes[CompNodeNo].IsLemma("МАЛЕНЬКИЙ") 		
				)
		   )
		{
			size_t k=CompNodeNo+1;
			for (; k < m_Clauses[ClauseNo].m_EndNodeNo; k++)
			  if (     (		HasRichPOS (k, NOUN) 
							||	HasRichPOS (k, PRONOUN) 
						)
				   &&  m_Nodes[k].HasOneGrammem (rInstrumentalis)
				 )
				 break;
			if ( k == m_Clauses[ClauseNo].m_EndNodeNo ) return;
			if (!m_Nodes[k].IsPrimitive()) return;
			m_Nodes[k].m_Words[0].m_Lemma = "МНОГО";
			InitInterps(m_Nodes[k].m_Words[0], false, ClauseNo, m_Nodes[k]);
		};
	};


};

/*
   Клаузы, где в вершине стоит слово с отрицанием или "нет" и где ест слово
  "больше", например:
  "он не хочет пить больше вина"
  "У меня нет больше надежды."
Здесь  нужно разорвать стрелку QUANTIT, которая идет от "больше"  к следующему  за ним существительному, поскольку "больше" относится к глаголу.

*/
void CRusSemStructure::ZaplataCompar2(long ClauseNo)
{
   if (m_Clauses[ClauseNo].m_ClauseSyntaxTop != -1)
   if (    m_Nodes[m_Clauses[ClauseNo].m_ClauseSyntaxTop].HasRelOperator("НЕ") 
	    || m_Nodes[m_Clauses[ClauseNo].m_ClauseSyntaxTop].IsLemma ("НЕТ")
	  )
   for (long CompNodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  CompNodeNo <   m_Clauses[ClauseNo].m_EndNodeNo; CompNodeNo++)
		if (    m_Nodes[CompNodeNo].HasOneGrammem (rComparative)
			&&  m_Nodes[CompNodeNo].IsLemma("МНОГО") 		
		   )
		{
			vector<long> Rels;
			GetIncomingSynRelations(CompNodeNo, Rels);
			if (Rels.size() ==  1)
			{
				/*
				 кроме удаления, нужно еще скопировать внутренние граммемы во внешние,
например, "больше воды" - здесь, когда есть связь QUANTIT,  у "воды" внешние граммемы "им,вн",а поскольку связь QUANTIT разрывается, внешние граммемы нужно приравнять				 внутренним
				*/

				assert (m_SynRelations[Rels[0]].m_SynRelName == "НАР_ЧИСЛ_СУЩ");
				CRusSemNode& NounNode = m_Nodes[m_SynRelations[Rels[0]].m_SourceNodeNo];
				assert (NounNode.IsWordContainer());
				assert (NounNode.m_MainWordNo != -1);
				NounNode.SetGrammems ( NounNode.m_Words[NounNode.m_MainWordNo].GetAllGrammems());

				DeleteSynRelations(Rels);
			};
			
		};
};

// Я не знаю человека добрее Васи
// Я пел в два раза дольше тебя
// сейчас важнее эказамен
/// === ОБРАБАТЫВАЕМ ТОЛЬКО ПОСЛЕНДНИЙ КОМПАРАТИВ В КЛАУЗЕ 
void CRusSemStructure::BuildOutcomingOf_COMP_Obor(long ClauseNo)
{
try {
	int EndNodeNo	= m_Clauses[ClauseNo].m_EndNodeNo;

	/*
		Чтобы потсроить дерево	для
		1. "я	мечтал побыстрее его женить", 
		2. "чем лучше	он видел, тем хуже он шел"
		нужно,	чтобы была проведена обстоятельственная	стрелка	от всех	глагольных форм
		клаузы к  компаративу
	*/
	for (long	CompNodeNo = EndNodeNo-1;  CompNodeNo >=  m_Clauses[ClauseNo].m_BeginNodeNo; CompNodeNo--)
		if (	m_Nodes[CompNodeNo].m_bCompAdj		)
		{

			if (!m_Nodes[CompNodeNo].IsLemma ("БОЛЬШОЙ"))
				for	(long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo <	 EndNodeNo;	NodeNo++)
					if	(		IsVerbForm (m_Nodes[NodeNo])	
							||	HasRichPOS (NodeNo, PREDK) // "у	меня нет больше	 надежды"
						)
					{
						AddRelation(CRusSemRelation(CValency("ACT",A_C), NodeNo, CompNodeNo,  "чистое_обстоятельство"));			
					};
		}

	for (long CompNodeNo =	EndNodeNo-1;  CompNodeNo >=	 m_Clauses[ClauseNo].m_BeginNodeNo;	CompNodeNo--)
		if	(		(m_Nodes[CompNodeNo].m_MNAType !=	CHEMOborot)
				&&	m_Nodes[CompNodeNo].m_bCompAdj		
			)
		{
			vector<long> ChildNodes;

			size_t	k=CompNodeNo+1;
			for	(;	k <	EndNodeNo; k++)
				if	(		( HasRichPOS (k, NOUN) || HasRichPOS (k, PRONOUN))
						&&  m_Nodes[k].HasOneGrammem(rGenitiv)
					)
				{
					ChildNodes.push_back(k);
					break;
				};

			if (k == EndNodeNo)	
				// в клаузе, где	главное	слово -	компаратив,	 может быть	не выражено	
				//  дополнение компаратива "я	буду умнее"
				if (m_Clauses[ClauseNo].m_ClauseType != COMPARATIVE_T) 
					continue;

			/*
			для некоторых узлов было вычислено, что для них не	нужно искать второго актанта
			например, "прыгайте выше, друзья""
			*/
			if (m_Nodes[CompNodeNo].m_bCompWithOneActant)
				continue;

			// я не	знаю  человека добрее Васи
			// поищем именную группу до	сравнительного прилагательного
			for	(long l=CompNodeNo-1; l	>= m_Clauses[ClauseNo].m_BeginNodeNo; l--)
				if (!m_Nodes[l].m_bSaveIsParenthesis)
					if (HasRichPOS (l, NOUN)	|| HasRichPOS (l, PRONOUN))
						if	(FindFirstRelation(CompNodeNo, l) == -1)
							ChildNodes.push_back(l);


			//	если не	нашлось	ни одной ИГ, то	поищем ИГ в	им.	после компаратива
			//	"сейчас	важнее эказамен"
			if	(		(m_Clauses[ClauseNo].m_ClauseType	== COMPARATIVE_T) 
					&&	(ChildNodes.size() ==	0) 
				)
				for (size_t k=CompNodeNo+1; k < EndNodeNo;	k++)
					if (	  (	HasRichPOS (k, NOUN) ||	HasRichPOS (k, PRONOUN))
						&&	 m_Nodes[k].HasOneGrammem(rNominativ)
						)
					{
						ChildNodes.push_back(k);
						break;
					};


			if	(	 (ChildNodes.size()	< 2	) )
			{
				if	(	  (m_Clauses[ClauseNo].m_ClauseType	!= COMPARATIVE_T) 
					|| (ChildNodes.size() !=	1) 
					)
					continue;
			};

			//получаем SF1 компаратива из РОСС		
			vector<QWORD> SemFets =	GetSemFetsOfFirstValency(CompNodeNo);

			for	(long l=0; l	< ChildNodes.size(); l++)
			{
				AddRelation(CRusSemRelation(CValency(), CompNodeNo, ChildNodes[l],	 ""));
				m_Relations[m_Relations.size()	- 1].m_SemFets = SemFets;
				if	(l == 0) //	группа после компаратива, пишем	на эту стрелку 
					//	к_доп+Р, взятые	как	будто из РОССа
				{
					m_Relations[m_Relations.size() - 1].m_SynReal.m_Cortege.m_DomItemNos[0]	= GetRossHolder(Ross)->IndirObjSynONo;
					m_Relations[m_Relations.size() - 1].m_SynReal.m_Cortege.m_DomItemNos[1]	= GetRossHolder(Ross)->GenitivNo;
					m_Relations[m_Relations.size() - 1].m_Valency.m_RossHolder = GetRossHolder(Ross);
				};
			};


			m_Nodes[CompNodeNo].m_NodeType = MNA;
			m_Nodes[CompNodeNo].m_MNAType =	CHEMOborot;
			break;
		};
}
catch (...)
{
	ErrorMessage ("BuildOutcomingOf_COMP_Obor Failed");
	throw;
};
};


