#include "stdafx.h"
#include "SemanticRusStructure.h"

void	CRusSemStructure::GetColorAndWidthOfRelation(int RelNo, float& Width, std::string& Color) const
{
	Width = 1;
	Color = m_Relations[RelNo].m_bSemFetAgree ? "blue" : "darkgreen";
}

static std::string GetStringVectorInOneStr(const StringVector& Vec, std::string Delimiter)
{
	std::string Result;
	for (long i = 0; i < Vec.size(); i++)
	{
		Result += Vec[i];
		Result += Delimiter;
	};
	return Result;
};


std::string CSemanticStructure::GetBaseTclGraph(bool ShowUnusedValencies, bool UseIsTop) const
{
	try {
		// уничтожение графа 
		std::string Res = "";
		Res += "$GT($main,graph) delete nodes\1";

		Res += Format("$main.switches.resultText configure -text \"%s\"\1", m_ClausePropertiesProtocol.c_str());
		Res += Format("set GT($main,tree_variant_props) \"%s\"\1", m_WeightProtocol.c_str());
		Res += Format("set GT($main,statistics) \"%s\"\1", m_TimeStatictics.c_str());
		size_t debug = m_ClauseVariantsStatistics.length();
		Res += Format("set GT($main,variants_statistics) \"%s\"\1", m_ClauseVariantsStatistics.c_str());
		Res += "set values {\"Orthogonal\" \"Next middle\" \"Next corner\" \"Explicit\"}\1";


		for (size_t i = 0; i < GetNodesSize(); i++)
		{
			Res += Format("set nds(%i) [$GT($main,graph) create node]\1", i);;
			auto nodeStr = GetNodeStr(i, 30);
			Res += Format("$GT($main,graph) set $nds(%i) -label \"%s\" -type rectangle -x 0 -y 0\1", i, nodeStr.c_str());
			Res += Format("foreach p [$GT($main,graph) get $nds(%i) -ports] {lappend values [lindex $p 0]}\1", i);

			if (GetNode(i).m_SynGroupTypeStr != KEYB_STR)
			{
				std::string Q = GetMorphologyOfNode(i);
				if (GetNode(i).IsWordContainer())
					Q += Format("\nMinWordNo = %i", GetNode(i).GetMinWordNo());

				if (GetNode(i).IsPrimitive())
					if (!GetNode(i).GetWord(0).m_WordEquals.empty())
					{
						Q += "\n Copies:";
						for (long j = 0; j < GetNode(i).GetWord(0).m_WordEquals.size(); j++)
							Q += Format("\n%s", GetNode(i).GetWord(0).m_WordEquals[j].c_str());

					};

				if (GetNode(i).m_SynGroupTypeStr != "")
				{
					Q += Format("\nFixed group  = %s", GetNode(i).m_SynGroupTypeStr.c_str());
				};
				;
				Res += Format("$GT($main,graph) set $nds(%i)  .morph_dict \"%s\"\1", i, Q.c_str());
			};

			std::string ss = SynRealToStr(GetNode(i).m_SynReal, "\n");
			if (!ss.empty())
				Res += Format("$GT($main,graph) set $nds(%i)  .preps \"%s\"\1", i, ss.c_str());

			if ((GetNode(i).GetType() == LocRoss)
				|| ((GetNode(i).m_Colloc.m_CollocSource == ThesType)
					&& (GetNode(i).m_Colloc.GetThesInterp().m_ThesaurusId == LocThes)
					)
				)
			{

				Res += Format("$GT($main,graph) set $nds(%i)  -fill green\1", i);
			};

			if ((GetNode(i).GetType() == CollocRoss) || (GetNode(i).GetType() == EngCollocRoss))
			{
				Res += Format("$GT($main,graph) set $nds(%i)  -fill snow4\1", i);
			};

			if (GetNode(i).IsTimeRossNode())
			{
				Res += Format("$GT($main,graph) set $nds(%i)  -fill \"cyan\"\1", i);
			};


			if (GetNode(i).IsThesNode())
			{
				Res += Format("$GT($main,graph) set $nds(%i)  -fill \"bisque1\"\1", i);
			};

			if (GetNode(i).m_bProper)
			{
				Res += Format("$GT($main,graph) set $nds(%i)  -fill \"yellow\"\1", i);
			};


			Res += Format("$GT($main,graph) set $nds(%i)  .dict_interp \"%s\"\1", i, join_string(GetNodeDictInterps(i), "\n").c_str());

			if (GetNode(i).IsPrimitive())
			{
				Res += Format("$GT($main,graph) set $nds(%i)  .lemma \"%s\"\1", i, GetNode(i).GetWord(0).m_Lemma.c_str());
			};

			std::string SemFets = std::string("CAT = ") + GetStrByCategory(GetNode(i).m_SemCategory) + std::string("\n");
			SemFets += "SF = " + GetSemFetsInOneStr(GetNode(i).m_NodeSemFets);
			if (GetNode(i).m_POs.size() > 0)
			{
				SemFets += "\nDOMAIN = ";
				for (long j = 0; j < GetNode(i).m_POs.size(); j++)
					SemFets += GetNode(i).m_POs[j] + "\n";
			};


			;
			Res += Format("$GT($main,graph) set $nds(%i)  .sem_fets \"%s\"\1", i, SemFets.c_str());

			std::string Props;
			if (GetNode(i).m_bQuoteMarks)
				Props += "Quote marks\n";
			if (GetNode(i).m_ConnectedSits.size() > 0)
			{
				Props += "Connected Situations:";
				for (long j = 0; j < GetNode(i).m_ConnectedSits.size(); j++)
					Props += Format("%s\n", GetNode(i).m_ConnectedSits[j].m_UnitStr.c_str());
			};
			Res += Format("$GT($main,graph) set $nds(%i)  .props \"%s\"\1", i, Props.c_str());
			Res += Format("$GT($main,graph) set $nds(%i)  .type_dict \"%i\"\1", i, GetNode(i).GetType());
			Res += Format("$GT($main,graph) set $nds(%i)  .unit_no \"%i\"\1", i, GetNode(i).GetUnitNo());
			Res += Format("$GT($main,graph) set $nds(%i)  .higher_concepts \"%s\"\1", i, GetStringVectorInOneStr(GetNode(i).m_HigherConcepts, "\n").c_str());

			std::string AllVals;
			for (size_t k = 0; k < GetNode(i).m_Vals.size(); k++)
			{
				if (GetNode(i).m_Vals[k].m_bObligatory)
					AllVals += " ! ";
				if (GetNode(i).m_Vals[k].m_bOptional)
					AllVals += " ? ";
				AllVals += GetNode(i).m_Vals[k].m_RelationStr;
				AllVals += " ";
				AllVals += (GetNode(i).m_Vals[k].m_Direction == A_C) ? "A,C" : "C,A";
				if (GetNode(i).m_Vals[k].IsFromDict())
					if (GetNode(i).m_Vals[k].m_UnitNo != ErrUnitNo)
					{
						AllVals += " (from ";;
						AllVals += GetNode(i).m_Vals[k].m_RossHolder->GetRoss()->GetEntryStr(GetNode(i).m_Vals[k].m_UnitNo);
						AllVals += " )";
					};
				if (GetNode(i).m_Vals[k].m_IsAddition)
				{
					AllVals += " (added)";
				};
				AllVals += "\n";
			};
			Res += Format("$GT($main,graph) set $nds(%i)  .vals \"%s\"\1", i, AllVals.c_str());

		};



		for (size_t i = 0; i < GetRelationsSize(); i++)
		{
			if (GetRelation(i)->m_bDopRelation) {
				continue;
			}
			std::vector <long> Rels;
			FindRelations(GetRelation(i)->m_SourceNodeNo, GetRelation(i)->m_TargetNodeNo, Rels);
			if (Rels[0] < i) continue;

			size_t FirstNodeNo = GetRelation(i)->m_SourceNodeNo;
			size_t SecondNodeNo = GetRelation(i)->m_TargetNodeNo;
			Res += Format("set edge [$GT($main,graph) create edge $nds(%i) $nds(%i)]\1", FirstNodeNo, SecondNodeNo);


			std::string Label;
			for (size_t k = 0; k < Rels.size(); k++)
			{
				Label += GetRelation(Rels[k])->m_Valency.m_RelationStr;
				Label += " ";

				Label += SynRealToStr(GetRelation(Rels[k])->m_SynReal, "\n");
				long g = FindLexFunctBySemRel(Rels[k]);
				if (g != -1)
					Label += std::string("(") + GetLexFuncts()[g].m_LexFunctName + std::string(")");
			};

			//
			Res += Format("$GT($main,graph) set $edge -label \"%s\"\1", Label.c_str());

			std::string Props;
			if (GetRelation(i)->m_Valency.IsFromDict())
				if (GetRelation(i)->m_Valency.m_UnitNo != ErrUnitNo)
				{
					Props += "Article = ";
					Props += GetRelation(i)->m_Valency.m_RossHolder->GetRoss()->GetEntryStr(GetRelation(i)->m_Valency.m_UnitNo);
					Props += "\n";
				};

			std::string LeafIdStr;
			BYTE LeafId = GetRelation(i)->m_Valency.m_LeafId;
			if (LeafId < 32)
				LeafIdStr = Format("%i", LeafId);
			else
				LeafIdStr = Format("%i(%i)", LeafId & 31, LeafId >> 5);

			Props += "Actant = ";
			Props += "A";
			Props += LeafIdStr;
			Props += "\n";

			if (GetRelation(i)->m_Valency.IsFromDict() && (GetRelation(i)->m_Valency.m_UnitNo != ErrUnitNo))
				if (!GetRelation(i)->m_SynReal.m_Cortege.is_null(0))
				{
					TCortege C = GetRelation(i)->m_SynReal.m_Cortege;
					const CStructDictHolder* RossHolder = GetRelation(i)->m_Valency.m_RossHolder;
					std::string GramFet = RossHolder->GetRoss()->WriteToString(C);
					Props += "GF";
					Props += LeafIdStr;
					Props += " = ";
					Props += GramFet;
					Props += "\n";
				};
			if (!GetRelation(i)->m_SyntacticRelation.empty())
			{
				Props += " SynR = " + GetRelation(i)->m_SyntacticRelation;
				Props += "\n";
			};
			//

			if (GetRelation(i)->m_Position != "")
			{
				Props += " Postions = " + GetRelation(i)->m_Position;
				Props += "\n";

				Props += " Position type = ";

				if (GetRelation(i)->m_PosType == FromAlgorithmPosType)
					Props += "Algorithm";
				else
					if (GetRelation(i)->m_PosType == FromArticlePosType)
						Props += "Article";

				Props += "\n";
			};

			if (GetRelation(i)->m_bReverseRel)
			{
				Props += "Reverse Relation\n";
			};
			if (GetRelation(i)->m_Valency.m_bOptional)
			{
				Props += "Optional Valency\n";
			};


			Res += Format("$GT($main,graph) set $edge .props \"%s\"\1", Props.c_str());

			//
			Res += Format("$GT($main,graph) set $edge .sem_fets \"%s\"\1", GetSemFetsInOneStr(GetRelation(i)->m_SemFets).c_str());

			;
			Res += Format("$GT($main,graph) set $edge .lex_fets \"%s\"\1", GetStringVectorInOneStr(GetRelation(i)->m_LexFets, "\n").c_str());


			//
			std::string Color;
			float width;
			GetColorAndWidthOfRelation(i, width, Color);
			Res += Format("$GT($main,graph) configure $edge graphics -fill %s -width %f\1", Color.c_str(), width);

		};

		return Res;
	}
	catch (...)
	{
		ErrorMessage("GetTclGraph failed");
		throw;
	};
};

std::string    CRusSemStructure::GetClauseProperiesStr(long ClauseNo) const
{
	std::string Res = "Свойства:\n";
	if (m_Clauses[ClauseNo].m_bQuestion)
	{
		Res += "Вопросительная клауза\n";
	};
	return Res;
};

std::string    CRusSemStructure::GetClauseTreeForTcl() const
{
	//установка дерева клауз
	std::string Res = "$GT($main,clause_graph) delete nodes\1";

	for (long i = 0; i < m_Clauses.size(); i++)
	{
		Res += Format("set clause_nds(%i) [$GT($main,clause_graph) create node]\1", i);
		std::string name;
		if (IsEmptyClause(i))
			name = "_empty_";
		else
			if (m_Clauses[i].m_BeginNodeNo == m_Clauses[i].m_EndNodeNo - 1)
				name = GetNodeStr(m_Clauses[i].m_BeginNodeNo, 20);
			else
				if (m_Clauses[i].m_BeginNodeNo + 6 >= m_Clauses[i].m_EndNodeNo)
					for (long k = m_Clauses[i].m_BeginNodeNo; k < m_Clauses[i].m_EndNodeNo; k++)
						name += GetNodeStr(k, 20) + " ";
				else
				{
					name += GetNodeStr(m_Clauses[i].m_BeginNodeNo, 20) + " ";
					name += GetNodeStr(m_Clauses[i].m_BeginNodeNo + 1, 20) + " .... ";
					name += GetNodeStr(m_Clauses[i].m_EndNodeNo - 2, 20) + " ";
					name += GetNodeStr(m_Clauses[i].m_EndNodeNo - 1, 20);
				};

		if (m_Clauses[i].m_bQuestion)
			name += "?";
		name += "         ";
		name = "         " + name;

		Res += Format("$GT($main,clause_graph) set $clause_nds(%i) -label \"%s\" -type oval -x 0 -y 0\1", i, name.c_str());

		Res += Format("$GT($main,clause_graph) set $clause_nds(%i)  .props \"%s\"\1", i, GetClauseProperiesStr(i).c_str());

	};

	for (long i = 0; i < m_Clauses.size(); i++)
		if (m_Clauses[i].m_HostClause != -1)
		{
			Res += Format("set edge [$GT($main,clause_graph) create edge $clause_nds(%i) $clause_nds(%i)]\1", m_Clauses[i].m_HostClause, i);

			if (m_Clauses[i].m_ClauseRuleNo != -1)
			{
				std::string name = m_ClauseRules[m_Clauses[i].m_ClauseRuleNo].m_Name;
				Res += Format("$GT($main,clause_graph) set $edge -label \"%s\"\1", name.c_str());
			};
		};

	return Res;
};


std::string CRusSemStructure::GetTclGraph() const
{
	std::string s;
	s = GetBaseTclGraph(false, true);
	s += "if { [catch SetLayout] != 0 } {  $GT($main,graph) draw  }\1";
	s += GetOtherRelations();
	s += GetClauseTreeForTcl();

	std::ofstream outp;
	outp.open("graph_debug.txt");
	auto d = s;
	std::replace(d.begin(), d.end(), '\1', '\n');
	outp << d;
	outp.close();

	return s;

}

