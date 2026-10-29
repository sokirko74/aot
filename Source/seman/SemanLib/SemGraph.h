#pragma once

#include "dict_type.h"
#include "dicts/StructDictLib/StructDictConsts.h"

#include <string>
#include <vector>

// a graph of a semantic (or syntax) structure for a viewer, it replaces the Tcl representation (sem_tcl.cpp)

struct CSemGraphAttr {
	// "Morphology", "Valencies"...
	std::string m_Title;
	std::string m_Value;
};

struct CSemGraphNode {
	std::string m_Label;
	// a background color (a name from the SVG color list), empty for the default
	std::string m_Color;
	// the dictionary article of the node
	DictTypeEnum m_DictType = NoneRoss;
	uint16_t m_UnitNo = ErrUnitNo;
	std::vector<CSemGraphAttr> m_Attrs;
};

struct CSemGraphEdge {
	size_t m_SourceNodeNo;
	size_t m_TargetNodeNo;
	std::string m_Label;
	std::string m_Color;
	float m_Width = 1;
	// relations that are not a part of the tree (lexical functions, additional relations)
	bool m_bAuxiliary = false;
	std::vector<CSemGraphAttr> m_Attrs;
};

struct CSemGraph {
	std::vector<CSemGraphNode> m_Nodes;
	std::vector<CSemGraphEdge> m_Edges;
	// the tree of clauses (only for the Russian structure)
	std::vector<CSemGraphNode> m_ClauseNodes;
	std::vector<CSemGraphEdge> m_ClauseEdges;
	// protocols of the analysis
	std::string m_ClauseProperties;
	std::string m_TreeVariantProperties;
	std::string m_Statistics;
	std::string m_VariantsStatistics;
};
