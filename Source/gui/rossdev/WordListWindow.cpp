#include "WordListWindow.h"
#include "ArticleWindow.h"
#include "Dialogs.h"
#include "MainWindow.h"
#include "NewArticleWizard.h"
#include "RossDictionary.h"

#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include <algorithm>
#include <fstream>
#include <tuple>

WordListModel::WordListModel(RossDictionary* dict, QObject* parent) : QAbstractTableModel(parent), m_Dict(dict) {
}

int WordListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)m_UnitNos.size();
}

int WordListModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : 7;
}

QVariant WordListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static const char* headers[] = { "Title", "Sense", "Comm.", "Last modif.", "Mark", "Author", "Editor" };
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section < 7) {
        return headers[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant WordListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= (int)m_UnitNos.size()) {
        return QVariant();
    }
    uint16_t UnitNo = m_UnitNos[index.row()];
    const CDictionary* ross = m_Dict->GetRoss();
    if (role == Qt::TextAlignmentRole && index.column() == 4) {
        return Qt::AlignCenter;
    }
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (index.column()) {
        case 0: return fromUtf8(m_Dict->GetEntryStrUtf8(UnitNo));
        case 1: return ross->GetUnitMeanNum(UnitNo);
        case 2: {
            auto c = ross->GetCommentsByUnitId(ross->GetEntries()[UnitNo].m_EntryId);
            return c ? fromUtf8(c->Comments) : QString();
        }
        case 3: return fromUtf8(ross->GetUnitModifTimeStr(UnitNo));
        case 4: return ross->GetEntries()[UnitNo].m_bSelected ? "+" : "-";
        case 5: return fromUtf8(ross->GetEntries()[UnitNo].GetAuthorStr());
        case 6: return fromUtf8(ross->GetUnitEditor(UnitNo));
    }
    return QVariant();
}

void WordListModel::setIndex(std::vector<uint16_t> index) {
    beginResetModel();
    m_UnitNos = std::move(index);
    endResetModel();
}

void WordListModel::refresh() {
    if (!m_UnitNos.empty()) {
        emit dataChanged(this->index(0, 0), this->index((int)m_UnitNos.size() - 1, columnCount() - 1));
    }
}

//----------------------------------------------------------------------------

WordListWindow::WordListWindow(RossDictionary* dict, QWidget* parent) : QWidget(parent), m_Dict(dict) {
    setAttribute(Qt::WA_DeleteOnClose);

    m_Locator = new QLineEdit(this);
    m_Locator->setPlaceholderText("Find an entry by its beginning");
    connect(m_Locator, &QLineEdit::textChanged, this, &WordListWindow::onLocatorChanged);
    connect(m_Locator, &QLineEdit::returnPressed, this, &WordListWindow::onEdit);

    m_Model = new WordListModel(dict, this);
    m_Table = new QTableView(this);
    m_Table->setModel(m_Model);
    m_Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_Table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_Table->verticalHeader()->hide();
    m_Table->verticalHeader()->setDefaultSectionSize(m_Table->fontMetrics().height() + 4);
    m_Table->horizontalHeader()->setStretchLastSection(true);
    m_Table->setColumnWidth(0, 250);
    m_Table->setColumnWidth(1, 50);
    m_Table->setColumnWidth(2, 120);
    m_Table->setColumnWidth(3, 130);
    m_Table->setColumnWidth(4, 40);
    m_Table->setColumnWidth(5, 70);
    connect(m_Table, &QTableView::doubleClicked, this, &WordListWindow::onEdit);
    connect(m_Table, &QTableView::activated, this, &WordListWindow::onEdit);

    m_Status = new QLabel(this);
    m_ReadOnlyLabel = new QLabel("<b>READ ONLY</b>", this);
    m_ReadOnlyLabel->setStyleSheet("color: red");

    auto buttons = new QVBoxLayout();
    auto addButton = [&](const char* title, void (WordListWindow::*slot)()) {
        auto b = new QPushButton(title, this);
        connect(b, &QPushButton::clicked, this, slot);
        buttons->addWidget(b);
        return b;
    };
    buttons->addWidget(m_ReadOnlyLabel);
    addButton("Add", &WordListWindow::onAdd);
    addButton("Edit", &WordListWindow::onEdit);
    addButton("Delete", &WordListWindow::onDelete);
    addButton("Mark", &WordListWindow::onMark);
    auto changeTitle = addButton("Change Title", &WordListWindow::onChangeTitle);
    changeTitle->setEnabled(!dict->isThesRoss());
    addButton("Set Author", &WordListWindow::onSetAuthor);
    buttons->addStretch();

    auto left = new QVBoxLayout();
    left->addWidget(m_Locator);
    left->addWidget(m_Table, 1);
    left->addWidget(m_Status);
    auto layout = new QHBoxLayout(this);
    layout->addLayout(left, 1);
    layout->addLayout(buttons);

    createMenu();
    connect(dict, &RossDictionary::modifiedChanged, this, &WordListWindow::updateTitle);
    update();
    if (!m_Index.empty()) {
        m_Table->selectRow(0);
    }
    connect(m_Table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &WordListWindow::updateStatus);
    updateTitle();
    resize(900, 600);
}

void WordListWindow::createMenu() {
    m_Menu = new QMenu(this);
    auto sort = m_Menu->addMenu("Sort");
    auto byEntry = sort->addAction("By entry", this, [this]() { m_Sorted = SortByLemma; m_Locator->setEnabled(true); update(); });
    auto byTime = sort->addAction("By last modification time", this, [this]() { m_Sorted = SortByDateAndTime; m_Locator->setEnabled(false); update(); });
    auto sortGroup = new QActionGroup(this);
    for (auto a : { byEntry, byTime }) {
        a->setCheckable(true);
        a->setEnabled(!m_Dict->isThesRoss());
        sortGroup->addAction(a);
    }
    byEntry->setChecked(true);

    auto selection = m_Menu->addMenu("Selection");
    selection->addAction("Select all entries", this, [this]() { setSelected(true); });
    selection->addAction("Unselect all entries", this, [this]() { setSelected(false); });
    selection->addAction("Invert selection", this, &WordListWindow::onInvertSelection);
    selection->addAction("Set selection by author", this, &WordListWindow::onSelectByAuthor);
    selection->addAction("Set from the current line downwards", this, &WordListWindow::onSelectDownward);

    auto filter = m_Menu->addMenu("Filter");
    m_FilterAction = filter->addAction("Show only selected entries", this, [this]() { update(); });
    m_FilterAction->setCheckable(true);

    m_Menu->addSeparator();
    m_Menu->addAction("Export entries (full report)...", this, &WordListWindow::onSaveRossToTxt);
    m_Menu->addAction("Export entry names (only titles)...", this, &WordListWindow::onSaveWordlist);
    m_Menu->addAction("Reread from the disk", this, &WordListWindow::onReload);
    m_Menu->addSeparator();
    m_Menu->addAction("Find all entries, which include the current entry", this, &WordListWindow::onSearchByArticle);
    m_Menu->addAction("Find entry by a string...", this, &WordListWindow::onFindByString);
    m_Menu->addAction("Search for invalid cross-dictionary entries...", this, &WordListWindow::onFindWrongRefs);
    m_Menu->addAction("Frequence list for field values...", this, &WordListWindow::onStatisticFieldValue);
    m_Menu->addSeparator();
    m_Menu->addAction("Set Commentary...", this, &WordListWindow::onComments);
    m_Menu->addAction("Add the current entry to all other entries", this, &WordListWindow::onArticleAppend);
    m_Menu->addAction("Delete all entries", this, &WordListWindow::onDelAllSelected);
    m_Menu->addAction("Empty all entries", this, &WordListWindow::onEmptyArticles);
}

void WordListWindow::contextMenuEvent(QContextMenuEvent* event) {
    m_Menu->exec(event->globalPos());
}

void WordListWindow::updateTitle() {
    QString title = m_Dict->dictName();
    if (m_Dict->isReadOnly()) {
        title += " [read only]";
    }
    if (m_Dict->isModified()) {
        title += " *";
    }
    setWindowTitle(title);
    m_ReadOnlyLabel->setVisible(m_Dict->isReadOnly());
}

bool WordListWindow::isFiltered() const {
    return m_FilterAction->isChecked();
}

static std::tuple<int, int, int, int, int, int> parseTime(const std::string& s) {
    int day = 0, month = 0, year = 0, hour = 0, min = 0, sec = 0;
    sscanf(s.c_str(), "%i/%i/%i %i:%i:%i", &day, &month, &year, &hour, &min, &sec);
    return { year, month, day, hour, min, sec };
}

void WordListWindow::buildIndex() {
    const CDictionary* ross = m_Dict->GetRoss();
    m_Index.clear();
    for (uint16_t i = 0; i < ross->GetUnitsSize(); i++) {
        if (!isFiltered() || ross->GetEntries()[i].m_bSelected) {
            m_Index.push_back(i);
        }
    }
    if (m_Dict->isThesRoss()) {
        // termins are sorted by their strings
        std::vector<std::pair<std::string, uint16_t>> termins;
        for (auto i : m_Index) {
            termins.push_back({ m_Dict->GetEntryStrUtf8(i), i });
        }
        std::sort(termins.begin(), termins.end());
        m_Index.clear();
        for (auto& t : termins) {
            m_Index.push_back(t.second);
        }
    }
    else if (m_Sorted == SortByDateAndTime) {
        std::vector<std::tuple<int, int, int, int, int, int>> times(ross->GetUnitsSize());
        for (auto i : m_Index) {
            times[i] = parseTime(ross->GetUnitModifTimeStr(i));
        }
        std::stable_sort(m_Index.begin(), m_Index.end(), [&](uint16_t a, uint16_t b) { return times[a] < times[b]; });
    }
}

void WordListWindow::update() {
    uint16_t current = ErrUnitNo;
    getSelectedUnitNo(current, false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    buildIndex();
    m_Model->setIndex(m_Index);
    QApplication::restoreOverrideCursor();
    if (current != ErrUnitNo) {
        setCursorToUnit(current);
    }
    updateStatus();
}

void WordListWindow::updateStatus() {
    int row = m_Table->currentIndex().isValid() ? m_Table->currentIndex().row() + 1 : 0;
    m_Status->setText(QString("Entry No %1 (out of %2)").arg(row).arg(m_Index.size()));
}

void WordListWindow::setCursorToUnit(uint16_t unitNo) {
    auto it = std::find(m_Index.begin(), m_Index.end(), unitNo);
    if (it != m_Index.end()) {
        int row = (int)(it - m_Index.begin());
        m_Table->selectRow(row);
        m_Table->scrollTo(m_Model->index(row, 0), QAbstractItemView::PositionAtCenter);
    }
}

bool WordListWindow::getSelectedUnitNo(uint16_t& unitNo, bool warn) const {
    QModelIndex index = m_Table->currentIndex();
    if (!index.isValid() || index.row() >= (int)m_Index.size()) {
        if (warn) {
            QMessageBox::information(const_cast<WordListWindow*>(this), "RossDev", "No selection in the list");
        }
        return false;
    }
    unitNo = m_Index[index.row()];
    return true;
}

bool WordListWindow::noOpenArticles() {
    if (MainWindow::instance()->hasWindowsWithUnitNos(m_Dict)) {
        QMessageBox::warning(this, "RossDev", "Close all articles and entry lists of this dictionary!");
        return false;
    }
    return true;
}

void WordListWindow::onLocatorChanged(const QString& text) {
    if (m_Dict->isThesRoss()) {
        std::string s = toUtf8(text);
        for (size_t k = 0; k < m_Index.size(); k++) {
            if (m_Dict->GetEntryStrUtf8(m_Index[k]) >= s) {
                m_Table->selectRow((int)k);
                m_Table->scrollTo(m_Model->index((int)k, 0), QAbstractItemView::PositionAtTop);
                break;
            }
        }
        return;
    }
    if (m_Sorted != SortByLemma) {
        return;
    }
    uint16_t i = m_Dict->GetRoss()->UnitsLowerBound(toUtf8(text).c_str());
    size_t k = std::lower_bound(m_Index.begin(), m_Index.end(), i) - m_Index.begin();
    if (k < m_Index.size()) {
        m_Table->selectRow((int)k);
        m_Table->scrollTo(m_Model->index((int)k, 0), QAbstractItemView::PositionAtTop);
    }
}

void WordListWindow::onAdd() {
    if (!noOpenArticles()) {
        return;
    }
    NewArticleWizard wizard(m_Dict, this);
    if (wizard.exec() != QDialog::Accepted) {
        return;
    }
    try {
        CDictionary* ross = m_Dict->GetRoss();
        auto addUnit = [&](const std::string& entry, BYTE meanNum, const std::string& article) {
            uint16_t UnitNo = ross->InsertUnit(entry.c_str(), meanNum);
            ross->GetEntries()[UnitNo].m_bSelected = isFiltered();
            ross->SetUnitAuthor(UnitNo, m_Dict->author().c_str());
            ross->SetUnitEditor(UnitNo, m_Dict->author().c_str());
            CTempArticle A(ross);
            A.ReadFromDictionary(UnitNo, true, false);
            A.ReadFromUtf8String(article.c_str());
            A.WriteToDictionary();
            return UnitNo;
        };
        // a reference article for a perfective verb
        if (!wizard.refArticle().empty()) {
            if (ross->LocateUnit(wizard.refLemma().c_str(), 1) != ErrUnitNo) {
                QMessageBox::warning(this, "RossDev", "The reference article cannot be created");
            }
            else {
                addUnit(wizard.refLemma(), 1, wizard.refArticle());
            }
        }
        uint16_t UnitNo = addUnit(wizard.entryStr(), wizard.meanNum(), wizard.article());
        m_Dict->setModified(true);
        update();
        // unit numbers are changed after inserting
        UnitNo = ross->LocateUnit(wizard.entryStr().c_str(), wizard.meanNum());
        setCursorToUnit(UnitNo);
        emit m_Dict->entriesChanged();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "RossDev", QString("Cannot add the entry: ") + fromUtf8(e.what()));
    }
}

void WordListWindow::onEdit() {
    uint16_t UnitNo;
    if (getSelectedUnitNo(UnitNo)) {
        MainWindow::instance()->openArticle(m_Dict, UnitNo);
    }
}

void WordListWindow::onDelete() {
    if (!noOpenArticles()) {
        return;
    }
    uint16_t UnitNo;
    if (!getSelectedUnitNo(UnitNo)) {
        return;
    }
    QString q = QString("Delete entry %1%2 ?").arg(fromUtf8(m_Dict->GetEntryStrUtf8(UnitNo))).arg(m_Dict->GetRoss()->GetUnitMeanNum(UnitNo));
    if (QMessageBox::question(this, "RossDev", q, QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
        return;
    }
    int row = m_Table->currentIndex().row();
    m_Dict->GetRoss()->DelUnit(UnitNo);
    m_Dict->setModified(true);
    update();
    if (!m_Index.empty()) {
        m_Table->selectRow(std::min(row, (int)m_Index.size() - 1));
    }
    emit m_Dict->entriesChanged();
}

void WordListWindow::onMark() {
    uint16_t UnitNo;
    if (!getSelectedUnitNo(UnitNo)) {
        return;
    }
    auto& e = m_Dict->GetRoss()->GetEntries()[UnitNo];
    e.m_bSelected = !e.m_bSelected;
    m_Model->refresh();
}

void WordListWindow::onChangeTitle() {
    if (!noOpenArticles()) {
        return;
    }
    uint16_t UnitNo;
    if (!getSelectedUnitNo(UnitNo)) {
        return;
    }
    CDictionary* ross = m_Dict->GetRoss();
    std::string unitStr = ross->GetEntryStr(UnitNo);
    if (ross->LocateUnit(unitStr.c_str(), 2) != ErrUnitNo) {
        QMessageBox::warning(this, "RossDev", "You cannot change entry name, if this entry has many senses");
        return;
    }
    bool ok = false;
    QString s = QInputDialog::getText(this, "RossDev", "Edit entry name:", QLineEdit::Normal, fromUtf8(unitStr), &ok).trimmed();
    if (!ok || s.isEmpty() || toUtf8(s) == unitStr) {
        return;
    }
    if (ross->LocateUnit(toUtf8(s).c_str(), 1) != ErrUnitNo) {
        QMessageBox::warning(this, "RossDev", "This entry name already exists");
        return;
    }
    ross->SetUnitStr(UnitNo, toUtf8(s).c_str());
    m_Dict->setModified(true);
    update();
    setCursorToUnit(ross->LocateUnit(toUtf8(s).c_str(), 1));
    emit m_Dict->entriesChanged();
}

void WordListWindow::onSetAuthor() {
    if (m_Dict->isReadOnly()) {
        return;
    }
    QString q = QString("You are going to change the author of all entries (%1 entries). Proceed?").arg(m_Index.size());
    if (QMessageBox::question(this, "Confirmation", q) != QMessageBox::Yes) {
        return;
    }
    bool ok = false;
    QString s = QInputDialog::getText(this, "RossDev", "Enter author name:", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || s.isEmpty()) {
        return;
    }
    for (auto u : m_Index) {
        m_Dict->GetRoss()->SetUnitAuthor(u, toUtf8(s).c_str());
    }
    m_Dict->setModified(true);
    m_Model->refresh();
}

void WordListWindow::onComments() {
    uint16_t UnitNo;
    if (!getSelectedUnitNo(UnitNo) || m_Dict->isReadOnly()) {
        return;
    }
    CDictionary* ross = m_Dict->GetRoss();
    const TUnitComment* comments = ross->GetCommentsByUnitId(ross->GetEntries()[UnitNo].m_EntryId);
    bool ok = false;
    QString s = QInputDialog::getText(this, "RossDev", "Comments:", QLineEdit::Normal, fromUtf8(comments->Comments), &ok);
    if (!ok || toUtf8(s) == comments->Comments) {
        return;
    }
    std::string str = toUtf8(s).substr(0, UnitCommentSize - 1);
    ross->SetUnitCommentStr(UnitNo, str.c_str());
    m_Dict->setModified(true);
    m_Model->refresh();
}

void WordListWindow::setSelected(bool value) {
    for (auto u : m_Index) {
        m_Dict->GetRoss()->GetEntries()[u].m_bSelected = value;
    }
    m_Model->refresh();
}

void WordListWindow::onInvertSelection() {
    for (auto u : m_Index) {
        auto& e = m_Dict->GetRoss()->GetEntries()[u];
        e.m_bSelected = !e.m_bSelected;
    }
    m_Model->refresh();
}

void WordListWindow::onSelectByAuthor() {
    bool ok = false;
    QString s = QInputDialog::getText(this, "RossDev", "Input author name:", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok) {
        return;
    }
    for (auto u : m_Index) {
        auto& e = m_Dict->GetRoss()->GetEntries()[u];
        if (e.GetAuthorStr() == toUtf8(s)) {
            e.m_bSelected = true;
        }
    }
    m_Model->refresh();
}

void WordListWindow::onSelectDownward() {
    if (!m_Table->currentIndex().isValid()) {
        QMessageBox::information(this, "RossDev", "No selection found");
        return;
    }
    for (size_t i = m_Table->currentIndex().row(); i < m_Index.size(); i++) {
        m_Dict->GetRoss()->GetEntries()[m_Index[i]].m_bSelected = true;
    }
    m_Model->refresh();
}

static std::string exportHeader(RossDictionary* dict, size_t count, bool filtered) {
    return Format("//Export dictionary %s\r\n//Date = %s\r\n//Records count = %zu\r\n//Filtered = %s\r\n\r\n",
        dict->GetRoss()->GetDictName().c_str(), toUtf8(QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy")).c_str(),
        count, filtered ? "yes" : "no");
}

void WordListWindow::onSaveWordlist() {
    QString path = QFileDialog::getSaveFileName(this, "Export entry names", "wordlist.txt", "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    std::ofstream out(path.toStdString(), std::ios::binary);
    if (!out.is_open()) {
        QMessageBox::critical(this, "RossDev", "Cannot write file");
        return;
    }
    out << exportHeader(m_Dict, m_Index.size(), isFiltered());
    for (auto u : m_Index) {
        out << m_Dict->GetEntryStrUtf8(u) << " " << (int)m_Dict->GetRoss()->GetUnitMeanNum(u) << "\r\n";
    }
}

void WordListWindow::onSaveRossToTxt() {
    QString path = QFileDialog::getSaveFileName(this, "Export entries", "ross.txt", "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    std::ofstream out(path.toStdString(), std::ios::binary);
    if (!out.is_open()) {
        QMessageBox::critical(this, "RossDev", "Cannot write file");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << exportHeader(m_Dict, m_Index.size(), isFiltered());
    CTempArticle A(m_Dict->GetRoss());
    for (auto u : m_Index) {
        out << "============\r\n" << m_Dict->GetRoss()->GetUnitTextHeader(u);
        try {
            A.ReadFromDictionary(u, false, true);
            out << A.GetArticleStrUtf8();
        }
        catch (...) {
            out << "Error! Cannot get the article\r\n";
        }
    }
    QApplication::restoreOverrideCursor();
}

void WordListWindow::onSearchByArticle() {
    uint16_t UnitNo;
    if (!getSelectedUnitNo(UnitNo)) {
        return;
    }
    CDictionary* ross = m_Dict->GetRoss();
    if (ross->IsEmptyArticle(UnitNo)) {
        QMessageBox::information(this, "RossDev", "No search by empty article!");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::vector<PocketItem> items;
    CTempArticle A1(ross);
    A1.ReadFromDictionary(UnitNo, false, true);
    CTempArticle A2(ross);
    for (auto u : m_Index) {
        if (u != UnitNo) {
            A2.ReadFromDictionary(u, false, true);
            if (A1.IsPartOf(&A2, true)) {
                items.push_back({ m_Dict, u });
            }
        }
    }
    QApplication::restoreOverrideCursor();
    MainWindow::instance()->openPocket(items, QString("dictionary entry %1%2").arg(fromUtf8(m_Dict->GetEntryStrUtf8(UnitNo))).arg(ross->GetUnitMeanNum(UnitNo)));
}

void WordListWindow::onStatisticFieldValue() {
    bool ok = false;
    QString s = QInputDialog::getText(this, "RossDev", "Enter field name (* for all fields):", QLineEdit::Normal, "*", &ok).trimmed();
    if (!ok || s.isEmpty()) {
        return;
    }
    CDictionary* ross = m_Dict->GetRoss();
    BYTE FieldNo = ErrUChar;
    if (s != "*") {
        FieldNo = ross->GetFieldNoByFieldStr(toUtf8(s).c_str());
        if (FieldNo == ErrUChar) {
            QMessageBox::warning(this, "RossDev", "Cannot find this field");
            return;
        }
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // (field, value) -> frequence
    std::map<std::pair<std::string, std::string>, int> freqs;
    for (size_t i = 0; i < ross->_GetCortegesSize(); i++) {
        const TCortege& c = ross->GetCortege(i);
        if (FieldNo != ErrUChar && c.m_FieldNo != FieldNo) {
            continue;
        }
        freqs[{ ross->Fields[c.m_FieldNo].FieldStr, ross->WriteToString(c) }]++;
    }
    QString report = "_______________________________________________________\n";
    for (auto& [key, freq] : freqs) {
        report += fromUtf8(Format("%-16s | %-30s | %i\n", key.first.c_str(), key.second.c_str(), freq));
    }
    QApplication::restoreOverrideCursor();
    MainWindow::instance()->openReport(report, "Dictionary fields");
}

void WordListWindow::onArticleAppend() {
    uint16_t MainUnitNo;
    if (!getSelectedUnitNo(MainUnitNo)) {
        return;
    }
    if (QMessageBox::question(this, "Confirmation",
        "The environment adds the current entry to all entries, if there is no intersection by fields. Proceed?") != QMessageBox::Yes) {
        return;
    }
    try {
        CDictionary* ross = m_Dict->GetRoss();
        CTempArticle A1(ross);
        A1.ReadFromDictionary(MainUnitNo, false, false);
        CTempArticle A2(ross);
        for (auto u : m_Index) {
            if (u == MainUnitNo) {
                continue;
            }
            A2.ReadFromDictionary(u, false, false);
            A2.AddArticle(&A1);
            A2.WriteToDictionary();
        }
        m_Dict->setModified(true);
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "RossDev", fromUtf8(e.what()));
    }
}

void WordListWindow::onDelAllSelected() {
    if (!noOpenArticles()) {
        return;
    }
    QString q = QString("You are going to delete all entries (%1 entries). Proceed?").arg(m_Index.size());
    if (QMessageBox::question(this, "Confirmation", q) != QMessageBox::Yes) {
        return;
    }
    CDictionary* ross = m_Dict->GetRoss();
    std::vector<std::pair<std::string, BYTE>> units;
    for (auto u : m_Index) {
        units.push_back({ ross->GetEntryStr(u), ross->GetUnitMeanNum(u) });
    }
    for (auto& [entry, meanNum] : units) {
        ross->DelUnit(ross->LocateUnit(entry.c_str(), meanNum));
    }
    m_Dict->setModified(true);
    update();
    emit m_Dict->entriesChanged();
}

void WordListWindow::onEmptyArticles() {
    QString q = QString("You are going to clear all entries (%1 entries). Proceed?").arg(m_Index.size());
    if (QMessageBox::question(this, "Confirmation", q) != QMessageBox::Yes) {
        return;
    }
    for (auto u : m_Index) {
        m_Dict->GetRoss()->ClearUnit(u);
    }
    m_Dict->setModified(true);
    update();
}

void WordListWindow::onFindByString() {
    FindByStrDialog dlg(m_Dict, this);
    dlg.exec();
}

void WordListWindow::onFindWrongRefs() {
    FindWrongRefsDialog dlg(m_Dict, this);
    dlg.exec();
}

void WordListWindow::onReload() {
    if (!noOpenArticles()) {
        return;
    }
    if (m_Dict->isModified()
        && QMessageBox::question(this, "Confirmation", "You are going to lose all changes, which you have made",
            QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
        return;
    }
    try {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        // load() sets a new update time, so the semantic analysis rebuilds its indices
        m_Dict->load(m_Dict->folder(), m_Dict->isReadOnly());
        m_Dict->setModified(false);
        QApplication::restoreOverrideCursor();
    }
    catch (std::exception& e) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, "RossDev", fromUtf8(e.what()));
    }
    update();
    emit m_Dict->entriesChanged();
}

void WordListWindow::closeEvent(QCloseEvent* event) {
    if (MainWindow::instance()->closeDictionary(m_Dict, this)) {
        event->accept();
    }
    else {
        event->ignore();
    }
}
