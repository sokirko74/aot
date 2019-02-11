#include "stdafx.h"
#include "SemanticRusStructure.h"



void CRusSemStructure::InterpretOrganisations (long ClauseNo)
{
		// ORG + GEO     => LOK (GEO, ORG)	
	for (long i=m_Clauses[ClauseNo].m_BeginNodeNo; i < m_Clauses[ClauseNo].m_EndNodeNo - 1 ; i++)
	   if (m_Nodes[i].m_bOrgName && IsLocNode(i+1)) 
		   {
				CValency V ("LOK", A_C);			  
				AddRelation(CRusSemRelation(V, i, i+1, ""));
				m_Nodes[i].m_Vals.push_back(V);
		   }
		   else 
// ORG1 + ORG2,рд  => BELNG(ORG1, ORG2)		   
			if (m_Nodes[i].m_bOrgName && m_Nodes[i+1].m_bOrgName) 
			{
				CValency V ("BELNG", C_A);			  
				AddRelation(CRusSemRelation(V, i, i+1, ""));
				m_Nodes[i].m_Vals.push_back(V);
			}
};

// все термины были разделены на отдельные слова 
// создаем отношение из Nd2 в Nd1  (нумерация берется из R (Nd1, Nd2))) 
void CRusSemStructure::ApplyTerminSemStrForOneRel (string RelationStr, long Nd1, long Nd2, const CRossHolder* RossHolder)
{
	vector<long> Rels;

	//  удаляем все отношения, которые идут из Nd2 c названием RelationStr кроме тех, что идут в MUA GetOutcomingRelations(Nd2, Rels, false);
	for (long i=0; i < Rels.size();)
		if (    (m_Relations[Rels[i]].m_Valency.m_RelationStr != RelationStr)
			|| (m_Nodes[m_Relations[Rels[i]].m_TargetNodeNo].m_NodeType == MNA)
			)
			Rels.erase(Rels.begin() + i);
		else
			i++;
	DeleteRelations(Rels);

	//  удаляем все отношения, которые идут в Nd1 кроме тех, что идут из MUA 
	CRelSet R = GetIncomingRelations(Nd1, false);
	for (long i=0; i < R.m_RelsCount;)
		if ( m_Nodes[m_Relations[R.m_Rels[i]].m_SourceNodeNo].m_NodeType == MNA)
			R.Erase(i);
		else
			i++;
	DeleteRelSet(R);


	// удаляем отношение, идущее из Nd1 в Nd2, если такое было
	FindRelations(Nd1, Nd2, Rels);
	DeleteRelations(Rels);

	// создаем новое отношение 
	CValency V (RelationStr, A_C,RossHolder);
	AddRelation(CRusSemRelation(V, Nd2, Nd1, ""));
	if (find (m_Nodes[Nd2].m_Vals.begin(),m_Nodes[Nd2].m_Vals.end(), V) == m_Nodes[Nd2].m_Vals.end())
		m_Nodes[Nd2].m_Vals.push_back(V);
};




long  CRusSemStructure::AddThesSemRelations(const CRossHolder* Dict, long UnitNo, long StartNodeNo)
{
  vector<CDopField> DopFields;
  long MainItemNo = Dict->GetDopFields(UnitNo, DopFields);
  if (MainItemNo == -1) return false;
  long Result = -1;
  long EndClauseNo = m_Clauses[m_Nodes[StartNodeNo].m_ClauseNo].m_EndNodeNo;
  long SaveDopRelationsCount = m_ThesSemRelations.size();
  for (size_t k=0; k < DopFields.size(); k++)
	 {
		 CSemThesRelation Rel;
		 Rel.m_TargetNodeNo = StartNodeNo + DopFields[k].m_Word1-1;
  		 Rel.m_SourceNodeNo = StartNodeNo + DopFields[k].m_Word2-1;
		 Rel.m_SemRelName = DopFields[k].m_RelationStr;
		 if (MainItemNo == DopFields[k].m_Word2)
			Result = Rel.m_SourceNodeNo;
		 if (   (Rel.m_SourceNodeNo >= EndClauseNo)
			 || (Rel.m_TargetNodeNo >= EndClauseNo)
			)
		 {
			 ErrorMessage (" Ошибка в интерпретации поля AUX");
			 m_ThesSemRelations.erase(m_ThesSemRelations.begin() +SaveDopRelationsCount, m_ThesSemRelations.end());
			 return -1;
		 };
		 m_ThesSemRelations.push_back(Rel);
	 };
  return Result;	
};

bool CRusSemStructure::ReadDopField(long ClauseNo, long StartNodeNo, const CRossHolder* Dict, long UnitNo, long CollocId)
{
	long SaveDopRelationsCount = m_ThesSemRelations.size();
	long MainNodeNo = AddThesSemRelations(Dict, UnitNo, StartNodeNo);
	if (MainNodeNo == -1)
		return false;
	const CThesaurus* Thes = m_pData->GetThes(m_Nodes[StartNodeNo].m_ThesaurusId);
	string ErrorMess = "Ошибка в поле AUX термина ";  
	ErrorMess += Thes->m_Termins[Thes->GetTerminNoByTextEntryId(m_Nodes[StartNodeNo].m_TerminId)].m_TerminStr;

	for (size_t k=SaveDopRelationsCount; k < m_ThesSemRelations.size(); k++)
	{
		long Nd1 = m_ThesSemRelations[k].m_TargetNodeNo;
		m_Nodes[Nd1].m_CollocId = CollocId;
		m_Nodes[Nd1].m_Colloc.m_Type = ThesType;
		m_Nodes[Nd1].m_Colloc.GetThesInterp().m_ThesaurusId =  m_Nodes[StartNodeNo].m_ThesaurusId;
		m_Nodes[Nd1].m_Colloc.GetThesInterp().m_ItemNo = Nd1 - StartNodeNo;
		m_Nodes[Nd1].m_Colloc.GetThesInterp().m_TerminId = m_Nodes[StartNodeNo].m_TerminId; 
		m_Nodes[Nd1].m_Colloc.GetThesInterp().m_bMainWord = (MainNodeNo == Nd1);
		


		long Nd2 = m_ThesSemRelations[k].m_SourceNodeNo;
		m_Nodes[Nd2].m_CollocId = CollocId;
		m_Nodes[Nd2].m_Colloc.m_Type = ThesType;
		m_Nodes[Nd2].m_Colloc.GetThesInterp().m_ThesaurusId =  m_Nodes[StartNodeNo].m_ThesaurusId;
		m_Nodes[Nd2].m_Colloc.GetThesInterp().m_ItemNo = Nd2 - StartNodeNo;
		m_Nodes[Nd2].m_Colloc.GetThesInterp().m_TerminId = m_Nodes[StartNodeNo].m_TerminId; 
		m_Nodes[Nd2].m_Colloc.GetThesInterp().m_bMainWord =(MainNodeNo == Nd2);
	};

	return m_ThesSemRelations.size() > SaveDopRelationsCount;
};

WORD CRusSemStructure::GetArticleByModel (long TerminId, int ThesaurusId) const
{
	const CThesaurus* Thes = m_pData->GetThes(ThesaurusId);
    int No =  Thes->GetTerminNoByTextEntryId(TerminId);
	const CInnerTermin& Termin = Thes->m_Termins[No];
	long ModelNo = Termin.m_ModelNo;
	const CInnerModel& Model =	Thes->m_Models[ModelNo];
	long ModelId = Model.m_ModelId;
	string S = Format("@Модель%i",ModelId);
	return  GetRoss(GetRossIdByThesId(ThesaurusId))->LocateUnit(S.c_str(), 1);
};



/*
 функция загружает поле AUX в слот m_ThesSemRelations
*/

void CRusSemStructure::ReadDopFieldForClause(long ClauseNo)
{
	for (long StartNodeNo=m_Clauses[ClauseNo].m_BeginNodeNo; StartNodeNo < m_Clauses[ClauseNo].m_EndNodeNo ; StartNodeNo++)
		if (     (m_Nodes[StartNodeNo].m_ThesaurusId != NoneThes)
			&& (m_Nodes[StartNodeNo].m_bFirstInTermin)
			)
		{
			int ThesaurusId = m_Nodes[StartNodeNo].m_ThesaurusId;

			const CRossHolder* Dict = GetRossHolder(GetRossIdByThesId(ThesaurusId));

			if (Dict == 0) continue;
			string UnitStr = Format("%i",m_Nodes[StartNodeNo].m_TerminId);

			assert (m_Nodes[StartNodeNo].m_Words.size() > 0);
			long CollocId = m_Nodes[StartNodeNo].m_Words[0].m_WordNo;

			long UnitNo = Dict->LocateUnit(UnitStr.c_str(), 1);

			if  (     (UnitNo == ErrUnitNo) 
				||  !ReadDopField(ClauseNo, StartNodeNo, Dict, UnitNo, CollocId)
				)
				/*
				если нет статьи или нет поля AUX, тогда пробем прочесть 
				статью заглушку.
				*/
				try  {
					UnitNo = GetArticleByModel(m_Nodes[StartNodeNo].m_TerminId, ThesaurusId);
					if  (UnitNo != ErrUnitNo) 
						ReadDopField(ClauseNo, StartNodeNo, Dict, UnitNo, CollocId);
				}
				catch (...) {
					continue;
				}
		};
};

void CRusSemStructure::ReadThesInterps(long ClauseNo)
{
	for (long StartNodeNo=m_Clauses[ClauseNo].m_BeginNodeNo; StartNodeNo < m_Clauses[ClauseNo].m_EndNodeNo ; StartNodeNo++)

		if (		 (m_Nodes[StartNodeNo].m_ThesaurusId != NoneThes)
				&&	(m_Nodes[StartNodeNo].m_bFirstInTermin)
			)
		{
			const CRossHolder* Dict = GetRossHolder(GetRossIdByThesId(m_Nodes[StartNodeNo].m_ThesaurusId));
			if (Dict == 0) continue;
			string UnitStr = Format("%i",m_Nodes[StartNodeNo].m_TerminId);
			long UnitNo = Dict->LocateUnit(UnitStr.c_str(), 1);
			if  (     (UnitNo != ErrUnitNo) 
				&&  !Dict->GetRoss()->IsEmptyArticle(UnitNo)
				)
			{
				/*
				если в статье термина есть поле VAL, тогда находим главное слово термина
				и приписываем ему эту словарную статью, стираяя все его 
				предыдущие словарные интерпретации
				*/
				size_t i = Dict->GetRoss()->GetUnitStartPos(UnitNo);

				for (; i<= Dict->GetRoss()->GetUnitEndPos(UnitNo); i++)
					if ( GetCortege(Dict->GetRoss(),i).m_FieldNo ==  Dict->ValFieldNo )
						break;

				if (	i <= Dict->GetRoss()->GetUnitEndPos(UnitNo))
				{
					for (long  i=0; i <  m_Nodes.size(); i++)
						if (   (m_Nodes[i].m_CollocId == m_Nodes[StartNodeNo].m_CollocId)
							&& (m_Nodes[i].m_Colloc.m_Type != NoneType)
							&& m_Nodes[i].m_Colloc.GetThesInterp().m_bMainWord
							)
						{
							m_Nodes[i].SetInterp(CDictUnitInterp(Dict, GetRossIdByThesId(m_Nodes[StartNodeNo].m_ThesaurusId), UnitNo, false, false));
							m_Nodes[i].GetInterp()->m_TerminId = m_Nodes[StartNodeNo].m_TerminId;
							// главное слово может быть только одно
							break;
						};

				};
			};


		};
};


void CRusSemStructure::ApplyTerminSemStr(long ClauseNo)
{

  for (long i =0; i < m_ThesSemRelations.size(); i++)
  if (m_Nodes[m_ThesSemRelations[i].m_SourceNodeNo].m_ClauseNo == ClauseNo)
	 // признаков может быть много
	if (m_ThesSemRelations[i].m_SemRelName != "PROPERT")	 
	{
		  long SourceNodeNo = m_ThesSemRelations[i].m_SourceNodeNo;
		  const CRossHolder* Dict = GetRossHolder(GetRossIdByThesId(m_Nodes[SourceNodeNo].m_ThesaurusId));

		  ApplyTerminSemStrForOneRel (m_ThesSemRelations[i].m_SemRelName, 
			  m_ThesSemRelations[i].m_TargetNodeNo, 
			  SourceNodeNo, 
			  Dict);
	 };

};



WORD CRusSemStructure::GetUnitNoByTerminId(DictTypeEnum   DictType,	long TerminId) const
{
     const CRossHolder* Dict = GetRossHolder(DictType);
     if (Dict == 0) return ErrUnitNo;
     string S = Format("%i",TerminId);
     long UnitNo = Dict->LocateUnit(S.c_str(), 1);
 
     if (UnitNo == ErrUnitNo)  
	 {
		 UnitNo = GetArticleByModel (TerminId, GetThesIdByRossId(DictType));
	 };
     return UnitNo;
};


void CRusSemStructure::GetThesInterps(string UnitStr, const CRusSemWord& W, const CThesaurus* Thes, DictTypeEnum   DictType,	CRusSemNode& N) const
{
	try 
	{

		long TerminId = Thes->GetTerminIdBySingleWord(UnitStr);
		if (TerminId ==-1) return;
		long TerminNo = Thes->GetTerminNoByTextEntryId(TerminId);


		/*
		выше по тексту мы объявили, что все первые слова предложений  как бы не написаны 
		с маленькой буквы (чтобы на синтезе они не писались автоматически с большой буквы)
		но при сравнении с тезаурусом учитывается регистр: если в тексте было написано 
		с маленькой  буквы, а в тезаурусе с большой, то такой теримн не рассматривается.
		Таким образом, для слова Microsoft  в начале предложения прописывается LowLow, из-за
		чего оно не интерпретируется в тезаурусе. 
		Поэтому приходится здесь считать, то первой слово написано с большой буквы.

		*/

		/*
		если не совпадает регистр, то нужно выйти 
		*/
		RegisterEnum   CharCase = W.m_CharCase;
		if (W.m_WordNo == 0)
			CharCase = UpLow;

		const CInnerTermin& T = Thes->m_Termins[TerminNo];
		assert(T.m_Items.size() > 0);
		// assert(T.m_Items.size() == 1); этого делать нельзя, поскольку дефисные слова имеют леммы для каждой дефисной части

		if	(CharCase == LowLow)
		{
			const CInnerSynItem&  SynItem = Thes->m_SynItems[T.m_Items[0]];
			if	(		(SynItem.m_Flags & siUpLw) 
					||	(SynItem.m_Flags & siUpUp)
				)
				return;
		}

		//  проверка части речи
		int ModelNo = Thes->m_Termins[TerminNo].m_ModelNo;
		if (ModelNo == -1) return;
		const CInnerModel& M = Thes->m_Models[ModelNo];
		if (M.m_AtomGroups.size() != 1) return;
		if ( !W.HasPOS(M.m_AtomGroups[0].m_PartOfSpeech) )  return;

		//  добавление статьи, если она есть
		WORD UnitNo = GetUnitNoByTerminId(DictType, TerminId);
		if (UnitNo == ErrUnitNo)  return;

		CDictUnitInterp I (GetRossHolder(DictType), DictType, UnitNo, false, false);
		I.m_TerminId = TerminId;
		N.AddInterp( CDictUnitInterp(I) );

	}
	catch (...)
	{
		ErrorMessage ("Ошибка тезаурусной интерпретации");
		throw;
	};
};



void CRusSemStructure::AscribeFindConceptFets(long ClauseNo, const StringVector& ConceptStrs)
{
  // идем по всем узлам 
  for (long i=0; i < m_Nodes.size(); i++)
  if (    IsThesRoss(m_Nodes[i].GetType())
	   && (m_Nodes[i].m_ClauseNo == ClauseNo)
	 )
  {
	  long TerminId = m_Nodes[i].GetInterp()->m_TerminId;
	  assert (TerminId != -1);
	  const CThesaurus* Thes = m_pData->GetThes(GetThesIdByRossId(m_Nodes[i].GetType()));
	  for (long  k=0; k < ConceptStrs.size(); k++)
	  {
		 string S = ConceptStrs[k].substr(1, ConceptStrs[k].length() - 1).c_str();
		 Trim(S);
		 bool Result = Thes->IsA(TerminId, S.c_str());
		 if (Result )
			 m_Nodes[i].m_HigherConcepts.push_back(ConceptStrs[k]); 
	  };

  };

};


void CRusSemStructure::FindConceptFetsFromArticles(long ClauseNo)
{
	// получаем все LEX или MANLEX, которые начинаются с '#'
	StringVector ConceptStrs;

	const vector<CAbstractArticle>* pAbstractArticles = m_pData->GetAbstractArticles(Ross);

	assert (pAbstractArticles);
	if (!pAbstractArticles)
		return;

	const vector<CAbstractArticle>& AbstractArticles = *pAbstractArticles;

	for (long i=0; i< AbstractArticles.size(); i++)
		if  (AbstractArticles[i].m_Type == atAdditionArticle)
		{
			const CAbstractArticle& A = AbstractArticles[i];
			for (int k=0; k < A.m_LexFets.size(); k++)
				if (    (A.m_LexFets[k].size() > 0)
					&& (A.m_LexFets[k][0] == '#')
			  )
			  ConceptStrs.push_back(A.m_LexFets[k]);
		};

	for (long i=0; i< m_Nodes.size(); i++)
		if (IsInClause(i, ClauseNo))
			for (long j = 0; j < m_Nodes[i].GetInterps().size(); j++)
			{
				const CRossHolder* RossDoc = GetRossHolder(m_Nodes[i].GetInterps()[j].m_DictType);
				const CDictionary* Ross = RossDoc->GetRoss();
				WORD UnitNo = m_Nodes[i].GetInterps()[j].m_UnitNo;
				if (!Ross->IsEmptyArticle(UnitNo) )	
					for (size_t j = Ross->GetUnitStartPos(UnitNo); j<= Ross->GetUnitEndPos(UnitNo); j++)
					{
						TCortege C = GetCortege(Ross,j);
						string FieldStr = Ross->Fields[C.m_FieldNo].FieldStr;
						if (   (FieldStr == "LEX") 
							|| (FieldStr == "MANLEX")
							|| (FieldStr == "ELEX")
							)
						{
							string S =   RossDoc->GetDomItemStrInner(C.m_DomItemNos[0]);
							EngRusMakeUpper(S);
							if  (    (S.length() > 0) 
								&& (S[0] == '#')
								)
								ConceptStrs.push_back(S);
						}
					};
			};

	AscribeFindConceptFets(ClauseNo,ConceptStrs);

};

struct CTerminItem {
	// номер элемента в термине 
	UINT m_ItemNo;
	/*
	при загрузке английских терминов m_TextItemId - WordNo;
    при загрузке разделенных русских m_TextItemId - NodeNo;
	*/
	UINT m_TextItemId; 
	bool m_bSelected;
	CTerminItem () 
	{
		m_bSelected = false;
	};

};

struct CSemanTermin {
	long m_ThesaurusId;   
	UINT m_TerminNo;
	vector<CTerminItem> m_Items;
	long	m_TextItemsCount;
	long	m_TerminId;

	bool operator == ( const CSemanTermin& X)   const
	{
		return    (m_ThesaurusId == X.m_ThesaurusId)
			   && (m_TerminNo == X.m_TerminNo);
	};
};


void LoadTerminItems(CRusSemStructure& R, long ThesaurusId, long TextItemId,  const char*  Lemma, vector<CSemanTermin>& Result)
{
	const CThesaurus* Thes  = R.m_pData->GetThes(ThesaurusId);
	vector<int> TerminItems;
	Thes->QueryTerminItem(Lemma, TerminItems);
	long Count  = TerminItems.size();
	for (long i=0; i < Count;  i++)
	{	
		CSemanTermin T;
		CTerminItem I;
		I.m_TextItemId = TextItemId;
		T.m_ThesaurusId = ThesaurusId;
		T.m_TerminNo = Thes->m_SynItems[TerminItems[i]].m_TerminNo;    
		I.m_ItemNo = Thes->m_SynItems[TerminItems[i]].m_ItemPos;
		// односоставные теримны пропускаем
		long ModelNo = Thes->m_Termins[T.m_TerminNo].m_ModelNo;
		if  (ModelNo == -1) continue;
		if (Thes->m_Models[ModelNo].m_AtomGroups.size() == 1)  continue;

		vector<CSemanTermin>::iterator It= find(Result.begin(),Result.end(), T);
		if (It == Result.end())
		{
			Result.push_back(T);
			It = Result.begin()+Result.size() - 1;
		};
		It->m_Items.push_back(I);
	};
};


// проверяет один вариант приписывания элементов разделенных терминов словам
bool CheckAndBuildOneDividedTermin(CRusSemStructure& R,const CSemanTermin& T,  const vector<CDopField>& DopFields, long MainItemNo)
{
	for (long DopFieldNo=0;  DopFieldNo < DopFields.size(); DopFieldNo++)
	{
		const CDopField& F = DopFields[DopFieldNo];
		long ItemNo1 = -1;
		long ItemNo2 = -1;
		for (long j=0; j< T.m_Items.size(); j++)
		if (T.m_Items[j].m_bSelected)
			if (T.m_Items[j].m_ItemNo == F.m_Word1)
				ItemNo1 = j;
			else
			  if (T.m_Items[j].m_ItemNo == F.m_Word2)
				ItemNo2 = j;
		// если не найдено элементов, это, скорее всего, означает, что неверно составлено поле AUX
		if ( (ItemNo1 == -1) || (ItemNo2 == -1)) return false;

		// если элементы уже являются частью тезаурусной  интерпретации надо выйти
		if (R.m_Nodes[T.m_Items[ItemNo1].m_TextItemId].m_Colloc.m_Type != NoneType) return false;
		if (R.m_Nodes[T.m_Items[ItemNo2].m_TextItemId].m_Colloc.m_Type != NoneType) return false;

		// Ищем отношение, которое их связывает
		long RelNo = R.FindFirstRelation (T.m_Items[ItemNo2].m_TextItemId, T.m_Items[ItemNo1].m_TextItemId);
		if (RelNo == -1) return false;
		if (R.m_Relations[RelNo].m_Valency.m_RelationStr !=  F.m_RelationStr) return false;
	};

	long CollocId = -1;
	for (long j=0; j< T.m_Items.size(); j++)
	if (T.m_Items[j].m_bSelected)
	{
		long Nd = T.m_Items[j].m_TextItemId;
		if (CollocId == -1)
		  CollocId = R.m_Nodes[Nd].GetMinWordNo();
		R.m_Nodes[Nd].m_CollocId = CollocId;
		R.m_Nodes[Nd].m_Colloc.m_Type = ThesType;
		R.m_Nodes[Nd].m_Colloc.GetThesInterp().m_ThesaurusId =  T.m_ThesaurusId;
		R.m_Nodes[Nd].m_Colloc.GetThesInterp().m_ItemNo = T.m_Items[j].m_ItemNo-1;
		R.m_Nodes[Nd].m_Colloc.GetThesInterp().m_TerminId = T.m_TerminId; 
		R.m_Nodes[Nd].m_Colloc.GetThesInterp().m_bMainWord = (MainItemNo == T.m_Items[j].m_ItemNo);
	};
	return true;
};


// проверяет, что для разделенного термина были найдены все  его элементы
bool CheckIfAllItemsFoundForDividedTermin(const CRusSemStructure& R, const CSemanTermin& T) 
{
	vector<bool> V (T.m_TextItemsCount, false);

	for (int i = 0;  i < T.m_Items.size(); i++)
	{
		int z = T.m_Items[i].m_ItemNo - 1;
		assert (z <  V.size());
		V[z] = true;
	};

	for (int  j = 0; j < V.size(); j++)
		if (!V[j] ) 
			return  false;

	return true;
};

void GetThesInterpFirstVariant(CSemanTermin& T)
{
	vector<bool> V (T.m_TextItemsCount, false);
	for (int i = 0; i <T.m_Items.size(); i++)
	{
		int z = T.m_Items[i].m_ItemNo - 1;
		assert (z <  V.size());
		if( !V[z] )
		{
			T.m_Items[i].m_bSelected = true;
			V[z] = true;
		};
	};

};

bool NextPosition (CSemanTermin& T, int i)
{
	T.m_Items[i].m_bSelected = false;

	int k = i+1;
	for (; k < T.m_Items.size(); k++)
		if (T.m_Items[k].m_ItemNo == T.m_Items[i].m_ItemNo)
		{
			T.m_Items[k].m_bSelected = true; 
			return true;
		};

	for (k = 0; k < T.m_Items.size(); k++)
		if (T.m_Items[k].m_ItemNo == T.m_Items[i].m_ItemNo)
			break;

	assert (k < T.m_Items.size());
	T.m_Items[k].m_bSelected = true; 
	return false;
};

bool GetInterpNextVariant(CSemanTermin& T)
{
	int CurrItemNo = 1;
	for (int i = 0; i <T.m_Items.size(); i++)
	if (T.m_Items[i].m_bSelected)
		if (T.m_Items[i].m_ItemNo == CurrItemNo)
		{	
			if (NextPosition (T, i)) return true;
			CurrItemNo++;
			if (CurrItemNo > T.m_TextItemsCount) return false;
			i = -1;
		};

	assert (false);
	return false;


};
void CRusSemStructure::FindDividedTermins()
{
try {
	vector<CSemanTermin> Termins;
	for (long i=0; i < m_Nodes.size(); i++)
	{
		if (   !m_Nodes[i].IsPrimitive() 
			|| m_Nodes[i].m_Words[0].m_Lemma.empty()
			)
		continue;;

		LoadTerminItems(*this,  LocThes,i,m_Nodes[i].m_Words[0].m_Lemma.c_str(), Termins);
		LoadTerminItems(*this, FinThes,i,  m_Nodes[i].m_Words[0].m_Lemma.c_str(),Termins);
		LoadTerminItems(*this, OmniThes, i, m_Nodes[i].m_Words[0].m_Lemma.c_str(),Termins);
		LoadTerminItems(*this,  CompThes,i, m_Nodes[i].m_Words[0].m_Lemma.c_str(),Termins);

	};
	// строим разорванные термины
	for (int TerminNo=0; TerminNo< Termins.size(); TerminNo++)
	{

		CSemanTermin& T = Termins[TerminNo];

		const CThesaurus* Thes  = m_pData->GetThes(T.m_ThesaurusId);

		T.m_TextItemsCount = Thes->m_Termins[T.m_TerminNo].m_Items.size();

		if (!CheckIfAllItemsFoundForDividedTermin(*this, T)) continue;
		// дальше  мы будем уничтожать элементы вектора  T.m_Items, оставляя в нем только те, 
		// для которых были найдены отношения из поля AUX
		// Если, например,  для некого двухсловного термина для первого слова было найдено две гипотезы  в тексте,
		// и только одна из этих двух слов-гипотез состоит в нужным отношении со вторым словом,
		// тогда  второе гипотеза будет удалена

		vector<CDopField> DopFields;
		T.m_TerminId = Thes->m_Termins[T.m_TerminNo].m_TerminId;
		WORD UnitNo = GetUnitNoByTerminId(GetRossIdByThesId(T.m_ThesaurusId), T.m_TerminId);
		if (UnitNo == ErrUnitNo) continue;
		long MainItemNo = GetRossHolder(GetRossIdByThesId(T.m_ThesaurusId))->GetDopFields(UnitNo, DopFields);

		// если поле AUX пусто, тогда не на что опираться, призодится выходить
		if (DopFields.size() == 0) continue;

		GetThesInterpFirstVariant(T);

		do {
			if (CheckAndBuildOneDividedTermin(*this, T, DopFields, MainItemNo)) break;
		} 
		while (GetInterpNextVariant(T));

	};
  }
  catch (...)
  {
		ErrorMessage("void CRusSemStructure::FindDividedTermins failed!");
	    throw;
  };


};

/*
Создаем интерпретацию для тезаурусных сокращений типа "ВДНХ"
*/

void CRusSemStructure::FindAbbrTermins()
{
try {
	for (long i=0; i < m_Nodes.size(); i++)
	 if (   m_Nodes[i].IsPrimitive() 
		 && m_Nodes[i].m_Words[0].m_CharCase == UpUp
		)
	 {
		 string WordStr =  m_Nodes[i].m_Words[0].m_Word;
		 long TerminNo =  -1;
		 long ThesId = 0;
		 if (TerminNo == -1)
		 {
			 TerminNo = m_pData->GetThes(FinThes)->FindAbbr(WordStr.c_str());
			 ThesId = FinThes;
		 };
		 // ОПЕК - 
		 if (TerminNo == -1)
		 {
			 TerminNo = m_pData->GetThes(CompThes)->FindAbbr(WordStr.c_str());
			 ThesId = CompThes;
		 };
		 // СПС - Союз Правых Сил
		 if (TerminNo == -1)
		 {
			 TerminNo = m_pData->GetThes(OmniThes)->FindAbbr(WordStr.c_str());
			 ThesId = OmniThes;
		 };
		 // США - Соединенные Штаты Америки
		 if (TerminNo == -1)
		 {
			 TerminNo = m_pData->GetThes(LocThes)->FindAbbr(WordStr.c_str());
			 ThesId = LocThes;
		 };

			if (TerminNo == -1) continue;

			m_Nodes[i].m_ThesAbbrForm = true;
			const CThesaurus* Thes = m_pData->GetThes(ThesId);
			long TerminId = Thes->m_Termins[TerminNo].m_TerminId;
			m_Nodes[i].m_TerminId = TerminId;
			m_Nodes[i].m_ThesaurusId =  ThesId;
			bool IsIdent = Thes->IsA(TerminId, "IDENT");		 
			if (IsIdent)
				m_Nodes[i].m_bProper = true;

			if (		Thes->IsA(TerminId, "ORG")  			 
					&&	IsIdent
				)
			m_Nodes[i].m_bOrgName = true;
	 }
  }
  catch (...)
  {
		ErrorMessage("void CRusSemStructure::FindAbbrTermins failed!");
	    throw;
  };

 
};


/*
  Создаем интерпретацию для английских многословных названий типа Adobe Photoshop
*/

void CRusSemStructure::FindEnglishTermins()
{
try {
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	 if (m_Nodes[NodeNo].m_Words.size() > 1)
	 {
		 long i=0;
		 for (; i < m_Nodes[NodeNo].m_Words.size(); i++)
		   if (!m_Nodes[NodeNo].m_Words[i].m_ILE)
			   break;
		 if (i < m_Nodes[NodeNo].m_Words.size()) continue;

	 	vector<CSemanTermin> Termins;
  	    for (i=0; i < m_Nodes[NodeNo].m_Words.size(); i++)
		{
 			LoadTerminItems(*this,  LocThes,i,m_Nodes[NodeNo].m_Words[i].m_Lemma.c_str(), Termins);
			LoadTerminItems(*this, FinThes,i,  m_Nodes[NodeNo].m_Words[i].m_Lemma.c_str(),Termins);
			LoadTerminItems(*this, OmniThes, i, m_Nodes[NodeNo].m_Words[i].m_Lemma.c_str(),Termins);
			LoadTerminItems(*this,  CompThes,i, m_Nodes[NodeNo].m_Words[i].m_Lemma.c_str(),Termins);
		};

		for (i=0; i< Termins.size(); i++)
		  if (Termins[i].m_Items.size() == m_Nodes[NodeNo].m_Words.size()) 
			  break;

		if (i==Termins.size()) continue;

	    CSemanTermin& T = Termins[i];
	    m_Nodes[NodeNo].m_ThesaurusId = T.m_ThesaurusId;
		const CThesaurus* Thes = m_pData->GetThes(T.m_ThesaurusId);
		long TerminId = Thes->m_Termins[T.m_TerminNo].m_TerminId;
		m_Nodes[NodeNo].m_TerminId = TerminId;
		const CInnerTermin& Term = Thes->m_Termins[T.m_TerminNo];
		assert(!Term.m_Items.empty());
		
		const CInnerSynItem& SynItem = Thes->m_SynItems[Term.m_Items[0]];
		if  (SynItem.m_Flags & siUpUp)
			m_Nodes[NodeNo].m_Words[0].m_CharCase  = UpUp;

		if (SynItem.m_Flags & siUpLw) 
			m_Nodes[NodeNo].m_Words[0].m_CharCase  = UpLow;

		bool IsIdent = Thes->IsA(TerminId, "IDENT");
		if (IsIdent)
			m_Nodes[i].m_bProper = true;

		if (		Thes->IsA(TerminId, "ORG")  			
				&&	IsIdent
		   )
			m_Nodes[i].m_bOrgName = true;
	 };

  }
  catch (...)
  {
		ErrorMessage("void CRusSemStructure::FindEnglishTermins failed!");
	    throw;
  };

};
