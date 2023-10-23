#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"

const int NounSemRelCount = 11;
std::string NounSemRel[NounSemRelCount] = { "SUB","OBJ", "ADR", "PACIEN","AUTHOR", "AGENT", "SRC-PNT", "C-AGENT", "TRG-PNT", "LOK","MEANS" };

void CRusSemStructure::MarkSemRelPOSViolations()
{

	for (size_t i = 0; i < m_Relations.size(); i++)
	{
		CRusSemNode& N = m_Nodes[m_Relations[i].m_TargetNodeNo];
		if (!HasRichPOS(N, NOUN)
			&& !HasRichPOS(N, PRONOUN)  // "я пришел"
			&& !((N.m_MainWordNo != -1) // "каждый должен это сделать"
				&& N.m_Words[N.m_MainWordNo].IsRusSubstPronounP()
				)
			&& !(HasRichPOS(N, PRONOUN_P)  // "на наш взгляд" (SUB - отношение)			         
				&& HasRichPOS(m_Relations[i].m_SourceNodeNo, NOUN)
				)
			)
		{
			for (size_t k = 0; k < NounSemRelCount; k++)
				if (m_Relations[i].m_Valency.m_RelationStr == NounSemRel[k])
				{
					m_Relations[i].m_bSemRelPOS = false;
					break;
				};
		};
	};

};

long CRusSemStructure::GetSemRelPOSViolationsCount(long Tag)
{
	size_t Result = 0;
	long Count = m_Relations.size();
	for (size_t i = 0; i < Count; i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (!m_Relations[i].m_bSemRelPOS)
					Result++;

	return Result;
};



//========================================================
//========================================================

long CRusSemStructure::GetOnlyCommaBetweenViolationsCount(long Tag)
{
	size_t Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (m_Relations[i].m_CannotHaveOnlyCommaBetween)
					if (IsWordContainerRel(i))
						if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType == MNA)
						{
							// если стрелка идет в однородный ряд, то 
							// пусть Х=GetRelation(i)->m_SourceNodeNo, а M1, ..., Mn - члены однородного ряда,
							// упорядоченные по тексту. Если X < M1, то проверим отрицание HaveOnlyPunctBetween
							// между X и M1, если Х > Мn, то - между X и Mn. Таким образом мы считаем, что
							// однородный ряд, как бы, является одной группой, которая начинается с М1 и заканчивается Мn.
							// это нужно для того, чтобы член однородного ряда не мог управлять всем
							// однородным рядом.

							long MnaNodeNo = m_Relations[i].m_TargetNodeNo;
							const CRusSemClause& C = m_Clauses[m_Nodes[MnaNodeNo].m_ClauseNo];
							long StartNodeNo = -1;
							long EndNodeNo = -1;
							long NodesCount = m_Nodes.size();
							for (long k = 0; k < NodesCount; k++)
								if (HasTag(k, Tag))
								{
									long Count = m_Nodes[k].m_InRels.size();
									for (long j = 0; j < Count; j++)
										if ((m_Relations[m_Nodes[k].m_InRels[j]].m_SourceNodeNo == MnaNodeNo)
											&& m_Relations[m_Nodes[k].m_InRels[j]].m_bRelUse
											)
										{
											if (StartNodeNo == -1) StartNodeNo = k;
											EndNodeNo = k;
										};
								};
							/*если у MUA нет ни одного актанта?*/			   if (StartNodeNo == -1) continue;;


							if (m_Relations[i].m_SourceNodeNo < m_Nodes[StartNodeNo].GetMinWordNo())
							{
								if (C.GetGramMatrix(m_Relations[i].m_SourceNodeNo, StartNodeNo) & HaveOnlyCommaBetween)
									Result++;
							}
							else
								if (m_Relations[i].m_SourceNodeNo > EndNodeNo)
									if (C.GetGramMatrix(m_Relations[i].m_SourceNodeNo, EndNodeNo) & HaveOnlyCommaBetween)
										Result++;
						}
						else
						{
							const CRusSemClause& C = m_Clauses[m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo];

							if (C.GetGramMatrix(m_Relations[i].m_TargetNodeNo, m_Relations[i].m_SourceNodeNo) & HaveOnlyCommaBetween)
								Result++;
						};

	return Result;
};

//========================================================
//========================================================
// проверка того, что найденное семантикой  подлежащее равно синтаксическому подлежащему 
bool CRusSemStructure::GetAgreeWithSyntaxTop(long Tag) const
{
	long ClauseNo = -1;
	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag)) {
			ClauseNo = m_Nodes[i].m_ClauseNo;
			break;
		}
	if (ClauseNo == -1) return true;
	long SubjectNodeNo = (m_Clauses[ClauseNo].m_SubjectWordNo == -1) ? -1 : FindNodeByWordNo(m_Clauses[ClauseNo].m_SubjectWordNo, ClauseNo);

	if (SubjectNodeNo != -1) {
		LOGV << "check node " << SubjectNodeNo  << " is syntax subject";
		for (auto relNo : m_Nodes[SubjectNodeNo].m_InRels)
		{
			if (HasTag(m_Relations[relNo].m_SourceNodeNo, Tag))
				if (m_Relations[relNo].m_bRelUse)
					if (m_Relations[relNo].m_SyntacticRelation != _R("подл")) {
						LOGV << "found rel "<< m_Relations[relNo].m_SyntacticRelation << ", so it is not a subject relation";
						return false;
					}
		};
	}

	return true;
};

//========================================================
//========================================================
/*
функция  берет узел Х. Если Х является MUA, пропускаем его.  Берем все  потомки Х. Если два потомка Х разделяет
  только запятая, то функция увеличивает результирующий штраф.
  Например, по этому правилу будет дан один штраф предложению:
   "Мама, папу любит"
*/
long CRusSemStructure::GetCommaBetweenBrothersExceptMNAViolationsCount(long Tag) const
{
	long Result = 0;

	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag)
			&& (m_Nodes[i].m_NodeType != MNA)
			)
		{
			size_t ClauseNo = m_Nodes[i].m_ClauseNo;
			const CRusSemClause& C = m_Clauses[ClauseNo];
			const std::vector<long>& Rels = m_Nodes[i].m_OutRels;

			for (long k = 0; k + 1 < Rels.size(); k++)
				if (m_Relations[Rels[k]].m_bRelUse)
				{
					long  NodeNo1 = m_Relations[Rels[k]].m_TargetNodeNo;
					if (!m_Nodes[NodeNo1].IsWordContainer()) continue;
					if (!IsInClause(NodeNo1, ClauseNo)) continue; // from BuildAnaphoricRels
					size_t j = k + 1;
					for (; j < Rels.size(); j++)
						if (m_Relations[Rels[j]].m_bRelUse)
							break;
					if (j == Rels.size()) break;

					long  NodeNo2 = m_Relations[Rels[j]].m_TargetNodeNo;
					if (!m_Nodes[NodeNo2].IsWordContainer()) continue;
					if (!IsInClause(NodeNo2, ClauseNo)) continue; // from BuildAnaphoricRels

					if (C.GetGramMatrix(NodeNo1, NodeNo2) & HaveOnlyCommaBetween)
						Result++;
				}
		};

	return Result;
};



/*
	Здесь используется порядок на узлах
*/
bool CRusSemStructure::IsProjectedNew(long Tag) const
{
	long Count = m_Relations.size();
	// здесь неважно направление,  поэтому n*n/2
	for (long i = 0; i < Count; i++)
		for (long k = i + 1; k < Count; k++)
			if (m_Relations[i].m_bRelUse
				&& m_Relations[k].m_bRelUse
				&& IsWordContainerRel(i)
				&& IsWordContainerRel(k)
				)
			{
				long X1 = m_Relations[i].m_SourceNodeNo;
				long X2 = m_Relations[i].m_TargetNodeNo;
				long Y1 = m_Relations[k].m_SourceNodeNo;
				long Y2 = m_Relations[k].m_TargetNodeNo;

				// =================================================================
				// проверка условия слабой проективности: все узлы, которые лежат под некоторой 
				// стрелкой, должны быть подчинены главному узлу этой стрелки, то есть
				// запрещено следующее X1<X2<X3 и (X2->X3, X3->X1). Здесь  вершина Х2 не подчинена Х3,
				// а это должно быть
				// инфинитив в качестве промежуточной вершины не проверяется, например:
				// "на него стоит настроиться"
				// "с ним нелегко иметь дело"

				// 
				// сравнительный оборот не будем проверять на проективность
				// например: "в 2000 году  мы построим больше школ, чем в прошлом году

				// InterMediaRel - отношение, в выходящий узел которого взодит другое отношение
				long InterMediaRel = (X1 == Y2) ? i : k;
				if (!HasRichPOS(m_Relations[InterMediaRel].m_SourceNodeNo, INFINITIVE))
					if (m_Nodes[m_Relations[InterMediaRel].m_SourceNodeNo].m_MNAType != CHEMOborot)
						if (((X1 == Y2)
							&& (X2 < Y1)
							&& (Y1 < X1)
							)
							|| ((X1 == Y2)
								&& (X1 < Y1)
								&& (Y1 < X2)
								)
							|| ((Y1 == X2)
								&& (Y2 < X1)
								&& (X1 < Y1)
								)
							|| ((Y1 == X2)
								&& (Y1 < X1)
								&& (X1 < Y2)
								)
							)
							return false;


				// ==========================================================
				// проверка сильной проективности (стрелки не должны пересекаться)

				if (X1 > X2) swap(X1, X2);
				if (Y1 > Y2) swap(Y1, Y2);
				if (X1 > Y1)
				{
					swap(X1, Y1);
					swap(X2, Y2);
				};
				if ((X1 < Y1) && (Y1 < X2) && (X2 < Y2))
				{
					// приходится обрабатывать отдельно конструкцию с инфинитивом типа:
					// "это собирались сделать враги"
					// будем считать, что следующий if - достаточнре условие для обнаружения этого случая	
					if (HasRichPOS(X2, INFINITIVE)
						&& (Y1 + 1 == X2)
						&& HasRichPOS(Y1, VERB)
						)
						continue;

					return false;
				};
			}
	return true;

};

//========================================================
//========================================================
//  число валентностей у Copul должно быть равно 0 или двум (П_АКТ, S-ACT).// Если у Copul нет валетностей, то он  будет удален  на этапе Идеализации.
// к копулам приравниваются словарные статьи, у которых в SF стоит Copul, но
// не стоит CAUS и NEG. (SF(выращивать) = CAUS,COpul - это не настоящий копул)

long CRusSemStructure::GetCopulViolationsCount(long Tag) const
{
	long Result = 0;
	long Count = m_Nodes.size();
	for (size_t i = 0; i < Count; i++)
		if (HasTag(i, Tag))
			if ((m_Nodes[i].m_NodeType == Copul)
				|| HasSemFetPro(m_Nodes[i], "Copul")
				)
			{
				const std::vector<long>& Rels = m_Nodes[i].m_OutRels;
				long CopulRelsCount = 0;

				for (long k = 0; k < Rels.size(); k++)
					if (m_Relations[Rels[k]].m_bRelUse)
						if ((m_Relations[Rels[k]].m_Valency.m_RelationStr == "F-ACT") || (m_Relations[Rels[k]].m_Valency.m_RelationStr == "S-ACT"))
							CopulRelsCount++;

				if ((CopulRelsCount != 0) && (CopulRelsCount != 2)) Result++;
			};
	return Result;
};

//========================================================
//========================================================
/*
 считает число узлов-оборотов, которым приписано GF=ОБСТ_ГР
*/
long CRusSemStructure::GetOborAdverbialCount(long Tag) const
{
	long Result = 0;

	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (m_Nodes[i].GetType() == OborRoss)
				if (m_pData->Oborottos[m_Nodes[i].GetUnitNo()].m_bRusOborAdverbial)
					Result++;

	return Result;
};



//========================================================
//========================================================

// будем считать, что у слова моежт быть только 10 обязательных валентностей
struct CObligatoryVal {
	BYTE ValsCount;
	char ValsNo[10]; //must be unsigned
	/*
	  Если  первая валентнсть из любого несовместного
	  множества заполнена,  то все остальные первые валентности
	  уже не считаются обязательными.
	  Например, "я заключил договор" (заключить 1)
	  Слот FirstValencyWasFilled равен истине, если
	  у узла хотя бы одна первая валентнсть заполнена.
	*/
	bool FirstValencyWasFilled;
	CObligatoryVal()
	{
		ValsCount = 0;
		FirstValencyWasFilled = false;
	};
};
/*
 слот FirstValencyWasFilled решает только частный случай,
 на самом деле, здесь нужно программировать по-другому.
 Нужно искать все несовместные пары валентностей, которые
 обе являются обязательными. Тогда если заполнена одна из них
 (а обе никак не могут быть заполнены), тогда  то, что вторая валенетность
 не заполнена, не надо считать за ошибку.
 Если обе несовместные обязательные валентности не заполнены, тогда
 это нужно считать за одну ошибку.
 Если же из двух несовместных валентностей  только одна обязательна,
 тогда незаполненность обязательной валентности всегда ошибка.
 Встает вопрос, что делать, когда у тебя много пар несовместных валентностей.
 Как подсчитать количество незаполненных обязательных валентностей?
*/

// считает число узлов, у которых не заполнена первая валентность (или валентность,
// которая входит в поле ОБЯЗ_ВАЛ)
// рассматриваются только глаголы, предикативы, краткие формы (для всех остальных
// правило первой валентности не проверяется)
long CRusSemStructure::GetObligatoryValencyViolation(long Tag)
{
	long NodesCount = m_Nodes.size();
	std::vector<CObligatoryVal> X(NodesCount);

	long StartNodeOfTag = -1;
	for (long i = 0; i < NodesCount; i++)
		if (HasTag(i, Tag))
		{
			if (StartNodeOfTag == -1) StartNodeOfTag = i;
		};

	if (StartNodeOfTag == -1)	  return  0;
	for (long i = StartNodeOfTag; i < NodesCount; i++)
		if (HasTag(i, Tag))
			for (long j = 0; j < m_Nodes[i].m_Vals.size(); j++)
				if (m_Nodes[i].m_Vals[j].m_bObligatory)
					X[i - StartNodeOfTag].ValsNo[X[i - StartNodeOfTag].ValsCount++] = j;


	long RelsCount = m_Relations.size();
	/*
	будем рассматривать только активные и уникальные отношения,
	а то получается, что во фразе "сколько времени это у тебя займет "
	"это" может заполнять 1 и 2 валентности глагола "займет". Первая валнтеность - обязательная.
	В гипотетическом графе одновременно сосуществуют обе стрелки к "это": как 1 и как 2 валлентности.
	Если убрать условие об активности или уникальности в варианте, где "это" - 2 валентность
	мы будем считать, что первая вале=нтность заполнена.
	*/
	for (long i = 0; i < RelsCount; i++)
		if (m_Relations[i].m_bRelUse || m_Relations[i].m_bUniqueRelation)
		{
			long NodeNo = m_Relations[i].m_SourceNodeNo;
			if (NodeNo >= StartNodeOfTag)
				for (int j = 0; j < X[NodeNo - StartNodeOfTag].ValsCount; j++)
				{
					char& ValNo = X[NodeNo - StartNodeOfTag].ValsNo[j];
					if (ValNo != -1)
						if (m_Relations[i].m_Valency == m_Nodes[NodeNo].m_Vals[ValNo])
						{
							if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
								X[NodeNo - StartNodeOfTag].FirstValencyWasFilled = true;
							ValNo = -1;

						};
				}


		};
	// поищем в LF, если в слово входит LF тита Oper1, тогда не нужно искать заполнение  
	// первой валентности, поскольку она заполняется с помощью присвязочного глагола
	RelsCount = m_LexFuncts.size();
	for (long i = 0; i < RelsCount; i++)
	{
		long NodeNo = m_LexFuncts[i].m_SourceNodeNo;
		if (IsOper1(m_LexFuncts[i].m_LexFunctName))
			if (m_Nodes[NodeNo].m_Vals.size() > 0)
				if (NodeNo >= StartNodeOfTag)
					for (int j = 0; j < X[NodeNo - StartNodeOfTag].ValsCount; j++)
					{
						char& ValNo = X[NodeNo - StartNodeOfTag].ValsNo[j];
						if (ValNo != -1)
							if (m_Nodes[NodeNo].m_Vals[0] == m_Nodes[NodeNo].m_Vals[ValNo])
							{
								if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
									X[NodeNo - StartNodeOfTag].FirstValencyWasFilled = true;
								ValNo = -1;
							};
					};
	};

	// поищем в дополнительных отношениях, которые, правда почти всегда пусты, но
	// при обработке  межклаузных связей могут заполниться
	RelsCount = m_DopRelations.size();
	for (long i = 0; i < RelsCount; i++)
	{
		long NodeNo = m_DopRelations[i].m_SourceNodeNo;
		if (NodeNo >= StartNodeOfTag)
			for (int j = 0; j < X[NodeNo - StartNodeOfTag].ValsCount; j++)
			{
				char& ValNo = X[NodeNo - StartNodeOfTag].ValsNo[j];
				if (ValNo != -1)
					if (m_DopRelations[i].m_Valency == m_Nodes[NodeNo].m_Vals[ValNo])
					{
						if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
							X[NodeNo - StartNodeOfTag].FirstValencyWasFilled = true;
						ValNo = -1;

					};
			};
	};



	long Result = 0;
	for (long i = StartNodeOfTag; i < NodesCount; i++)
		for (int j = 0; j < X[i - StartNodeOfTag].ValsCount; j++)
			if (X[i - StartNodeOfTag].ValsNo[j] != -1)
			{
				char& ValNo = X[i - StartNodeOfTag].ValsNo[j];

				if (X[i - StartNodeOfTag].FirstValencyWasFilled)
					if (m_Nodes[i].m_Vals[ValNo].m_bObligatoryBecauseFirst)
						continue;
				Result++;
			};
	return Result;
};

//========================================================
//========================================================
/*
 выдает число пассивных валентностей. Пассивная валентность
 имеет меньший приоритет, чем активная.
  Можно, наверно, объявить, что все пассивные валентности
  являются  факультативными, но по сути своей они, конечно,
  не факультаивны, а синтаксически обязательны.
*/
long CRusSemStructure::GetPassiveValencyCount(long Tag) const
{
	size_t Result = 0;

	long Count = m_Relations.size();

	for (size_t i = 0; i < Count; i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (m_Relations[i].m_bReverseRel)
					Result++;

	return Result;
};


//===========================

// проверка согласования SF  (кроме сравнительного оборота)
void CRusSemStructure::FindSemFetDisagree(long Tag)
{
	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag)
			&& HasTag(m_Relations[i].m_TargetNodeNo, Tag)

			&& m_Relations[i].m_bRelUse
			)
		{
			m_Relations[i].m_bSemFetAgree = false;
			// для сравнительного оборота нужно проводить специальную проверку (см. файл 
			// сравнительные конструкции), которая, на самом деле, равна стандартной,
			// поэтому  здесь считаем, что CHEMOborot - не является разновидностью MNA
			//if (  m_Nodes[m_Relations[i].m_SourceNodeNo].IsMNA_ButNotCHEM() ) continue;

			bool HasOneContradiction = false;
			bool NeighboursAreGleicheSemFet = true;
			long LastNeighbour = -1;
			long NodesCount = 0;
			long Rels[200];
			long RelsCount = 0;
			for (long k = 0; k < m_Relations.size(); k++)
				if (((m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType == MNA)
					&& (m_Relations[k].m_SourceNodeNo == m_Relations[i].m_TargetNodeNo)
					&& m_Relations[k].m_Valency.m_RelationStr.empty()
					&& m_Relations[k].m_bRelUse
					)
					|| ((m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType != MNA)
						&& (i == k)
						)
					)
				{
					Rels[RelsCount++] = k;
					if (LastNeighbour != -1)
						NeighboursAreGleicheSemFet = NeighboursAreGleicheSemFet
						&& GleicheSemFet(m_Nodes[LastNeighbour].m_NodeSemFets, m_Nodes[m_Relations[k].m_TargetNodeNo].m_NodeSemFets, false);
					LastNeighbour = m_Relations[k].m_TargetNodeNo;
					NodesCount++;
					size_t HostUnitNo = m_Relations[k].m_Valency.m_UnitNo;
					long TargetNodeNo = m_Relations[k].m_TargetNodeNo;

					// проверка временных валентностей (до учета m_bReverseRel)
					if ((m_Relations[i].m_Valency.m_RelationStr == "TIME")
						&& IsTimeAdverbial(TargetNodeNo)
						)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					if (m_Relations[i].m_bReverseRel)
						TargetNodeNo = m_Relations[i].m_SourceNodeNo;
					size_t SlaveUnitNo = m_Nodes[TargetNodeNo].GetSemCorrectUnitNo();

					m_Relations[k].m_bSemFetAgree = false;

					if ((m_Relations[k].m_SyntacticRelation == _R("врем_группа"))
						|| (m_Relations[k].m_Valency.m_RelationStr == "THESAME"))
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					/*
					считаем, что вводные обороты семантически согласованы 	 с любой ситуацией
					*/
					if (m_Nodes[m_Relations[k].m_TargetNodeNo].m_bSaveIsParenthesis)
						if (HasSemFetPro(m_Nodes[m_Relations[i].m_SourceNodeNo], "SITUAT")) {
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};





					if (find(m_Relations[k].m_LexFets.begin(), m_Relations[k].m_LexFets.end(), "KEY") != m_Relations[k].m_LexFets.end())
						if (m_Nodes[TargetNodeNo].m_SynGroupTypeStr == KEYB_STR)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};

					if (m_Nodes[TargetNodeNo].m_NodeType == Copul)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					if (m_Nodes[TargetNodeNo].m_CollocId != -1)
						if (m_Nodes[TargetNodeNo].m_CollocId == m_Nodes[m_Relations[i].m_SourceNodeNo].m_CollocId)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};




					if (m_Relations[i].m_Valency.m_RelationStr == "VALUE")
						if (m_Nodes[TargetNodeNo].m_SynGroupTypeStr == NUMERAL_NOUN_STR)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};

					/*Этот оператор введен для примера "сорок человек", где "сорок"
					интерпретируется как "сорока"	 (птица)
					*/
					if (m_Relations[i].m_Valency.m_RelationStr == "QUANTIT")
						if (m_Relations[i].m_SyntacticRelation == _R("ЧИСЛ_СУЩ"))
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};



					// проверка местоимений
					if (HasRichPOS(TargetNodeNo, PRONOUN))
					{
						// местоимения, которые стоят в среднем роде, не должны 
						// заполнять одушевленную валентность
						if (m_Nodes[TargetNodeNo].HasOneGrammem(rNeutrum))
						{
							if (HasSemFetPro(m_Relations[i].m_SemFets, "ANIM")
								&& (m_Relations[i].m_SemFets.size() == 1)
								)
							{
								HasOneContradiction = true;
								continue;
							};
						};

						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					// проверка локативных валентностей
					if (IsLocSemRel(m_Relations[i].m_Valency.m_RelationStr)
						&& ((m_Nodes[TargetNodeNo].GetType() == LocRoss)
							|| (m_Nodes[TargetNodeNo].m_ThesaurusId == LocThes)
							)
						)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					std::string LocSemRel[2] = { "SRC-PNT","TRG-PNT" };
					size_t j = 0;
					for (; j < 2; j++)
						if ((m_Relations[i].m_Valency.m_RelationStr == LocSemRel[j])
							&& (m_Nodes[TargetNodeNo].GetType() == LocRoss)
							&& HasItem(LocRoss, SlaveUnitNo, "SF", LocSemRel[j], "D_SEM_REL", 0, 0)
							)
							break;

					if (j < 2)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					if ((m_Relations[i].m_Valency.m_RelationStr == "IDENT")
						&& (m_Nodes[TargetNodeNo].m_MainWordNo != -1)
						&& ((m_Nodes[TargetNodeNo].m_Words[m_Nodes[TargetNodeNo].m_MainWordNo].m_CharCase != LowLow)
							|| m_Nodes[TargetNodeNo].m_bQuoteMarks
							|| m_Nodes[TargetNodeNo].m_Words[m_Nodes[TargetNodeNo].m_MainWordNo].m_ILE
							)
						)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};



					if ((HostUnitNo == ErrUnitNo)
						|| (SlaveUnitNo == ErrUnitNo))
					{
						HasOneContradiction = true;
						continue;
					};


					//  главная проверка по словарным статьям
					if (m_Relations[i].m_Valency.m_RossHolder != 0)
					{
						// когда прописаны LEX (здесь же "вышестоящие концепты"), но нет SFi, тогда будем считать, что
						// по SF не согласовалось, поскольку у проверка на LEX есть своя собственная процедура
						if (!m_Relations[i].m_SemFets.empty()
							|| m_Relations[i].m_LexFets.empty()
							)
						{
							m_Relations[k].m_bSemFetAgree = (m_Relations[k].m_bRelUse && GleicheSemFet(m_Relations[i].m_SemFets, m_Nodes[TargetNodeNo].m_NodeSemFets, true));
						}
					};

					if (!m_Relations[k].m_bSemFetAgree)
						HasOneContradiction = true;
				};

			// будем считать, что узел MUA удовлетворяет SFi,	 
			// если все его члены удовлетворяют SFi или между собой согласованы попарно
			// ближайшие соседи.
			m_Relations[i].m_bSemFetAgree = !HasOneContradiction
				|| (NeighboursAreGleicheSemFet
					&& (NodesCount > 1)
					);

			for (long k = 0; k < RelsCount; k++)
				if (Rels[k] != i)
					m_Relations[Rels[k]].m_bSemFetAgree = (NeighboursAreGleicheSemFet
						&& (RelsCount > 1)
						);



		};


};


long CRusSemStructure::GetSemFetDisagreeCount(long Tag)
{

	FindSemFetDisagree(Tag);

	long Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
	{
		if (!HasTag(m_Relations[i].m_SourceNodeNo, Tag)) continue;
		if (!HasTag(m_Relations[i].m_TargetNodeNo, Tag)) continue;

		if (m_Relations[i].m_bRelUse
			&& !m_Relations[i].m_bSemFetAgree)
			Result++;

	};

	return Result;
};

//=======================================
// проверка того, что семантическая вершина клаузы удовлетворяет синтаксическим критериям 
bool CRusSemStructure::GetTopAgreeWithSyntaxCriteria(long Tag)
{
	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (m_Nodes[i].IsWordContainer())
			{
				CRelSet R = GetIncomingRelations(i, true);

				if (R.m_RelsCount == 0) // i - корень дерева
					if (!IsCoordConj(i) && !HasRichPOS(i, CONJ))
						return    HasRichPOS(i, INFINITIVE)
						|| HasRichPOS(i, VERB)
						|| HasRichPOS(i, PARTICIPLE)
						|| HasRichPOS(i, PARTICIPLE_SHORT)
						|| HasRichPOS(i, PREDK)
						|| (HasRichPOS(i, NOUN)
							&& m_Nodes[i].HasOneGrammem(rNominativ)
							)
						|| HasRichPOS(i, ADVERB_PARTICIPLE)
						|| HasRichPOS(i, ADJ_SHORT);
			};

	return false;
};



//=======================================
typedef short_string<40> string40;
long CRusSemStructure::IsLexFetAgree(long NodeNo) const
{
	long ParentNodeNo = NodeNo;

	CRelSet R1 = GetIncomingRelations(NodeNo, true);

	if (R1.m_RelsCount != 1) return false;


	if (m_Nodes[m_Relations[R1.m_Rels[0]].m_SourceNodeNo].m_NodeType == MNA)
		ParentNodeNo = m_Relations[R1.m_Rels[0]].m_SourceNodeNo;



	CRelSet R2 = GetIncomingRelations(ParentNodeNo, true);

	// ищем в вышестоящих концептах	
	for (long i = -0; i < m_Nodes[NodeNo].m_HigherConcepts.size(); i++)
		if (_find(m_Relations[R1.m_Rels[0]].m_LexFets, m_Nodes[NodeNo].m_HigherConcepts[i])
			|| ((R2.m_RelsCount == 1) && _find(m_Relations[R2.m_Rels[0]].m_LexFets, m_Nodes[NodeNo].m_HigherConcepts[i]))
			)
			return true;

	// ищем как простую лемму 
	if (m_Nodes[NodeNo].m_MainWordNo == -1) return false;
	string40 Lemmas[10];
	int LemmasCount = 0;
	Lemmas[LemmasCount++] = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma;
	if (m_Nodes[NodeNo].GetType() == CollocRoss)
	{
		Lemmas[LemmasCount++] = string40(GetRoss(CollocRoss)->GetEntryStr(m_Nodes[NodeNo].GetUnitNo()).c_str());
		EngRusMakeUpper((char*)Lemmas[LemmasCount - 1]);
	};

	if (m_Nodes[NodeNo].IsPrimitive())
		for (long j = 0; j < m_Nodes[NodeNo].m_Words[0].m_WordEquals.size(); j++)
		{
			Lemmas[LemmasCount++] = m_Nodes[NodeNo].m_Words[0].m_WordEquals[j];
			EngRusMakeUpper((char*)Lemmas[LemmasCount - 1]);
		};

	for (long i = 0; i < LemmasCount; i++)
	{
		std::string lem((const char*)Lemmas[i]);
		if (_find(m_Relations[R1.m_Rels[0]].m_LexFets, lem)
			|| ((R2.m_RelsCount == 1)
				&& _find(m_Relations[R2.m_Rels[0]].m_LexFets, lem)
				)
			)
			return true;
	}
	return false;
};


long CRusSemStructure::GetLexFetAgreeCount(long Tag)
{
	size_t Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (IsLexFetAgree(m_Relations[i].m_TargetNodeNo))
					Result++;

	return Result;
};



//=======================================
bool CRusSemStructure::CheckSubjectPredicateRelation(long SubjNodeNo, long PredNodeNo)
{
	if (m_Nodes[SubjNodeNo].m_NodeType == MNA)  return true;
	if (m_Nodes[PredNodeNo].m_NodeType == Copul) return true;;
	if (m_Nodes[PredNodeNo].m_NodeType == ModalCopul) return true;
	if (m_Nodes[SubjNodeNo].IsPrimitive())
	{
		// "ничего не произошло"
		if (m_Nodes[SubjNodeNo].m_Words[0].m_Word == _R("НИЧЕГО")) return true;
		// "никто не пришел"
		if (m_Nodes[SubjNodeNo].m_Words[0].m_Word == _R("НИКТО")) return true;
	};
	/*
	  Разрешаем обе формы: "два мальчика пришло" и "два мальчика пришли", т.е
	  если у субъекта есть числительное>1, тогда выходим из проверки.
	*/


	for (auto& rel_no: m_Nodes[SubjNodeNo].m_OutRels)
	{
		if (!m_Relations[rel_no].m_bRelUse) continue;
		long NodeNo = m_Relations[rel_no].m_TargetNodeNo;
		if (HasRichPOS(NodeNo, NUMERAL)
			&& m_Nodes[NodeNo].IsPrimitive()
			&& (atoi(m_Nodes[NodeNo].m_Words[0].m_Word.c_str()) > 1)
			)
			return true;
	};
	auto target_gram_codes = m_Nodes[PredNodeNo].m_GramCodes;
	if (target_gram_codes.empty() and m_Nodes[PredNodeNo].GetWordsSize() > 0) {
		target_gram_codes = m_Nodes[PredNodeNo].m_Words[0].m_GramCodes;
	}

	return m_pData->GetRusGramTab()->GleicheSubjectPredicate(m_Nodes[SubjNodeNo].m_GramCodes.c_str(), target_gram_codes.c_str());

};

long CRusSemStructure::GetSubjectPredicateViolationsCount(long Tag)
{
	size_t Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (m_Relations[i].m_SyntacticRelation == _R("подл"))
					if (!CheckSubjectPredicateRelation(m_Relations[i].m_TargetNodeNo, m_Relations[i].m_SourceNodeNo))
						Result++;

	return Result;
};

//=======================================
long CRusSemStructure::GetOptionalValenciesCount(long Tag)
{
	size_t Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if (m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType == MNA)
				{
					CRelSet R = GetIncomingRelations(m_Relations[i].m_SourceNodeNo, true);
					if ((R.m_RelsCount > 0)
						&& IsOptional(R.m_Rels[0])
						)
						Result++;
				}
				else
					if (IsOptional(i))
						Result++;

	return Result;
};

//=======================================
//=======================================
/*
 пока эта функция обрабатывает следующие случаи:
1. Она ищет отношение, идущее от UNIT (метр, день, век) в чилительное и повышает   вес такому отношению.
   Это нужно для примера "два века", где вместо "век" берется  "веко". "Веко"
не является UNIT, поэтому оно должно уйти.*/
long CRusSemStructure::GetMiscSemAgreeCount(long Tag) const
{
	long Result = 0;

	for (size_t i = 0; i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
			{
				if (HasSemFetPro(m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeSemFets, "UNIT"))
					if (HasPOS(m_Nodes[m_Relations[i].m_TargetNodeNo], NUMERAL)
						|| HasPOS(m_Nodes[m_Relations[i].m_TargetNodeNo], NUMERAL_P)
						)
						Result++;

			}
	return Result;

}

//=====================================
//=====================================
// проверка уровня беспорядка в валентностях
long CRusSemStructure::GetValencyDisorderCount(long Tag)
{
	long Result = 0;
	for (size_t NodeNo = 0; NodeNo < m_Nodes.size(); NodeNo++)
		if (HasTag(NodeNo, Tag))
		{
			CRelSet OutRels;
			CRelSet ValencyPlaces = GetValencyPlaces(NodeNo, OutRels);
			if (ValencyPlaces.m_RelsCount < 2) continue;

			// установить порядок такой, как указан в диатезе
			for (long i = 0; i < ValencyPlaces.m_RelsCount; i++)
				for (long j = 0; j < m_Nodes[NodeNo].m_ValsLayout.size(); j++)
					if (ValencyPlaces.m_Rels[i] == m_Nodes[NodeNo].m_ValsLayout[j])
					{
						ValencyPlaces.m_Rels[i] = j;
						break;
					};

			/*
			Факультатитвные валентности на место и время учитывать не будем
			*/
			/*
			если в начале клаузы стоит союзное слово, тогда его учитывать не надо
			например "я знаю, где ты жил."
			Здесь формально локатив стоит перед субъектом, но нарушения
			порядка следования нет.
			*/


			long ValTextPos = 0;
			long PrevValRelNo = -1;
			for (long i = 0; i < ValencyPlaces.m_RelsCount; i++)
				if (!m_Relations[OutRels.m_Rels[i]].m_Valency.m_bOptional
					&& !m_Nodes[m_Relations[OutRels.m_Rels[i]].m_TargetNodeNo].m_bConjWord
					)
				{

					if (ValencyPlaces.m_Rels[i] < PrevValRelNo)
						Result++;

					PrevValRelNo = ValencyPlaces.m_Rels[i];
				};

		};

	return Result;
};

//=======================================

long CRusSemStructure::GetWordWeightSumma(long Tag) const
{
	long Result = 0;

	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
		{
			if (m_Nodes[i].GetType() == OborRoss)
				Result += 30; // добавляем вес оборотам

			for (size_t j = 0; j < m_Nodes[i].m_Words.size(); j++)
				Result += m_Nodes[i].m_Words[j].m_WordWeight;
		}

	return Result;
};

//=======================================
/*
 считаем число полных причастий, у которых нет ни одного актанта. Это
 считается плохо, тогда лучше выбирать прилагательные или существительные.
 Например: "предстоящие выборы" -> "forthcoming elections" (лучше так!)
*/
long CRusSemStructure::GetPrichastieWithoutActants(long Tag) const
{
	long Result = 0;

	for (size_t i = 0; i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (HasRichPOS(i, PARTICIPLE))
			{
				long j = 0;
				for (; j < m_Nodes[i].m_OutRels.size(); j++)
					if (m_Relations[m_Nodes[i].m_OutRels[j]].m_bRelUse)
						break;
				if (j == m_Nodes[i].m_OutRels.size())
					Result++;
			};

	return Result;
};


