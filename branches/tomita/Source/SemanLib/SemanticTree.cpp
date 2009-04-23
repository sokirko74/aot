#include "stdafx.h"
#include "SemanticRusStructure.h"



void CRusSemStructure::SetUseFalse ()
{
	long count = m_Relations.size();
	for (size_t i = 0;  i < count; i++)
   	  m_Relations[i].m_bRelUse = false;
};

void CRusSemStructure::SetUseTrue ()
{
	long count = m_Relations.size();
	for (size_t i = 0;  i < count; i++)
   	  m_Relations[i].m_bRelUse = true;
};


long CRusSemStructure::GetUseRelationsCount() const
{
	long count = m_Relations.size();
	long Result = 0;

	for (size_t i = 0;  i < count; i++)
   	  if (m_Relations[i].m_bRelUse)
		 if ( m_Nodes[m_Relations[i].m_TargetNodeNo].IsWordContainer() )
		    Result ++;

    return Result; 

};

// выдает объем семантической структуры 
// количество отношений + v(1)+...+v(n)
// где v(i) = (кол-во слов в узле) - 1, если i-й узел - словосочетание
// и v(i)=0, в противном случае.
// считается, что словосоечетание - это маленьнкое дерево
long CRusSemStructure::GetSemanticVolume(long Tag) const 
{
	long Result = GetUseRelationsCount();

    long count = m_Nodes.size();

	for (size_t i = 0;  i < count; i++)
		if (m_Nodes[i].m_Tag == Tag) 
		   if (m_Nodes[i].GetType() == CollocRoss)
			   Result += m_Nodes[i].m_Words.size()  - 1;

    return Result; 
};


void CRusSemStructure::AddTreeVariant (const CTreeVariant& Variant)
{
	for (size_t i = 0;  i < Variant.GetRelsCount(); i++)
		if (Variant.GetRel(i) != Unhosted)
			m_Relations[Variant.GetRel(i)].m_bRelUse  = true;
};


void CRusSemStructure::SetTreeVariant (const CTreeVariant& Variant)
{ 
	SetUseFalse();
	AddTreeVariant(Variant);
};
void CRusSemStructure::dfs(size_t NodeNo, bool ConsiderUseOfNodes, long Tag)
{
  m_Nodes[NodeNo].m_bReached = true;
  if (Tag != -1) m_Nodes[NodeNo].m_Tag = Tag;
  long Count = m_Relations.size();
  for (size_t i=0; i<Count; i++)
	  {
		  if(m_Relations[i].m_SourceNodeNo == NodeNo)
			  	  if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_bReached == false)
					  if (!ConsiderUseOfNodes || m_Relations[i].m_bRelUse)
						  if (!m_Relations[i].m_bUniqueRelation || (Tag == -1))
					        dfs(m_Relations[i].m_TargetNodeNo, ConsiderUseOfNodes, Tag);

		   if (m_Nodes[m_Relations[i].m_SourceNodeNo].m_bReached == false)
			    if ( NodeNo == m_Relations[i].m_TargetNodeNo)
					if (!ConsiderUseOfNodes || m_Relations[i].m_bRelUse)
						if (!m_Relations[i].m_bUniqueRelation || (Tag == -1))
				  	       dfs(m_Relations[i].m_SourceNodeNo, ConsiderUseOfNodes, Tag);

	  };
};


void CRusSemStructure::dfs_quick_for_connected_component(size_t NodeNo)
{
	const CRusSemNode& N = m_Nodes[NodeNo];
	m_Nodes[NodeNo].m_bReached = true;

	long Count = N.m_InRels.size();
	for (size_t i=0; i<Count; i++)
		if (m_Nodes[m_Relations[N.m_InRels[i]].m_SourceNodeNo].m_bReached == false)
			if (m_Relations[N.m_InRels[i]].m_bRelUse)
				dfs_quick_for_connected_component(m_Relations[N.m_InRels[i]].m_SourceNodeNo);

	Count = N.m_OutRels.size();
	for (size_t i=0; i<Count; i++)
		if (m_Nodes[m_Relations[N.m_OutRels[i]].m_TargetNodeNo].m_bReached == false)
			if (m_Relations[N.m_OutRels[i]].m_bRelUse)
				dfs_quick_for_connected_component(m_Relations[N.m_OutRels[i]].m_TargetNodeNo);
};



void CRusSemStructure::DelUnusedRelations (long ClauseNo)
{
	for (size_t i = 0;  i < m_Relations.size(); i++)
		if (IsInClause(m_Relations[i].m_SourceNodeNo, ClauseNo))
			if (!m_Relations[i].m_bRelUse)
			{
				EraseRelation(i);
				i--;
			};
};


long CRusSemStructure::MarkConnectedComponentsWithoutUniqueRelations(long ClauseNo)
{
	for (size_t NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	{
		m_Nodes[NodeNo].m_bReached = !IsInClause(NodeNo, ClauseNo);
		m_Nodes[NodeNo].m_Tag = -1;
	}

	long TagId = 0;

	for (size_t NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
		if (IsInClause(NodeNo, ClauseNo))
			if (!m_Nodes[NodeNo].m_bReached)
			{
				dfs (NodeNo, false, TagId);
				TagId ++;
			}

			return TagId;
};

long CRusSemStructure::GetConnectedComponentsCount(long Tag)
{

	for (size_t NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
		if (HasTag(NodeNo, Tag))
			m_Nodes[NodeNo].m_bReached = false;

	long Result = 0;

	for (size_t NodeNo=0; NodeNo<m_Nodes.size(); NodeNo++)
		if (HasTag(NodeNo, Tag))
			if (!m_Nodes[NodeNo].m_bReached)
			{
				dfs_quick_for_connected_component (NodeNo);
				Result ++;
			};

	return Result;
};



long CRusSemStructure::SetTagToClause(long ClauseNo)
{
  for (size_t i=0; i < GetNodesSize(); i++)
	  if (IsInClause(i, ClauseNo))
       m_Nodes[i].m_Tag = m_Clauses.size();
	  else
	   m_Nodes[i].m_Tag = -1;

  return m_Clauses.size();
};

long CRusSemStructure::SetTagToSentence()
{
  for (size_t i=0; i < GetNodesSize(); i++)
     m_Nodes[i].m_Tag = m_Clauses.size();

  return m_Clauses.size();
};


class IsLessBySourceNodeNo : public binary_function<bool,const long, const long>
{
public:  
	const CRusSemStructure* m_SemStr;

	IsLessBySourceNodeNo(const CRusSemStructure* SemStr)
	{m_SemStr = SemStr;}  

	result_type operator()(const long& RelNo1, const long& RelNo2)
	{
		return m_SemStr->m_Relations[RelNo1].m_SourceNodeNo < m_SemStr->m_Relations[RelNo2].m_SourceNodeNo;
	}
};
class IsLessByTargetNodeNo : public binary_function<bool,const long, const long>
{
public:  
	const CRusSemStructure* m_SemStr;

	IsLessByTargetNodeNo(const CRusSemStructure* SemStr)
	{m_SemStr = SemStr;}  

	result_type operator()(const long& RelNo1, const long& RelNo2)
	{
		return m_SemStr->m_Relations[RelNo1].m_TargetNodeNo < m_SemStr->m_Relations[RelNo2].m_TargetNodeNo;
	}
};


void CRusSemStructure::UpdateBlockedRelations()
{
  for (size_t i=0; i<GetNodesSize(); i++)
	{
	    CSemanticStructure::GetIncomingRelations(i, m_Nodes[i].m_InRels, false); 
		sort (m_Nodes[i].m_InRels.begin(), m_Nodes[i].m_InRels.end(), IsLessBySourceNodeNo(this));
		
        GetOutcomingRelations(i, m_Nodes[i].m_OutRels, false); 
		sort (m_Nodes[i].m_OutRels.begin(), m_Nodes[i].m_OutRels.end(), IsLessByTargetNodeNo(this));
	};
  
};


bool CRusSemStructure::dfs_to_test_unique_relation (size_t NodeNo, size_t RelNo)
{
  if (m_Nodes[NodeNo].m_bReached) return true;

  m_Nodes[NodeNo].m_bReached = true;

  long Count = m_Nodes[NodeNo].m_OutRels.size();

  for (long i=0; i<Count; i++)
   if (m_Nodes[NodeNo].m_OutRels[i] != RelNo )
   {
	   long CurrNodeNo = m_Relations[m_Nodes[NodeNo].m_OutRels[i]].m_TargetNodeNo;
	   if  (    (CurrNodeNo == m_Relations[RelNo].m_SourceNodeNo)
		     || !dfs_to_test_unique_relation(CurrNodeNo, RelNo)
		   )
		   return false;
   };

  Count = m_Nodes[NodeNo].m_InRels.size();
  for (long i=0; i<Count; i++)
   if (m_Nodes[NodeNo].m_InRels[i] != RelNo )
   {
	   long CurrNodeNo = m_Relations[m_Nodes[NodeNo].m_InRels[i]].m_SourceNodeNo;
	   if  (    (CurrNodeNo == m_Relations[RelNo].m_SourceNodeNo)
		     || !dfs_to_test_unique_relation(CurrNodeNo, RelNo)
		   )
		   return false;
   };

  return true;
};

void CRusSemStructure::MarkUniqueRelations()
{
	for (size_t i=0; i < m_Relations.size(); i++)
	{
		size_t NodeNo=0;
		for (; NodeNo < m_Nodes.size(); NodeNo++)
			m_Nodes[NodeNo].m_bReached = false;

		NodeNo = m_Relations[i].m_SourceNodeNo;

		long k=0;
		for (; k < m_Nodes[NodeNo].m_OutRels.size(); k++)
			if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType != Copul)
			{
				long RelNo = m_Nodes[NodeNo].m_OutRels[k];
				if (      (m_Nodes[m_Relations[RelNo].m_TargetNodeNo].m_NodeType != Copul)
					&& (m_Relations[i].m_Valency.m_RelationId != -1)
					&& (i != RelNo)
					&& !m_Relations[i].m_bReverseRel
					&& !m_Nodes[NodeNo].RelsCanBeTogether (m_Relations[i].m_Valency.m_RelationId, m_Relations[RelNo].m_Valency.m_RelationId)
			  )
			  break;
			};


		m_Relations[i].m_bUniqueRelation = 
			(k == m_Nodes[NodeNo].m_OutRels.size())
			&&   (m_Nodes[m_Relations[i].m_TargetNodeNo].m_InRels.size() == 1)
			&& dfs_to_test_unique_relation (m_Relations[i].m_TargetNodeNo, i);


	};
};



// ===========    Поиск циклов в графе Шалимова  =================
// предполагается, что это процедура запускается только на граф Шалимова
bool CRusSemStructure::dfs_for_cycle_search (size_t NodeNo, size_t GoalNodeNo)
{
  long Count = m_Nodes[NodeNo].m_OutRels.size();
  m_Nodes[NodeNo].m_bReached = true;

  for (long i=0; i<Count; i++)
   if (m_Relations[m_Nodes[NodeNo].m_OutRels[i]].m_bRelUse)
   {
	   long CurrNodeNo = m_Relations[m_Nodes[NodeNo].m_OutRels[i]].m_TargetNodeNo;
	   if  (    (CurrNodeNo == GoalNodeNo)
		     || (dfs_for_cycle_search(CurrNodeNo, GoalNodeNo))
		   )
	   {
		   m_Nodes[CurrNodeNo].m_bInCycle = true;
		   return true;
	   }
   };

  return false;
};



bool CRusSemStructure::FindCycle (long Tag)
{
  long Count = m_Nodes.size();
  for (size_t k=0; k<Count; k++)
  {
  	  m_Nodes[k].m_bReached = false;
	  m_Nodes[k].m_bInCycle = false;
  };
/*
 эта функция  опирается на то, что в вершины цикла 
 входит только одна стрелка (часть цикла), а больше стрелок
 входить не может, поскольку мы уже имеем дело с графом Шалимова,
 где в каждый узел входит не более одной стрелки.
 Например, конструкция типа (1,2) (2,3) (3,2) запрещена.
*/
  for (size_t i = 0;  i < Count; i++)
   if (    HasTag(i, Tag) 
	   && !m_Nodes[i].m_bReached
	  )
   {
 	   if (dfs_for_cycle_search(i, i))
		   return true;
   };

  return false;   
};


 
void CRusSemStructure::UncycleVariant(size_t  VarNo, 
					                  vector<CTreeVariant>& Variants,
									  long Tag)

{
  bool  b = true;
  CTreeVariant NewVariant = Variants[VarNo];
  for (size_t i = 0;  i < NewVariant.GetRelsCount(); i++)
	 if (NewVariant.GetRel(i) != Unhosted)
     if (m_Nodes[m_Relations[NewVariant.GetRel(i)].m_TargetNodeNo].m_bInCycle)
	  if (b)
	  {
         Variants[VarNo].SetRel(i,Unhosted);
		 b =  false;
	  }
	  else
	  {
		 Variants.push_back(NewVariant);
		 Variants[Variants.size() - 1].SetRel(i, Unhosted);
	  };
};

/*


*/

bool CRusSemStructure::SplitVariantWithRepeats(size_t  VarNo, 
											   vector<CTreeVariant>& Variants)

{
	for (size_t i =0; i< Variants[VarNo].GetRelsCount(); i++)
		if (Variants[VarNo].GetRel(i) != Unhosted)
		{
			bool Found = false;
			size_t k = i;
			for (; k<Variants[VarNo].GetRelsCount(); k++)
				if (Variants[VarNo].GetRel(k) != Unhosted)
					if (m_Relations[Variants[VarNo].GetRel(i)].m_EqualFieldId == m_Relations[Variants[VarNo].GetRel(k)].m_EqualFieldId)
					{
						if (k > i ) Found = true; 
						if (m_Relations[Variants[VarNo].GetRel(k)].m_bUniqueRelation)
							break;
					};

			if (!Found) 
				continue;

			if ( k == Variants[VarNo].GetRelsCount()) k = i;

			Variants.push_back(Variants[VarNo]);
			// заново надо вычислить ссылку, поскольку после push_back она сбивается
			Variants[Variants.size() - 1].SetRel(k, Unhosted);


			size_t Rel_i = Variants[VarNo].GetRel(i);

			for (size_t j = i; j<Variants[VarNo].GetRelsCount(); j++)
				if (Variants[VarNo].GetRel(j) != Unhosted)
					if (m_Relations[Rel_i].m_EqualFieldId == m_Relations[Variants[VarNo].GetRel(j)].m_EqualFieldId)
						if (j != k)
							Variants[VarNo].SetRel(j, Unhosted);
		};

	return true; 
};

// 
/*
 Функция делит все отношения на классы эквивалентности.
 Номер класс эквивалентности - это слот CRusSemRelation::m_EqualFieldId
 Функция идет по всем отношениям с помощью итератора i .Если текущий
 номер класса равен -1, тогда этому отношению ставится 
 номер класса, совпадающий с номером отношения.
 Проставив номер, функция идет по последющим отношениям с помощью итератора k
 Если 
 1. Отношение k выбрано в текущем множестве ((V.Rels[k]  != Unhosted))
 2. Отношение k и отношение i исходят из одного узла
   (m_Relations[V.Rels[k]].m_SourceNodeNo  == m_Relations[V.Rels[i]].m_SourceNodeNo)
   (поскольку несовместные отношения бывают только те, которые
   выходят из одного узла)
 3. Отношение k проиндексировано (например, отношения,
которые идут из MUA (без названия) не индексируются, поскольку их может много выходить из одного узла)
 4. Отношение k не идет в КОПУЛ, поскольку оно не может 
 быть ни с чем несовместно;
 5.Отношение k не является реверсивным (поскольку, например,
реверсивные PROPERT могут с большом кол-ве у одного узла) 6. и неверно, что отношения k и i совместны.
 Тогда  отношения k помечается тем же номером класса 
 эквивалентности, что и отношение i.

 Таким образом,  функция  выделяет классы отношений, внутри которых
 отношения не являются совместными.

 Функция выдает  число отношений, которые вошли в классы,
 состоящие  более, чем из одного отношения минус количество
 таких классов.
*/
long CRusSemStructure::SetEqualFieldId(const CTreeVariant& V)
{
	for (size_t i =0; i< V.GetRelsCount(); i++)
		if (V.GetRel(i) != Unhosted)
			m_Relations[V.GetRel(i)].m_EqualFieldId = -1;

	long Count = 0;

	for (size_t i =0; i< V.GetRelsCount(); i++)
		if (V.GetRel(i) != Unhosted)
			if (m_Relations[V.GetRel(i)].m_EqualFieldId == -1)
				if (m_Nodes[m_Relations[V.GetRel(i)].m_TargetNodeNo].m_NodeType != Copul)
				{
					m_Relations[V.GetRel(i)].m_EqualFieldId = i;

					for (size_t k = i+1; k< V.GetRelsCount(); k++)
						if (    (V.GetRel(k)  != Unhosted)
							&& (m_Relations[V.GetRel(k)].m_SourceNodeNo  == m_Relations[V.GetRel(i)].m_SourceNodeNo)
							&& (m_Relations[V.GetRel(k)].m_Valency.m_RelationId != -1)
							&& (m_Nodes[m_Relations[V.GetRel(k)].m_TargetNodeNo].m_NodeType != Copul)
							&& !m_Relations[V.GetRel(k)].m_bReverseRel
							&& !m_Nodes[m_Relations[V.GetRel(k)].m_SourceNodeNo].RelsCanBeTogether (m_Relations[V.GetRel(k)].m_Valency.m_RelationId, m_Relations[V.GetRel(i)].m_Valency.m_RelationId)
							)
						{
							m_Relations[V.GetRel(k)].m_EqualFieldId = i;
							Count++;
						};
				};
	return Count;
};


long GetComponentNum (const CTreeVariant& V)
{
	long Result = 0;

	for (long i=0; i<V.GetRelsCount(); i++)
		if (V.GetRel(i) == Unhosted)
			Result++;

	return Result;
};


long AreEqual (const CTreeVariant& V1, const CTreeVariant& V2)
{ 
	long i=0;
	for (; i<V1.GetRelsCount(); i++)
		if (V1.GetRel(i) != V2.GetRel(i))
			break;

	return i == V1.GetRelsCount();
};

void TestEquality (const vector<CTreeVariant>& Variants)
{ 
	for (size_t VarNo=0;  VarNo < Variants.size(); VarNo++)
		for (size_t VarNo1=VarNo+1;  VarNo1 < Variants.size(); VarNo1++)
			if (AreEqual(Variants[VarNo],Variants[VarNo1]))
			{
				rml_TRACE  ("error !!!\n");
			};

};

void GetTreeVariants(const vector<CTreeVariant>&  Parents, 
					 CTreeVariant&   V,
					 vector<CTreeVariant>&  Variants, 
					 long       Position)
{ 
	if (Variants.size() > 30000) return;

	long RelsCount = Parents.size();
	if (Position < RelsCount)
	{
		for (size_t i=0; i< Parents[Position].GetRelsCount(); i++)
		{
			V.AddRel( Parents[Position].GetRel(i) );
			GetTreeVariants(Parents, V, Variants, Position+1);
			V.DeleteLast();
		}
	}
	else
	{
		assert (V.GetRelsCount() == RelsCount);
		Variants.push_back(V);

	};

};



void CRusSemStructure::GetTree (long Tag, TreeAndValueVector& VarAndVals)
{
	if (m_Relations.size() >  250) 
	{
		ErrorMessage ("PanicRelationsCount для одной клаузы больше 250. Это очень сложный граф.");
		VarAndVals.resize(1);
		VarAndVals[0].second.Panic = true;
		throw;
		//return;
	};


	vector<CTreeVariant> Parents;
	long HypotVariantCount = 1;
	StartTimer("Граф Шалимова",1);

	for (size_t i = 0;  i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
		{
			long ParentNo = Parents.size();
			Parents.push_back(CTreeVariant()); 

			long Count = m_Nodes[i].m_InRels.size();
			if (Count == 0)
				Parents[ParentNo].AddRel( Unhosted );
			else
				for (size_t k = 0;  k < Count; k++)
					Parents[ParentNo].AddRel(m_Nodes[i].m_InRels[k]);

			HypotVariantCount *= Parents[ParentNo].GetRelsCount();
			if (HypotVariantCount > 200000) 
		 {
			 VarAndVals.resize(1);
			 VarAndVals[0].second.Panic = true;
			 return;
		 };

			// добавляем уникальные выходящие отношения
			Count = m_Nodes[i].m_OutRels.size();
			for (size_t k = 0;  k < Count; k++)
				if (m_Relations[m_Nodes[i].m_OutRels[k]].m_bUniqueRelation )
				{
					long ParentNo = Parents.size();
					Parents.push_back(CTreeVariant()); 
					Parents[ParentNo].AddRel(m_Nodes[i].m_OutRels[k]);
				};

		};

	if (HypotVariantCount == 0)
	{
		VarAndVals.resize(1);
		VarAndVals[0].second.Panic = true;
	};

	vector<CTreeVariant>  Variants;
	Variants.reserve(HypotVariantCount*2);
	CTreeVariant V; // текущий вариант 
	GetTreeVariants(Parents, V, Variants, 0);
	size_t VariantCount = Variants.size();
	EndTimer("Граф Шалимова");

	if (Variants.size() > m_PanicTreeVariantCount*100) 
	{
		VarAndVals.resize(Variants.size());
		VarAndVals[0].second.Panic = true;
		return;
	};

	StartTimer("Расцикливание",1);

	long CycledMinComponentNum = Parents.size();

	for (long VarNo=0;  VarNo < Variants.size(); VarNo++)
	{
		SetTreeVariant(Variants[VarNo]);

		while (FindCycle(Tag)) 
		{

			UncycleVariant(VarNo, Variants, Tag);
			SetTreeVariant(Variants[VarNo]);

		};
	};
	EndTimer("Расцикливание");
	StartTimer("Удаление повторов для неуник. отношений",1);

	long MinComponentNum = Parents.size();

	for (long VarNo=0;  VarNo< Variants.size(); VarNo++)
	{
		long RepeatsCount = SetEqualFieldId (Variants[VarNo]);
		if ( (GetComponentNum(Variants[VarNo]) + RepeatsCount)  > MinComponentNum)
		{
			Variants[VarNo].SetRel(0, MustBeIgnored);
		}
		else
		{
			SplitVariantWithRepeats(VarNo, Variants);
			// после отработки SplitVariantWithRepeats повторы могут остаться, если
			// стрелки уникальны. 
			//assert (SetEqualFieldId(Variants[VarNo]) == 0);
			long CurrComp =  GetComponentNum(Variants[VarNo]);
			if (CurrComp < MinComponentNum)
				MinComponentNum = CurrComp;
		};


	};
	EndTimer("Удаление повторов для неуник. отношений");

	StartTimer("Удаление малосвязных вариантов",1);
	// уничтожаем все варианты, у которых число компонент связности меньше минимального значения 
	// для всех вариантов

	vector<PairOfLong> CompCountAndNumber;
	CompCountAndNumber.reserve(Variants.size());
	for (long VarNo=0;  VarNo< Variants.size(); VarNo++)
	{
		PairOfLong M;
		M.first = (Variants[VarNo].GetRel(0) ==MustBeIgnored) ? Parents.size() : GetComponentNum(Variants[VarNo]);
		M.second = VarNo;
		CompCountAndNumber.push_back (M);
	};
	sort (CompCountAndNumber.begin(), CompCountAndNumber.end());
	long GoodVariantCount = 0;
	for (size_t i=0; i < CompCountAndNumber.size(); i++)
		if (CompCountAndNumber[i].first != CompCountAndNumber[0].first)
		{
			Variants[CompCountAndNumber[i].second].SetRel(0, MustBeIgnored);
		}
		else
			GoodVariantCount++;

	VarAndVals.resize(GoodVariantCount);

	if (GoodVariantCount > m_PanicTreeVariantCount) 
	{
		VarAndVals[0].second.Panic = true;
		return;
	};

	EndTimer("Удаление малосвязных вариантов");
	StartTimer("Оценка деревьев",1);


	long l=0;
	for (long VarNo=0;  VarNo< Variants.size(); VarNo++)
		if (Variants[VarNo].GetRel(0) != MustBeIgnored)
		{
			SetTreeVariant(Variants[VarNo]);
			ValueTreeVariant(VarAndVals[l].second, Tag);
			VarAndVals[l].second.ConnectedComponentsCount = 1;
			VarAndVals[l].first = Variants[VarNo];
			l++;
		};

	EndTimer("Оценка деревьев");

};



void QuickGetIndexedVariants(const vector<TreeAndValueVector>&  Parents, 
							long& VarNo,
							const long VariantSize,
							long* Variants, 
							const long HypotCount,
							long       Position)
{ 
	if (Position < VariantSize)
	for (long i=0; i< Parents[Position].size(); i++)
	{
			Variants[VarNo*VariantSize+Position] = i;
			QuickGetIndexedVariants(Parents, VarNo, VariantSize, Variants, HypotCount, Position+1);
	}
	else
	{
			VarNo++;
			if (VarNo < HypotCount)
				for (long i=0; i < VariantSize; i++)
					Variants[(VarNo)*VariantSize+i] = Variants[(VarNo-1)*VariantSize+i];
	};

};



long CRusSemStructure::GetTreeByConnectedComponents (size_t ClauseNo,  TreeVariantValue& ResultValue)
{
	bool TryWithoutLongRelations = true;
	//  эта метка используется всего один раз - когда возникает панический граф, 
	//  мы удаляем все длинные отношения, идущие от существительного, и пробуем собрать дерево заново. Если и на этот раз
	//  не удалось, тогда выходим...


	bool LongRelationsOfAnyKind = true;
	//  эта метка используется всего один раз - когда возникает панический граф, 
	//  мы удаляем все длинные отношения, идущие от любых вершин, и пробуем собрать дерево заново. Если и на этот раз
	//  не удалось, тогда выходим...


TryWithoutLongRelationsLabel:

	StartTimer("Общее время построения дерева",0);

	// узел, содежащий союз, который стоит в начале клаузы, удаленные на время построения дерева
	CRusSemNode  FirstConjNode;
	DeleteConjInTheBegining (ClauseNo, FirstConjNode);

	// сохраняем текущий состав отношений, для того чтобы  запустить MarkUniqueRelations
	UpdateBlockedRelations();
	IndexRelations();
	InitGrammarMatrix1(ClauseNo);

	PrintRelations();

	// помечаем безусловные отношения 
	MarkUniqueRelations(); 

	long  TopConnectedComponentsCount = MarkConnectedComponentsWithoutUniqueRelations(ClauseNo);

	PrintNodes();



	if (TopConnectedComponentsCount == 0)  // это возможно, если ввести одно слова "сам" или "один", которое удаляется при некоторых значениях
	{
		ResultValue.Panic = true;
		if (FirstConjNode.m_Words.size() > 0)
			InsertNode (FirstConjNode);
		EndTimer("Общее время построения дерева");
		return 0;
	};


	//  i-й элемент вектора хранит варианты дерева и его оценку для i-й компоненты связности 
	vector<TreeAndValueVector> VarAndVals;

	VarAndVals.resize(TopConnectedComponentsCount);


	long HypotCount =1;

	for (size_t i=0; i < TopConnectedComponentsCount;i++)
	{
		GetTree(i, VarAndVals[i]);
		if (VarAndVals[i][0].second.Panic)
		{
			ResultValue.Panic = true;
			EndTimer("Общее время построения дерева");
			if (FirstConjNode.m_Words.size() > 0)
				InsertNode (FirstConjNode);
			if (TryWithoutLongRelations)
		 {
			 TryWithoutLongRelations = false;
			 // уничтожаем длинные отношения, у которых (MaxRelationLeapsCount > 0)  
			 // попробуем  собрать так (ведь это лучше, чем ничего!)
			 DeleteLongRelations(ClauseNo, 0, true);
			 VarAndVals.clear();
			 goto TryWithoutLongRelationsLabel;
		 };

			if (LongRelationsOfAnyKind)
		 {
			 LongRelationsOfAnyKind = false;
			 // уничтожаем длинные отношения идущие от глаголов, у которых (MaxRelationLeapsCount > 2)  
			 // попробуем  собрать так (ведь это лучше, чем ничего!)
			 DeleteLongRelations(ClauseNo, 2, false);
			 VarAndVals.clear();
			 goto TryWithoutLongRelationsLabel;
		 };


			/*
			раньше мы выдавали панический граф, если он возникал,  а теперь
			будем давать синтаксис 
			*/
			m_Relations.clear();
			ApplySynStr (ClauseNo);

			EndTimer("Общее время построения дерева");
			return VarAndVals[i].size();
		};
		// может быть ни одно 
		HypotCount *= VarAndVals[i].size();
	};

	assert (TopConnectedComponentsCount*HypotCount);  

	long* Variants =  new long[TopConnectedComponentsCount*HypotCount];

	long VarNo = 0;
	QuickGetIndexedVariants(VarAndVals, VarNo,  TopConnectedComponentsCount,Variants, HypotCount, 0); 
	assert (VarNo == HypotCount);


	vector<PairOfLong>    WeightAndPositions;
	WeightAndPositions.resize(HypotCount);



	for (size_t i=0; i < WeightAndPositions.size(); i++)
	{
		WeightAndPositions[i].first = 0;
		for (size_t k=0; k < TopConnectedComponentsCount; k++)
			WeightAndPositions[i].first += VarAndVals[k][Variants[i*TopConnectedComponentsCount+k]].second.GetTreeWeight();

		WeightAndPositions[i].second = i;
	};



	sort (WeightAndPositions.begin(), WeightAndPositions.end());

	long VarNum;


	if (m_UserTreeVariantNo >=  WeightAndPositions.size()) 
		VarNum = WeightAndPositions.size() - 1;
	else 
		VarNum = WeightAndPositions[m_UserTreeVariantNo].second;

	SetUseFalse();
	for (size_t i=0; i < TopConnectedComponentsCount; i++)
		AddTreeVariant(VarAndVals[i][Variants[VarNum*TopConnectedComponentsCount+i]].first);

	delete Variants;
	DelUnusedRelations(ClauseNo);
	DelChildlessCopul();
	UpdateBlockedRelations();


	long TagClause = SetTagToClause(ClauseNo);
	UpdateBlockedRelations();

	ValueTreeVariant(ResultValue,TagClause );
	ValueTreeVariantAdditional(ResultValue,TagClause);

	ResultValue.ConnectedComponentsCount = GetConnectedComponentsCount(TagClause);

	// вставляет союз в начало клаузы, если таковый был удален перед этим
	if (FirstConjNode.m_Words.size() > 0)
		InsertNode (FirstConjNode);



	EndTimer("Общее время построения дерева");

	return WeightAndPositions.size();
};





// число отношений, которые проведены из пассивного глагола в ИГ с творительным падежом,
// который был получен из залоговой трансоформации
// Например, "дом строился рабочими"
// Здесь  SUB (рабочими, строился) -   то самое отношение.
long CRusSemStructure::GetInstrAgentRelsCount (long Tag) const
{
 long Result =0;

 for (size_t i = 0;  i < m_Relations.size(); i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
	   if (m_Relations[i].m_Valency.m_InstrAgent)
		   Result++;
  return Result;
};







long IndexRelationStr (StringVector&  Rels, string RelationStr)
{
// считаем, что отношение TIME и LOK могут  встречаться сколько угодно раз,	// поэтому считаем RelationId=-1. Значение "-1"  используется для того, чтобы
// два разных отношения TIME, идущих их одного узла. считались совместными.	  
	if (		RelationStr.empty() 
			||	(RelationStr == "TIME")|| (RelationStr == "LOK")		  
		)
		  return -1;
      else
	  {
		  StringVector::const_iterator It =  find (Rels.begin(),Rels.end(), RelationStr);
		  if (It == Rels.end())
		  {
			  Rels.push_back(RelationStr);
			  return Rels.size() - 1;
		  }
		  else
			  return It - Rels.begin();
	  };

};

void CRusSemStructure::IndexRelations ()
{
	StringVector  Rels;
	for (long i=0; i < m_Relations.size(); i++)
		  m_Relations[i].m_Valency.m_RelationId = IndexRelationStr(Rels, m_Relations[i].m_Valency.m_RelationStr);
	  
	for (long NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
	  for (long k=0; k <m_Nodes[NodeNo].m_Vals.size(); k++)
		  m_Nodes[NodeNo].m_Vals[k].m_RelationId = IndexRelationStr(Rels, m_Nodes[NodeNo].m_Vals[k].m_RelationStr);
};


void CRusSemStructure::ValueTreeVariant (TreeVariantValue& Value, long Tag)
{
  long SemanticVolume = GetSemanticVolume(Tag);  
  long RelsCount = GetUseRelationsCount();  
  Value.Coefs = &m_SemCoefs;

  Value.ValencyDisorder = GetValencyDisorderCount(Tag);

  //  оценка отношений 
  StartTimer("Проверка по SF",2);
  Value.SemFetDisagree = RelsCount ? GetSemFetDisagreeCount(Tag)*1000 / RelsCount : 0;
  EndTimer("Проверка по SF");
  // должно идти после GetSemFetDisagreeCount
  // другие оценки
  StartTimer("Оценка деревьев_1",2);
  Value.OptionalValencyCount = GetOptionalValenciesCount(Tag);
  Value.LexFetAgreeCount = RelsCount ? GetLexFetAgreeCount(Tag)*1000  / RelsCount : 0;
  Value.RelationsLength = SemanticVolume ? GetRelationsLength(Tag)*1000  / SemanticVolume : 0;
  Value.InstrAgentRelsCount =  SemanticVolume ? GetInstrAgentRelsCount(Tag)*1000  / SemanticVolume: 0;
  EndTimer("Оценка деревьев_1");
  // другие оценки
  StartTimer("Проверка проективности",2);
  Value.ProjectnessCoef = IsProjectedNew(Tag) ? 0 : 1;
  EndTimer("Проверка проективности");

  StartTimer("Оценка деревьев_2",2);
  Value.AgreeWithSyntaxTopCoef = AgreeWithSyntaxTop(Tag) ? 1 : 0;
  Value.TopAgreeWithSyntaxCriteria = TopAgreeWithSyntaxCriteria(Tag) ? 1 : 0;
  Value.MNAViolationsCount = GetMNAViolationsCount(Tag);
  Value.SemFetAgreeMNACount = GetSemFetAgreeMNACount(Tag);
  Value.SemRelPOSViolationsCount = GetSemRelPOSViolationsCount(Tag);
  Value.OnlyCommaBetweenViolationsCount = GetOnlyCommaBetweenViolationsCount(Tag);
  Value.SubjectPredicateViolationsCount = GetSubjectPredicateViolationsCount(Tag);
  Value.CommaBetweenBrothersExceptMNAViolationsCount = GetCommaBetweenBrothersExceptMNAViolationsCount(Tag);
  Value.CopulViolationsCount = GetCopulViolationsCount(Tag);
  Value.CorporaGleicheCount = 0;
  Value.ObligatoryValencyViolationCount = GetObligatoryValencyViolation(Tag);
  Value.MiscSemAgreeCount =  GetMiscSemAgreeCount(Tag);
  Value.PrichastieWithoutActantsCount = GetPrichastieWithoutActants(Tag);

  Value.PassiveValencyCount = GetPassiveValencyCount(Tag);
  EndTimer("Оценка деревьев_2");
  
};

void CRusSemStructure::ValueTreeVariantAdditional (TreeVariantValue& Value, long Tag)
{
  Value.ColloquialInterpsCount = GetColloquialInterpsCount(Tag);
  Value.OborotAdverbialCount = GetOborAdverbialCount(Tag);
  Value.WordWeightCount = GetWordWeightSumma(Tag);
};




struct CAntecedentHypot 
{
	CRusSemNode				m_Node;
	long					m_NodeNo;
	long					m_Distance;

	CTreeOfLexVariantWeight	m_Variant;
	
	CAntecedentHypot (const CRusSemNode& Node, long Distance, long NodeNo = -1)
	{
		m_NodeNo = NodeNo;
		m_Node =  Node;
		m_Distance = Distance;
	};
	long GetWeightOfAntHypot () const 
	{
		return m_Distance +  m_Variant.GetBestTreeWeight() - m_Variant.m_BestValue.GetWordWeight();
	};
    bool operator == ( const CAntecedentHypot& X ) const
    {
		return GetWeightOfAntHypot() == X.GetWeightOfAntHypot();
	};

	bool operator < ( const CAntecedentHypot& X ) const
    {
		return GetWeightOfAntHypot() < X.GetWeightOfAntHypot();
	}; 


};
size_t CRusSemStructure::GetSynWordsCount() const
{
	return m_piSent->m_Words.size();
}

void CRusSemStructure::FindPossibleAntecedents()
{
	size_t WordCount = GetSynWordsCount();

	for (map<pair<int,CRusSemNode>, int>::iterator it = m_PriorNounNodes2Distance.begin(); it != m_PriorNounNodes2Distance.end(); it++)
	{
		it->second += WordCount;
		printf ("%i,%s -> %i\n", it->first.first, GetNodeStr((const CSemNode&)it->first.second).c_str(), it->second);
	}
	
	printf ("===\n");
	for (long NodeNo=0; NodeNo <  m_Nodes.size(); NodeNo++)
		if	(		m_Nodes[NodeNo].IsWordContainer()
				&&	m_Nodes[NodeNo].IsPrimitive()
				&&	HasRichPOS (NodeNo, NOUN) 
			)
		{
			m_PriorNounNodes2Distance[pair<int,CRusSemNode>(m_SentenceCount, m_Nodes[NodeNo]) ] =  WordCount - m_Nodes[NodeNo].GetMaxWordNo();
		}

	printf ("new m_PriorNounNodes2Distance ===\n");
	for (map<pair<int,CRusSemNode>, int>::iterator it = m_PriorNounNodes2Distance.begin(); it != m_PriorNounNodes2Distance.end(); it++)
	{
		printf ("%i,%s -> %i\n", it->first.first, GetNodeStr((const CSemNode&)it->first.second).c_str(), it->second);
	}

	m_SentenceCount++;

}
//#pragma optimize( "", off ) 
void CRusSemStructure::BuildAnaphoricRels()
{
	// необходимо получить пересечение SF уже сейчас, иначе личные местоимения
	// окажутся без SF
	PrintRelations();
	ApplyDopInfinitive ();

	ApplyDopLexFuncts ();
	CopyDopRelationsExceptAnaphor();
	ApplySubordinationSemfets ();


	AssertValidGraph();
	for (long NodeNo=0; NodeNo <  m_Nodes.size(); NodeNo++)
		if (m_Nodes[NodeNo].IsAnaphoricPronoun() )
		{
			QWORD Grammems = ((rAllNumbers|rAllGenders) & m_Nodes[NodeNo].m_Words[0].GetAllGrammems());
			vector<CAntecedentHypot> Hypots;
			vector<long>  BrotherNodes;
			GetBrothers(NodeNo, BrotherNodes, false);
			long ClauseNo = m_Nodes[NodeNo].m_ClauseNo;

			// получение всех гипотез
			for (long i=0; i<m_Nodes.size(); i++)
				if (     m_Nodes[i].IsWordContainer()
					&& (i !=NodeNo)
					&& m_Nodes[i].IsPrimitive()
					&& (m_Nodes[i].GetMinWordNo()  <  m_Nodes[NodeNo].GetMinWordNo())
					&& ((m_Nodes[i].GetGrammems() & Grammems) == Grammems)
					&& ( HasRichPOS (i, NOUN) || m_Nodes[i].IsAnaphoricPronoun() )
					&& !_find(BrotherNodes, i)
					//  не идет ни одного отношения напрямую из этого слова в местоимение
					// например, "бронирование его", здесь "его"(лемма "ОНО") не может обоозначать "бронирование" 
					&& (FindFirstRelation(i, NodeNo) == -1)
					)
					Hypots.push_back(CAntecedentHypot(m_Nodes[i], GetDistance(i, NodeNo), i));


			for (map<pair<int,CRusSemNode>, int>::const_iterator it = m_PriorNounNodes2Distance.begin(); it != m_PriorNounNodes2Distance.end(); it++)
				if (((it->first.second.GetGrammems() & Grammems) == Grammems))
				{
					Hypots.push_back(CAntecedentHypot(it->first.second, it->second + m_Nodes[NodeNo].GetMinWordNo() ) );
				}

				if ( Hypots.empty() ) continue;

				for (long HypotNo = 0; HypotNo < Hypots.size(); HypotNo++)
				{   
					vector<CRusSemNode>     SaveNodes   = m_Nodes;
					vector<CRusSemRelation> SaveRels    = m_Relations;
					vector<CSynRelation>	SaveSynRels	 = m_SynRelations;
					vector<CRusSemClause>   SaveClauses = m_Clauses;
					vector<CLexFunctRel>   SaveLexFuncts = m_LexFuncts;
					vector<CSemThesRelation>   SaveThesSemRelations = m_ThesSemRelations;

					CRusSemNode N = Hypots[HypotNo].m_Node;
					N.SetGrammems( m_Nodes[NodeNo].GetGrammems() );
					N.m_GramCodes = m_Nodes[NodeNo].m_GramCodes;
					N.m_SynReal = m_Nodes[NodeNo].m_SynReal;

					if (     (N.m_MainWordNo != -1) 
						&& ( m_Nodes[NodeNo].m_MainWordNo != -1) 
						)
					{
						N.m_Words[N.m_MainWordNo].SetFormGrammems( m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].GetFormGrammems());
						N.m_Words[N.m_MainWordNo].m_WordNo = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_WordNo;
					};

					N.m_ClauseNo = ClauseNo;

					m_Nodes[NodeNo] =  N;
					/*
					Удалим все абстрактные узлы, поскольку они будут простоены 
					функцией BuildTheVariant (а то они два раза строятся)
					*/
					for (long i=0; i < m_Nodes.size(); i++)
						if (   IsInClause(i,ClauseNo)
							&& m_Nodes[i].IsAbstract() 
							&& CanBeDeleted (i)
							)
						{
							DelNode(i);
							i--;
						};

					AssertValidGraph();
					// на самом деле, здесь нужны только оценка,
					// а отношения не нужны 
					Hypots[HypotNo].m_Variant = (CTreeOfLexVariantWeight)BuildTheVariant(ClauseNo);



					m_Nodes     = SaveNodes;
					m_Relations =  SaveRels;
					m_Clauses   = SaveClauses;
					m_LexFuncts = SaveLexFuncts;
					m_ThesSemRelations = SaveThesSemRelations;
					m_SynRelations = SaveSynRels;
				};


				// оценка гипотез
				sort (Hypots.begin(), Hypots.end());
				if (Hypots.size()  > 1)
				{
					string s = Hypots[0].m_Variant.m_BestValue.GetDifference(Hypots[1].m_Variant.m_BestValue);
					int u = 0;
				}
				//printf ("Node:%s\n", GetNodeStr(NodeNo).c_str());
				for (size_t i=0; i < Hypots.size(); i++)
				{
					//printf ("\tHypot:%s %i\n", GetNodeStr(Hypots[i].m_Node).c_str(), Hypots[i].m_Distance);
					//if ( i >0 )
					//printf ("\t\tdiff with prev:%s\n", Hypots[i].m_Variant.m_BestValue.GetDifference(Hypots[i-1].m_Variant.m_BestValue).c_str());

				}



				m_Nodes[NodeNo].m_AntecedentStr = Format("%s[%i]",GetNodeStr((const CSemNode&)Hypots[0].m_Node).c_str(), Hypots[0].m_Distance);

				if (Hypots[0].m_NodeNo != -1)
				{
					// построение отношения
					CRusSemRelation R(CValency("THESAME", A_C, GetRossHolder(Ross)),  NodeNo,  Hypots[0].m_NodeNo,  "анафора");
					m_DopRelations.push_back(R);


				}
				// копируем SF
				m_Nodes[NodeNo].m_NodeSemFets = Hypots[0].m_Node.m_NodeSemFets;


				// копируем оценку лучшего варианта 
				CRusSemClause& C = m_Clauses[ClauseNo];
				m_AlreadyBuiltClauseVariants[C.m_AlreadyBuiltClauseVariantNo].m_BestLexVariants[C.m_CurrLexVariantNo].m_BestValue  =  Hypots[0].m_Variant.m_BestValue;

		};
};


//#pragma optimize( "", on) 




CRusSemNode CreateDummyNode(long WordNo, const CSemanticsHolder* pData) 
{
	  CRusSemNode N;
	  CRusSemWord SemWord(WordNo, Format("%i", WordNo));
	  SemWord.m_pData =  pData;
	  N.m_Words.push_back(SemWord);
	  N.m_Words[0].m_Word = Format("%i", WordNo+1);
	  N.m_ClauseNo = 0;
	  return N;
};

// разбирает строку, например: п1,1,2;п2,2,1
//  здесь через точку с запятой перечислены  отношения
// на первом месте стоит название  отношения, на втором 
// откуда идет, на втором куда. 
bool CRusSemStructure::TryTestTree(string s)
{
	m_Nodes.clear();
	vector<CRusSemRelation> Relations;
	m_Clauses.clear();
	m_Clauses.push_back (CRusSemClause());
	if (s.length() < 7) return false;
	if (s.substr(0, 6) != "#graph") return false;
	s.erase(0,6);
	try {
		StringTokenizer tok (s.c_str(), ";");
		long CountRels = 0;
		while (tok ()) {
			string OneRel = tok.val();
			StringTokenizer tok1 (OneRel.c_str(), ",");
			if (!tok1()) return false;
			string RelStr = tok1.val();
			if (!tok1()) return false;
			const char* s = tok1.val();
			if (!isdigit((unsigned char)s[0]) ) return false;
			int Node1 = atoi(s);
			if (!tok1()) return false;
			s = tok1.val();
			if (!isdigit((unsigned char)s[0]) ) return false;
			int Node2 = atoi(s);
			if (!Node1 || !Node2) return false;
			if (Node1 == Node2) return false;
			Trim(RelStr);
			if (RelStr.find (" ") != -1) return false;
			Relations.push_back(CRusSemRelation(CValency (RelStr, C_A), Node1-1, Node2-1, ""));
			CountRels ++;
		};
		if (CountRels == 0) return false;
		for (long i=0; i <  Relations.size(); i++)
		{
		  if (FindNodeByWordNo(Relations[i].m_SourceNodeNo)  == -1)
		  {
			  CRusSemNode RusSemNode = CreateDummyNode(Relations[i].m_SourceNodeNo, m_pData);
			  InsertNode (RusSemNode);
		  }

		  if (FindNodeByWordNo(Relations[i].m_TargetNodeNo) == -1)
		  {
			  CRusSemNode RusSemNode =  CreateDummyNode(Relations[i].m_TargetNodeNo, m_pData);
			  InsertNode (RusSemNode);
		  }
		};
		m_Relations = Relations;
		TreeVariantValue ResultValue;
		GetTreeByConnectedComponents (0, ResultValue);
		return true;
	}
	catch (...) {
		return false;
	};
};





long CRusSemStructure::GetColloquialInterpsCount(long Tag) const 
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Nodes.size(); i++)
   if (HasTag(i, Tag))
	  if (find (m_Nodes[i].m_POs.begin(), m_Nodes[i].m_POs.end(), "разг") != m_Nodes[i].m_POs.end())
	    Result++;

  return Result; 
};


long CRusSemStructure::GetCorporaGleicheCount(long Tag) const 
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Nodes.size(); i++)
   if (HasTag(i, Tag))
   {
      return m_Clauses[m_Nodes[i].m_ClauseNo].m_BestPONodesCount; 
   };

  return 0; 
};


