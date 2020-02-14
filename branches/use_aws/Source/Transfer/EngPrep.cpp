#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "StdAfx.h"
#include "EngSynthes.h"


void CEngSynthes::handle_prep(int node_no)
{
	const CSynRealization *eng = 0;
	int eng_rel = get_in_rel(node_no);
	int eng_clause_rel = get_in_clause_rel(node_no);
	/*
		Для определения предлога внутриклаузное отношение важнее, чем внешнее, например:
		"сообщает , в какой из зон находится BBS" здесь 
		"сообщает" требует, чтобы which было прямым дополнением, а "situate"
		требует, чтобы "which" было бы с предлогом. Побеждает "which".
	*/
	if(			(eng_clause_rel != -1)
			&& !ValencyIs(eng_clause_rel, "THESAME")		
			&& (eng_rel == -1)
	  ) 
	eng_rel = eng_clause_rel;

	/*
	  мне больше нечего добавить к тому, что вы уже сказали ->
	  I have nothing else to add to what wou have already said.
	*/
	if (   (E.m_Nodes[node_no].m_Words[0].m_Word == "what")
		&& (eng_clause_rel != -1)
	   )
	eng_rel = eng_clause_rel;

	if(eng_rel != -1 && is_subj_rel(eng_rel)) return;

	if(eng_rel == -1)
	{
		if(    Node(node_no).IsTimeRossNode() 
			|| Node(node_no).GetType() == TimeRoss
		  )
		return;

		eng = & (Node(node_no).m_SynReal);
		
	}
	else
	{
		eng = &(E.m_Relations[eng_rel].m_SynReal);
	}

	

	//нужно добавить предлоги к непервым членам однородного ряда, если они были приписаны к ним
	//Я жил в доме и на огороде
	if	(		(eng_rel != -1)
			&&	(Node(Rel(eng_rel).m_SourceNodeNo).m_NodeType == MNA) 
		)
	{
		int nd = Rel(eng_rel).m_SourceNodeNo;
		vector<long> Nodes;
		E.GetChildNodes(nd, Nodes);
		sort(Nodes.begin(),Nodes.end(),IsLessByMinWordNo(&E));
		if (node_no != Nodes[0])
			if (!Node(node_no).m_SynReal.m_Preps.empty())
				eng = & (Node(node_no).m_SynReal);
	};

    if (!Res(node_no).has_prep())
	  if(!eng->m_Preps.empty())
	  {
			const CDictUnitInterp &prep_int = eng->m_Preps[0];
			if(    prep_int.m_UnitNo != ErrUnitNo
			    && prep_int.m_DictType == EngObor
			  )
			Res(node_no).m_prep = E.m_pData->GetEngOborStr(prep_int.m_UnitNo);
	  }


// Он читал, улыбаясь и напевая песенку
// Он пел, бежавши домой
	if(	!Res(node_no).has_prep()
		&& eng_rel != -1 
		&& (E.m_Relations[eng_rel].m_Valency.m_RelationStr == "TOGETHER")		
		&& (E.m_Nodes[E.m_Relations[eng_rel].m_SourceNodeNo].m_ClauseNo !=  E.m_Nodes[E.m_Relations[eng_rel].m_TargetNodeNo].m_ClauseNo)
		&& (Node(node_no).GetTense() == gerund_tn)
		&& !E.IsAuxVerb(node_no)
	   )
	{
		int iRusNode = E.m_Nodes[node_no].RusNode;
		if( iRusNode!=-1 &&
			!(E.RusStr.GetNode(iRusNode).GetGrammems() & _QM(rPresentTense) ) 
			)
			Res(node_no).m_prep = "while";
	}

	if(   !Res(node_no).has_prep() 
		&& eng_rel != -1 
		&& !Rel(eng_rel).m_SynReal.m_Other.empty()
	  )
	{
		Res(node_no).m_prep = Rel(eng_rel).m_SynReal.m_Other;
	}

	// by 
	if(    !Res(node_no).has_prep()
		&& eng_rel != -1
	    && !Rel(eng_rel).m_Pattern.IsEmpty() 
	 	&& Res(Rel(eng_rel).m_SourceNodeNo).is_passive_verb
		&& E.HasThisGX(Rel(eng_rel).m_Pattern.m_GramCorteges, "obj", Node(Rel(eng_rel).m_SourceNodeNo).GetType())
	  )
	{
	  Res(node_no).m_prep = "by";
	}
 		

}







