#include "VisualGraph.h"
#include <assert.h>

const size_t NodeHeight = 60;
const size_t NodeWidth = 100;


CVisualSemGraph::CVisualSemGraph()
{

}

void CVisualSemGraph::InitFromSemantics (const CSemStructureBuilder& SemBuilder)
{
	m_Nodes.clear();
	m_Relations.clear();
	for (size_t  i=0; i < SemBuilder.m_RusStr.m_Nodes.size(); i++)
	{
		CVisualNode N;
		N.m_Label = SemBuilder.m_RusStr.GetNodeStr(i, 30);
		N.m_x = -1;
		N.m_y = -1;
		N.m_Morphology = SemBuilder.m_RusStr.GetMorphologyOfNode(i);
		m_Nodes.push_back(N);
	}

	for (size_t  i=0; i < SemBuilder.m_RusStr.m_Relations.size(); i++)
	{
		CVisualRelation R;
		R.m_SourceNodeNo = SemBuilder.m_RusStr.m_Relations[i].m_SourceNodeNo;
		R.m_TargetNodeNo = SemBuilder.m_RusStr.m_Relations[i].m_TargetNodeNo;
		R.m_RelationStr = SemBuilder.m_RusStr.m_Relations[i].m_Valency.m_RelationStr;
		if (SemBuilder.m_RusStr.m_Relations[i].m_bDopRelation)
			m_DopRelations.push_back(R);
		else
			m_Relations.push_back(R);
	}
}
vector<int> CVisualSemGraph::GetChildren(int NodeNo) const 
{
	vector<int> Nodes;
	for (size_t i=0;  i <m_Relations.size(); i++)
		if (m_Relations[i].m_SourceNodeNo == NodeNo)
			Nodes.push_back(m_Relations[i].m_TargetNodeNo);
	return Nodes;
}

vector<int> CVisualSemGraph::GetParents(int NodeNo) const 
{
	vector<int> Nodes;
	for (size_t i=0;  i <m_Relations.size(); i++)
		if (m_Relations[i].m_TargetNodeNo == NodeNo)
			Nodes.push_back(m_Relations[i].m_SourceNodeNo);
	return Nodes;
}




string CVisualSemGraph::GetResultStr() const
{
	/*
	set Result "nodes"
 set i 0
 foreach node [$graph nodes] {
   set x [$graph get $node -x]
   set y [$graph get $node -y]
   set label [$graph get $node -label]
   set morph [$graph get $node .morphology]
   set Result "$Result#(#x $x#y $y#label $label#Morphology $morph#)"
   $graph set $node  .node_id $i
   incr i 1
 };

 set Result "$Result#edges"
 foreach edge [$graph edges] {
 	    set source [$graph get $edge -source]
	    set target [$graph get $edge -target]
	    set source_id [$graph get $source .node_id]
        set target_id [$graph get $target .node_id]
		set label [$graph get $edge -label]
		set Result "$Result#(#source_id $source_id#target_id $target_id#label $label#)"

 }
 set Result "$Result#"
 */

	string Result = "nodes";
	for(int i=0; i < m_Nodes.size(); i++)
	{
		const CVisualNode& N =  m_Nodes[i];
		Result += Format("#(#x %i#y %i#label %s#Morphology %s#)",N.m_x+20, N.m_y, N.m_Label.c_str(), N.m_Morphology.c_str());
	}
	Result += "#edges";
	for(int i=0; i < m_Relations.size(); i++)
	{
		const CVisualRelation& R =  m_Relations[i];
		Result += Format("#(#source_id %i#target_id %i#label %s#)",R.m_SourceNodeNo, R.m_TargetNodeNo, R.m_RelationStr.c_str());
	}
	for(int i=0; i < m_DopRelations.size(); i++)
	{
		const CVisualRelation& R =  m_DopRelations[i];
		Result += Format("#(#source_id %i#target_id %i#label %s#)",R.m_SourceNodeNo, R.m_TargetNodeNo, R.m_RelationStr.c_str());
	}

	Result += "#";
	return Result;
}
int CVisualSemGraph::GetLeavesCount (int Root) const
{
	vector<int> Children = GetChildren(Root);
	if (Children.empty()) return 1;
	int Result = 0;
	for (size_t i=0; i <Children.size(); i++)
		Result += GetLeavesCount(Children[i]);
	return Result;
}

vector<int>  CVisualSemGraph::FindTreeRoots()   const
{
	vector<int> Parents(m_Nodes.size(),0);
	for (size_t i=0; i < m_Nodes.size(); i++)
		Parents[i] = GetParents(i).size();

	vector<int> Roots; 
	
	for (size_t i=0; i < m_Nodes.size(); i++)
	{
		if (Parents[i] == 0) 
		{
			Roots.push_back(i);
		}
		else
			if (Parents[i] != 1) 
			{
				Roots.clear();
				break;
			}
	}

	return Roots;
}



int CVisualSemGraph::SetTreeLayout (int Root, int Left, int Top)
{
	int LeavesCount = GetLeavesCount(Root);

	size_t  TreeWidth = LeavesCount*NodeWidth;
	m_Nodes[Root].m_y = Top;
	m_Nodes[Root].m_x = Left+TreeWidth/2;
	vector<int> Children = GetChildren(Root);
	int Bottom = Top+NodeHeight;
	int CurrLeavesCount = 0;
	for (size_t i=0; i <Children.size(); i++)
	{
		int c = SetTreeLayout(Children[i], Left+CurrLeavesCount*NodeWidth,Top+NodeHeight);
		CurrLeavesCount += GetLeavesCount(Children[i]);
		if (c > Bottom)
			Bottom = c;
	}
	return Bottom;
}

void CVisualSemGraph::SetGraphLayout()   
{
	{
		vector<int> Roots = FindTreeRoots();
		if (!Roots.empty())
		{
			int Bottom = NodeHeight;
			for (size_t i=0;  i < Roots.size(); i++)
			{
				Bottom = SetTreeLayout(Roots[i], NodeWidth, Bottom);
			}
		}
	}

}

