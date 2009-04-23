#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"

//#pragma optimize( "", off )

//#define Trace 0
#define Trace 1


// Загружает одну GFi(j). После этого оставляет только те строки из значения, которые  
// помечены "(стр)" или равны строке Copul.
// Если  значение GFi(j)  начинается со строки "(стр)", то это означает, что эту грамматическую характеристику
// нужно использовать только для пассивной валентности.

bool CRusSemStructure::LoadFromDictForPassive(WORD UnitNo, BYTE LeafId, BYTE BracketLeafId, CSemPattern& P)
{
	assert (UnitNo != ErrUnitNo);

	P.InitSemPattern(GetRossHolder(Ross), UnitNo, LeafId, BracketLeafId);
	P.LoadGramFromDict();
	
	for (size_t i=0; i < P.m_GramCorteges.size(); i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] != P.m_pRossDoc->CopulNo)
			if (P.m_GramCorteges[i].m_DomItemNos[0] != P.m_pRossDoc->PassiveNo)
			{
				P.m_GramCorteges.erase(P.m_GramCorteges.begin() + i );
				i--;
			}
			else
				for (size_t k=0; k < 9; k++) 
					P.m_GramCorteges[i].m_DomItemNos[k] = P.m_GramCorteges[i].m_DomItemNos[k+1];

	return    P.m_GramCorteges.empty();
};

// функция, которая определяет валентную структуру для пассивной формы глагола
// (CDictUnitInterp::m_bPassiveForm)
void  CRusSemStructure::InitPassivePattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P)
{
	if (m_Nodes[NodeNo].GetType() != Ross) return;

	WORD UnitNo = m_Nodes[NodeNo].GetUnitNo();

	assert (UnitNo != ErrUnitNo);

	if   (ValencyNo == 0)
	{
		if (m_Nodes[NodeNo].m_Vals.size()< 2) return;
		LoadFromDictForPassive(UnitNo, 2, 0, P);
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[1];

		if (P.m_GramCorteges.size() == 0)
			P.m_GramCorteges.push_back(GetSubjCortege(P.m_pRossDoc));

		/*
		"отмечается, что продолжительность составляет 55,5 лет."
		Здесь  вся вторая клауза занимает место подлежащего. Поэтому все межклаузные 
		GF2 должны перейти к подлежащему.	  */
		CSemPattern Q;
		Q.InitSemPattern(GetRossHolder(Ross), UnitNo, 2, 0);
		Q.LoadGramFromDict();
		for (long i=0; i < Q.m_GramCorteges.size(); i++)
		{
			string SynRel = Q.GetSynRelStr(i);
			string SynFet = Q.GetSynFetStr(i);
			if (  (SynRel ==  "ПРИД_ПР")
				||(    (SynFet.length() > 4)
				&& (    (SynFet.substr(SynFet.length()-3) ==  "+ГГ")
				||  (SynFet.substr(SynFet.length()-4) ==  "+инф")
				)
				)
			 )
			{
				P.m_GramCorteges.push_back(Q.m_GramCorteges[i]);

			};
		};
	}
	else if (ValencyNo == 1)
	{
		if (m_Nodes[NodeNo].m_Vals.size() < 1) return;
		LoadFromDictForPassive(UnitNo, 1, 0, P);
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[0];
		if (P.m_GramCorteges.empty())
			P.m_GramCorteges.push_back(GetInstrObj(P.m_pRossDoc));
		P.m_PatternValency.m_InstrAgent = true;
	}
	else 
	{
		P.m_PatternValency = m_Nodes[NodeNo].m_Vals[ValencyNo];
		P.InitSemPattern(GetRossHolder(Ross), UnitNo, m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, m_Nodes[NodeNo].m_Vals[ValencyNo].m_BracketLeafId);
		P.LoadGramFromDict();
	};

};






inline TCortege GetFullGleicheRightCortege (const CRossHolder* RossDoc, BYTE LeafId)
{
    TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;;
	C.m_LeafId = LeafId;
	C.m_LevelId = 0;
	C.m_DomItemNos[0] = RossDoc->RightDirectionNo;
	C.m_DomItemNos[1] = RossDoc->ReverseSynONo;
	C.m_DomItemNos[2] = RossDoc->NounGrpNo;
	return C;
};



void  CRusSemStructure::GetActantPattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P)
{
	assert (ValencyNo < m_Nodes[NodeNo].m_Vals.size());

	P.m_SourceNo = NodeNo;
	P.m_GramCorteges.clear();
	P.m_LexFets.clear();

	if (m_Nodes[NodeNo].GetType() != NoneRoss)
		P.m_pRossDoc = GetRossHolder(m_Nodes[NodeNo].GetType());
	else
		P.m_pRossDoc = GetRossHolder(Ross); // по умолчанию все в РОСС

	P.m_PatternValency = m_Nodes[NodeNo].m_Vals[ValencyNo];

	if  (     (HasRichPOS(P.m_SourceNo, ADJ_SHORT))
			&& (ValencyNo == 0)
		)
	{
		// получение первой валентности для краткого прилагательного
		if (m_Nodes[P.m_SourceNo].m_Vals.size() >= 1) 
		{
			P.m_GramCorteges.push_back(GetSubjCortege(P.m_pRossDoc));
			P.m_PatternValency = m_Nodes[P.m_SourceNo].m_Vals[0];
		}
	}
	else 
		if (m_Nodes[NodeNo].GetUnitNo() != ErrUnitNo)
		{
			if  (     m_Nodes[NodeNo].IsPassiveVerb() && (ValencyNo < 2) )
				InitPassivePattern(NodeNo, ValencyNo, P);
			else
				if (!P.m_PatternValency.m_IsAddition)
				{
					P.InitSemPattern(P.m_pRossDoc, m_Nodes[NodeNo].GetUnitNo(), P.m_PatternValency.m_LeafId, P.m_PatternValency.m_BracketLeafId);
					P.LoadGramFromDict();
				}
				else
				{
					P.InitSemPattern(GetRossHolder(Ross), P.m_PatternValency.m_UnitNo, P.m_PatternValency.m_LeafId, P.m_PatternValency.m_BracketLeafId);
					P.LoadGramFromDict();
				}
		}

		/*
		Инициализируем GF причастий, которые не являются вершинами клауз. Это нужно,
		чтобы потроить отношение между ИГ и причастием, которое стоит в препозиции.
		Например, "расположенные в разных местах компьютеры"
		Если причастие  страдательное, тогда вторая валентность
		делается "> X!:ИГ", иначе первая валентносто делается  "> X!:ИГ"
		*/
		if (m_Clauses[m_Nodes[NodeNo].m_ClauseNo].m_ClauseSyntaxTop != NodeNo)
			if (HasRichPOS(NodeNo,PARTICIPLE))
				if (m_Nodes[NodeNo].HasOneGrammem (rActiveVoice) )
				{
					if (ValencyNo == 0)
					{
						P.m_GramCorteges.clear();
						P.m_GramCorteges.push_back(GetFullGleicheRightCortege(P.m_pRossDoc, 1));
					};
				}
				else
				if (m_Nodes[NodeNo].HasOneGrammem ( rPassiveVoice ))
				{
					/*
					для пассивного причастия (например, "подписанный"), если валентность 
					выражена прямым дополнением, значит нужно конвертировать ее в > X!:ИГ,
					например:
					"подписанное группой  обращение"
					*/
					for (long i = 0; i < P.m_GramCorteges.size(); i++)
						if (P.GetSynRel(i) == P.m_pRossDoc->DirectObjSynONo)
						{
							P.m_GramCorteges.clear();
							P.m_GramCorteges.push_back(GetFullGleicheRightCortege(P.m_pRossDoc, 2));
							break;
						};
				}

	P.LoadSemFromDict();

};



bool IsEqualWithPhonetics (const string& TextItem,  const string& FormatItem)
{
	return         (TextItem ==  FormatItem)
		        || (FormatItem == "*")
		        || ( (TextItem == "СО") && (FormatItem == "С") )
				|| ( (TextItem == "КО") && (FormatItem == "К") )
				|| ( (TextItem == "ВО") && (FormatItem == "В") )
				|| ( (TextItem == "ПОДО") && (FormatItem == "ПОД") )
				|| ( (TextItem == "ИЗО") && (FormatItem == "ИЗ") )
				|| ( (TextItem== "ПЕРЕДО") && (FormatItem == "ПЕРЕД") )
				|| ( (TextItem == "ОБ") && (FormatItem == "О") )				
				|| ( (TextItem == "ОБО") && (FormatItem == "О") )
				|| ( (TextItem == "ОБО") && (FormatItem == "OBJ") )				
				|| ( (TextItem == "НАДО") && (FormatItem == "НАД") )
				|| ( (TextItem == "ОТО") && (FormatItem == "ОТ") );
}




bool CRusSemStructure::CheckSemanOfPrep(string RelationStr, bool IsAdditionValency, long NodeNo, long&  PrepNo)
{
   assert (m_Nodes[NodeNo].HasSomePrep());

   PrepNo = -1;

   if     (     IsLocSemRel(RelationStr) )
   {
   	   if (IsAdditionValency && !IsLocNode(NodeNo)) return false;

	   string    RelStr;
		// приравниваем IN-DIRECT и TRG-PNT
	   if (RelationStr == "IN-DIRECT") RelationStr = "TRG-PNT";	   
	   for (long i=0; i <m_Nodes[NodeNo].m_SynReal.m_Preps.size(); i++)
	   {
		  long UnitNo = m_Nodes[NodeNo].m_SynReal.m_Preps[i].m_UnitNo;
	   	  if  (		(HasItem(OborRoss, UnitNo, "SF",RelationStr,"D_SEM_REL", 0 , 0))
				||	(   (		RelationStr == "IN-DIRECT" )
							&&	(HasItem(OborRoss, UnitNo, "SF","TRG-PNT","D_SEM_REL", 0 , 0))				   
						)
				||	(		( RelationStr == "TRG-PNT" )
						&&	(HasItem(OborRoss, UnitNo, "SF","IN-DIRECT","D_SEM_REL", 0 , 0))				  
					)
			  )
		  { 
		     PrepNo = i;
			 return true;
		  };
	   };
	   return false;
   }
   else
	if (RelationStr == "TIME")		  
		return m_Nodes[NodeNo].IsMainTimeRossNode();
	else 
		return true;
};

bool CRusSemStructure::CheckDirection (long NodeNo1, long NodeNo2, string Direction)  const 
{
  if (   Direction == ">>" )
  {
	  if (NodeNo1 >= m_Clauses[ m_Nodes[NodeNo1].m_ClauseNo ].m_EndNodeNo  - 1)
		  return false;
	  long nd_curr = GetDefiniteRightHostByNotWeakSynRelation(NodeNo1+1);
	  if (nd_curr != NodeNo2) return false;
  }
  else
  if ( Direction == ">" )
  {
	  if (m_Nodes[NodeNo2].GetMinWordNo() < m_Nodes[NodeNo1].GetMinWordNo()) return false;
  }
  else
  if ( Direction == "<" )
  {
	  if (m_Nodes[NodeNo1].GetMinWordNo() < m_Nodes[NodeNo2].GetMinWordNo()) return false;
  }
  else
  if (   Direction == "<<" )
  {
	  long u = m_Nodes[NodeNo1].GetMinWordNo() - m_Nodes[NodeNo2].GetMaxWordNo();
      if  ((u < 0) || (u > 1)) return false;
  }
  else
  if (   Direction == "^" )
  {
      if    (   (m_Nodes[NodeNo2].GetMaxWordNo() > m_Nodes[NodeNo1].GetMaxWordNo())
		     || (m_Nodes[NodeNo1].GetMinWordNo() > m_Nodes[NodeNo2].GetMaxWordNo()))
      return false; 
  }
  return true;
};


bool CRusSemStructure::CheckPatternReverseGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo) 
{
	 long SynFet = P.GetSynFet(SynRealization.m_CortegeNo);
	 string SynFetStr  = P.GetSynFetStr(SynRealization.m_CortegeNo);
	 TCortege& C = P.m_GramCorteges[SynRealization.m_CortegeNo];

	 if  (   m_Nodes[P.m_SourceNo].m_bFullGleiche
		  || HasRichPOS (P.m_SourceNo, ADV) 
		 )
	   if (   SynFetStr == "ПРИЛ"
		   || SynFetStr == "ИГ"
		  )
		 {
		    /* Связь между  существительное и прилагательным  не может идти через  предлог.
			   (т.е. предлог находился между прилагательным и существительным)
			   например, "красивом в доме",   здесь "красивом" не может относиться к "доме",
			   или:  "доме в красивом"
			*/
		    long nd1 = NodeNo; 
			long nd2 = P.m_SourceNo;
			if (nd2 < nd1) swap (nd1, nd2);
			if (    m_Nodes[nd2].HasSomePrep()  
				&&  (m_Nodes[nd1].GetMinWordNo() < m_Nodes[nd2].m_PrepWordNo) 
				&&  (m_Nodes[nd2].m_PrepWordNo < m_Nodes[nd2].GetMinWordNo()) // постпредлог
			   )
			   return false;

			if (m_Nodes[P.m_SourceNo].m_bFullGleiche)
				if ( !FullGleiche(NodeNo, P.m_SourceNo) )
				 return false;
		 };
	 // один, оба
	 // к сожалению, у числительных (и у этих,в частности) 
	  //не стоит граммемы  рода в морфологии, поэтому приходится 
	 // проверять это вручную
 	 if (HasRichPOS(P.m_SourceNo, NUMERAL))
	 {
		 if (m_Nodes[P.m_SourceNo].GetGrammems() & rAllGenders)
		   if ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllGenders & m_Nodes[NodeNo].GetGrammems()) == 0)
			   return false;

		  if (   m_Nodes[P.m_SourceNo].IsPrimitive() )
		  {
			  if (m_Nodes[P.m_SourceNo].m_Words[0].m_Lemma == "ОДИН")
				 if ( !m_Nodes[P.m_SourceNo].HasOneGrammem (rSingular) )
					 return false;

			  if (m_Nodes[P.m_SourceNo].m_Words[0].m_Lemma == "ОБА")
			     if ( !m_Nodes[P.m_SourceNo].HasOneGrammem (rPlural) )
				     return false;
		  };
	 };
 	


 
	 // в клаузе COMPARATIVE_T  сравнительная степень играет роль главного слова
	if (m_Nodes[NodeNo].m_bCompAdj)
	 if (SynFetStr == "ГГ")
	  if (m_Clauses[m_Nodes[NodeNo].m_ClauseNo].m_ClauseType == COMPARATIVE_T)
		  return true;

	  
	// откулчаем первую валентностость у компаративов
	if (    m_Nodes[P.m_SourceNo].m_bCompAdj 
		&&  !HasRichPOS(P.m_SourceNo, NUMERAL)
	   )
	  if (P.m_PatternValency.m_LeafId == 1)
	    return  false;


     // для сравнительных степеней есть отдельные правила  (BuildOutcomingOf_CHEM_Obor и BuildOutcomingOf_COMP_Obor)
	 // однако бывают специальные GF только для сравнительнызх степеней (маленький и большой)
	 if (!P.m_pRossDoc->IsCompAdjCortege(C))
	    if (    m_Nodes[P.m_SourceNo].m_bCompAdj
			&&  !HasRichPOS(P.m_SourceNo, NUMERAL)
			)  return false;

	if ( IsVerbForm (m_Nodes[NodeNo]) ) 
	  if (SynFetStr == "ГГ_нсв")
		  return   m_Nodes[NodeNo].HasOneGrammem (rNonPerfective);
	  else
		 if (SynFetStr == "ГГ_св")
			 return  m_Nodes[NodeNo].HasOneGrammem (rPerfective);
		 else
 		   if (SynFetStr == "ГГ_личн")
			 return  !HasRichPOS (NodeNo,INFINITIVE);
  			else
 		      if (SynFetStr == "инф")
			   return  HasRichPOS (NodeNo,INFINITIVE);
			  else
   		        if (SynFetStr == "ПРОП_отр")
		          return  m_Nodes[NodeNo].HasRelOperator("НЕ");
				else
				  if ( (SynFetStr == "ГГ") || (SynFetStr == "ПРОП"))
					 return  true;

	
	// ГГ может обозначать краткое прилагательное
	if (HasRichPOS(NodeNo, ADJ_SHORT))
	 if ( (SynFetStr == "ГГ") || (SynFetStr == "ПРОП"))
		  return true;

	if (HasRichPOS (NodeNo, PREDK))	
		if ( (SynFetStr == "ГГ") || (SynFetStr == "ПРОП"))
				return  true;

    if (HasRichPOS(NodeNo, ADJ_SHORT))
       if (SynFetStr == "КР_ПРИЛ")  
		  return  true;
	 else
	    if (SynFetStr == "ПРОП_отр")
		    return  m_Nodes[NodeNo].HasRelOperator("НЕ");


	if (SynFetStr == "ИГ")
	  return  HasRichPOS(NodeNo, NOUN) || HasPOS(NodeNo, PRONOUN);

	// "такой красивый"
	if (SynFetStr == "ПРИЛ")
	  return  HasRichPOS(NodeNo, ADJ_FULL);

	// здесь нужно брать внутренние граммемы. Вообще, во всей этой процедуре нужно так делать.
	// Просто  это актуально только для этих двух условий.
   if (SynFetStr == "Р_ед")
   {
	   // не должна собираться конструкция типа "сколько у тебя",
	   // где "сколько" пассивно валентно на Р_ед, а "тебя" - это рд, ед
	  if (m_Nodes[NodeNo].HasSomePrep())
		  if (P.m_SourceNo < NodeNo)
			  return false;
	  return    (HasRichPOS(NodeNo, NOUN) || HasPOS(NodeNo, PRONOUN)) 
	         && ( ( m_Nodes[NodeNo].GetInnerGrammems() & ( _QM(rSingular) |  _QM(rGenitiv) )) == ( _QM(rSingular) |  _QM(rGenitiv) ));
   };

   if (SynFetStr == "Р_мн")
   {
   	  // не должна собираться конструкция типа "сколько у тебя",
	  // где "сколько" пассивно валентно на Р_ед, а "тебя" - это рд, ед
	  if (m_Nodes[NodeNo].HasSomePrep())
		  if (P.m_SourceNo < NodeNo)
			  return false;

	  return    (HasRichPOS(NodeNo, NOUN) || HasRichPOS(NodeNo, PRONOUN)) 
	         && ( (m_Nodes[NodeNo].GetInnerGrammems() & ( _QM(rPlural) |  _QM(rGenitiv) )) == ( _QM(rPlural) |  _QM(rGenitiv))  );
   };

	return false;
};


bool CRusSemStructure::IsHumanName (const CRusSemNode& N)  const
{
    return		(N.m_SynGroupTypeStr == NAMES)
		   ||	N.HasOneGrammem(rSurName) 
	       ||	N.HasOneGrammem(rName);
};


bool CRusSemStructure::CanBeSubjectForImperative (long NodeNo, long SourceNodeNo)  const
{
	/*
	 нужно особо рассмотреть случай типа "Будь он с нами, мы бы здесь не сидели"
	 Надо проверить, что подлежащее стоит сразу после императива, и потом есть 
	 еще клауза, которая имеет "бы"
	*/
	bool bDir = CheckDirection(SourceNodeNo, NodeNo, ">>");

	long ClauseNo = m_Nodes[NodeNo].m_ClauseNo+1;

	for (; ClauseNo < m_Clauses.size(); ClauseNo++)
		if (m_Clauses[ClauseNo].m_HasParticleBY)
			break;

	if (   bDir && ClauseNo < m_Clauses.size() )
	   return true;

	const CRusSemNode& N = m_Nodes[NodeNo];
	return     N.IsLemma("ЧЕРТ")  // черт тебя побери
		   ||  (bDir && N.IsLemma("ТЫ"))    // иди ты в баню
		   ||  N.IsLemma("БОЖЕ")    // Боже помилуй полярников.
		   ||  (bDir && N.IsLemma("ВЫ"));   // идите вы в баню
};

bool CanBeMorphSubject (const CRusSemNode& Node )
{
	if (Node.IsPrimitive())
	{
		// "ничего не произошло"
		if (Node.m_Words[0].m_Word == "НИЧЕГО") return true;
		// "никто не пришел"
		if (Node.m_Words[0].m_Word == "НИКТО") return true;
	};
     if (	  !Node.HasOneGrammem(rNominativ)
		   || Node.IsLemma("ЕГО")
		   || Node.IsLemma("ИХ") 
		   || Node.IsLemma("ЕЕ") 
		   || (     Node.HasPOS(PRONOUN_P) 
				&& (   !Node.IsPrimitive()
					|| !Node.m_Words[0].IsRusSubstPronounP()
				   )
				  
			  )
		)
		return false;

	return true;
}

bool CRusSemStructure::CheckPatternGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo) 
{
	SynRealization.SetEmpty();

	TCortege& C = P.m_GramCorteges[SynRealization.m_CortegeNo];


	if (P.m_pRossDoc->IsVerbFet(C.m_DomItemNos[0]))
		if (C.m_DomItemNos[0] == P.m_pRossDoc->NegativeNo) 	
			if (!m_Nodes[P.m_SourceNo].HasRelOperator("НЕ"))
				return false;

	if (P.m_pRossDoc->IsPosition(C.m_DomItemNos[0]))
		if (!CheckDirection  (P.m_SourceNo, NodeNo, P.m_pRossDoc->GetDomItemStrInner(C.m_DomItemNos[0])) )
			return false;


	if (P.m_pRossDoc->IsCompAdjCortege(C))
		if ( !m_Nodes[NodeNo].HasOneGrammem(rComparative) )
			return false;

	if ( P.HasSemFet("KEY") )	  
		if (m_Nodes[NodeNo].m_SynGroupTypeStr == KEYB)
			return true;

	if  (P.m_PatternValency.m_RelationStr == "NAME")    
		if(!IsHumanName(m_Nodes[NodeNo]) )
			return false;



	if  (P.m_PatternValency.m_RelationStr == "IDENT")	  
		if	(		(		(m_Nodes[NodeNo].m_Words[0].m_CharCase == LowLow)
						&&	!m_Nodes[NodeNo].m_Words[0].m_ILE
						&&	!m_Nodes[NodeNo].m_bQuoteMarks
					)
				||  m_Nodes[NodeNo].HasSomePrep()
				||  IsTimeAdverbial(NodeNo)
				|| (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
				||  ( IsHumanName(m_Nodes[NodeNo]) &&	!m_Nodes[NodeNo].m_bQuoteMarks) // Сухогруз "Михаил Лермонтов"
			)
			return false;

	// причастие не может иметь валентность на подлежащее
	if (     (C.m_DomItemNos[0] == P.m_pRossDoc->SubjSynONo) 
		&& HasRichPOS(P.m_SourceNo, PARTICIPLE)
		)
		return false;

	//у инфинитива нет валентности на подлежащее
	if  (    (C.m_DomItemNos[0] == P.m_pRossDoc->SubjSynONo) 
		&& (     HasRichPOS(P.m_SourceNo, INFINITIVE)
		||  HasRichPOS(P.m_SourceNo, ADVERB_PARTICIPLE)
		)

		)
		return false;

	/*
	проверяем, что актант находится в нужном отрезке, если
	этот отрезок был определен
	*/
	if(    m_Nodes[NodeNo].IsWordContainer()
		&& m_Nodes[P.m_SourceNo].IsWordContainer()
		&& (m_Nodes[P.m_SourceNo].m_MinActantWordNo != -1)
		)
	{
		long WordNo = m_Nodes[NodeNo].m_Words[0].m_SynWordNo;
		if (! (   (m_Nodes[P.m_SourceNo].m_MinActantWordNo < WordNo)
			&& ( WordNo < m_Nodes[P.m_SourceNo].m_MaxActantWordNo)
			)
			)
			return false;
	};


	// пока  будем считать, что  у нас структура  типа 
	//  син_отн:уточнение 
	//  син_отн:MUA:уточнение  //  позиция син_отн:уточнение
	long SynRelNo = P.GetSynRel(SynRealization.m_CortegeNo);
	string SynRelStr  = P.m_pRossDoc->GetDomItemStrInner(SynRelNo);
	long SynFet = P.GetSynFet(SynRealization.m_CortegeNo);
	string SynFetStr  = P.GetSynFetStr(SynRealization.m_CortegeNo);
	const CRusSemClause& Clause = m_Clauses[m_Nodes[NodeNo].m_ClauseNo];
	if (P.m_pRossDoc->ReverseSynONo == SynRelNo)
		return CheckPatternReverseGramFetLine (P,  SynRealization, NodeNo);


	/*
	рассматриваем случай тип "мама, красивая и радостная, пришла домой"
	*/

	if ( SynRelStr == "уточн" )	
		if ( SynFetStr == "ПРИЛ" )	
			if ( HasRichPOS(P.m_SourceNo, NOUN) )
				if ( HasRichPOS(NodeNo, ADJ_FULL)  )
				{

					if ( Clause.GetGramMatrix(NodeNo, P.m_SourceNo) & HaveCommaBetween )
						if ( !HaveNounNodeBetweenInOneClause(NodeNo, P.m_SourceNo) )
							if ( FullGleiche(NodeNo, P.m_SourceNo) )
								return true;
				};

	/*
	не будем строит отношения между узлами,которые разделяет только одна запятая,
	кроме  случаев
	1. вводного оборота или слова "Он, видимо, ушел"
	2. уточнения ("мама, красивая и радостная, пришла домой")
	(предыдущий случай)
	*/
	if (   m_Nodes[P.m_SourceNo].IsWordContainer()
		&& (Clause.GetGramMatrix(P.m_SourceNo, NodeNo) & HaveOnlyCommaBetween)  // только одна запятая
		&& !m_Nodes[P.m_SourceNo].m_bSaveIsParenthesis	// это не вводный оборот
		)  
		return false;


	if	(     IsTimeAdverbial(NodeNo)
		&&	( P.m_PatternValency.m_RelationStr != "TIME")	   /* свободные таймгруппы могут заполнять не только валентность времени*/
		&&		(		!m_Nodes[NodeNo].IsMainTimeRossNode()
		||	!m_pData->m_TimeUnits[m_Nodes[NodeNo].m_Colloc.GetRossInterp().m_UnitNo].m_bCanFillNotTimeValency
		)
		)
	{
		return false;
	};

	// предложная группа не может управлять ни чем, что идет до нее,
	// кроме случая c местоименным предикативом типа "нечего", например в форме "не о чем", 
	if (    m_Nodes[P.m_SourceNo].HasSomePrep() 
		&&  m_Nodes[NodeNo].IsWordContainer()	
		&&  (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
		&&  !HasRichPOS(NodeNo, ADJ_FULL) 
		&&  !HasRichPOS(NodeNo, NUMERAL) 
		&&  !HasRichPOS(NodeNo, PRONOUN_P)
		)
		return false;



	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo)
		&& (SynFet == P.m_pRossDoc->AccusativNo)  
		&& m_Nodes[NodeNo].HasOneGrammem(rGenitiv)
		)
		// переходный глагол с отрицанием может управлять с помощью родителного,
		if (     m_Nodes[P.m_SourceNo].HasRelOperator("НЕ") 

			//  иногда наличие "НЕ" у хозяина позволяет переходному глаголу управлять 
			//  с помощью родительного, а не винительного, например:
			//  "Он не терпит вмешательства"
			|| m_Clauses[m_Nodes[P.m_SourceNo].m_ClauseNo].m_bSaveClauseHasNegationInVerb
			)
			SynFet = P.m_pRossDoc->GenitivNo;

	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo)
		&& (SynFet == P.m_pRossDoc->AccusativNo)  
		)
		if (     m_Nodes[NodeNo].IsWordContainer()
			&&  (m_Nodes[NodeNo].m_MainWordNo != -1)
			&&  (       (m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma == "НЕЧЕГО")
			||    (m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma == "НЕКОГО")
			)
			&&  m_Nodes[NodeNo].HasOneGrammem(rGenitiv)
			)
			SynFet = P.m_pRossDoc->GenitivNo;


	if ( SynRelStr == "с_опр" )	
		if ( SynFetStr == "ПРИЛ" )	
			return false;



	if (SynRelNo == P.m_pRossDoc->SubjSynONo)
	{
		if (m_Nodes[NodeNo].HasSomePrep()) return false;
		if (HasIncomingNotWeakSynRelation (NodeNo)) return false;
		long SourceNodeNo = P.m_SourceNo;


		if (m_Nodes[P.m_SourceNo].m_NodeType != Copul)
			if (SynFet !=  P.m_pRossDoc->InfinitiveNo)
				if (!CanBeMorphSubject(m_Nodes[NodeNo]) )
					return false;
				else
				{
					if  (    (SynFetStr == "И_мн") 
						&& ! m_Nodes[NodeNo].HasOneGrammem (rPlural)
						)
						return false;
					/*
					если повелительное наклонение не омонимично изъявительному наклонению,
					тогда нужно проверить ограничения на субъект для повелительного наклонения
					*/
					if (   m_Nodes[P.m_SourceNo].HasOneGrammem(rImperative)
						&& !CanBeSubjectForImperative(NodeNo, P.m_SourceNo)
						&& ((m_Nodes[P.m_SourceNo].GetGrammems() & rAllTimes) == 0)
				  )
				  return false;
					SynRealization.m_Grammems = _QM(rNominativ);
					return true;
				}
			else 
				if (HasRichPOS(NodeNo, INFINITIVE))
				{
					SynRealization.m_Grammems = 0;
					return true;
				};
		return false;;
	}

	if (     (m_Nodes[P.m_SourceNo].m_SemCategory != scSituation)
		&&  HasRichPOS(P.m_SourceNo, NOUN)  
		&&  HasRichPOS(NodeNo, PRONOUN)  
		&&  (SynFet == P.m_pRossDoc->GenitivNo)
		)
		return false;

	// существительное не может управлять  стоящим от него слева существительным
	// Если отноешение не является реверсивным, тогда это же касается
	// прилагательных или местоимений, а реверсивные отношения были выше.  
	if (   
		(    ( SynRelStr == "к_доп") 
		|| ( SynRelStr == "ОБСТ_ГР")
		)
		&&    HasRichPOS(P.m_SourceNo, NOUN) 
		&&  (     HasRichPOS(NodeNo, PRONOUN)  
		|| HasRichPOS(NodeNo, NOUN) 
		|| HasRichPOS(NodeNo, NUMERAL) 
		|| HasRichPOS(NodeNo, NUMERAL_P) 
		|| HasRichPOS(NodeNo, PRONOUN_P) 
		)
		&&	(		!IsTimeAdverbial(NodeNo)
		||	!(P.m_PatternValency.m_RelationStr == "TIME")		   
		)
		&&  (P.m_SourceNo > NodeNo)
		)
		return false;



	// GF = с_опр : ПРИТ_МЕСТМ
	if (SynRelStr == "с_опр")
		if (SynFetStr == "ПРИТ_МЕСТМ")
			if (     IsPossesivePronoun(m_Nodes[NodeNo])
				|| (     m_Nodes[NodeNo].IsPrimitive()
				&& (m_Nodes[NodeNo].m_Words[0].m_Lemma == "ЧЕЙ")
			 )
			 )
			 if (  (      FullGleiche(P.m_SourceNo, NodeNo) 
				 &&   (GetSynRelationsCount(NodeNo)  == 0)
				 )
				 ||     
				 _find(m_SynRelations, CSynRelation (P.m_SourceNo, NodeNo, "ПРИЛ_СУЩ"))
				 )
			 {
				 //SynRealization.m_SynRelation = NOUN_ADJ;
				 return true;
			 }
			 else
				 return false;

	// GF = числ-сущ : *
	if (SynRelStr == "числ_сущ")	
		if (_find(m_SynRelations, CSynRelation(P.m_SourceNo, NodeNo, "ЧИСЛ_СУЩ")))
		{
			return true;
		}
		else
			return false;



	if ( SynRelStr == "с_опр" )	
		if ( SynFetStr == "ПРИЛ" )	
			if ( !HaveNounNodeBetweenInOneClause(NodeNo, P.m_SourceNo) )
				if ( HasRichPOS(NodeNo, ADJ_FULL)   || HasPOS(NodeNo, PRONOUN_P)   )
					if ( FullGleiche(NodeNo, P.m_SourceNo)   )
					{
						return true;
					};



	// GF =    отпредик : И
	if (SynRelStr == "отпредик")
		if (SynFetStr == "И")
			if  ( HasRichPOS (NodeNo, ADJ_SHORT) )
				return true;



	//примыкающее определение должно стоять только справа
	if (SynRelStr == "прим_опр")
		if (m_Nodes[NodeNo].GetMinWordNo() - m_Nodes[P.m_SourceNo].GetMinWordNo() <=  0  )
			return false;

	long PrepNo = -1;

	if (m_Nodes[NodeNo].m_MainWordNo != -1) 
	{
		const CRusSemWord& SynMainWord = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];

		// GF =    отпредик : Р_ПРИЛ
		if (SynRelStr == "отпредик")
			if (SynFetStr == "Р_ПРИЛ")
				if  ( HasRichPOS (NodeNo, ADJ_FULL)  || HasPOS (NodeNo, PARTICIPLE))
					if (GetSynHost (NodeNo)  == -1)
						if ( SynMainWord.HasOneGrammem(rGenitiv) ) 
							return true;

		// GF =    отпредик : Т_ПРИЛ
		if (SynRelStr == "отпредик")
			if (SynFetStr == "Т_ПРИЛ")
				if  ( HasRichPOS (NodeNo, ADJ_FULL)  || HasPOS (NodeNo, PARTICIPLE))
					if (GetSynHost (NodeNo)  == -1)
						if ( SynMainWord.HasOneGrammem(rInstrumentalis) ) 
							return true;

		if (SynRelStr == "отпредик")
			if (P.m_pRossDoc->IsCompAdjCortege( C))
				return m_Nodes[NodeNo].HasOneGrammem(rComparative) ;



		/*
		Алексей, я не хочу, чтобы собиралась группа "Я вижу него"!
		Олег.
		*/
		if (   m_Nodes[NodeNo].IsPrimitive() 
			&& (SynRelStr == "п_доп")
			&& (m_Nodes[NodeNo].m_Words[0].m_Word == "НЕГО")
			)
			return false;


		if (   P.m_pRossDoc->IsSynGroup(SynRelNo)
			|| P.m_pRossDoc->IsSynRel(SynRelNo)
			) 
			if ( IsCase(P.m_pRossDoc, SynFet) )
				if (    (m_Nodes[NodeNo].m_MainWordNo != -1)
					&&  !m_Nodes[NodeNo].HasSomePrep() 
					&&  (    !m_Nodes[P.m_SourceNo].HaveCommaAfter() 
					|| !HasRichPOS(P.m_SourceNo, NOUN)
					)
					&&  (     SynMainWord.HasPOS(NOUN)  
					|| SynMainWord.HasPOS(PRONOUN) 
					|| SynMainWord.HasPOS(NUMERAL)  // "одной из главных задач банк считает восстановление доверия"
					|| SynMainWord.HasPOS(NUMERAL_P)  // "первым пришел Иван"
					|| (    SynMainWord.HasPOS(PRONOUN_PREDK)  // мне больше нечего добавиьт
					&& HasRichPOS(P.m_SourceNo, INFINITIVE)			 	
					)
					|| SynMainWord.m_ILE   // "BBS больше не отвечала" 
					|| (  
					SynMainWord.HasPOS(ADJ_FULL)  
					&& (GetSynRelationsCount(NodeNo)  == 0)
					// В нижеследующих примерах GetSynRelationsCount (NodeNo)>0,
					// для них не будем проверять пассивные валентности 
					//  "красивый мальчик весело прыгал"
					//  "в какую группу стоит поместить сообщение" 
					)

					|| (     m_Nodes[NodeNo].IsPrimitive()
					&&  SynMainWord.IsRusSubstPronounP()   
					&&  !HasIncomingNotWeakSynRelation(NodeNo)
					// "Одной из задач Банк считает восстановление"
					// " которой он защищался
					)
				 )
				 )
				{
					QWORD Grammems  = m_Nodes[NodeNo].GetGrammems();

					// "много домов" ( у узла "домов" граммема - им, мн, 
					// а у слова "домов" - рд, мн.)
					// если проводится стрелка от наречия к слову, то нужно брать граммемы главного
					// слова
					if (  HasRichPOS (P.m_SourceNo, NUMERAL) ) 
						Grammems =  SynMainWord.GetAllGrammems();
					long  CaseGrammems = GetCaseGrammem(P.m_pRossDoc, SynFet);

					if   (     ((CaseGrammems & Grammems) == CaseGrammems) 
						|| m_Nodes[NodeNo].IsILENode()
						)
				 {

					 SynRealization.m_Grammems = GetCaseGrammem(P.m_pRossDoc, SynFet);
					 return true;
				 }
				}
				else;
			else; 
			if (P.m_pRossDoc->IsLemGroupBegining( SynFet)) 
				if (  m_Nodes[NodeNo].HasSomePrep())
					if (   !P.m_PatternValency.m_IsAddition	 
						|| CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo) 
						)
						if (CheckGroupBeginAndCase(SynFetStr, NodeNo, PrepNo))
						{
							if (PrepNo != -1) 
								SynRealization.m_Preps.push_back( m_Nodes[NodeNo].m_SynReal.m_Preps[PrepNo]); 
							return true;
						}
						else;
					else;
				else 
					if (P.m_pRossDoc->ILENo == SynFet)
						return (m_Nodes[NodeNo].m_MainWordNo != -1) && (SynMainWord.m_ILE); 
			// все остальные валентности не могут содержать валентного MUA,
			// вообще, валентное MUA может быть только в подлежащем и в прямом дополнении.  

			//GF* = *  прим_опр : ИГ
			if (SynRelNo == P.m_pRossDoc->ParatAttrSynONo) // достаточно только того, что это "прим_опр" 
				if (SynFet !=  P.m_pRossDoc->HyphenNo) // //GF* = *  прим_опр:ДЕФ должно разбираться отдельно, так как морфология  соединяет в одно слово 
					// комплексы типа "врач-терапевт", "студент-заочник"
					if  (		m_pData->GetRusGramTab()->GleicheCaseNumber(m_Nodes[P.m_SourceNo].m_GramCodes.c_str(), m_Nodes[NodeNo].m_GramCodes.c_str()) 
							||	SynMainWord.m_ILE
						)
					{
						return true;
					};

	};

	//GF* = *  ОБСТ_ГР

	PrepNo = -1;

	if (SynRelStr == "ОБСТ_ГР" ) 
		if ( SynFetStr == "НАР" )
			return HasRichPOS(NodeNo,ADV);
		else
			if (SynFet == -1)
				if (   (    m_Nodes[NodeNo].HasSomePrep()
					&&  CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo)  
					)
					|| (   (m_Nodes[NodeNo].m_MainWordNo != -1)
					&& m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].HasPOS(ADV)
					&& (P.m_PatternValency.m_RelationStr != "TIME")			&& !IsLocSemRel (P.m_PatternValency.m_RelationStr)
					)
					|| (		IsTimeAdverbial(NodeNo)  
					&&	(P.m_PatternValency.m_RelationStr == "TIME")		   )
					|| (		(m_Nodes[NodeNo].GetType() == LocRoss)
					&&	(GetRossHolder(LocRoss)->HasCX(m_Nodes[NodeNo].GetUnitNo(), P.m_PatternValency.m_RelationStr, "D_SEM_REL")) 
					)
					|| (		(P.m_PatternValency.m_RelationStr == "LOK")			
					&&		IsLocNode(NodeNo) 
					&&		(	(		 m_Nodes[NodeNo].HasSomePrep()
					&&	CheckSemanOfPrep(P.m_PatternValency.m_RelationStr, P.m_PatternValency.m_IsAddition, NodeNo, PrepNo)  
					)
					||	HasRichPOS (NodeNo, ADV)
					)

					)

					/*|| (     (m_Nodes[NodeNo].GetType() == OborRoss)
					&&  m_pData->Oborottos[m_Nodes[NodeNo].GetUnitNo()].m_bRusOborAdverbial
					)*/
					)
				{
					if (PrepNo != -1) 
						SynRealization.m_Preps.push_back( m_Nodes[NodeNo].m_SynReal.m_Preps[PrepNo]); 
					return true;
				};

	// GF* = *  п_доп : инф
	// GF*     = *  к_доп : инф
	if (   (SynRelNo == P.m_pRossDoc->DirectObjSynONo) 
		|| (SynRelNo == P.m_pRossDoc->IndirObjSynONo) 
		|| (SynRelStr == "отпредик")
		)
		if (SynFet == P.m_pRossDoc->InfinitiveNo) 
		{
			// существительное не может управлять инфинитивом, который стоит до него
			// например "считать задачей восстановление доверия"
			// Здесь  "считать" становится валентностью слова "задачей"
			if (    HasRichPOS(P.m_SourceNo, NOUN) 
				&& (m_Nodes[NodeNo].GetMinWordNo() < m_Nodes[P.m_SourceNo].GetMinWordNo())
				)
				return false;

			return HasRichPOS(NodeNo, INFINITIVE);
		};

	if (		(P.m_PatternValency.m_RelationStr == "IDENT")  	   
		&& (   (isdigit((BYTE) m_Nodes[NodeNo].m_Words[0].m_Word[0]))
		|| (m_Nodes[NodeNo].m_Words[0].m_ILE)
		|| (m_Nodes[NodeNo].m_bQuoteMarks)
		|| (    (m_Nodes[NodeNo].m_SynGroupTypeStr == NOUN_NUMERAL)
		&& (m_Nodes[NodeNo].m_Words.size() > 0)
		&& (m_Nodes[NodeNo].m_Words[0].m_Word == "N")
		)
		)
		)
		return true;

	if (SynRelStr == "*" ) 
		return true;

	return false;
};



bool CRusSemStructure::HaveOnlyPunctBetween (size_t NodeNo1, size_t NodeNo2, int punct_char) const
{
  if ( m_Nodes[NodeNo1].GetMaxWordNo() > m_Nodes[NodeNo2].GetMaxWordNo() )
	  swap (NodeNo1, NodeNo2);

  // проверяем, если в знаках препинания после самого левого узла была запятая, то возвращаем истину,
  // если между узлами стоять только абстрактыне узлы (на границце клауз) или узлы, которые находятся 
  // за узлом NodeNo
  if  (     (m_Nodes[NodeNo1].GetMaxWordNo()+1  == m_Nodes[NodeNo2].GetMinWordNo())
		 && (m_Nodes[NodeNo1].HasPostPunct(punct_char))
	  )
  {
	  for (long i=NodeNo1+1;  i < NodeNo2;i++)
		 if (IsBetween(m_Nodes[i], m_Nodes[NodeNo1], m_Nodes[NodeNo2]))
		  if (!m_Nodes[i].IsAbstract())
			   return false;
      return true;
  }
  else
  if (m_Nodes[NodeNo1].GetMaxWordNo()+2  == m_Nodes[NodeNo2].GetMinWordNo() )
  {
	  // если найден узел между узлами NodeNo1 и NodeNo2, состоящий только из одного этого знака препинания,
	  // то считаем это удачей (такой примитивный узел возможен в однородных группах, которые построил 
      // синтаксис).
	  long Count_Punct = 0;
	  for (long i=NodeNo1+1;  i < NodeNo2; i++)
		if ( IsBetween(m_Nodes[i], m_Nodes[NodeNo1], m_Nodes[NodeNo2]) )
         if (      m_Nodes[i].IsPrimitive() 
			   && (m_Nodes[i].m_Words[0].m_Word[0] == punct_char)
			)
		 	 Count_Punct ++;
		 else
		   if (!m_Nodes[i].IsAbstract())
		 	    return false;

      return Count_Punct == 1;
  };
  return false;
};

bool CRusSemStructure::HasCommaBefore (size_t NodeNo) const
{
  long nd = FindLeftClosestNode(NodeNo);
  if (nd == -1) return false;
  if (m_Nodes[nd].IsWordContainer()) 
	  if (   (m_Nodes[nd].m_Words[0].m_Word == ",")
		   || m_Nodes[nd].HasPostPunct(',') 
		 )
		  return true;

  return false;
};


bool CRusSemStructure::HaveNounNodeBetweenInOneClause (size_t NodeNo1, size_t NodeNo2) const
{
  assert (m_Nodes[NodeNo1].m_ClauseNo == m_Nodes[NodeNo2].m_ClauseNo);

  if ( NodeNo1 > NodeNo2 )
	  swap (NodeNo1, NodeNo2);

  for (long i=NodeNo1+1; i <  NodeNo2 ; i++)
	  if  ( HasRichPOS(i,NOUN) )
		  return true;

  return false;
};



void CRusSemStructure::InitGrammarMatrix1 (long ClauseNo)
{
	CRusSemClause& C = m_Clauses[ClauseNo];

	// обнуляем грамматическую матрицу
	long NodesCount = C.GetNodesCount();
	if (NodesCount == 0) return;
	/*
	нужно проинициализировать всю матрицу, потому что
	могут  появиться новые узлы (ModalCopul). Нужно, чтобы 
	для них все флаги были равны нулю.
	*/
	C.InitGramMatrix();


	long NodeNo=C.m_BeginNodeNo;
	for (; NodeNo < C.m_EndNodeNo; NodeNo++)
		if (!m_Nodes[NodeNo].IsWordContainer())
			break;
	/*
	EndWordContNodeNo - номер узла, который находится за последним 
	узлом, содежащим слова (IsWordContainer)
	Матрица будет инициализирована только для  WordContainer.
	*/
	long EndWordContNodeNo = NodeNo;

	// инициализируем флаг  HaveOnlyCommaBetween
	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo-1; i++)
		if (m_Nodes[i].IsWordContainer())
		{
			long k = -1;

			if (HaveOnlyPunctBetween(i, i+1, ','))
				k = i+1;

			if (i+2 < EndWordContNodeNo)
				if (HaveOnlyPunctBetween(i, i+2, ','))
					k = i+2;

			if ( k != -1)
			{
				C.AddFlagToGramMatrix(i,k, HaveOnlyCommaBetween);

				// инициализируем флаг  HaveCommaBetween
				// этот флаг вычисляется по флагу HaveOnlyCommaBetween
				for (long j=k; j < EndWordContNodeNo; j++)
					for (long l=i; l>=C.m_BeginNodeNo; l--)
					{
						C.AddFlagToGramMatrix(j,l, HaveCommaBetween);
					}
			};

		};




	// инициализируем флаги HaveFiniteVerbBetween и HaveOnlyConjBetween
	vector<bool> StrongRoots(m_Nodes.size(), false);
	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		StrongRoots[i] =		(HasRichPOS(i, VERB) || HasPOS(i, ADJ_SHORT) || HasPOS(i, PARTICIPLE_SHORT))
							&&	!ContainsSemCopul(i);
		// сильные узлы - это клаузные предикаты, которые не могут выполнять роль копулов,
		// если отключить условие про копулы, тогда фраза "мне это было полезным" не будет собираться
	    
							

	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		if (!HasRichPOS(i, INFINITIVE))
			for (size_t k=i+2; k < EndWordContNodeNo; k++)
				if (!HasRichPOS(k, INFINITIVE))
				{

					// инициализируем флаг наличия между двумя словами глагола в личной форме, 
					// краткого прилагательного или краткого причастия (безусловная вершина)
					for (long l=i+1; l < k; l++)
						if  ( StrongRoots[l] )
						{
							C.AddFlagToGramMatrix(i,k, HaveFiniteVerbBetween);
							break;
						}

						// инициализируем флаг наличия только одного союза
						if (k == i+2)
							if ( m_Nodes[i+1].IsLemmaList("И", "ИЛИ", "НО", "А", "") )
							{
								C.AddFlagToGramMatrix(i,k, HaveOnlyConjBetween);
							};
				};

	for (long i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++)
		for (size_t k=i+1; k < EndWordContNodeNo; k++)
		{
			if (AreOldNodesOfTheSameCase(i, k))
			{
				C.AddFlagToGramMatrix(i,k, OldNodesOfTheSameCase);
			}
		}
};

void CRusSemStructure::InitGrammarMatrix2 (long ClauseNo)
{
  CRusSemClause& C = m_Clauses[ClauseNo];

  long i=C.m_BeginNodeNo;
  for (; i < C.m_EndNodeNo; i++)
	  if (!m_Nodes[i].IsWordContainer())
		  break;
   /*
		EndWordContNodeNo - номер узла, который находится за последним 
		узлом, содежащем слова (IsWordContainer)
		Матрица будет инициализирована только для  WordContainer.
   */
  long EndWordContNodeNo = i;

  for (i=C.m_BeginNodeNo; i < EndWordContNodeNo; i++) 
     for (size_t k=i+1; k < EndWordContNodeNo; k++)
		 if (AreOldNodesOfTheSameCase(i, k))
		 {
			 C.AddFlagToGramMatrix(i,k, OldNodesOfTheSameCase);
		 }
};








bool CRusSemStructure::IsPattern (CSemPattern& P, size_t NodeNo, string& SyntacticRelation, CSynRealization& SynRealization)
{
	SynRealization.SetEmpty();
	SyntacticRelation = "";
	//if (P.m_SourceNo != -1)

    if (P.m_ObligLexFets.size() > 0)
	{
		long i=0;
		for (; i <P.m_ObligLexFets.size(); i++)
			if (P.m_ObligLexFets[i][0] == '#')
			{
				if (_find(m_Nodes[NodeNo].m_HigherConcepts, P.m_ObligLexFets[i]))
					break;
			}
			else
			{
			  if (m_Nodes[NodeNo].IsLemma(P.m_ObligLexFets[i]))
				  break;
			};

		if (i == P.m_ObligLexFets.size()) return false;
	};
	


	if (P.m_GramCorteges.size() == 0) return false;

    if (P.m_GramCorteges[0].m_DomItemNos[0] == -1) return false;


	for (SynRealization.m_CortegeNo =  0; SynRealization.m_CortegeNo < P.m_GramCorteges.size(); SynRealization.m_CortegeNo++)
      if (   CheckPatternGramFetLine(P, SynRealization, NodeNo)
		  
		 ) 
	  {
         
		 if (P.m_pRossDoc->IsSynRel (P.GetSynRel(SynRealization.m_CortegeNo)) )
			 SyntacticRelation = P.GetSynRelStr(SynRealization.m_CortegeNo);
		  SynRealization.m_Cortege = P.m_GramCorteges[SynRealization.m_CortegeNo];
		 return true;
	  }
	 SynRealization.m_CortegeNo= - 1;
	
	 return false;

};

bool CRusSemStructure::AreOldNodesOfTheSameCase (long NodeNo1, long NodeNo2) const 
{
	if  (     (m_Nodes[NodeNo1].m_MemNodeNo == -1)
	       || (m_Nodes[NodeNo2].m_MemNodeNo == -1)
	    )
		return false;
	assert (m_Nodes[NodeNo1].m_MemNodeNo < m_MemNodes.size() );
	const CMemNode& N1 = m_MemNodes[m_Nodes[NodeNo1].m_MemNodeNo];

	assert (m_Nodes[NodeNo2].m_MemNodeNo < m_MemNodes.size() );
	const CMemNode& N2 = m_MemNodes[m_Nodes[NodeNo2].m_MemNodeNo];
	
	long s[10000];
	assert (N1.m_Cases.size() + N2.m_Cases.size() < 10000);
	long* Out = set_intersection(
		N1.m_Cases.begin(), N1.m_Cases.end(), 
		N2.m_Cases.begin(), N2.m_Cases.end(),
		s); 
	return Out > s;
};

/*
 в этой функции NodeNo иногда отличается от P.m_SourceNo.
 например, в случаях копула, когда NodeNo - это номер 
 копула, а P.m_SourceNo - номер статьи, из которой этот
 копул возник
*/
void CRusSemStructure::TryPatternOnNodes(size_t NodeNo, size_t ClauseNo, CSemPattern& P)
{
	string  SyntacticRelation;
	CSynRealization SyntacticRealization;
	const CRusSemClause& C = m_Clauses[ClauseNo];

	
#if Trace == 1
	P.TracePattern();
#endif


	for (size_t i=0; i<m_Nodes.size(); i++)
		if (     (i != NodeNo)
			&&  IsInClause(i, ClauseNo)
			&&  m_Nodes[i].IsWordContainer()
			/*  эти два узла  должны встречаться первый раз
			*/
			&& !(C.GetGramMatrix(NodeNo,i) & OldNodesOfTheSameCase)
			)
		{
			StartTimer("TryPatternOnNodes",2);
#if Trace == 1
			rml_TRACE (GetNodeStr(NodeNo).c_str());
			rml_TRACE (" <--?--> ");
			rml_TRACE (GetNodeStr(i).c_str());
#endif

			bool Result = false;


			if (   
					!(C.GetGramMatrix(P.m_SourceNo,i) & HaveFiniteVerbBetween)
				&&	!(C.GetGramMatrix(P.m_SourceNo,i) & HaveOnlyConjBetween)
				&&	IsPattern (P, i, SyntacticRelation,  SyntacticRealization) 
				)
			{

				if (SyntacticRelation == "X!")
				{
					AddRelation(CRusSemRelation(P.m_PatternValency, i, NodeNo, SyntacticRelation));
					m_Relations.back().m_bReverseRel = true;
				}
				else
					AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, i, SyntacticRelation));

				CRusSemRelation&  R  = m_Relations.back();
				R.m_CannotHaveOnlyCommaBetween = true;

				// случай уточнения прилагательным
				if (SyntacticRelation == "уточн")
					if (HasRichPOS (i, ADJ_FULL))
						R.m_CannotHaveOnlyCommaBetween = false;

				R.m_SynReal =  SyntacticRealization;
				
				R.m_SemFets = GetIndexedSemFets(P, true, false);
				/*  предлоги в тайм-группах записываются внутри статьи*/
				if	(    m_Nodes[i].IsMainTimeRossNode() 
					&& (P.m_PatternValency.m_RelationStr == "TIME") // если тайм-группа заполнила валентность не TIME, значит она свободная			 
					)
					R.m_SynReal.m_Preps.clear();

				R.m_LexFets = P.m_LexFets;
				R.m_LexFets.insert(R.m_LexFets.begin(), P.m_ObligLexFets.begin(), P.m_ObligLexFets.end());

				Result = true;
			}

			EndTimer("TryPatternOnNodes");
#if Trace == 1
			rml_TRACE (" => %s\n", Result ? "success" : "failed");
#endif
		};


};

bool HasCopulPattern (const CSemPattern& P)
{
	for (long i=0; i < P.m_GramCorteges.size();i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] == P.m_pRossDoc->CopulNo)
			return true;

    return false;
};

bool HasModalCopulPattern (const CSemPattern& P)
{
	for (long i=0; i < P.m_GramCorteges.size();i++)
		if (P.m_GramCorteges[i].m_DomItemNos[0] == P.m_pRossDoc->ModalCopulNo)
			return true;

    return false;
};

/*
 Рассмотрим работу этой функции на примере. 
 "Я одержал победу". Здесь идет стрелка LF от 
 "победы" к "одержать" по названием Oper1. Эта
 стрелка предполагает, что первая валентность слова "победа"
 будет выражена подлежащим при глаголе, а это означает,
 что она не может быть выражена при существительном (например,
 с помощью родительного падежа). Нельзя сказать,
 "Он одержал победу его" - здесь два раза заполнена валентность.
 Поэтому во фразе "Я одержал победу" первая валентность
 "победы" глушится.
*/
bool ValencyIsOccupiedByLexFunct(const CRusSemStructure& R, size_t NodeNo, size_t ValencyNo)
{
	int ValNo;	
	for (long i=0;i < R.m_LexFuncts.size(); i++)
	 if (R.m_LexFuncts[i].m_SourceNodeNo == NodeNo)
	   if (IsOper(R.m_LexFuncts[i].m_LexFunctName, ValNo))
		   for (long j=0; j<R.m_Nodes[NodeNo].m_NotProhibitedValSets.size(); j++)
			   if  (     ( R.m_Nodes[NodeNo].m_NotProhibitedValSets.size() > ValNo-1)
				     &&  ( R.m_Nodes[NodeNo].m_NotProhibitedValSets[j][ValNo-1] ==  ValencyNo)
				   )
				   return true;

	return false;	
};



void CRusSemStructure::FindActants(size_t NodeNo)
{
	CSemPattern P;
	size_t ClauseNo = m_Nodes[NodeNo].m_ClauseNo;
#if Trace == 1
	rml_TRACE ("\n");
	rml_TRACE (GetNodeStr(NodeNo).c_str());
	rml_TRACE ("\n");
#endif

	for (size_t ValencyNo = 0; ValencyNo < m_Nodes[NodeNo].m_Vals.size(); ValencyNo++ )
		if (!ValencyIsOccupiedByLexFunct(*this, NodeNo, ValencyNo))
		{
			GetActantPattern (NodeNo, ValencyNo, P);

			if  ( HasCopulPattern(P) )    
			{
				CSemPattern CopulPattern = P;
				CRusSemNode NewCopulNode = CreateCopulNode(ClauseNo);
				long CopulNodeNo = InsertNode(NewCopulNode);
				assert (m_Clauses[ClauseNo].m_EndNodeNo - 1 == CopulNodeNo);
				m_Clauses[ClauseNo].UpdateSizeOfGramMatrix();
				AddRelation(CRusSemRelation(P.m_PatternValency, NodeNo, CopulNodeNo,  ""));

				bool PassiveCase =    m_Nodes[NodeNo].IsPassiveVerb() && (ValencyNo == 0);
				size_t k = PassiveCase ? 1 : ValencyNo;
				m_Nodes[CopulNodeNo].m_Vals.clear();
				m_Nodes[CopulNodeNo].m_Vals.push_back (CValency("F-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo()));
				m_Nodes[CopulNodeNo].m_Vals.push_back (CValency("S-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo()));	

				// строим F-ACT		
				CopulPattern.m_GramCorteges.clear();
				if  (PassiveCase)
				{
					LoadFromDictForPassive(m_Nodes[NodeNo].GetUnitNo(), 1, 1, CopulPattern);
					CopulPattern.m_PatternValency = CValency ("F-ACT",A_C,GetRossHolder(Ross), 1, 1, m_Nodes[NodeNo].GetUnitNo());
					if (CopulPattern.m_GramCorteges.size() == 0) CopulPattern.m_GramCorteges.push_back(GetSubjCortege(GetRossHolder(Ross)));
					TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
				}
				else
				{
					CopulPattern.InitSemPattern(GetRossHolder(Ross), m_Nodes[NodeNo].GetUnitNo(), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1);
					CopulPattern.LoadSemFromDict();
					if (CopulPattern.LoadGramFromDict())
					{
						CopulPattern.m_PatternValency = CValency ("F-ACT",A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo());			
						TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
					};
				};

				// строим S-ACT		
				CopulPattern.m_GramCorteges.clear();
				CopulPattern.InitSemPattern(GetRossHolder(Ross), m_Nodes[NodeNo].GetUnitNo(), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = CValency ( "S-ACT", A_C, GetRossHolder(Ross), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo());			
					TryPatternOnNodes(CopulNodeNo, ClauseNo, CopulPattern);
				};
			}

			if ( HasModalCopulPattern (P) )
			{
				CSemPattern CopulPattern = P;

				CRusSemNode NewCopulNode = CreateModalCopulNode(ClauseNo);
				CopulPattern.m_SourceNo = InsertNode(NewCopulNode);
				assert (m_Clauses[ClauseNo].m_EndNodeNo - 1 == CopulPattern.m_SourceNo);
				m_Clauses[ClauseNo].UpdateSizeOfGramMatrix();

				// построение дополнительного отношения
				m_DopRelations.push_back(CRusSemRelation(CValency("ModalCopul", A_C, GetRossHolder(Ross)),  NodeNo,  CopulPattern.m_SourceNo,  "ModalCopul"));     

				m_Nodes[CopulPattern.m_SourceNo].m_Vals.clear();

				m_Nodes[CopulPattern.m_SourceNo].m_Vals.push_back (CValency("SUB", A_C, GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1, m_Nodes[NodeNo].GetUnitNo()));
				m_Nodes[CopulPattern.m_SourceNo].m_Vals.push_back (CValency("CONTEN", A_C, GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2, m_Nodes[NodeNo].GetUnitNo()));		
				CopulPattern.m_GramCorteges.clear();

				CopulPattern.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 1);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = m_Nodes[CopulPattern.m_SourceNo].m_Vals[0];
					TryPatternOnNodes(CopulPattern.m_SourceNo, ClauseNo, CopulPattern);
				};

				CopulPattern.m_GramCorteges.clear();
				CopulPattern.InitSemPattern(GetRossHolder(m_Nodes[NodeNo].GetType()), m_Nodes[NodeNo].GetUnitNo(),m_Nodes[NodeNo].m_Vals[ValencyNo].m_LeafId, 2);
				CopulPattern.LoadSemFromDict();
				if (CopulPattern.LoadGramFromDict())
				{
					CopulPattern.m_PatternValency = m_Nodes[CopulPattern.m_SourceNo].m_Vals[1];
					TryPatternOnNodes(CopulPattern.m_SourceNo, ClauseNo, CopulPattern);
				};


			}

			TryPatternOnNodes(NodeNo, ClauseNo, P);
		};

};



void CRusSemStructure::FindReverseActantForPreps(size_t ClauseNo)
{
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	  if (    m_Nodes[NodeNo].HasSomePrep()
		   && (GetIncomingRelationsCount (NodeNo, false) == 0)
		   && IsInClause (NodeNo, ClauseNo)
		  )
	  {
		  CSemPattern P;
		  P.m_SourceNo = NodeNo;
		  P.m_pRossDoc = GetRossHolder(OborRoss);
		  WORD PrepUnitNo = m_Nodes[NodeNo].m_SynReal.m_Preps[0].m_UnitNo;
		  // загружаем первую валентность предлога - это пассивная валентность
		  P.InitSemPattern(P.m_pRossDoc, PrepUnitNo, 1, 0);
		  P.LoadGramFromDict();
		// нужно добавить X!, потому что предлог является CAT=RELAT		  
		  P.InsertReverseSynOToTheBegining();
		  // получаем первую валентность предлога (то отношение, которое оно выражает)
		  StringVector Vals;
		  P.m_pRossDoc->GetSimpleFieldItemsFromArticle (PrepUnitNo, "VAL", 0, 0, Vals);
		  if (Vals.size() > 0)
		     P.m_PatternValency.m_RelationStr = Vals[0];
		  P.m_PatternValency.m_bOptional = true;
		  long OldRelsCount = m_Relations.size();
		  /*
		    Нужно оключить полное согласование с прилагательным,
			чтобы  построить пример "тел из числа погибших"
		  */
		  bool bsave = m_Nodes[NodeNo].m_bFullGleiche;
		  m_Nodes[NodeNo].m_bFullGleiche = false;
		  TryPatternOnNodes(NodeNo, ClauseNo, P);
		  m_Nodes[NodeNo].m_bFullGleiche = bsave;


  		  // будем считать, что эти отношения факультативны не из 
		  // словаря, поэтому  P.m_pRossDoc = 0;
		  // В противном случае, другое значение глагола, в котором есть валентность
		  // именно на эту предложную группу не сможет вылезти наверх.
		  // Например, "он стал на холм" здесь будет выбираться стал 1, а должен
		  // стал 3, у которого прописана валентность на "на+В"

		  
		  // добавляем всем тольок что построенным отношениям предлог
		  for (long i=OldRelsCount; i <m_Relations.size();i++)
		  {
			  m_Relations[i].m_SynReal.m_Preps.push_back (m_Nodes[NodeNo].m_SynReal.m_Preps[0]);
			  m_Relations[i].m_Valency.m_RossHolder = 0;
		  };


	  };

};

/*
Имеем:
локативная предложная группа не интерпретируется
она стоит между ИГ и глаголом, принадлежащими одной клаузе
Действие:
вешаем ее на существительное

Примеры:
Все лифты в здании очень быстро опускались.
Пылинки на свету заплясали яростнее.
Стена позади Марвина всхлипнула и рассыпалась в пыль.
Все и так это знали, представления в Ресторане были известны во
всей обитаемой Галактике
Главный обзорный экран над пультом тоже зажегся
*/

void CRusSemStructure::HeuristicsLocative(long ClauseNo) 
{
  UpdateBlockedRelations();
  for (long NodeNo = m_Clauses[ClauseNo].m_BeginNodeNo;  NodeNo < m_Clauses[ClauseNo].m_EndNodeNo; NodeNo++)
  if (    m_Nodes[NodeNo].IsWordContainer()
	  &&  m_Nodes[NodeNo].HasSomePrep()
	 )
  {
	  long NounNodeNo = -1;
	  long VerbNodeNo = -1;
	  for (long i=0; i<m_Nodes[NodeNo].m_InRels.size();i++)
			if	(		(m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_Valency.m_RelationStr == "LOK")		   
					&&	m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_bReverseRel
				)
				{
					long N = m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_SourceNodeNo;
					if (HasRichPOS(m_Nodes[N], NOUN))
						if (m_Nodes[N].GetMinWordNo() < m_Nodes[NodeNo].GetMinWordNo())
							NounNodeNo = N;

					if (HasRichPOS(m_Nodes[N], VERB))
						if (m_Nodes[N].GetMinWordNo() > m_Nodes[NodeNo].GetMinWordNo())
							VerbNodeNo = N;
				};

	  if (  (NounNodeNo != -1) && (VerbNodeNo != -1))
	  {
		   vector<long> Rels;
		   FindRelations (VerbNodeNo, NodeNo, Rels);
		   DeleteRelations(Rels);
		   UpdateBlockedRelations();
	  };

  };
};

/*
Обрабатывает случаи, когда "нечего" не подчинилось никакому инфинитву, например: 
  Нечего мертвецам шляться вокруг, и вмешиваться в дела живых!
  Прошу заходить - нечего стоять на холоде.
  - Нечего нас оскорблять.
  Спорить было нечего.	
А например, в случае "вам не о чем волноваться" эта процедура не будет работать,
поскольку здесь "нечего" подсоединится "волноваться" по валентности .
*/
void CRusSemStructure::DealUnboundModalCopul(long ClauseNo) 
{
	for (long i=0; i < m_Nodes.size();i++)
		if (   IsInClause(i, ClauseNo)
			&& (m_Nodes[i].m_NodeType == ModalCopul)
		   )
		{
			vector<long> Rels;
			GetIncomingDopRelations(i, Rels);
			assert (Rels.size() == 1);
			if (GetIncomingRelationsCount(m_DopRelations[Rels[0]].m_SourceNodeNo, false) > 0)
				continue;
			AddRelation(CRusSemRelation(CValency("ModalCopul",A_C), i, m_DopRelations[Rels[0]].m_SourceNodeNo, ""));
			m_DopRelations.erase(m_DopRelations.begin() + Rels[0]);
		};
};


//#pragma optimize( "", on )
