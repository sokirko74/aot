#include "StdAfx.h"
#include "EngSemStructure.h"



// получение по типу EngVerbTenseEnum строкового представления
string CEngSemStructure::GetTenseString(EngVerbTenseEnum Tense) const
{
	switch (Tense) {
		case zero_tn : return "zero";
		case present_smp_tn : return "Present Simple Tense";
		case past_smp_tn	: return "Past Simple Tense";
		case future_smp_tn  : return "Future Simple Tense";
		case present_prf_tn : return "Present Perfect Tense";
		case past_prf_tn	: return "Past Perfect Tense";
		case future_prf_tn	: return "Future Perfect Tense";
		case present_cnt_tn	: return "Present Continuous Tense";
		case past_cnt_tn	: return "Past Continuous Tense";
		case future_cnt_tn  : return "Future Continuous Tense";
		case present_prf_cnt_tn	: return "Present Perfect Continuous Tense";
		case past_prf_cnt_tn	: return "Past Perfect Continuous Tense";
		case future_prf_cnt_tn	: return "Future Perfect Continuous Tense";
		case would_smp_tn		: return "Present Conditional Tense";
		case would_prf_tn		: return "Perfect Conditional Tense";
		case would_cnt_tn		: return "Present Conditional Continuous Tense";
		case would_prf_cnt_tn	: return "Perfect Conditional Continuous Tense";
		case gerund_tn			: return "Gerund";
		case gerund_prf_tn		: return "Perfect Gerund";
		case pp_tn		: return "Past Participle";
		case present_inf_tn		: return "Infinitive (present_inf_tn)";
		case present_cont_inf_tn: return "Present Continuous Infinitive";
		case prf_inf_tn			: return "Perfect Infinitive";
		case prf_cont_inf_tn	: return "Perfect Continuous Infinitive";
		case ing_tn				: return "ing-form";
		case pp_cont_tn		    : return "been+ing-form";
	};
	return "zero";
};


/*
   Неинлайновская функция is_past_tense только лишь
   вызывает функцию инлайновскую is_past. 
   Функция is_past_tense используется только в функции ApplySequenceOfTenseRule.
   Если в ApplySequenceOfTenseRule использовать 
   напрямую is_past или сделать is_past_tense инлайновской 
   (это одно и то же), тогда в оптимизированной релизной
   версии ApplySequenceOfTenseRule будет падать на 
   фразе 
   "Если вы нажмете return , nn  спросит вас , тема какой статьи вам надоела"
   Я не знаю почему. Думаю, проблема в оптимизации VС 6.0.
   В MSDN сказано
   Optimizing compilers can sometimes change your code to 
   something you wouldn't expect. If you find that the application 
   works when you build for release without optimizations, 
   but not when you build for release with optimizations, 
   this might be the case.
   Сокирко 26 мая 2001 
*/
bool is_past_tense(EngVerbTenseEnum t)
{
	return is_past(t);
}


//  Согласование времен в английском сложном предложении
void CEngSemStructure::ApplySequenceOfTenseRule()
{
	//  идем по все узлам 
	for( long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++ )
	{
		if( m_Nodes[NodeNo].m_MainWordNo == -1 )
			continue;
		// берем время узла
		CEngSemWord& MainWord = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];
		EngVerbTenseEnum NewTense;
		// конвертируем это время по правилам
		switch( MainWord.GetTense() )
		{
			case would_smp_tn:		NewTense = would_prf_tn;	break;
			case future_smp_tn:		NewTense = would_smp_tn;	break;
			case past_smp_tn:		NewTense = past_prf_tn;		break;
			case past_cnt_tn:		NewTense = past_prf_cnt_tn; break;
			case present_smp_tn:	NewTense = past_smp_tn;		break;
			case present_cnt_tn:	NewTense = past_cnt_tn;		break;
			case present_prf_tn:	NewTense = past_prf_tn;		break;
			default : NewTense = zero_tn; break;
		}

		// если не удалось отконвертировать,
		if (NewTense == zero_tn) 
			continue; // мы имеем дело не с тем глаголом

		// проверка, что ни у одного подчиненного нет пометы RESTR=not_seq_tn
		vector<long> OutRels;
		GetOutcomingRelations(NodeNo,OutRels);
		int i=0;
		for( ; i<OutRels.size(); i++ )
		{
			const CEngSemNode& EngNode = m_Nodes[m_Relations[OutRels[i]].m_TargetNodeNo];
			if( EngNode.GetType() == NoneRoss ) 
				continue;
			if( GetRossHolder(EngNode.GetType())->HasFieldValue ("RESTR", "not_seq_tn", EngNode.GetUnitNo() ) )
				break;
		}
		// если нашли подчиненное с  RESTR=not_seq_tn, то согласование не будем проводить
		if( i<OutRels.size() ) 
			continue;

// берем все входящие и ищем среди них межклаузные отношения

// придется перебрать все связи "я сказал, где они находятся"
		for( i=0; i<m_Relations.size(); i++ )
		{
			const CEngSemRelation& Rel = m_Relations[i];
			if( m_Nodes[Rel.m_TargetNodeNo].m_ClauseNo != m_Nodes[NodeNo].m_ClauseNo )
				continue;
			// должны быть из разных клауз
			if( m_Nodes[Rel.m_SourceNodeNo].m_ClauseNo == m_Nodes[NodeNo].m_ClauseNo) 
				continue;
			// отношение должно быть подчинительным
			if	(		(Rel.m_Type != Kotoryj) && (Rel.m_Type != ChtoKtoAsConjWords)
					&&	(Rel.m_Type != SubConj) && (Rel.m_Type != Kak)
					&&	(Rel.m_Valency.m_RelationStr!="CONTEN")			  
				) 
				continue;

			if( m_Nodes[Rel.m_SourceNodeNo].RusNode == -1 ) 
				continue;

			// берем русский узел
			const CSemNode& RusNode = RusStr.GetNode(m_Nodes[Rel.m_SourceNodeNo].RusNode);

			if	(		!RusStr.HasSemFet(RusNode, "COMMUNIC")
					&&	!RusStr.HasSemFet(RusNode, "INTEL")
					&&	!RusStr.HasSemFet(RusNode, "PERCPT")			  
				) continue;

			if( m_Nodes[Rel.m_SourceNodeNo].m_MainWordNo == -1 ) 
				continue;

			EngVerbTenseEnum t = m_Nodes[Rel.m_SourceNodeNo].m_Words[m_Nodes[Rel.m_SourceNodeNo].m_MainWordNo].GetTense();
			bool bRes = is_past_tense(t); 
			if( bRes )
			{
				// все, нашли правильного хозяина
				MainWord.SetTense(NewTense, "ApplySequenceOfTenseRule");
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//  "can" | "must" -> "be able" | "have"

void CEngSemStructure::ApplyModalVerbTenseRule()
{
	for( int iEngNode=0; iEngNode<m_Nodes.size(); iEngNode++ )
	{
		int iMainWord = m_Nodes[iEngNode].m_MainWordNo;
		if( iMainWord == -1 ) 
			continue;

		if( is_present(m_Nodes[iEngNode].m_Words[iMainWord].GetTense()) )
			continue;

		if( m_Nodes[iEngNode].m_Words[iMainWord].GetTense()==zero_tn )
			continue;

		if( m_Nodes[iEngNode].m_Words[iMainWord].m_Lemma == "cannot" )
		{
			m_Nodes[iEngNode].m_RelOperators.clear();
			m_Nodes[iEngNode].m_RelOperators.push_back("НЕ");
			m_Nodes[iEngNode].m_Words[iMainWord].m_Lemma = "can";
		}
		
		bool bCan  = m_Nodes[iEngNode].m_Words[iMainWord].m_Lemma == "can";
		bool bMust = m_Nodes[iEngNode].m_Words[iMainWord].m_Lemma == "must";
		if( !bCan && !bMust )
			continue;

// если это не так, то во первых что-то неладно в идеологии,
// а во вторых результат все равно будет неверным - Gri
		assert( m_Nodes[iEngNode].m_Words.size()==1 );

		CEngSemNode newNode;
		long wNo = m_Nodes[iEngNode].GetMinWordNo();

		if( bCan )
		{
			CEngSemNode beNode;
			int iSubjPattern = -1, iObjPattern = -1;
			CreateBeSemNode(beNode,iSubjPattern,iObjPattern);
			assert( (iObjPattern!=-1) && (iSubjPattern!=-1) );
			assert( beNode.m_Words.size()==1 );

			beNode.RusNode = m_Nodes[iEngNode].RusNode;
			beNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

			beNode.m_Words[0].SetTense(m_Nodes[iEngNode].m_Words[iMainWord].GetTense(),
				"ApplyModalVerbTenseRule");
			m_Nodes[iEngNode].m_Words[0].SetTense(zero_tn,"ApplyModalVerbTenseRule");
			beNode.m_RelOperators = m_Nodes[iEngNode].m_RelOperators;
			m_Nodes[iEngNode].m_RelOperators.clear();
			
			beNode.AddGrammems( m_Nodes[iEngNode].GetGrammemsRich() & (eAllPersons | eAllNumbers) );
		
			FreeWordNo(wNo);
			beNode.m_Words[0].m_WordNo = wNo;
			wNo++;
			m_Nodes.push_back(beNode); // be
			int iBeNode = m_Nodes.size()-1;
//
			vector<long> outRels;
			GetOutcomingRelations(iEngNode,outRels);
			for( int i=0; i<outRels.size(); i++ )
			{
				if( IsSubj(m_Relations[outRels[i]]) )
				{
					int iSubj = outRels[i];
					m_Relations[iSubj].m_SourceNodeNo = iBeNode; // be->subj
					m_Relations[iSubj].m_bInterpreted = true;

					m_Relations[iSubj].m_Pattern = beNode.m_Patterns[iSubjPattern];
					m_Relations[iSubj].m_SynReal.m_Cortege = beNode.m_Patterns[iSubjPattern].m_GramCorteges[0];
				}
			}
			
			CEngSemRelation newRel(CValency("AUX_VERB",A_C),iBeNode,iEngNode,"");
			if( is_future(beNode.m_Words[0].GetTense()) )
				newRel.m_Valency.m_RelationStr = "ModalVerbTenseRule";
			newRel.m_Position = ">>";
			newRel.m_PosType = FromAlgorithmPosType;
			m_Relations.push_back(newRel); // be->can(able)

// связи
			MoveMeaningRelsToNewNode(iEngNode,iBeNode);

			CreateSimpleEnglNode("able",newNode,0,true); // node "able"
		}

		else // must
		{
			int iSubjPattern = -1, iObjPattern = -1;
			CreateHaveSemNode(newNode,iSubjPattern,iObjPattern,2); // node "have"
		
			for( int i=0; i<newNode.m_Patterns.size(); i++ )
			{
				if( HasThisGX(newNode.m_Patterns[i].m_GramCorteges,"to+inf",Aoss) )
					iObjPattern = i;
			}
			assert( iObjPattern!=-1 );

			vector<long> outRels;
			GetOutcomingRelations(iEngNode,outRels);
			for( int i=0; i<outRels.size(); i++ )
			{
				if( m_Relations[outRels[i]].m_Valency.m_LeafId != 2 )
					continue;
				m_Relations[outRels[i]].m_bInterpreted = true;
				m_Relations[outRels[i]].m_Valency.m_RossHolder = GetRossHolder(newNode.GetType());
				m_Relations[outRels[i]].m_Valency.m_UnitNo = newNode.GetUnitNo();
				m_Relations[outRels[i]].m_Valency.m_LeafId = 2;
				m_Relations[outRels[i]].m_Pattern = newNode.m_Patterns[iObjPattern];
				m_Relations[outRels[i]].m_SynReal.m_Cortege = newNode.m_Patterns[iObjPattern].m_GramCorteges[0];
			}
		}

		assert( newNode.m_Words.size()==1 );

		newNode.RusNode = m_Nodes[iEngNode].RusNode;
		newNode.m_ClauseNo = m_Nodes[iEngNode].m_ClauseNo;

		newNode.m_Words[0].SetTense(m_Nodes[iEngNode].m_Words[iMainWord].GetTense(),
			"ApplyModalVerbTenseRule");
		
		newNode.m_Words[0].m_WordNo = m_Nodes[iEngNode].GetMinWordNo();
		m_Nodes[iEngNode] = newNode;


		/*
			ищем вторую валентность слова и интерпретируем ее по новой словарной статье
			(чтобы поставить to перед глаголом)
		*/
		vector<long> rels;
		GetOutcomingRelations(iEngNode,rels);
		for (int i = 0; i < rels.size(); i++)
		{
			CEngSemRelation& R = m_Relations[rels[i]];
			if (R.m_bReverseRel) continue;
			if (R.m_Valency.m_LeafId == 2)
			{
				R.m_Pattern = newNode.m_Patterns[1];
				R.m_SynReal.m_Cortege = R.m_Pattern.m_GramCorteges[0];
			};
		};
	}
}
	
/////////////////////////////////////////////////////////////////////////////

struct CAuxVerbInfo {
	string				m_AuxLemma;
	EngVerbTenseEnum	m_AuxTense;
	long				m_AuxEngPOS;
	EngVerbTenseEnum	m_MainTense;
	CAuxVerbInfo (string AuxLemma,	EngVerbTenseEnum AuxTense,	long engPOS, EngVerbTenseEnum MainTense)
	{
		m_AuxLemma = AuxLemma;
		m_AuxTense = AuxTense;
		m_AuxEngPOS = engPOS;
		m_MainTense = MainTense;
	};

};
CAuxVerbInfo GetAuxVerbLemmaAndTense(EngVerbTenseEnum Tense) 
{
	switch (Tense) {
		case present_smp_tn		: return CAuxVerbInfo("do", present_smp_tn, eVERB,present_inf_tn);
		case past_smp_tn		: return CAuxVerbInfo("do", past_smp_tn, eVERB,present_inf_tn); 
		case future_cnt_tn		: return CAuxVerbInfo("be", future_smp_tn, eVBE, present_cont_inf_tn);		 
		case future_prf_tn		: return CAuxVerbInfo("be", future_smp_tn, eVBE, prf_inf_tn);		 
		case future_prf_cnt_tn	: return CAuxVerbInfo("be", future_smp_tn, eVBE, prf_cont_inf_tn);		  
		case future_smp_tn		: return CAuxVerbInfo("be", future_smp_tn, eVBE, present_inf_tn);
		case present_prf_cnt_tn	: return CAuxVerbInfo("have", present_smp_tn, eVERB, pp_cont_tn);
		case present_prf_tn		: return CAuxVerbInfo("have", present_smp_tn, eVERB, pp_tn);
		case past_prf_cnt_tn	: return CAuxVerbInfo("have", past_smp_tn, eVERB, pp_cont_tn);
		case past_prf_tn		: return CAuxVerbInfo("have", past_smp_tn, eVERB, pp_tn);
		case present_cnt_tn		: return CAuxVerbInfo("be", present_smp_tn, eVBE, ing_tn);
		case past_cnt_tn		: return CAuxVerbInfo("be", past_smp_tn, eVBE, ing_tn);
		case would_smp_tn		: return CAuxVerbInfo("will", past_smp_tn, eMOD, present_inf_tn);
		case would_prf_tn		: return CAuxVerbInfo("will", past_smp_tn, eMOD, prf_inf_tn);
		case would_cnt_tn		: return CAuxVerbInfo("will", past_smp_tn, eMOD, present_cont_inf_tn);
		case would_prf_cnt_tn	: return CAuxVerbInfo("will", past_smp_tn, eMOD, present_prf_cnt_tn);
		case gerund_prf_tn		: return CAuxVerbInfo("have", gerund_tn, eVERB, pp_tn);
		default					: return CAuxVerbInfo("", zero_tn, -1, zero_tn);
	};
};


// является ли данный узел вспомогательным  глаголом
bool	CEngSemStructure::IsAuxVerb(long NodeNo) const
{
	vector<long> outRels;
	GetOutcomingRelations(NodeNo,outRels);
	for( int i=0; i<outRels.size(); i++ )
	  if( m_Relations[outRels[i]].m_Valency.m_RelationStr == "AUX_VERB")
		return true;
  	  return false;
};


// имеет  ли данный узел вспомогательный  глагол
bool	CEngSemStructure::HasBeVerb(long NodeNo) const
{
	vector<long> outRels;
	GetRelations(NodeNo,outRels);
	for( int i=0; i<outRels.size(); i++ )
	{
	  if( m_Nodes[m_Relations[outRels[i]].m_SourceNodeNo].GetPos() == eVBE)
		return true;
  	  if( m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo].GetPos() == eVBE)
		return true;
	};
 	return false;
};


// выделение вспомогательного глагола
void	CEngSemStructure::BuildAuxiliaryVerbs()
{
// идем по все узлам 
	for( int NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++ )
	{
		if( IsAuxVerb(NodeNo) )
			continue;
		if( !m_Nodes[NodeNo].IsWordContainer() )
			continue;
		if( m_Nodes[NodeNo].RusNode==-1 )
			continue;
		if( m_Nodes[NodeNo].m_MainWordNo == -1 )
			continue;

		// берем время узла
        CEngSemWord& MainWord = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo];
        if( MainWord.GetTense() == zero_tn ||  
			MainWord.GetTense() == present_inf_tn )
			continue;

		// у модальных глаголов не бывает вспомогательных глаголов
		if( m_Nodes[NodeNo].GetType()!=NoneRoss && 
			GetRossHolder(m_Nodes[NodeNo].GetType())->HasFieldValue("GF","VERB_MODL",m_Nodes[NodeNo].GetUnitNo()) )
			continue;

		// одно из правил синтеза. потом, наверно, оно должно уйти, когла
		// начнем обрабатывать условные конструкции
		if( m_Clauses[m_Nodes[NodeNo].m_ClauseNo].m_HasParticleBY )
			  MainWord.SetTense(would_smp_tn, "Частица \'БЫ\'");


		// у глагола to be нет вспомогательных глаголов в настоящем и прошедшем временах
        if( (MainWord.GetTense()==past_smp_tn || MainWord.GetTense() == present_smp_tn) 
			&& m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma ==  "be" )
			continue;

		// в не вопросительном предложении во временах past_smp_tn и present_smp_tn
		// вспомогательный глагол не нужен
        if( (MainWord.GetTense()==past_smp_tn || MainWord.GetTense() == present_smp_tn) 
			&& !MainWord.m_bMorphologicalPassiveForm  
			&& !m_Nodes[NodeNo].HasRelOperator("НЕ") )
		{
			if( !RusStr.IsQuestionClause(RusStr.GetNode(m_Nodes[NodeNo].RusNode).m_ClauseNo) )
				continue;
			else // это все из-за Маши
			{
				int iQueRel,iSubRel;
				if( CheckQuestionClause(NodeNo,iQueRel,iSubRel)	)
					continue;
			}
		}

		// получаем лемму и время вспомогательного узла
		CAuxVerbInfo AuxVerb = GetAuxVerbLemmaAndTense(MainWord.GetTense());

		// строим пассив для простых времен
        if( (MainWord.GetTense()==past_smp_tn || MainWord.GetTense() == present_smp_tn) 
			&& MainWord.m_bMorphologicalPassiveForm )
		{
			 AuxVerb.m_AuxLemma = "be";
			 AuxVerb.m_AuxEngPOS = eVBE;
			 AuxVerb.m_AuxTense = MainWord.GetTense();
			 AuxVerb.m_MainTense = pp_tn;
			 MainWord.m_bMorphologicalPassiveForm = false;
		}
		
		// например, герундий не нуждается во вспомог. глаголе
		if (AuxVerb.m_AuxLemma == "")
			continue;

		// создаем узел
		CEngSemNode newNode;
		newNode.m_ClauseNo = m_Nodes[NodeNo].m_ClauseNo;
		CreateSimpleEnglNode(AuxVerb.m_AuxLemma, newNode, AuxVerb.m_AuxEngPOS, false);
		newNode.m_Words[0].SetTense(AuxVerb.m_AuxTense, "BuildAuxiliaryVerbs");
		// переносим информацию о числе и лице во вспомогательный глагол
		newNode.SetGrammemsRich( m_Nodes[NodeNo].GetGrammemsRich() & (eAllPersons | eAllNumbers) );
		// удаляем  эту информацию  в главном
		m_Nodes[NodeNo].DeleteGrammemsRich( eAllPersons | eAllNumbers);

		// переносим инофрмаицю об операторах на вспомогательный глагол
		newNode.m_RelOperators = m_Nodes[NodeNo].m_RelOperators;
		m_Nodes[NodeNo].m_RelOperators.clear();

		MainWord.SetTense(AuxVerb.m_MainTense, "BuildAuxiliaryVerbs");
		m_Nodes[NodeNo].m_bNotUseTo = true;

		// ставим узел перед главным словом
		newNode.m_Words[0].m_WordNo = m_Nodes[NodeNo].GetMinWordNo();
		FreeWordNo(newNode.m_Words[0].m_WordNo);
		m_Nodes.push_back(newNode);
		long AuxNodeNo = m_Nodes.size() - 1;

		// связи
		MoveMeaningRelsToNewNode(NodeNo,AuxNodeNo);
		PrintNodes();

		//  перевешиваем все наречия, которые должны стоять после вспомогательного глагола
		//  и до основного, на  вспомогательный глагол

		// проходим по всем отношениям графа,а не только по 
		// выходящим из главного глагола, поскольку
		// главный глагол может возникуть из правил (например, ApplyPredicativeRule)
		// и тогда группы времени будут зависеть от предикатива, а не от главного глагола (to be)
		for( int i=0; i<m_Relations.size(); i++ )
		{
			const CEngSemNode& node = m_Nodes[m_Relations[i].m_TargetNodeNo];
			if( node.m_ClauseNo != m_Nodes[AuxNodeNo].m_ClauseNo )
				continue;

			StringVector Positions;
			if(node.GetType() != NoneRoss)
				GetRossHolder(node.GetType())->GetSimpleFieldItemsFromArticle(node.GetUnitNo(), "POS", 0, 0, Positions);
			if (_find(Positions, "HAV"))
			{
				m_Relations[i].m_SourceNodeNo = AuxNodeNo;
				// указываем противоположное направление 
				// поскольку связь явялется реверсивной
				m_Relations[i].m_Position = "<<";
			}
		}

		// создаем отношение от вспомогательного глагола к главному
		CEngSemRelation newRel(CValency("AUX_VERB",A_C),AuxNodeNo,NodeNo,"");
		newRel.m_Position = ">>";
		newRel.m_PosType = FromAlgorithmPosType;
		m_Relations.push_back(newRel);
	}

	PrintRelations();

	for( long NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++ )
		HandleAdverbPositions(NodeNo);
}

EngVerbTenseEnum CEngSemStructure::handle_AVREM_field(long RelNodeNo,bool bEngStr,long TimNodeNo) const
{
	typedef pair<string,string> PairOfString; 
	vector<PairOfString> AVREM_values;

	const CSemanticStructure &Rus = RusStr;
	const CSemanticStructure &Eng = *this;
	const CSemanticStructure &Str = bEngStr ? Eng : Rus;

	if( !bEngStr )
		TimNodeNo = RelNodeNo;
	assert( TimNodeNo!=-1 );
	
	vector<long> rels;
	Str.GetOutcomingRelations(RelNodeNo,rels);
	for( int i=0; i<rels.size(); i++ )
	{
		const CSemRelation* rel = Str.GetRelation(rels[i]);
		const CSemNode &node = Str.GetNode(rel->m_TargetNodeNo);
		if(node.GetType() == NoneRoss)
			continue;
		DictTypeEnum type = node.GetType();
		WORD unit = node.GetUnitNo();
		if(node.IsTimeRossNode())
		{
			type = TimeRoss;
			unit = node.m_Colloc.GetRossInterp().m_UnitNo;
		}

		vector<TCortege> vec;
		GetRossHolder(type)->GetFieldValues("RUSETENSE",unit,vec);
		if( vec.size() == 0 )
			continue;
		for( long j=0; j<vec.size(); j++ )
		{
           if(vec[j].m_DomItemNos[0] == -1) 
			   continue; 
		   if(vec[j].m_DomItemNos[2] == -1) 
			   continue; 
		   string s0 = (const char*)GetRoss(type)->GetDomItemStr(vec[j].m_DomItemNos[0]);
		   string s2 = (const char*)GetRoss(type)->GetDomItemStr(vec[j].m_DomItemNos[2]);
		   AVREM_values.push_back(make_pair(s0,s2));
		}
	}

	if( AVREM_values.empty() ) 
		return zero_tn;

// врезка для деепричастий
	if( bEngStr )
	{
		int iRusNode = m_Nodes[RelNodeNo].RusNode;
		assert( iRusNode != -1 );
		if( RusStr.GetNode(iRusNode).HasPOS(ADVERB_PARTICIPLE) )
		{
			if( !RusStr.GetNode(TimNodeNo).HasOneGrammem(rPastTense) )
				return zero_tn;
			for( int j=0; j<AVREM_values.size(); j++ )
			{
				string s1 = AVREM_values[j].first;
				string s2 = AVREM_values[j].second;
				if( s1=="прш" && s2=="Simple" )
					return gerund_prf_tn;
			}
			return zero_tn;
		}
	}
//

	string past, present, future;
	string sv, nsv;
	
    string DefaultTense;
	for( int j=0; j<AVREM_values.size(); j++ )
	{
		string s0 = AVREM_values[j].first;
		string s2 = AVREM_values[j].second;
		if(s0 == "*") 
		{
			DefaultTense = s2;
		}
		else if(s0 == "нст")
		{
			if( present.empty() )
				present = s2;
		}
		else if(s0 == "прш")
		{
			if( past.empty() )
				past = s2;
		}
		else if(s0 == "буд")
		{
			if(future.empty())
				future = s2;
		}
		else if(s0 == "нсв")
		{
			nsv = s2;		
		}
		else if(s0 == "св")
		{
			sv = s2;
		}
		else
		{
			assert(false);
		}
	}
	
	if( DefaultTense.length() != 0 )
	{
		if(past.empty())	past = DefaultTense;
		if(present.empty())	present = DefaultTense;
		if(future.empty())	future = DefaultTense;
	}

	string perfective;
	if(sv.size()  &&  RusStr.GetNode(TimNodeNo).HasOneGrammem(rPerfective) )	perfective = sv;
	if(nsv.size() && !RusStr.GetNode(TimNodeNo).HasOneGrammem(rPerfective) )	perfective = nsv;

	string str;// = present;
	if( RusStr.GetNode(TimNodeNo).HasOneGrammem (rPresentTense) )
	{
		str = present;
		if(perfective.size()) str = perfective;
		return	 str == "Simple"	? present_smp_tn
				:str == "Cont"		? present_cnt_tn
				:str == "Perf"		? present_prf_tn
				:str == "PerfCont"	? present_prf_cnt_tn
				: zero_tn;
	}
	else
	if( RusStr.GetNode(TimNodeNo).HasOneGrammem( rPastTense) )
	{
		str = past;
		if(perfective.size()) str = perfective;
		return 	str == "Simple" ?	past_smp_tn
			:str == "Cont" ?	past_cnt_tn
			:str == "Perf" ?	present_prf_tn // present_prf_tn here
			:str == "PerfCont" ?	past_prf_cnt_tn
			:			zero_tn;
	}
	else
	if( RusStr.GetNode(TimNodeNo).HasOneGrammem( rFutureTense) )
	{
		str = future;
		if(perfective.size()) str = perfective;
		return 	str == "Simple" ?	future_smp_tn
			:str == "Cont" ?	future_cnt_tn
			:str == "Perf" ?	future_prf_tn
			:str == "PerfCont" ?	future_prf_cnt_tn
			:			zero_tn;
	};

	return zero_tn; 
}


bool CEngSemStructure::CorrectTenseByDictVerbFeatures(CEngSemNode& EngNode) 
{
	if (EngNode.m_MainWordNo == -1)
		return false;
	CEngSemWord& EngWord = EngNode.m_Words[EngNode.m_MainWordNo];

	bool not_cont =    (EngNode.GetType()!=NoneRoss) 
			        && GetRossHolder(EngNode.GetType())->HasFieldValue("RESTR", "not_cont",  EngNode.GetUnitNo()); 

    if( not_cont ) 
	{
		EngVerbTenseEnum NewTense = zero_tn;
		switch( EngWord.GetTense() )
		{
			case present_cnt_tn: NewTense = present_smp_tn; break;
			case past_cnt_tn: NewTense= past_smp_tn; break;
			case future_cnt_tn: NewTense = future_smp_tn; break;
			case would_cnt_tn: NewTense = would_smp_tn; break;

			case present_prf_cnt_tn: NewTense = present_prf_tn; break;
			case past_prf_cnt_tn: NewTense = past_prf_tn; break;
			case future_prf_cnt_tn: NewTense = future_prf_tn; break;
			case would_prf_cnt_tn: NewTense = would_prf_tn; break;
		}
		if( NewTense != zero_tn )
			EngWord.SetTense(NewTense,"RESTR = not_cont!");
	}


   bool cont =    (EngNode.GetType()!=NoneRoss) 
			        && GetRossHolder(EngNode.GetType())->HasFieldValue("RESTR", "cont",  EngNode.GetUnitNo()); 

   if (cont)
   {
		EngVerbTenseEnum NewTense = zero_tn;
		switch( EngWord.GetTense() )
		{
			case present_smp_tn: NewTense = present_cnt_tn; break;
			case past_smp_tn: NewTense= past_cnt_tn; break;
			case future_smp_tn: NewTense = present_cnt_tn; break;
			case would_smp_tn: NewTense = would_cnt_tn; break;

			case present_prf_tn: NewTense = present_prf_cnt_tn; break;
			case past_prf_tn: NewTense = past_prf_cnt_tn; break;
			case future_prf_tn: NewTense = future_prf_cnt_tn; break;
			case would_prf_tn: NewTense = would_prf_cnt_tn; break;
		}
		if( NewTense != zero_tn )
			EngWord.SetTense(NewTense,"RESTR =  cont!");
	}

   return false;
}

//------------------------------------------------------------------------

bool CEngSemStructure::CorrectTenseByDictConjFeatures(int iNode,CDictUnitInterp ConjInterp) 
{
	if( ConjInterp.m_DictType==NoneRoss ) 
		return false;

	EngVerbTenseEnum NewTense;
	
	vector<long> outRels;
	if( m_Nodes[iNode].m_NodeType == MNA )	
	{
		GetOutcomingRelations(iNode,outRels);
		if( outRels.size()<1 )
			return false;
		CEngSemNode& engNode = m_Nodes[m_Relations[outRels[0]].m_TargetNodeNo];
		if( engNode.m_MainWordNo == -1 )
			return false;
		NewTense =	engNode.m_Words[engNode.m_MainWordNo].GetTense();
	}
	else
	{
		CEngSemNode& engNode = m_Nodes[iNode];
		if( engNode.m_MainWordNo == -1 )
			return false;
		NewTense =	engNode.m_Words[engNode.m_MainWordNo].GetTense();
	}

#define ConjHasRestr(FieldName, Value) (GetRossHolder(ConjInterp.m_DictType)->HasFullFieldValue(FieldName, Value,  ConjInterp.m_UnitNo))

	if(ConjHasRestr("RESTR","not_fut !"))
		switch(NewTense){
			case future_smp_tn: NewTense = present_smp_tn; break;
			case future_prf_tn: NewTense = present_prf_tn; break;
			case future_cnt_tn: NewTense = present_cnt_tn; break;
			case future_prf_cnt_tn: NewTense = present_prf_cnt_tn; break;
		}

	if(ConjHasRestr("RESTR", "not_fut_past !"))
		switch(NewTense){
			case would_prf_cnt_tn: NewTense = past_prf_cnt_tn; break;
			case would_cnt_tn: NewTense = past_cnt_tn; break;
			case would_prf_tn: NewTense = past_prf_tn; break;
			case would_smp_tn: NewTense = past_smp_tn; break;
		}

	if(ConjHasRestr("RUSETENSE", "* - > Cont"))
		if(is_present(NewTense))		NewTense = present_cnt_tn;
		else if(is_future(NewTense))		NewTense = future_cnt_tn;
		else if(is_past(NewTense))		NewTense = past_cnt_tn;
		else if(is_would(NewTense))		NewTense = would_cnt_tn;
	
	if(ConjHasRestr("RUSETENSE", "буд - > present"))
		switch(NewTense){
			case future_smp_tn: NewTense = present_smp_tn; break;
			case future_prf_tn: NewTense = present_prf_tn; break;
			case future_cnt_tn: NewTense = present_cnt_tn; break;
			case future_prf_cnt_tn: NewTense = present_prf_cnt_tn; break;
		}

	if(ConjHasRestr("RUSETENSE", "прш - > past_prf"))
		if(is_past(NewTense))	NewTense = past_prf_tn;

	if (GetRossHolder(ConjInterp.m_DictType)->HasFullFieldValue("GF", ">> gerund",  ConjInterp.m_UnitNo, 2))
		NewTense = gerund_tn;

#undef ConjHasRestr

	if( m_Nodes[iNode].m_NodeType==MNA )	
	{
		for( int i=0; i<outRels.size(); i++ )
		{
			CEngSemNode& engNode = m_Nodes[m_Relations[outRels[i]].m_TargetNodeNo];
			if( engNode.m_MainWordNo == -1 )
				continue;
			engNode.m_Words[engNode.m_MainWordNo].SetTense(NewTense,"CorrectTenseByDictConjFeatures");
		}
	}
	else
	{
		CEngSemNode& engNode = m_Nodes[iNode];
		engNode.m_Words[engNode.m_MainWordNo].SetTense(NewTense,"CorrectTenseByDictConjFeatures");
	}

	return true;
};
