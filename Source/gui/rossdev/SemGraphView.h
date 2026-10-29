#pragma once

#include "seman/SemanLib/SemGraph.h"

#include <QGraphicsView>

#include <vector>

// draws a semantic graph: nodes can be moved by the mouse, the layout is a tree
// by the main relations (Graphlet in the Tcl version)
class SemGraphView : public QGraphicsView {
    Q_OBJECT
public:
    explicit SemGraphView(QWidget* parent = nullptr);

    void setGraph(const std::vector<CSemGraphNode>& nodes, const std::vector<CSemGraphEdge>& edges);
    void clearGraph();
    // places nodes as a forest
    void layoutGraph();

signals:
    void nodeSelected(int nodeNo);
    void edgeSelected(int edgeNo);
    void selectionCleared();
    void nodeDoubleClicked(int nodeNo);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    std::vector<CSemGraphNode> m_Nodes;
    std::vector<CSemGraphEdge> m_Edges;
    std::vector<class GraphNodeItem*> m_NodeItems;

    void onSelectionChanged();
};
