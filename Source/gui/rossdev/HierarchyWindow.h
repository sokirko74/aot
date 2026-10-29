#pragma once

#include "seman/SemanLib/HierarchyHolder.h"

#include <QStringList>
#include <QWidget>

#include <string>
#include <utility>
#include <vector>

class CTempArticle;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
class RossDictionary;

// a hierarchy of semantic relations or features, read from the article and checked
// (CHierarchyHolder reads it in the semantic analysis)
struct HierarchyGraph {
    std::vector<std::string> m_Nodes;
    // (upper, lower)
    std::vector<std::pair<std::string, std::string>> m_Relations;
    // the article must not be saved with them
    QStringList m_Errors;
    QStringList m_Warnings;

    void read(RossDictionary* dict, const CTempArticle& article);
    std::vector<std::string> getChildren(const std::string& node) const;
    std::vector<std::string> getParents(const std::string& node) const;

private:
    void check();
};

// is the article of the dictionary a hierarchy entry (_иерархСО or _иерархСХ of Ross)
bool isHierarchyEntry(const RossDictionary* dict, uint16_t unitNo, CHierarchyEnum& type);

// shows the hierarchy as a tree (CHierarchyDoc + CHierarchyView in the MFC version)
class HierarchyWindow : public QWidget {
    Q_OBJECT
public:
    HierarchyWindow(RossDictionary* dict, CHierarchyEnum type, QWidget* parent = nullptr);

    RossDictionary* dictionary() const { return m_Dict; }
    CHierarchyEnum hierarchyType() const { return m_Type; }
    // rereads the hierarchy from the dictionary
    void refresh();

private:
    RossDictionary* m_Dict;
    CHierarchyEnum m_Type;
    QTreeWidget* m_Tree;
    QListWidget* m_Problems;

    uint16_t unitNo() const;
    void addSubtree(const HierarchyGraph& graph, const std::string& node, QTreeWidgetItem* parent, std::vector<std::string>& path);
    void openArticle();
};
