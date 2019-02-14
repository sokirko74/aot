#ifndef _INCL_VISUALGRAPH_H_
#define _INCL_VISUALGRAPH_H_

#include "../SemanLib/SemStructureBuilder.h"

struct CVisualNode 
{
	int		m_x;
	int		m_y;
	string	m_Label;
	string	m_Morphology;
};

struct CVisualRelation 
{
	size_t	m_SourceNodeNo;
	size_t	m_TargetNodeNo;
	string	m_RelationStr;
};

struct CVisualSemGraph {

	vector<CVisualNode>		m_Nodes;
	vector<CVisualRelation> m_Relations;
	vector<CVisualRelation> m_DopRelations;

	CVisualSemGraph();
	void InitFromSemantics (const CSemStructureBuilder& SemBuilder);
	vector<int> GetChildren(int NodeNo) const ;
	vector<int> GetParents(int NodeNo) const ;
	vector<int> FindTreeRoots() const;
	int SetTreeLayout (int Root, int Left, int Top);
	int GetLeavesCount (int Root) const;
	void  SetGraphLayout();
	string GetResultStr() const;
};

#endif
