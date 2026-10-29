#include "HierarchyWindow.h"
#include "MainWindow.h"
#include "RossDictionary.h"

#include "dicts/StructDictLib/TempArticle.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <algorithm>
#include <set>

void HierarchyGraph::read(RossDictionary* dict, const CTempArticle& article) {
    m_Nodes.clear();
    m_Relations.clear();
    m_Errors.clear();
    m_Warnings.clear();
    const CDictionary* ross = dict->GetRoss();
    BYTE nodeFieldNo = ross->GetFieldNoByFieldStr("NODE");
    BYTE relFieldNo = ross->GetFieldNoByFieldStr("ISA");
    if (nodeFieldNo == ErrUChar || relFieldNo == ErrUChar) {
        m_Errors << "The dictionary has no fields NODE and ISA";
        return;
    }
    auto itemStr = [dict](const TCortege& c, BYTE i) { return dict->GetRossHolder()->GetDomItemStrWrapper(c.GetItem(i)); };
    for (size_t i = 0; i < article.GetCortegesSize(); i++) {
        const TCortege& c = article.GetCortege(i);
        if (c.m_FieldNo == nodeFieldNo && !c.is_null(0)) {
            m_Nodes.push_back(itemStr(c, 0));
        }
        else if (c.m_FieldNo == relFieldNo && !c.is_null(0) && !c.is_null(1)) {
            m_Relations.emplace_back(itemStr(c, 0), itemStr(c, 1));
        }
    }
    check();
}

void HierarchyGraph::check() {
    std::set<std::string> nodes;
    for (auto& n : m_Nodes) {
        if (!nodes.insert(n).second) {
            m_Errors << QString("NODE %1 is written twice").arg(fromUtf8(n));
        }
    }
    std::set<std::pair<std::string, std::string>> relations;
    std::set<std::string> related;
    for (auto& r : m_Relations) {
        QString rel = QString("ISA %1 %2").arg(fromUtf8(r.first), fromUtf8(r.second));
        for (auto& n : {r.first, r.second}) {
            if (nodes.find(n) == nodes.end()) {
                // CHierarchyHolder ignores such relations
                m_Errors << QString("%1: there is no NODE %2").arg(rel, fromUtf8(n));
            }
        }
        if (!relations.insert(r).second) {
            m_Warnings << QString("%1 is written twice").arg(rel);
        }
        related.insert(r.first);
        related.insert(r.second);
    }

    // a node is on a cycle if it is reachable from itself; the transitive closure would make
    // all nodes of the cycle equal
    std::vector<std::string> cycle;
    for (auto& n : m_Nodes) {
        std::set<std::string> visited;
        std::vector<std::string> stack = getChildren(n);
        bool found = false;
        while (!stack.empty() && !found) {
            std::string cur = stack.back();
            stack.pop_back();
            if (cur == n) {
                found = true;
            }
            else if (visited.insert(cur).second) {
                for (auto& c : getChildren(cur)) {
                    stack.push_back(c);
                }
            }
        }
        if (found) {
            cycle.push_back(n);
        }
    }
    if (!cycle.empty()) {
        QStringList names;
        for (auto& n : cycle) {
            names << fromUtf8(n);
        }
        m_Errors << "The hierarchy has a cycle through " + names.join(", ");
    }

    for (auto& n : m_Nodes) {
        if (related.find(n) == related.end()) {
            m_Warnings << QString("NODE %1 is not used in ISA").arg(fromUtf8(n));
        }
    }
}

std::vector<std::string> HierarchyGraph::getChildren(const std::string& node) const {
    std::vector<std::string> result;
    for (auto& r : m_Relations) {
        if (r.first == node && std::find(result.begin(), result.end(), r.second) == result.end()) {
            result.push_back(r.second);
        }
    }
    return result;
}

std::vector<std::string> HierarchyGraph::getParents(const std::string& node) const {
    std::vector<std::string> result;
    for (auto& r : m_Relations) {
        if (r.second == node && std::find(result.begin(), result.end(), r.first) == result.end()) {
            result.push_back(r.first);
        }
    }
    return result;
}

bool isHierarchyEntry(const RossDictionary* dict, uint16_t unitNo, CHierarchyEnum& type) {
    if (dict->GetRossType() != Ross || dict->GetRoss()->GetUnitMeanNum(unitNo) != 1) {
        return false;
    }
    std::string entry = dict->GetRoss()->GetEntryStr(unitNo);
    for (auto t : {SemRel, SemFet}) {
        if (entry == GetHierarchyEntryName(t)) {
            type = t;
            return true;
        }
    }
    return false;
}

HierarchyWindow::HierarchyWindow(RossDictionary* dict, CHierarchyEnum type, QWidget* parent)
    : QWidget(parent), m_Dict(dict), m_Type(type) {
    setWindowTitle(QString("%1:   %2 (%3)")
        .arg(dict->dictName(), type == SemRel ? "hierarchy of semantic relations" : "hierarchy of semantic features",
            fromUtf8(GetHierarchyEntryName(type))));

    m_Tree = new QTreeWidget(this);
    m_Tree->setHeaderLabels({"Node", "Upper nodes"});
    m_Tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(m_Tree, &QTreeWidget::itemDoubleClicked, this, [this]() { openArticle(); });

    m_Problems = new QListWidget(this);
    connect(m_Problems, &QListWidget::itemDoubleClicked, this, [this]() { openArticle(); });

    auto splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_Tree);
    splitter->addWidget(m_Problems);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setSizes({500, 80});

    auto buttons = new QHBoxLayout();
    auto openBtn = new QPushButton("Open article", this);
    openBtn->setToolTip("Edit the hierarchy: NODE contains nodes, ISA contains pairs \"upper lower\"");
    connect(openBtn, &QPushButton::clicked, this, &HierarchyWindow::openArticle);
    auto refreshBtn = new QPushButton("Refresh", this);
    connect(refreshBtn, &QPushButton::clicked, this, &HierarchyWindow::refresh);
    auto expandBtn = new QPushButton("Expand all", this);
    connect(expandBtn, &QPushButton::clicked, m_Tree, &QTreeWidget::expandAll);
    auto collapseBtn = new QPushButton("Collapse all", this);
    connect(collapseBtn, &QPushButton::clicked, m_Tree, &QTreeWidget::collapseAll);
    buttons->addWidget(openBtn);
    buttons->addWidget(refreshBtn);
    buttons->addWidget(expandBtn);
    buttons->addWidget(collapseBtn);
    buttons->addStretch(1);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(splitter, 1);
    layout->addLayout(buttons);

    refresh();
    resize(600, 600);
}

uint16_t HierarchyWindow::unitNo() const {
    return m_Dict->GetRoss()->LocateUnit(GetHierarchyEntryName(m_Type), 1);
}

void HierarchyWindow::refresh() {
    m_Tree->clear();
    m_Problems->clear();
    HierarchyGraph graph;
    uint16_t no = unitNo();
    if (no == ErrUnitNo) {
        graph.m_Errors << QString("There is no article %1").arg(fromUtf8(GetHierarchyEntryName(m_Type)));
    }
    else {
        CTempArticle article(m_Dict->GetRoss());
        article.ReadFromDictionary(no, true, false);
        graph.read(m_Dict, article);
    }

    // a node with several upper nodes is shown under each of them
    for (auto& n : graph.m_Nodes) {
        if (graph.getParents(n).empty()) {
            std::vector<std::string> path;
            addSubtree(graph, n, nullptr, path);
        }
    }
    m_Tree->expandAll();

    for (auto& e : graph.m_Errors) {
        auto item = new QListWidgetItem("Error: " + e, m_Problems);
        item->setForeground(Qt::red);
    }
    for (auto& w : graph.m_Warnings) {
        new QListWidgetItem("Warning: " + w, m_Problems);
    }
    if (m_Problems->count() == 0) {
        new QListWidgetItem(QString("No errors: %1 nodes, %2 relations").arg(graph.m_Nodes.size()).arg(graph.m_Relations.size()), m_Problems);
    }
}

void HierarchyWindow::addSubtree(const HierarchyGraph& graph, const std::string& node, QTreeWidgetItem* parent,
    std::vector<std::string>& path) {
    auto item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(m_Tree);
    item->setText(0, fromUtf8(node));
    auto parents = graph.getParents(node);
    if (parents.size() > 1) {
        QStringList names;
        for (auto& p : parents) {
            names << fromUtf8(p);
        }
        item->setText(1, names.join(", "));
    }
    if (std::find(path.begin(), path.end(), node) != path.end()) {
        item->setText(1, "(cycle)");
        item->setForeground(0, Qt::red);
        return;
    }
    path.push_back(node);
    for (auto& c : graph.getChildren(node)) {
        addSubtree(graph, c, item, path);
    }
    path.pop_back();
}

void HierarchyWindow::openArticle() {
    uint16_t no = unitNo();
    if (no != ErrUnitNo) {
        MainWindow::instance()->openArticle(m_Dict, no);
    }
}
