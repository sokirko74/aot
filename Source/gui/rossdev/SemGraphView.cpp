#include "SemGraphView.h"
#include "RossDictionary.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <functional>

enum ItemKind { NodeKind = 1, EdgeKind = 2 };
// keys of QGraphicsItem::data
const int KindKey = 0;
const int NoKey = 1;

class GraphEdgeItem;

class GraphNodeItem : public QGraphicsRectItem {
public:
    GraphNodeItem(int no, const CSemGraphNode& node) {
        setData(KindKey, NodeKind);
        setData(NoKey, no);
        auto text = new QGraphicsSimpleTextItem(fromUtf8(node.m_Label), this);
        QRectF r = text->boundingRect();
        setRect(0, 0, r.width() + 16, r.height() + 10);
        text->setPos(8, 5);
        setBrush(node.m_Color.empty() ? QColor(Qt::white) : QColor(QString::fromStdString(node.m_Color)));
        setPen(QPen(Qt::black, 1));
        setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
        setZValue(1);
    }
    std::vector<GraphEdgeItem*> m_Edges;
    QPointF center() const { return pos() + rect().center(); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        setPen(isSelected() ? QPen(Qt::red, 2) : QPen(Qt::black, 1));
        QGraphicsRectItem::paint(painter, option, widget);
    }
};

class GraphEdgeItem : public QGraphicsPathItem {
public:
    GraphEdgeItem(int no, const CSemGraphEdge& edge, GraphNodeItem* source, GraphNodeItem* target)
        : m_Source(source), m_Target(target) {
        setData(KindKey, EdgeKind);
        setData(NoKey, no);
        m_Color = edge.m_Color.empty() ? QColor(Qt::darkGreen) : QColor(QString::fromStdString(edge.m_Color));
        m_Pen = QPen(m_Color, edge.m_Width);
        if (edge.m_bAuxiliary) {
            m_Pen.setStyle(Qt::DashLine);
        }
        m_Label = new QGraphicsSimpleTextItem(fromUtf8(edge.m_Label), this);
        m_Label->setBrush(m_Color);
        setFlags(ItemIsSelectable);
        source->m_Edges.push_back(this);
        target->m_Edges.push_back(this);
        adjust();
    }

    void adjust() {
        QPointF from = border(m_Source, m_Target->center());
        QPointF to = border(m_Target, m_Source->center());
        QPainterPath path(from);
        path.lineTo(to);
        QLineF line(to, from);
        if (line.length() > 1) {
            // an arrow head at the target
            QLineF a = line;
            a.setLength(10);
            a.setAngle(line.angle() + 25);
            QLineF b = line;
            b.setLength(10);
            b.setAngle(line.angle() - 25);
            path.moveTo(a.p2());
            path.lineTo(to);
            path.lineTo(b.p2());
        }
        setPath(path);
        QRectF r = m_Label->boundingRect();
        // closer to the target, since edges from one node start at the same point
        m_Label->setPos(from + (to - from) * 0.65 - QPointF(r.width() / 2, r.height() / 2));
    }

    QPainterPath shape() const override {
        // a wider line is easier to click
        QPainterPathStroker stroker;
        stroker.setWidth(8);
        return stroker.createStroke(path()) + m_Label->mapToParent(m_Label->shape());
    }

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        QPen pen = m_Pen;
        if (isSelected()) {
            pen.setColor(Qt::red);
            pen.setWidthF(pen.widthF() + 1);
        }
        painter->setPen(pen);
        painter->drawPath(path());
    }

private:
    GraphNodeItem* m_Source;
    GraphNodeItem* m_Target;
    QGraphicsSimpleTextItem* m_Label;
    QColor m_Color;
    QPen m_Pen;

    // the point of the node border on the line to the point p
    static QPointF border(const GraphNodeItem* n, QPointF p) {
        QPointF c = n->center();
        QPointF d = p - c;
        double hw = n->rect().width() / 2, hh = n->rect().height() / 2;
        if (std::abs(d.x()) < 1e-6 && std::abs(d.y()) < 1e-6) {
            return c;
        }
        double t = std::min(std::abs(d.x()) > 1e-6 ? hw / std::abs(d.x()) : 1e9, std::abs(d.y()) > 1e-6 ? hh / std::abs(d.y()) : 1e9);
        return c + d * std::min(t, 1.0);
    }
};

QVariant GraphNodeItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        for (auto e : m_Edges) {
            e->adjust();
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

SemGraphView::SemGraphView(QWidget* parent) : QGraphicsView(parent) {
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setBackgroundBrush(Qt::white);
    connect(scene(), &QGraphicsScene::selectionChanged, this, &SemGraphView::onSelectionChanged);
}

void SemGraphView::clearGraph() {
    setGraph({}, {});
}

void SemGraphView::setGraph(const std::vector<CSemGraphNode>& nodes, const std::vector<CSemGraphEdge>& edges) {
    scene()->clear();
    m_NodeItems.clear();
    m_Nodes = nodes;
    m_Edges = edges;
    for (size_t i = 0; i < nodes.size(); i++) {
        auto item = new GraphNodeItem(i, nodes[i]);
        scene()->addItem(item);
        m_NodeItems.push_back(item);
    }
    layoutGraph();
    for (size_t i = 0; i < edges.size(); i++) {
        auto& e = edges[i];
        if (e.m_SourceNodeNo < nodes.size() && e.m_TargetNodeNo < nodes.size() && e.m_SourceNodeNo != e.m_TargetNodeNo) {
            scene()->addItem(new GraphEdgeItem(i, e, m_NodeItems[e.m_SourceNodeNo], m_NodeItems[e.m_TargetNodeNo]));
        }
    }
    scene()->setSceneRect(scene()->itemsBoundingRect().adjusted(-20, -20, 20, 20));
    resetTransform();
}

void SemGraphView::layoutGraph() {
    const double HorSpace = 40;
    const double VerSpace = 70;
    size_t count = m_Nodes.size();
    // the tree by the main relations: the first main relation to a node gives its parent
    std::vector<int> parent(count, -1);
    std::vector<std::vector<int>> children(count);
    auto isAncestor = [&](int a, int n) {
        for (int p = n; p != -1; p = parent[p]) {
            if (p == a) {
                return true;
            }
        }
        return false;
    };
    for (auto& e : m_Edges) {
        int s = e.m_SourceNodeNo, t = e.m_TargetNodeNo;
        if (e.m_bAuxiliary || s == t || s >= (int)count || t >= (int)count || parent[t] != -1 || isAncestor(t, s)) {
            continue;
        }
        parent[t] = s;
        children[s].push_back(t);
    }
    for (auto& c : children) {
        std::sort(c.begin(), c.end());
    }
    std::vector<double> width(count);
    std::function<double(int)> subtreeWidth = [&](int n) {
        double w = 0;
        for (int c : children[n]) {
            w += subtreeWidth(c) + (w > 0 ? HorSpace : 0);
        }
        width[n] = std::max(w, m_NodeItems[n]->rect().width());
        return width[n];
    };
    std::function<void(int, double, double)> place = [&](int n, double left, double top) {
        auto item = m_NodeItems[n];
        item->setPos(left + (width[n] - item->rect().width()) / 2, top);
        double childrenWidth = 0;
        for (int c : children[n]) {
            childrenWidth += width[c] + (childrenWidth > 0 ? HorSpace : 0);
        }
        double x = left + (width[n] - childrenWidth) / 2;
        for (int c : children[n]) {
            place(c, x, top + item->rect().height() + VerSpace);
            x += width[c] + HorSpace;
        }
    };
    double left = 0;
    for (size_t n = 0; n < count; n++) {
        if (parent[n] == -1) {
            subtreeWidth(n);
            place(n, left, 0);
            left += width[n] + 2 * HorSpace;
        }
    }
}

void SemGraphView::onSelectionChanged() {
    auto items = scene()->selectedItems();
    if (items.size() != 1) {
        emit selectionCleared();
        return;
    }
    int no = items[0]->data(NoKey).toInt();
    if (items[0]->data(KindKey).toInt() == NodeKind) {
        emit nodeSelected(no);
    }
    else {
        emit edgeSelected(no);
    }
}

void SemGraphView::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double factor = event->angleDelta().y() > 0 ? 1.15 : 1 / 1.15;
        scale(factor, factor);
        event->accept();
        return;
    }
    QGraphicsView::wheelEvent(event);
}

void SemGraphView::mouseDoubleClickEvent(QMouseEvent* event) {
    for (auto item = itemAt(event->pos()); item; item = item->parentItem()) {
        if (item->data(KindKey).toInt() == NodeKind) {
            emit nodeDoubleClicked(item->data(NoKey).toInt());
            return;
        }
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}
