#include "stdafx.h"
#include "SemanticRusStructure.h"

// the graph for a viewer (rossdev)

void	CRusSemStructure::GetColorAndWidthOfRelation(int RelNo, float& Width, std::string& Color) const
{
	Width = 1;
	Color = m_Relations[RelNo].m_bSemFetAgree ? "blue" : "darkgreen";
}

std::string    CRusSemStructure::GetClauseProperiesStr(long ClauseNo) const
{
	std::string Res = "Свойства:\n";
	if (m_Clauses[ClauseNo].m_bQuestion)
	{
		Res += "Вопросительная клауза\n";
	};
	return Res;
};

static void AddAttr(std::vector<CSemGraphAttr>& attrs, const std::string& title, const std::string& value)
{
	if (!value.empty())
		attrs.push_back({ title, value });
}

static std::string JoinLines(const StringVector& v)
{
	std::string result;
	for (auto& s : v)
		result += s + "\n";
	return result;
}

void CSemanticStructure::GetGraph(CSemGraph& G) const
{
	G = CSemGraph();
	G.m_ClauseProperties = m_ClausePropertiesProtocol;
	G.m_TreeVariantProperties = m_WeightProtocol;
	G.m_Statistics = m_TimeStatictics;
	G.m_VariantsStatistics = m_ClauseVariantsStatistics;

	for (size_t i = 0; i < GetNodesSize(); i++)
	{
		const CSemNode& N = GetNode(i);
		CSemGraphNode V;
		V.m_Label = GetNodeStr(i, 30);
		V.m_DictType = N.GetType();
		V.m_UnitNo = N.GetUnitNo();

		if (N.m_SynGroupTypeStr != KEYB_STR)
		{
			std::string Q = GetMorphologyOfNode(i);
			if (N.IsWordContainer())
				Q += Format("\nMinWordNo = %i", N.GetMinWordNo());
			if (N.IsPrimitive() && !N.GetWord(0).m_WordEquals.empty())
			{
				Q += "\nCopies:";
				for (auto& e : N.GetWord(0).m_WordEquals)
					Q += "\n" + e;
			}
			if (!N.m_SynGroupTypeStr.empty())
				Q += "\nFixed group = " + N.m_SynGroupTypeStr;
			AddAttr(V.m_Attrs, "Morphology", Q);
		}
		if (N.IsPrimitive())
			AddAttr(V.m_Attrs, "Lemma", N.GetWord(0).m_Lemma);
		AddAttr(V.m_Attrs, "Dictionary interpretation", join_string(GetNodeDictInterps(i), "\n"));

		std::string AllVals;
		for (auto& v : N.m_Vals)
		{
			if (v.m_bObligatory)
				AllVals += "! ";
			if (v.m_bOptional)
				AllVals += "? ";
			AllVals += v.m_RelationStr;
			AllVals += (v.m_Direction == A_C) ? " A,C" : " C,A";
			if (v.IsFromDict() && v.m_UnitNo != ErrUnitNo)
				AllVals += " (from " + v.m_RossHolder->GetRoss()->GetEntryStr(v.m_UnitNo) + ")";
			if (v.m_IsAddition)
				AllVals += " (added)";
			AllVals += "\n";
		}
		AddAttr(V.m_Attrs, "Valencies", AllVals);
		AddAttr(V.m_Attrs, "Prepositions", SynRealToStr(N.m_SynReal, "\n"));

		std::string SemFets = std::string("CAT = ") + GetStrByCategory(N.m_SemCategory) + "\n";
		SemFets += "SF = " + GetSemFetsInOneStr(N.m_NodeSemFets);
		if (!N.m_POs.empty())
			SemFets += "\nDOMAIN = " + JoinLines(N.m_POs);
		AddAttr(V.m_Attrs, "Semantic features", SemFets);
		AddAttr(V.m_Attrs, "Higher concepts", JoinLines(N.m_HigherConcepts));

		std::string Props;
		if (N.m_bQuoteMarks)
			Props += "Quote marks\n";
		if (!N.m_ConnectedSits.empty())
		{
			Props += "Connected situations:\n";
			for (auto& s : N.m_ConnectedSits)
				Props += s.m_UnitStr + "\n";
		}
		AddAttr(V.m_Attrs, "Other properties", Props);

		// the last color wins
		if ((N.GetType() == LocRoss)
			|| ((N.m_Colloc.m_CollocSource == ThesType) && (N.m_Colloc.GetThesInterp().m_ThesaurusId == LocThes)))
			V.m_Color = "lightgreen";
		if ((N.GetType() == CollocRoss) || (N.GetType() == EngCollocRoss))
			V.m_Color = "lightgray";
		if (N.IsTimeRossNode())
			V.m_Color = "cyan";
		if (N.IsThesNode())
			V.m_Color = "bisque";
		if (N.m_bProper)
			V.m_Color = "yellow";
		G.m_Nodes.push_back(V);
	}

	for (size_t i = 0; i < GetRelationsSize(); i++)
	{
		const CSemRelation* R = GetRelation(i);
		if (R->m_bDopRelation)
			continue;
		// relations between the same nodes are shown as one edge
		std::vector<long> Rels;
		FindRelations(R->m_SourceNodeNo, R->m_TargetNodeNo, Rels);
		if (Rels[0] < (long)i)
			continue;

		CSemGraphEdge E;
		E.m_SourceNodeNo = R->m_SourceNodeNo;
		E.m_TargetNodeNo = R->m_TargetNodeNo;
		for (auto r : Rels)
		{
			if (!E.m_Label.empty())
				E.m_Label += " ";
			E.m_Label += GetRelation(r)->m_Valency.m_RelationStr;
			std::string s = SynRealToStr(GetRelation(r)->m_SynReal, " ");
			if (!s.empty())
				E.m_Label += " " + s;
			long g = FindLexFunctBySemRel(r);
			if (g != -1)
				E.m_Label += "(" + GetLexFuncts()[g].m_LexFunctName + ")";
		}

		std::string Props;
		bool fromArticle = R->m_Valency.IsFromDict() && R->m_Valency.m_UnitNo != ErrUnitNo;
		if (fromArticle)
			Props += "Article = " + R->m_Valency.m_RossHolder->GetRoss()->GetEntryStr(R->m_Valency.m_UnitNo) + "\n";
		BYTE LeafId = R->m_Valency.m_LeafId;
		std::string LeafIdStr = (LeafId < 32) ? Format("%i", LeafId) : Format("%i(%i)", LeafId & 31, LeafId >> 5);
		Props += "Actant = A" + LeafIdStr + "\n";
		if (fromArticle && !R->m_SynReal.m_Cortege.is_null(0))
			Props += "GF" + LeafIdStr + " = " + R->m_Valency.m_RossHolder->GetRoss()->WriteToString(R->m_SynReal.m_Cortege) + "\n";
		if (!R->m_SyntacticRelation.empty())
			Props += "SynR = " + R->m_SyntacticRelation + "\n";
		if (!R->m_Position.empty())
		{
			Props += "Positions = " + R->m_Position + "\n";
			if (R->m_PosType == FromAlgorithmPosType)
				Props += "Position type = Algorithm\n";
			else if (R->m_PosType == FromArticlePosType)
				Props += "Position type = Article\n";
		}
		if (R->m_bReverseRel)
			Props += "Reverse relation\n";
		if (R->m_Valency.m_bOptional)
			Props += "Optional valency\n";
		AddAttr(E.m_Attrs, "Properties", Props);
		AddAttr(E.m_Attrs, "Semantic features", GetSemFetsInOneStr(R->m_SemFets));
		AddAttr(E.m_Attrs, "Lexical features", JoinLines(R->m_LexFets));
		GetColorAndWidthOfRelation(i, E.m_Width, E.m_Color);
		G.m_Edges.push_back(E);
	}

	// lexical functions and additional relations that do not coincide with main relations
	for (auto& l : GetLexFuncts())
		if ((l.m_SourceNodeNo != l.m_TargetNodeNo)
			&& (FindFirstRelation(l.m_SourceNodeNo, l.m_TargetNodeNo) == -1)
			&& (FindFirstRelation(l.m_TargetNodeNo, l.m_SourceNodeNo) == -1))
		{
			CSemGraphEdge E;
			E.m_SourceNodeNo = l.m_SourceNodeNo;
			E.m_TargetNodeNo = l.m_TargetNodeNo;
			E.m_Label = l.m_LexFunctName;
			E.m_Color = "blue";
			E.m_bAuxiliary = true;
			E.m_Attrs.push_back({ "Properties", "Lexical function" });
			G.m_Edges.push_back(E);
		}

	for (long i = 0; i < GetDopRelationsSize(); i++)
	{
		const CSemRelation* R = GetDopRelation(i);
		if ((R->m_SourceNodeNo != R->m_TargetNodeNo)
			&& (FindFirstRelation(R->m_SourceNodeNo, R->m_TargetNodeNo) == -1)
			&& (FindFirstRelation(R->m_TargetNodeNo, R->m_SourceNodeNo) == -1))
		{
			CSemGraphEdge E;
			E.m_SourceNodeNo = R->m_SourceNodeNo;
			E.m_TargetNodeNo = R->m_TargetNodeNo;
			E.m_Label = R->m_Valency.m_RelationStr;
			E.m_Color = "blue";
			E.m_bAuxiliary = true;
			E.m_Attrs.push_back({ "Properties", "Additional relation" });
			AddAttr(E.m_Attrs, "Semantic features", GetSemFetsInOneStr(R->m_SemFets));
			G.m_Edges.push_back(E);
		}
	}
}

void CSemanticStructure::GetSyntaxGraph(CSemGraph& G) const
{
	G = CSemGraph();
	G.m_ClauseProperties = m_ClausePropertiesProtocol;
	G.m_TreeVariantProperties = m_WeightProtocol;
	G.m_Statistics = m_TimeStatictics;
	G.m_VariantsStatistics = m_ClauseVariantsStatistics;
	for (size_t i = 0; i < GetNodesSize(); i++)
	{
		CSemGraphNode V;
		V.m_Label = GetNodeStr(i, 30);
		AddAttr(V.m_Attrs, "Grammems", m_pData->GetRusGramTab()->GrammemsToStr(GetNode(i).GetGrammems()));
		AddAttr(V.m_Attrs, "Lemmas", GetNodeLemStr(i));
		G.m_Nodes.push_back(V);
	}
	for (auto& r : GetSynRels())
	{
		CSemGraphEdge E;
		E.m_SourceNodeNo = r.m_SourceNodeNo;
		E.m_TargetNodeNo = r.m_TargetNodeNo;
		E.m_Label = r.m_SynRelName;
		E.m_Color = !r.m_SynRelName.empty() ? "blue" : "darkgreen";
		G.m_Edges.push_back(E);
	}
}

void CRusSemStructure::GetClauseGraph(CSemGraph& G) const
{
	G.m_ClauseNodes.clear();
	G.m_ClauseEdges.clear();
	for (long i = 0; i < m_Clauses.size(); i++)
	{
		const CRusSemClause& C = m_Clauses[i];
		std::string name;
		if (IsEmptyClause(i))
			name = "_empty_";
		else if (C.m_BeginNodeNo == C.m_EndNodeNo - 1)
			name = GetNodeStr(C.m_BeginNodeNo, 20);
		else if (C.m_BeginNodeNo + 6 >= C.m_EndNodeNo)
		{
			for (long k = C.m_BeginNodeNo; k < C.m_EndNodeNo; k++)
				name += GetNodeStr(k, 20) + " ";
		}
		else
		{
			name += GetNodeStr(C.m_BeginNodeNo, 20) + " ";
			name += GetNodeStr(C.m_BeginNodeNo + 1, 20) + " .... ";
			name += GetNodeStr(C.m_EndNodeNo - 2, 20) + " ";
			name += GetNodeStr(C.m_EndNodeNo - 1, 20);
		}
		if (C.m_bQuestion)
			name += "?";
		CSemGraphNode V;
		V.m_Label = name;
		AddAttr(V.m_Attrs, "Properties", GetClauseProperiesStr(i));
		G.m_ClauseNodes.push_back(V);
	}
	for (long i = 0; i < m_Clauses.size(); i++)
		if (m_Clauses[i].m_HostClause != -1)
		{
			CSemGraphEdge E;
			E.m_SourceNodeNo = m_Clauses[i].m_HostClause;
			E.m_TargetNodeNo = i;
			if (m_Clauses[i].m_ClauseRuleNo != -1)
				E.m_Label = m_ClauseRules[m_Clauses[i].m_ClauseRuleNo].m_Name;
			E.m_Color = "darkgreen";
			G.m_ClauseEdges.push_back(E);
		}
}
