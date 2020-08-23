#include "VisualGraph.h"
#include "../common/json.h"
#include <assert.h>

const size_t NodeHeight = 60;
const size_t NodeWidth = 100;
const size_t NodeSpan = 5;


CVisualSemGraph::CVisualSemGraph() {

}

void CVisualSemGraph::InitFromSemantics(const CSemStructureBuilder &SemBuilder) {
    m_Nodes.clear();
    m_Relations.clear();
    for (size_t i = 0; i < SemBuilder.m_RusStr.m_Nodes.size(); i++) {
        CVisualNode N;
        N.m_Label = SemBuilder.m_RusStr.GetNodeStr(i, 30);
        N.m_x = -1;
        N.m_y = -1;
        N.m_Morphology = SemBuilder.m_RusStr.GetMorphologyOfNode(i);
        m_Nodes.push_back(N);
    }

    for (size_t i = 0; i < SemBuilder.m_RusStr.m_Relations.size(); i++) {
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

std::vector<int> CVisualSemGraph::GetChildren(int NodeNo) const {
    std::vector<int> Nodes;
    for (const auto &r : m_Relations) {
        if (r.m_SourceNodeNo == NodeNo)
            Nodes.push_back(r.m_TargetNodeNo);
    }
    return Nodes;
}

std::vector<int> CVisualSemGraph::GetParents(int NodeNo) const {
    std::vector<int> Nodes;
    for (const auto &r : m_Relations) {
        if (r.m_TargetNodeNo == NodeNo)
            Nodes.push_back(r.m_SourceNodeNo);
    }
    return Nodes;
}

std::string CVisualSemGraph::GetResultStr() const {
    auto result = nlohmann::json::object();
    result["nodes"] = nlohmann::json::array();
    for (const auto &n : m_Nodes) {
        result["nodes"].push_back({
                                          {"x",     n.m_x + 20},
                                          {"y",     n.m_y + 20},
                                          {"label", n.m_Label},
                                          {"morph", n.m_Morphology}
                                  });
    }
    result["edges"] = nlohmann::json::array();
    for (const auto &r : m_Relations) {
        result["edges"].push_back({
                                          {"source", r.m_SourceNodeNo},
                                          {"target", r.m_TargetNodeNo},
                                          {"label",  r.m_RelationStr}
                                  });
    }
    for (const auto &r : m_DopRelations) {
        result["edges"].push_back({
                                          {"source", r.m_SourceNodeNo},
                                          {"target", r.m_TargetNodeNo},
                                          {"label",  r.m_RelationStr}
                                  });
    }
    ConvertToUtfRecursive(result, morphRussian);
    return result.dump();
}

int CVisualSemGraph::GetLeavesCount(int Root) const {
    std::vector<int> Children = GetChildren(Root);
    if (Children.empty()) return 1;
    int Result = 0;
    for (size_t i = 0; i < Children.size(); i++)
        Result += GetLeavesCount(Children[i]);
    return Result;
}


int CVisualSemGraph::SetTreeLayout(int Root, int Left, int Top) {
    int LeavesCount = GetLeavesCount(Root);

    size_t TreeWidth = LeavesCount * (NodeWidth + NodeSpan);
    if (m_Nodes[Root].m_y == -1) {
        m_Nodes[Root].m_y = Top;
        m_Nodes[Root].m_x = Left + TreeWidth / 2;
    }
    int Bottom = Top + NodeHeight;
    int CurrLeavesCount = 0;
    for (const auto &c : GetChildren(Root)) {
        int b = SetTreeLayout(c, Left + CurrLeavesCount * (NodeWidth + NodeSpan), Top + NodeHeight);
        CurrLeavesCount += GetLeavesCount(c);
        if (b > Bottom)
            Bottom = b;
    }
    return Bottom;
}

void CVisualSemGraph::SetGraphLayout() {
    for (auto& n : m_Nodes) {
        n.m_y = -1;
        n.m_x = -1;
    }
    int Bottom = NodeHeight;
    for (size_t i = 0; i < m_Nodes.size(); i++) {
        if (GetParents(i).empty()) {
            Bottom = SetTreeLayout(i, NodeWidth, Bottom);
        }
    }
}

