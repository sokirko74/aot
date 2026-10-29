#pragma once

#include <QAbstractTableModel>
#include <QWidget>

#include <cstdint>
#include <vector>

class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QTableView;
class RossDictionary;

// the list of entries of a dictionary, rows are given by the index of unit numbers
class WordListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    WordListModel(RossDictionary* dict, QObject* parent);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setIndex(std::vector<uint16_t> index);
    // the visual rows have been changed (marks, comments)
    void refresh();

private:
    RossDictionary* m_Dict;
    std::vector<uint16_t> m_UnitNos;
};

// the main window of a dictionary (CWordList in the MFC version)
class WordListWindow : public QWidget {
    Q_OBJECT
public:
    explicit WordListWindow(RossDictionary* dict, QWidget* parent = nullptr);

    RossDictionary* dictionary() const { return m_Dict; }
    QMenu* contextMenu() const { return m_Menu; }
    // rebuilds the index of entries (after adding, deleting or sorting)
    void update();
    // selects the row of the unit
    void setCursorToUnit(uint16_t unitNo);
    // the units in the list order
    const std::vector<uint16_t>& unitNos() const { return m_Index; }

public slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onMark();
    void onChangeTitle();
    void onSetAuthor();
    void onComments();
    void onReload();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    enum SortType { SortByLemma, SortByDateAndTime };

    RossDictionary* m_Dict;
    WordListModel* m_Model;
    QTableView* m_Table;
    QLineEdit* m_Locator;
    QLabel* m_Status;
    QLabel* m_ReadOnlyLabel;
    QMenu* m_Menu;
    QAction* m_FilterAction;
    SortType m_Sorted = SortByLemma;
    std::vector<uint16_t> m_Index;

    bool isFiltered() const;
    bool getSelectedUnitNo(uint16_t& unitNo, bool warn = true) const;
    void buildIndex();
    void updateStatus();
    void updateTitle();
    void createMenu();
    // adding, deleting, renaming entries changes unit numbers, so open articles must be closed
    bool noOpenArticles();
    void setSelected(bool value);
    void onLocatorChanged(const QString& text);
    void onSelectByAuthor();
    void onSelectDownward();
    void onInvertSelection();
    void onSaveWordlist();
    void onSaveRossToTxt();
    void onSearchByArticle();
    void onStatisticFieldValue();
    void onArticleAppend();
    void onDelAllSelected();
    void onEmptyArticles();
    void onFindByString();
    void onFindWrongRefs();
};
