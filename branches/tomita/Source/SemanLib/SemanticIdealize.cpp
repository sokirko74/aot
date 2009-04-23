#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"

void CRusSemStructure::DelChildlessCopul ()
{
  // =============================
  // если у  Copul нет ни одного актанта, то удаляем этот узел
  // он представлялся мне великим учителем 	
  for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
	 if (m_Nodes[NodeNo].m_NodeType == Copul)
	   if ( GetOutcomingRelationsCount(NodeNo,  false) == 0)
	   {
		 DelNode(NodeNo);
		 NodeNo--;
	   };

};

long CRusSemStructure::Idealize ()
{
	long Bonus = 0;
	try {
		// =============================
		// дом строился рабочими => пассив
		// мы строились  => возвратная форма (коллективность)
		// ALG:  // нужно взять глаголы найденные в РОССе, которые заканчиваются на "ся" ("cь"). 
		// Причем в РОССе были найдены невозвратные формы.
		// Нужно проверить, заполнилась ли первая валентность этого глагола (АГЕНТА). Если нет,
		// то нужно создать новый узел с пометой (СЯ) и провести стрелку с первой валентностью 
		// к этому узлу.

		for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
			if (   
				m_Nodes[NodeNo].IsPassiveVerb()
				&& m_Nodes[NodeNo].IsWordContainer() // могут быть глагольные словосочетания
				&& (m_Nodes[NodeNo].m_Vals.size() > 0)
				)
			{
				vector<long> Rels;
				// ищем сначала в обычных отношениях, а потом  в дополнительных
				GetOutcomingRelations(NodeNo, Rels, false);
				long i=0;
				for (; i < Rels.size(); i++)
					if (m_Relations[Rels[i]].m_Valency.m_RelationStr == m_Nodes[NodeNo].m_Vals[0].m_RelationStr)
						break;

				if (i < Rels.size())  continue;

				GetOutcomingDopRelations(NodeNo, Rels);
				for (i=0; i < Rels.size(); i++)
					if (m_DopRelations[Rels[i]].m_Valency.m_RelationStr == m_Nodes[NodeNo].m_Vals[0].m_RelationStr)
						break;

				if (i < Rels.size())  continue;


				CRusSemNode N;
				N.m_NodeType = SJA;
				assert (m_Nodes[NodeNo].m_MainWordNo != -1);

				N.m_ClauseNo = m_Nodes[NodeNo].m_ClauseNo;
				long nd = InsertNode(N);
				AddRelation(CRusSemRelation(m_Nodes[NodeNo].m_Vals[0], NodeNo, nd,  ""));

				// удаляем "СЯ" из леммы и слова

				CRusSemWord& W = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];
				if (HasReflexiveSuffix(W.m_Lemma))
				{
					W.m_Lemma.erase(W.m_Lemma.length() -2, 2);
					W.m_Word.erase(W.m_Word.length() -2, 2);
				};
			}



			// если SF  "ОН" или "ОНА" равна ANIM, то нужно  
			// прставить грамматическую одушевленность у местоимения, в противном случае будем ставить 
			// неоудушевленнсть
			for (long i=0;  i < m_Relations.size(); i++)
				if (m_Relations[i].m_Valency.IsFromDict())
					if (m_Relations[i].m_Valency.m_UnitNo != ErrUnitNo)
					{
						CRusSemNode& N =  m_Nodes[m_Relations[i].m_TargetNodeNo];
						if    (     !N.IsPrimitive()  ) 
							continue;
						if    (     !(N.m_Words[0].m_Lemma == "ОН")
							&& !(N.m_Words[0].m_Lemma == "ОНА")
							&& !(N.HasOneGrammem(rAnimative) &&  N.HasOneGrammem(rNonAnimative))
				   )
				   continue;
						N.DeleteGrammems (  _QM(rAnimative) | _QM(rNonAnimative) );
						if ( HasSemFetOrLower(N,"ANIM") )				
							N.AddOneGrammem( rAnimative);
						else
							N.AddOneGrammem (rNonAnimative);

					};

			// ====  достройка синтаксиса
			// Напиши на стрелке, идущей в узел КАК, во фразах типа  "они думали , как она красива" с "как1 из РОСС" 
			// соответствующее синтаксическое отношение НАР-НАР, НАР-ПРИЛ или НАР_ПРЕДИК
			// синтаксис не мог потсроить этого отношения, поскольку тогда получилась бы разрывная группа,
			// поэтому достраиваем синтаксис
			for (long i=0;  i < m_Relations.size(); i++)
				if (m_Nodes[m_Relations[i].m_TargetNodeNo].IsPrimitive())
					if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[0].m_Lemma == "КАК")
					{
						if (HasRichPOS(m_Relations[i].m_SourceNodeNo, ADJ_SHORT))
							m_Relations[i].m_SyntacticRelation = "НАР_ПРИЛ";
						if (HasRichPOS(m_Relations[i].m_SourceNodeNo,PREDK))
							m_Relations[i].m_SyntacticRelation = "НАР_ПРЕДИК";
						if (HasRichPOS (m_Relations[i].m_SourceNodeNo, ADV))
							m_Relations[i].m_SyntacticRelation = "НАР_НАР";
					};


			// MUA[','](x1, ... ,MUA[z](y1, ..., yn), .. xn)  => MUA[z] (x1,...,xn, y1...,yk)  
			// если в оператор однородности A входит оператор однородности B, и  оператор A - запятая,
			// а оператор B - союз, тогда операторы А и B нужно слить
			// В арифметике это правило соответствует следующему преобразованию:
			// a+(b+c) = a+b+c
			for (long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
				if (    (m_Nodes[NodeNo].m_NodeType == MNA)
					&& m_Nodes[NodeNo].IsPrimitive()
					&& (m_Nodes[NodeNo].m_Words[0].m_Word.length() > 0)
					&& (m_Nodes[NodeNo].m_Words[0].m_Word[0] != ',')   
					)
				{
					CRelSet R  = GetIncomingRelations(NodeNo, false);
					long i=0;
					for (; i < R.m_RelsCount; i++)
					{
						const CRusSemNode& N = m_Nodes[m_Relations[R.m_Rels[i]].m_SourceNodeNo];
						if (      N.IsPrimitive()
							&& (N.m_Words[0].m_Word.length() > 0)
							&& (N.m_Words[0].m_Word[0] == ',')   
							)
							break;
					};
					if (i == R.m_RelsCount) continue;
					long ParentNodeNo = m_Relations[R.m_Rels[i]].m_SourceNodeNo;
					EraseRelation(R.m_Rels[i]);
					MoveRelations(ParentNodeNo, NodeNo);
					DelNode(ParentNodeNo);
				};


			// другие функции идеализации
			InterpretAdjNounGroupsAfter ();
			ApplySubordinationCases ();
			//   ProcessIsolatedParticipleAnd();
			DealInvitatoryMoodAfterTree();
			NumeralAdverbRule();
			FindQuestionClauses();
			FindDividedTermins();
			FindAbbrTermins();
			FindEnglishTermins();
			MovePrepNodeToRelationForMainTimeGroups();
			ClearInterpsForFreeTimeGroups();
			Bonus += CreateDefaultSubjectFromPreviousClause();
			ConvertVSE2_toOperator();
			SolveImperativeHomonymy();

	}
	catch (...)
	{
		ErrorMessage("void CRusSemStructure::Idealize failed!");
		throw;
	};

	return Bonus;
};

/*
 эта функция конвертирует  узел, в  котором записан все2, в оператор ПРОДОЛЖ,
 если этот узел зависит от глагола
 Пример: "он все уходил взглянуть"
 На оператор ПРОДЛЖ  реагирует английский алгоритм ApplyKeepNode
*/

void CRusSemStructure::ConvertVSE2_toOperator()
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++) 
	{
		if (m_Nodes[NodeNo].GetType() != Ross) continue;
		if (!m_Nodes[NodeNo].IsLemma("ВСЕ"))  continue;
		if (GetRoss(Ross)->GetUnitMeanNum(m_Nodes[NodeNo].GetUnitNo()) != 2)  continue;
		CRelSet R = GetIncomingRelations(NodeNo, false);
		if (R.m_RelsCount  != 1) continue;
		long nd = m_Relations[R.m_Rels[0]].m_SourceNodeNo;
		if (!IsVerbForm (m_Nodes[nd])) continue;
		m_Nodes[nd].m_RelOperators.push_back("ПРОДОЛЖ");
		if (CanBeDeleted(NodeNo))
		{
			DelNode(NodeNo);
			NodeNo--;
		}
	};
};


void CRusSemStructure::InterpretAdjNounGroupsAfter ()
{
  // меняем направление стрелки для отношений ЧИСЛ_СУЩ, ПРИЛ_СУЩ
  for (long i=0; i < m_Relations.size(); i++) 
   if  (    ( m_Relations[i].m_SyntacticRelation == "ЧИСЛ_СУЩ" )
	     || ( m_Relations[i].m_SyntacticRelation == "АППРОКС_ИГ" )
		 || ( m_Relations[i].m_SyntacticRelation == "НАР_ЧИСЛ_СУЩ" )
	   )
   {
	  // swap(m_Relations[i].m_SourceNodeNo, m_Relations[i].m_TargetNodeNo);
	   m_Relations[i].m_Valency.m_Direction = A_C;
   	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
			m_Relations[i].m_Valency.m_RelationStr = "QUANTIT";   
   }
   else
   if  (    ( m_Relations[i].m_SyntacticRelation == "ПРИЛ_СУЩ" )
	    ||  ( m_Relations[i].m_SyntacticRelation == "НАР_ПРИЛ" )
		||  ( m_Relations[i].m_SyntacticRelation == "МОДИФ_ПРИЛ" )
	   )
   {
	   //swap(m_Relations[i].m_SourceNodeNo, m_Relations[i].m_TargetNodeNo);
	   m_Relations[i].m_Valency.m_Direction = A_C;
	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
			m_Relations[i].m_Valency.m_RelationStr = "PROPERT";
   }
   else
   if  (    ( m_Relations[i].m_SyntacticRelation == "ОДНОР_ПРИЛ" ) )
   {
	   m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType  = MNA;
	   m_Nodes[m_Relations[i].m_SourceNodeNo].m_MNAType  = SimpleMNA;

   }
   else
   if  ( m_Relations[i].m_SyntacticRelation == "НАРЕЧ_ГЛАГОЛ" )
   {
	   	   if (m_Relations[i].m_Valency.m_RelationStr.empty())
				m_Relations[i].m_Valency.m_RelationStr = "ASPECT";
   };


   

};


void CRusSemStructure::ApplySubordinationCases ()
{
	// если в отношении прописан конкретный падеж, то нужно удалить граммемы и грам-коды, которые 
	// противоречат этому падежу 
	for (long i=0;  i < m_Relations.size(); i++)
	{
		long MainRelNo = i;
		if (m_Nodes[m_Relations[i].m_SourceNodeNo].IsMNA_ButNotCHEM())
		{
			CRelSet R = GetIncomingRelations(m_Relations[i].m_SourceNodeNo, false);
			if (R.m_RelsCount > 0) MainRelNo = R.m_Rels[0];
		};

		// на сравнительную степень не будем проецировать падежи
		if (m_Nodes[m_Relations[MainRelNo].m_TargetNodeNo].HasOneGrammem(rComparative)) continue;

		if (m_Relations[MainRelNo].m_Valency.IsFromDict())
		{
			TCortege& C = m_Relations[MainRelNo].m_SynReal.m_Cortege;
			if (C.m_DomItemNos[1] == -1) continue;
			const CRossHolder* RossHolder  =  m_Relations[MainRelNo].m_Valency.m_RossHolder;
			long CaseItemNo = -1;
			if (   !IsCase(RossHolder, RossHolder->GetSynFet(C) ) ) 
			{

				if (RossHolder->IsLemGroupBegining(RossHolder->GetSynFet(C))) 
				{
					string s =  RossHolder->GetDomItemStrInner(RossHolder->GetSynFet(C)); 
					int j = s.find("+");
					if (j==-1) continue;
					s.erase(0, j+1);
					CaseItemNo = RossHolder->GetItemNoByItemStr (s.c_str(), "D_CASE");
				};
			}
			else
				CaseItemNo = RossHolder->GetSynFet(C);

			if (CaseItemNo == -1) continue;

			QWORD Grammems = GetCaseGrammem(RossHolder, CaseItemNo);

			// если у глагола есть отрицание, и он  валентен на п_доп, тогда 
			// он может управлять как номинативом, так и генитивом
			if (m_Nodes[m_Relations[MainRelNo].m_SourceNodeNo].HasRelOperator("НЕ"))
			 if (RossHolder->GetSynRel( C) ==  RossHolder->DirectObjSynONo)
				Grammems = _QM(rAccusativ) | _QM(rGenitiv);
			 // запускаем удаление анкодов, только если  внешние и внутренние граммемы равны,
			 // а в случае "много домов сгорело" эту функцию запускать не нужно, поскольку
			 // анкоды и граммемы не совпадают.
			if (    (m_Nodes[m_Relations[i].m_TargetNodeNo].m_MainWordNo != -1)
				 && (m_Nodes[m_Relations[i].m_TargetNodeNo].GetGrammems() == m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[m_Nodes[m_Relations[i].m_TargetNodeNo].m_MainWordNo].GetAllGrammems())
			   )
			  DeleteAllGramCodesWithoutTheGrammems (m_Nodes[m_Relations[i].m_TargetNodeNo], Grammems);
			m_Nodes[m_Relations[i].m_TargetNodeNo].DeleteGrammems( rAllCases);
			m_Nodes[m_Relations[i].m_TargetNodeNo].AddGrammems ( Grammems );
		};
	};

	// полтора рубля - здесь нужно изменить мн. на ед.
	for (long i=0;  i < m_Relations.size(); i++)
		if	(		(m_Relations[i].m_Valency.m_RelationStr == "QUANTIT")		  
				&&	m_Nodes[m_Relations[i].m_TargetNodeNo].IsPrimitive()
				&&	(m_Nodes[m_Relations[i].m_TargetNodeNo].m_Words[0].m_Word == "1.5")
			)
		{
			m_Nodes[m_Relations[i].m_SourceNodeNo].DeleteGrammems ( rAllNumbers);
			m_Nodes[m_Relations[i].m_SourceNodeNo].AddOneGrammem ( rSingular);
		};

 
   // если русское подлежащее было неизменяемым, а в него идет стрелка  от сказуемого, 
   // которое стоит строго либо в мн., либо  ед., тогда нужно удалить граммему, 
   // противоречающую сказуемому
   // Например, "пальто порвалось"
   //			"плато было красным"
   for (long i=0;  i < m_Relations.size(); i++)
	  if ( IsRusSubj (i) )
	  {
		  CRusSemNode& Source = m_Nodes[m_Relations[i].m_SourceNodeNo];
		  CRusSemNode& Target = m_Nodes[m_Relations[i].m_TargetNodeNo];
		  if (Target.m_NodeType  == MNA) continue;
		  bool b_pl = Source.HasOneGrammem(rPlural);
		  bool b_sg = Source.HasOneGrammem (rSingular);
		  if (b_pl && b_sg) continue;

		  // проверяем на неизменяемость
		  // иностранная лексема всгда неизменяема
		  if (!Target.IsILENode())
		  {
			  if (!Target.IsPrimitive()) continue;
			  long ParadigmId = Target.m_Words[0].m_ParadigmId;
			  if (ParadigmId == -1) continue;
			  CFormInfo P;
			  if (! m_pData->GetRusLemmatizer()->CreateParadigmFromID(ParadigmId, P) ) continue;
			  if (P.GetCount() != 1) continue;
			//проверка не неизменяемость закончена
		  };

		  Target.DeleteGrammems( rAllNumbers );
		  if (b_pl)
			  Target.AddOneGrammem( rPlural);
		  else
			  Target.AddOneGrammem( rSingular);
	  };

   /*  
     если отношении стоял падеж с числом (например "Р_ед"),
	 тогда нужно спрецировать это число на выходной узел 
     Например, "чашечка кофе " Р_ед
   */
   for (long i=0;  i < m_Relations.size(); i++)
   if (m_Relations[i].m_Valency.IsFromDict())
   {
   		  const CRusSemRelation& R = m_Relations[i]; 
		  const CRossHolder* RossHolder =  R.m_Valency.m_RossHolder;
		  const TCortege& C = R.m_SynReal.m_Cortege;
		  long SynFet = RossHolder->GetSynFet(R.m_SynReal.m_Cortege);
		  if ( !IsCase(RossHolder, SynFet) ) continue;
		  long  CaseGrammems = GetCaseGrammem(RossHolder, SynFet);
		  if ((CaseGrammems & rAllNumbers) == 0) continue;
		  CRusSemNode& Target = m_Nodes[m_Relations[i].m_TargetNodeNo];
		  if ( !Target.HasGrammems (rAllNumbers) ) continue;

		  Target.DeleteGrammems( rAllNumbers );

		  if ((CaseGrammems & _QM(rPlural)) != 0) 
			  Target.AddOneGrammem( rPlural);
          else
			  Target.AddOneGrammem( rSingular );
	};


};


//  без контектса SF(газета) = 1. ORG 2. MEDINF, а в словосочетании
// "абзац газеты" -  должно остаться только SF(газета) = 1 MEDINF
// должен  исчезнуть, так как "вопрос" валентен на MEDINF.
// Наоборот, в словосочетании "акция газеты" должно остаться 
// SF(газета) = 1 ORG, поскольку "акция" валентна на ORG.
void CRusSemStructure::ApplySubordinationSemfets ()
{

	// если в отношении прописан конкретный набор SF, то нужно перечечь его  
	// с SF узла, в который входит это отношение и результат пересечения записать
	// в SF узла
	for (long RelNo=0;  RelNo< m_Relations.size(); RelNo++)
	{
		long MainRelNo = RelNo;
		if (m_Nodes[m_Relations[RelNo].m_SourceNodeNo].m_NodeType == MNA)
		{
			CRelSet R = GetIncomingRelations(m_Relations[RelNo].m_SourceNodeNo, false);
			if (R.m_RelsCount > 0) MainRelNo = R.m_Rels[0];
		};

		if (    m_Relations[MainRelNo].m_Valency.IsFromDict()
			&& (m_Relations[MainRelNo].m_Valency.m_UnitNo != ErrUnitNo)
		   )
		{
			const vector<QWORD>& SemFetsRel =  m_Relations[MainRelNo].m_SemFets;
			vector<QWORD> SemFetsNode = m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets;
			if (!SemFetsRel.empty())
				if (	   SemFetsNode.empty()
						|| m_Nodes[m_Relations[RelNo].m_TargetNodeNo].IsAnaphoricPronoun()
					)
				{
					m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets = SemFetsRel;
					continue;
				};

            // идем по всем дизъюнктам SF узла
			for (long i=0; i <SemFetsNode.size(); i++)
			{
				// идем по всем дизъюнктам SF отношения
				long k=0;
				for (; k < SemFetsRel.size(); k++)
					if  (SemFetsRel[k] & SemFetsNode[i]) break;
				
                if (k < SemFetsRel.size())
					SemFetsNode[i] = SemFetsRel[k] & SemFetsNode[i];
				else
				{
					SemFetsNode.erase(SemFetsNode.begin() +i);
					i--;
				};

			};

		   if (SemFetsNode.size() > 0)
				m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeSemFets = SemFetsNode;
			

		};
	};

	MoveDopRelationsBack(*this);


};


/*
Если есть глагол или краткая форма, управляющая инфинитивом, например:
 попробовал представить
 дай посмотреть
 удалось послать
 пытался вспомнить
 собирался доказать
 начал читать
 мог закрыть
 хочу начать
 успел уйти
 мешать говорить
 любит выпить
 не собиралался возвращаться
 продолжала вращаться
 просить уехать 
 должен ознакомиться 

 Если у этого глагола  есть управление дательным падежом, которое является не 
 первой валентностью,тогда считаем, что субъект главного глагола совпадает 
 с субъектом инфинитива. В противном случае, субъект, скорее всего, совпадает, с 
 валентностью, которая выражена дательным падежом, например:
мешать мне уйти -> SUB(я,уйти)я дам тебе посмотреть -> SUB(ты,посмотреть)он просил тебя уехать -> SUB(ты,уехать)
*/

bool HasSynFet (const CSemPattern& P, long ItemNo)
{
	
	for (long i=0;i < P.m_GramCorteges.size(); i++)
		if (P.GetSynFet(i) == ItemNo)
			return true;

     return false;
	
};

void CRusSemStructure::ApplyDopInfinitive ()
{
    for (long RelNo=0; RelNo < m_Relations.size(); RelNo++)
        if (   HasRichPOS (m_Relations[RelNo].m_TargetNodeNo, INFINITIVE)
            && (         IsVerbForm (m_Nodes[m_Relations[RelNo].m_SourceNodeNo])
            || m_Nodes[m_Relations[RelNo].m_SourceNodeNo].HasPOS(ADJ_SHORT)
            )
            )
        {
            long NodeNo = m_Relations[RelNo].m_SourceNodeNo;
            vector<long> Rels;
            GetOutcomingRelations(NodeNo, Rels, false);
            long DativSubjNodeNo = -1;
            long ParentSubjNodeNo = -1;
            bool HasDativNotFirst = false;
            for (BYTE  ValencyNo = 0; ValencyNo < m_Nodes[NodeNo].m_Vals.size(); ValencyNo++)
            { 
                CSemPattern P;
                GetActantPattern (NodeNo, ValencyNo, P);
                const CValency& V= m_Nodes[NodeNo].m_Vals[ValencyNo];

                long i=0;
                for (;i <Rels.size(); i++)
                    if (m_Relations[Rels[i]].m_Valency == V)
                        break;

                if (i == Rels.size()) i = -1;


                if (ValencyNo == 0)
                    if (i != -1)
                        ParentSubjNodeNo = m_Relations[Rels[i]].m_TargetNodeNo;


                if (ValencyNo > 0)
                    if (!HasDativNotFirst)
                    {
                        HasDativNotFirst |= HasSynFet(P,V.m_RossHolder->DativNo);
                        if (HasDativNotFirst)
                            if ( i != -1)
                                DativSubjNodeNo = m_Relations[Rels[i]].m_TargetNodeNo;
                    };

            };
            long 	InfSubNodeNo = -1;
            if (HasDativNotFirst)
                InfSubNodeNo = DativSubjNodeNo;
            else
                InfSubNodeNo = ParentSubjNodeNo;
            if (InfSubNodeNo == -1) continue;


            NodeNo = m_Relations[RelNo].m_TargetNodeNo;
            if (m_Nodes[NodeNo].m_Vals.size() == 0) continue;
            vector<CValency> ValencyMisses;
            GetValencyMisses (NodeNo, ValencyMisses);
            // у инфинитва обычно не заполненна первая валентность, но если вдруг 
            // заполнена, то это не наш случай
            if (!_find (ValencyMisses, m_Nodes[NodeNo].m_Vals[0]) ) continue;
            m_DopRelations.push_back(CRusSemRelation(m_Nodes[NodeNo].m_Vals[0],  NodeNo,  InfSubNodeNo,  ""));     
            CSemPattern P;
            GetActantPattern (NodeNo, 0, P);
            m_DopRelations[m_DopRelations.size() - 1].m_SemFets = GetIndexedSemFets(P, true, false);
        };


};

long CRusSemStructure::GetValencyNoWithouProhibited (long NodeNo, long ValencyNo) const
{
  vector<long> Rels;
  GetOutcomingRelations(NodeNo, Rels);

  long Curr =	0; // текущий номер совместной валентности
  for (long i=0;i < m_Nodes[NodeNo].m_Vals.size(); i++)
  {
	  long j=0;
	   for (; j <  Rels.size(); j++)
        if (m_Relations[Rels[j]].m_Valency.m_RelationId != m_Nodes[NodeNo].m_Vals[i].m_RelationId)
		if  (!m_Nodes[NodeNo].RelsCanBeTogether 
							(m_Relations[Rels[j]].m_Valency.m_RelationId, 
							 m_Nodes[NodeNo].m_Vals[i].m_RelationId))
							 break;
	 if ( j == Rels.size()) 
		 Curr++;
	 if (Curr == ValencyNo+1)
		 return i;
  };

  return -1;
};

void CRusSemStructure::ApplyDopLexFuncts ()
{
	for (long RelNo=0; RelNo < m_LexFuncts.size(); RelNo++)
		if (IsOper(m_LexFuncts[RelNo].m_LexFunctName) )
		{
			long SubjNodeNo = GetRusSubj(m_LexFuncts[RelNo].m_TargetNodeNo);
			long NodeNo = m_LexFuncts[RelNo].m_SourceNodeNo;
			if (SubjNodeNo == -1) continue;
			CSemPattern P;
			long ValencyNo = -1;
			if (IsOper1(m_LexFuncts[RelNo].m_LexFunctName) )
			  ValencyNo = GetValencyNoWithouProhibited(NodeNo,0);
			else
			   if (IsOper2(m_LexFuncts[RelNo].m_LexFunctName) )
				 ValencyNo =  GetValencyNoWithouProhibited(NodeNo,1);
			  else 
  			   if (IsOper3(m_LexFuncts[RelNo].m_LexFunctName) )
				 ValencyNo =  GetValencyNoWithouProhibited(NodeNo,2);
			   else 
				  continue;
            if (ValencyNo == -1) continue;
			GetActantPattern (NodeNo, ValencyNo, P);
			CRusSemRelation Rel (P.m_PatternValency, NodeNo,  SubjNodeNo,"");
			Rel.m_SemFets = GetIndexedSemFets(P, true, false);
			m_DopRelations.push_back(Rel);     
		};
}


void CRusSemStructure::FindQuestionClauses ()
{
   /*
    ищем первую клаузу предложения "кто бы ни пришел, я уйду"
   */
   for (long i=0; i< m_Clauses.size(); i++)
	if (m_Clauses[i].m_HasParticleBY)
	 if (m_Nodes[m_Clauses[i].m_BeginNodeNo].m_bQuestionWord)
	 {
		 vector<long> Nodes;
		 GetClauseRootsWithoutDeleted(i,Nodes);
		 if (Nodes.size() != 1)  continue;
		 if (!m_Nodes[Nodes[0]].HasRelOperator("НИ")) continue;
		 m_Clauses[i].m_bQuestion = true;
		 m_Nodes[Nodes[0]].m_RelOperators.push_back("no_matter"); 
		 m_Clauses[i].m_HasParticleBY = false;
	 };

	/*
	дальше рассматриваются только предложения с вопросительным знаком в конце
	 */
   if (m_SentenceMood != Interrogative) return;

   vector<long> HostClauses;
   for (long i=0; i< m_Clauses.size(); i++)
   	if (m_Clauses[i].m_HostClause  == -1)
	  HostClauses.push_back(i);

   
   for (long i=0; i< HostClauses.size(); i++)
    if (HostClauses.size() > 1)
	   for (long j=m_Clauses[HostClauses[i]].m_BeginNodeNo; (j < m_Clauses[HostClauses[i]].m_EndNodeNo) && ((j - m_Clauses[HostClauses[i]].m_BeginNodeNo) < ConjWordDistance); j++)
			if (m_Nodes[j].IsLemmaList("ЧТОБЫ",  "ЕСЛИ",  "ХОТЯ", "ПОСКОЛЬКУ", "ПОКА", "КОТОРЫЙ", ""))
			{
				HostClauses.erase(HostClauses.begin() +i);
			   i--;
			   break;
			};
       
   if (HostClauses.size() == 1)
   {
	   vector<long> SimilClauses;
	   SimilClauses.push_back(HostClauses[0]);

	   for (long i=0; i< m_Clauses.size(); i++)
		 if (    (m_Clauses[i].m_ClauseRuleNo != -1)
			  && (m_ClauseRules[m_Clauses[i].m_ClauseRuleNo].m_Name == "сочинение")
			)
		   for (long j=0; j< SimilClauses.size(); j++)
			   if (m_Clauses[i].m_HostClause  == SimilClauses[j])
				   SimilClauses.push_back(i);

	   for (long j=0; j< SimilClauses.size(); j++)
   	     m_Clauses[SimilClauses[j]].m_bQuestion = true;

   };


};


/*
 функция решает омонимию между повелительным и 
 изъявительным наклонением
 Проверяется наличие подлежащего.
*/
void CRusSemStructure::SolveImperativeHomonymy()
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++) 
	{
		if ( !m_Nodes[NodeNo].HasOneGrammem (rImperative) ) continue;
		if ( ! (m_Nodes[NodeNo].GetGrammems()&rAllTimes) ) continue;
		long SubjNodeNo = GetRusSubj(NodeNo);
		if (SubjNodeNo != -1)
		{
		 if (CanBeSubjectForImperative(SubjNodeNo, NodeNo))
			 continue;

		 m_Nodes[NodeNo].DeleteGrammems ( _QM(rImperative) );
		};
	};
};


void CRusSemStructure::dfs_in_clause(size_t NodeNo, vector<long>& Nodes) const
{
  Nodes.push_back(NodeNo);
  for (size_t i=0; i<m_Relations.size(); i++)
	if (m_Relations[i].m_SourceNodeNo == NodeNo)
     if ( !_find(Nodes, m_Relations[i].m_TargetNodeNo) )
		 if (m_Nodes[NodeNo].m_ClauseNo ==  m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo)
    	  dfs_in_clause(m_Relations[i].m_TargetNodeNo, Nodes);
};


void CRusSemStructure::ConvertParticipleTreeToClause()
{
	int ClauseCount = m_Clauses.size();
	for (long ClauseNo=0; ClauseNo < ClauseCount; ClauseNo++)
		for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo; NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
			if ( m_Nodes[NodeNo].HasPOS(PARTICIPLE) )
			{
				  /*
				    причастия без зависимых слов будем пропускать
				  */
				  vector<long> Rels;
				  GetOutcomingRelations(NodeNo, Rels, false);
				  long i =0;
				  for (; i< Rels.size(); i++)
					  if (!m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo].IsAbstract())
						break;

				  if (i == Rels.size()) continue;


				  vector<long> Roots;
				  GetClauseRootsWithoutDeleted(ClauseNo,Roots);
				  if (Roots.size() != 1) continue;
				  // если причастие уже стоит во главе клаузы, тогда надо выйти
				  if (Roots[0] == NodeNo) continue;
				  vector<long> iNodes;
				  dfs_in_clause (NodeNo, iNodes);
				  vector<CRusSemNode> Nodes;
			      SetNodeToDeleteFalse();
				  int NewClauseNo = m_Clauses.size();
				  int NodesCount = m_Nodes.size();
				  for (i =0; i < iNodes.size(); i++) 
				  {
					  Nodes.push_back(m_Nodes[iNodes[i]]);
					  m_Nodes[iNodes[i]].m_bToDelete = true;
					  Nodes.back().m_ClauseNo  = NewClauseNo;
					  MoveAllRelations(iNodes[i], -iNodes[i] - 1);
				  };
				  DelNodesToDelete();
			
			  	  m_Clauses.push_back (CRusSemClause());

				  for (i =0; i < Nodes.size(); i++) 
				  {
					long NewNodeNo = InsertNode(Nodes[i]);
					
					if (i ==  0)
						m_Clauses.back().m_ClauseSyntaxTop = NewNodeNo;

					 MoveAllRelations(-iNodes[i] - 1, NewNodeNo);
					 
				  };
				  m_Clauses.back().m_HostClause = ClauseNo;
				  for (i=0; i < m_ClauseRules.size(); i++)
					  if (m_ClauseRules[i].m_Name == "причастный оборот")
					  {
						  m_Clauses.back().m_ClauseRuleNo = i;
						  break;
					  };


			  	 AssertValidGraph();

			};

};

