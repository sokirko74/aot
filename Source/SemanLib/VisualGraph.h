#ifndef _INCL_VISUALGRAPH_H_
#define _INCL_VISUALGRAPH_H_

#include "SemStructureBuilder.h"

struct CVisualNode 
{
	int		m_x;
	int		m_y;
	std::string	m_Label;
	std::string	m_Morphology;
};

struct CVisualRelation 
{
	size_t	m_SourceNodeNo;
	size_t	m_TargetNodeNo;
	std::string	m_RelationStr;
};

struct CVisualSemGraph {

	std::vector<CVisualNode>		m_Nodes;
	std::vector<CVisualRelation> m_Relations;
	std::vector<CVisualRelation> m_DopRelations;

	CVisualSemGraph();
	void InitFromSemantics (const CSemStructureBuilder& SemBuilder);
	std::vector<int> GetChildren(int NodeNo) const ;
	std::vector<int> GetParents(int NodeNo) const ;
	int SetTreeLayout (int Root, int Left, int Top);
	int GetLeavesCount (int Root) const;
	void  SetGraphLayout();
	std::string GetResultStr() const;
};

#endif
