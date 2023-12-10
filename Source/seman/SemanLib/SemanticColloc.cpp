#include "stdafx.h"
#include "SemanticRusStructure.h"


void CRusSemStructure::AddCollocHyp (long StartNodeNo, 
									 const std::unordered_map<long, long>& ref_colloc_items,
									 long ItemNo,
									 CSemCollocHyp& Hyp,
									 std::vector<CSemCollocHyp>& Hyps)
{
	if (ItemNo == 0)
		Hyp.m_Coords.clear();

	if (StartNodeNo >= m_Nodes.size()) return;

	const CRusSemClause&  Clause = m_Clauses[m_Nodes[StartNodeNo].m_ClauseNo];

	for (; StartNodeNo < Clause.m_EndNodeNo; StartNodeNo++)
		if (ref_colloc_items.find(StartNodeNo) != ref_colloc_items.end() )
			for (const auto& c: m_pData->m_RusCollocItemRefs[ref_colloc_items.find(StartNodeNo)->second].Refs)
			{
				if (		(c.CollocNo == Hyp.m_CollocNo)
						&&	(c.ItemNo == ItemNo)
					)
				{
					
					CSemCollocHyp SaveHyp = Hyp;
					{
						CSemCollocHyp::CCollocItemNoAndWordNo Pair;
						Pair.m_CollocItemNo = ItemNo;
						Pair.m_WordNo =  m_Nodes[StartNodeNo].m_Words[0].m_WordNo;
						Hyp.m_Coords.push_back(Pair);
					};

					
					const CColloc& Colloc = m_pData->m_RusCollocs[Hyp.m_CollocNo];
					bool bCheckPunctMark = true;

					//  passing all holes and checking punctuation marks
					long NextItemNo = ItemNo+1;
					while	(		(NextItemNo < Colloc.Items.size()) 
								&&	(		Colloc.Items[NextItemNo].IsHole() 
										||	std::iswpunct(Colloc.Items[NextItemNo].Item[0])
									)
							)
					{
						if (!Colloc.Items[NextItemNo].IsHole())
							if (std::iswpunct(Colloc.Items[NextItemNo].Item[0]))
							{
								/*
								Здесь приходится рассматривать два случая:
									1.  когда знак препинания был удален из m_Nodes и включен в СRusSemWord::m_PostPuncts
									2.  когда он не был удален.  
									В первом случае мы должны сейчас проверить его  с помощью HasPostPunct,
									во втором - проверка бедет сделана на общих основаниях, как будто 
									этот знак препинания обычное слово.

								*/
								if (	(StartNodeNo +1 != Clause.m_EndNodeNo)
										&&	m_Nodes[StartNodeNo +1].IsWordContainer()
										&&	std::iswpunct(m_Nodes[StartNodeNo +1].m_Words[0].GetWord()[0])
									)
									break;
								else
								if (!m_Nodes[StartNodeNo].HasPostPunct(Colloc.Items[NextItemNo].Item[0]))
								{
									bCheckPunctMark = false;
									break;
								};
							};
						NextItemNo++; 
					};



					if  (NextItemNo < Colloc.Items.size()) 
					{
						if	(		(StartNodeNo+1 != Clause.m_EndNodeNo)
								&&	bCheckPunctMark
							)
						{

							AddCollocHyp (StartNodeNo+1, ref_colloc_items, NextItemNo,Hyp, Hyps);
						};
					}
					else
						Hyps.push_back(Hyp);
					Hyp = SaveHyp;
				}
			}

};


bool CRusSemStructure::CheckCollocItemGramFet(long CollocNo, long ItemNo, long NodeNo)
{
	assert (NodeNo != -1); 

	CCollocItem& I = m_pData->m_RusCollocs[CollocNo].Items[ItemNo];
	if (    !I.RelOperator1.empty()
		&&  !m_Nodes[NodeNo].HasRelOperator(I.RelOperator1)
		)
		return false;

	if (    !I.RelOperator2.empty()
		&&  !m_Nodes[NodeNo].HasRelOperator(I.RelOperator2)
		)
		return false;

	// проверка частицы "не"
	// факультативно частицу "не" может принимать только первое слово словосочетания
	if (    ( (I.RelOperator1 == "НЕ") || (I.RelOperator2 == "НЕ") )
		&&  m_Nodes[NodeNo].HasRelOperator("НЕ")
		&&  (ItemNo > 0)
		)
		return false;

	// проверка предлога
	std::vector<CRossInterp> Preps;
	m_pData->GetPrepsFromArticle (GetRoss(CollocRoss), m_pData->m_RusCollocs[CollocNo].UnitNo, 0, ItemNo+1, Preps);
	long l=0;
	for (; l <Preps.size(); l++)
		if (m_Nodes[NodeNo].HasThePrep(Preps[l].m_UnitNo)) 
			break;
	if ( (Preps.size() > 0) && (l == Preps.size())) 
		return false;


	// проверка граммем
	CSemPattern P;
	P.InitSemPattern(GetRossHolder(CollocRoss), m_pData->m_RusCollocs[CollocNo].UnitNo, 0, ItemNo+1);
	P.LoadGramFromDict();
	if (P.m_GramCorteges.empty())  return true;

	long GramCortegeNo=0;
	for (; GramCortegeNo < P.m_GramCorteges.size(); GramCortegeNo++)
	{
		TCortege& C = P.m_GramCorteges[GramCortegeNo];
		part_of_speech_mask_t Pose;
		grammems_mask_t Grammems;
		std::string GramFet = GetRoss(CollocRoss)->WriteToString(C);
		Trim(GramFet);
		m_pData->GetCustomGrammems(GramFet, Grammems, Pose);
		if (   m_Nodes[NodeNo].HasGrammems (Grammems) 
			&& ((m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Poses & Pose) >  0)
		 )
		 break;

	};


	return  GramCortegeNo < P.m_GramCorteges.size();
};

bool TwoSemCollocHypsCanBeTogether(const CSemCollocHyp& H1, const CSemCollocHyp& H2)
{
  for (long i=0; i < H1.m_Coords.size(); i++)
    for (long k=0; k < H2.m_Coords.size(); k++)
      if (H1.m_Coords[i].m_WordNo == H2.m_Coords[k].m_WordNo)
	    return false;
  return true;  
};

bool VectorSemCollocHypCanBeTogether(const CSemCollocHypVector& H)
{
  for (long i=0; i < H.size(); i++)
   for (long k=i+1; k < H.size(); k++)
	 if (!TwoSemCollocHypsCanBeTogether(H[i], H[k]))
		 return false;
  return true;  
};


void get_subsets_with_this_len(long len, std::vector<int>& subset, std::vector<std::vector<int> >& subsets, std::vector<int>::iterator the_set_it, std::vector<int>::iterator set_end)
{	
	std::vector<int>::iterator it = the_set_it;
	int size = subset.size();
	for(; it != set_end ; )
	{
		subset.push_back(*it);
		if( len - 1 > 0 )
			get_subsets_with_this_len(len - 1, subset, subsets, ++it, set_end);
		else
		{
			subsets.push_back(subset);
			++it;
		}
		subset.erase(subset.begin() + size, subset.begin() + subset.size());
	}
}

void get_all_subsets(std::vector<int>& the_set, std::vector<std::vector<int> >& subsets)
{
	for(int i = 1 ; i <= the_set.size() ; i++ )
	{
		std::vector<int> subset;
		get_subsets_with_this_len(i, subset, subsets, the_set.begin(), the_set.end() );
	}

}

// строит множество всех возможных наборов словосочетаний
// и записывает его в слот  m_Clauses[ClauseNo].m_SetCollocHyps, без последнего 
// не будет работать процедура BuildBlackBoxCollocs, которая вызывается из процедуры
// SetLexVariant (построение лексического варианта) 
void CRusSemStructure::FindCollocsHyps(long ClauseNo)
{
	std::vector<CSemCollocHyp> AllHyps;
	CRusSemClause& Clause = m_Clauses[ClauseNo];
	std::unordered_map<long, long> ref_colloc_items;

	for (long i = Clause.m_BeginNodeNo; i < Clause.m_EndNodeNo; i++)
	{
		// мы предполагаем, что в поле CONTENT входят только примитивные узлы
		if (!m_Nodes[i].IsPrimitive()) continue;
		std::string S  =  m_Nodes[i].m_Words[0].m_Lemma;
		if (S == "") S = m_Nodes[i].m_Words[0].GetWord();
		auto& refs = m_pData->m_RusCollocItemRefs;
		auto it = lower_bound(refs.begin(), refs.end(), S, LessCollocItemRefCollect());
		if ((it != refs.end()) && (it->Item == S))
			ref_colloc_items[i] = it - refs.begin();
	};

	// получение множетсва гипотез
	for (long i=0; i<m_pData->m_RusCollocs.size();i++)
	{
		CSemCollocHyp Hyp(i);
		AddCollocHyp(Clause.m_BeginNodeNo, ref_colloc_items, 0, Hyp, AllHyps);
	}

	// проверка GF(i)
	for (long i=0; i<AllHyps.size();)
	{
		LOGV << "check GF(*) for " << GetRoss(CollocRoss)->GetEntryStr(m_pData->m_RusCollocs[AllHyps[i].m_CollocNo].UnitNo);
		long k=0;
		for (; k < AllHyps[i].m_Coords.size(); k++)
			if (!CheckCollocItemGramFet(
				AllHyps[i].m_CollocNo, 
				AllHyps[i].m_Coords[k].m_CollocItemNo, 
				FindNodeByWordNo(AllHyps[i].m_Coords[k].m_WordNo, ClauseNo)
				)
				)
				break;

		if (k < AllHyps[i].m_Coords.size())
			AllHyps.erase(AllHyps.begin() + i);
		else
			i++;
	};

	// проверка SYNREP
	for (long i=0; i<AllHyps.size();i++)
	{
		LOGV << "check SYNREP for " << GetRoss(CollocRoss)->GetEntryStr(m_pData->m_RusCollocs[AllHyps[i].m_CollocNo].UnitNo);
		const CColloc& C = m_pData->m_RusCollocs[AllHyps[i].m_CollocNo];
		for (long j=0; j <  C.m_Rels.size(); j++)
			if (C.m_Rels[j].m_SynRelName == "ПРИЛ_СУЩ")
			{
				std::vector<long> Rels;
				long TargetNodeNo = FindNodeByWordNo(AllHyps[i].m_Coords[C.m_Rels[j].m_TargetNodeNo].m_WordNo, ClauseNo);
				long SourceNodeNo = FindNodeByWordNo(AllHyps[i].m_Coords[C.m_Rels[j].m_SourceNodeNo].m_WordNo, ClauseNo);
				GetIncomingSynRelations(TargetNodeNo, Rels);
				long l=0;
				for (; l < Rels.size(); l++)
					if ( (SourceNodeNo == m_SynRelations[Rels[l]].m_SourceNodeNo))
						if (C.m_Rels[j].m_SynRelName == m_SynRelations[Rels[l]].m_SynRelName)
							break;
				if (l == Rels.size() ) 
				{
					AllHyps.erase(AllHyps.begin() +i);
					i--;
					break;
				};
			}
	}



	LOGV << "Hypots of collocations for clause: " << ClauseNo;
	for (long i=0; i<AllHyps.size();i++)
		LOGV  << GetRoss(CollocRoss)->GetEntryStr(m_pData->m_RusCollocs[AllHyps[i].m_CollocNo].UnitNo);

	// получение всех подмножеств множества гипотез для условных словосочетаний
	std::vector<std::vector<int> > subsets;
	std::vector<int> the_set;
	for (long i=0; i<AllHyps.size();i++)
		if (m_pData->m_RusCollocs[AllHyps[i].m_CollocNo].IsConditional)
			the_set.push_back(i);
	get_all_subsets(the_set, subsets);

	//добавление пустого подмножества
	m_ClauseSetCollocHyps[ClauseNo].clear();
	CSemCollocHypVector C;
	m_ClauseSetCollocHyps[ClauseNo].push_back(C);

	// сначала добавляем условные словосочетания
	for (long i=0; i <subsets.size(); i++)
	{
		CSemCollocHypVector C;

		for(long k=0; k <subsets[i].size(); k++)
			C.push_back(AllHyps[subsets[i][k]]);

		m_ClauseSetCollocHyps[ClauseNo].push_back(C);
	};


	// затем безусловные (для всех подмножеств одно и то же)
	for (long i=0; i < m_ClauseSetCollocHyps[ClauseNo].size(); i++)
	{
		CSemCollocHypVector& C = m_ClauseSetCollocHyps[ClauseNo][i];

		for (long k=0; k<AllHyps.size();k++)
			if (!m_pData->m_RusCollocs[AllHyps[k].m_CollocNo].IsConditional)
				C.push_back(AllHyps[k]);
	};


	// удаление всех несовместных множеств
	for (long i=0; i < m_ClauseSetCollocHyps[ClauseNo].size(); )
		if (!VectorSemCollocHypCanBeTogether(m_ClauseSetCollocHyps[ClauseNo][i]))
			m_ClauseSetCollocHyps[ClauseNo].erase(m_ClauseSetCollocHyps[ClauseNo].begin() + i);
		else
			i++;


	if (m_ClauseSetCollocHyps[ClauseNo].size() == 0)
	{
		m_ClauseSetCollocHyps[ClauseNo].push_back(CSemCollocHypVector());
	};
};



void CRusSemStructure::BuildBlackBoxCollocs(long ClauseNo, size_t SetNo)
{
	SetNodeToDeleteFalse();

	// идем по всем словосочетаниям, которые нужно построить
	for (long i=0; i <m_ClauseSetCollocHyps[ClauseNo][SetNo].size(); i++)
	{
		const CSemCollocHyp& H = m_ClauseSetCollocHyps[ClauseNo][SetNo][i];
		CColloc& Colloc = m_pData->m_RusCollocs[H.m_CollocNo];
		if (!Colloc.IsBlackBox) continue;
		CRusSemNode N;
		N.m_ClauseNo = ClauseNo;
		N.SetMainWordNo(-1);
		bool	IsClauseSyntaxTop = false;


		for (long k=0; k <H.m_Coords.size(); k++)
		{
			long NodeNo = FindNodeByWordNo(H.m_Coords[k].m_WordNo, ClauseNo);
			IsClauseSyntaxTop |= (m_Clauses[ClauseNo].m_ClauseSyntaxTop == NodeNo);
			assert (NodeNo != -1);
			assert (m_Nodes[NodeNo].IsPrimitive());
			bool bMainItem = (Colloc.m_SemMainWord-1  == H.m_Coords[k].m_CollocItemNo) 
					||	( (k ==0) && (Colloc.m_SemMainWord == -1));
			{
				CRusSemWord W = m_Nodes[NodeNo].m_Words[0];

				//  adding prepositon for the synthesis and for a better format
				std::string Prep = GetPrepOrConjOfNode(m_Nodes[NodeNo]);
				if (!Prep.empty() && !bMainItem  )
					W.SetWord(Prep + " " + W.GetWord());
				N.m_Words.push_back(W);
			};
			// если это главное слово, то нужно изменить характеристики всего словосочетания
			// если главного слова нет, тогда нужно взять характеристики от первого слова словосочетания
			if	(bMainItem)
			{
				N.SetMainWordNo(k);
				N.SetGrammems ( m_Nodes[NodeNo].m_Words[0].GetAllGrammems() );
				std::string debug =  m_pData->GetRusGramTab()->GrammemsToStr( m_Nodes[NodeNo].m_Words[0].GetAllGrammems() );
				N.m_GramCodes =  m_Nodes[NodeNo].m_GramCodes;
				N.m_SynReal.m_Preps = m_Nodes[NodeNo].m_SynReal.m_Preps;
				m_pData->GetPrepsFromArticle (GetRoss(CollocRoss), m_pData->m_RusCollocs[H.m_CollocNo].UnitNo, 0, k, N.m_SynReal.m_Preps);
				if ( k==0)
				{
					N.m_RelOperators=m_Nodes[NodeNo].m_RelOperators;
				};
			};
			// удаляем примитивный узел
			assert (CanBeDeleted(NodeNo));
			m_Nodes[NodeNo].m_bToDelete = true;;
		};
		N.AddInterp( CDictUnitInterp(GetRossHolder(CollocRoss), CollocRoss,m_pData->m_RusCollocs[H.m_CollocNo].UnitNo, false,false) );
		InitVals(N);
		long OldNodeNo = FindNodeByWordNo(N.m_Words[N.m_MainWordNo].m_WordNo, ClauseNo);
		//сохраняю синтаксические отношения, которые былит приписаны  OldNodeNo
		// в виртуальный узел -2 (-1 используется как ошибка)
		MoveSynRelations(OldNodeNo, -2);
		//удаляю примитивные узлы, из которых состоит словосочетание, в том числе и  OldNodeNo
		DelNodesToDelete();
		// строю новый узел со словосочетанием, куда вошли только что удаленные,
		// а удалить их нужно было, поскольку  функция InsertNode не может добавить узел, который
		// по словам пересекается с другим существующим уже узлом
		long nd = InsertNode(N);
		// теперь вспоминаем синтаксические отношения, приписанные  OldNodeNo
		MoveSynRelations(-2, nd);

		if (IsClauseSyntaxTop)
			m_Clauses[ClauseNo].m_ClauseSyntaxTop = nd;

	};

};
