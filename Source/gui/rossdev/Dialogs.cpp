#include "Dialogs.h"
#include "MainWindow.h"
#include "RossDictionary.h"

#include "dicts/StructDictLib/TempArticle.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <fstream>

PocketWindow::PocketWindow(const std::vector<PocketItem>& items, const QString& title, QWidget* parent)
    : QWidget(parent), m_Items(items), m_Title(title) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("An entry subset by " + title);

    m_Table = new QTableWidget((int)items.size(), 3, this);
    m_Table->setHorizontalHeaderLabels({ "Dictionary entry", "Sense ID", "Dictionary name" });
    m_Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_Table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_Table->verticalHeader()->hide();
    m_Table->horizontalHeader()->setStretchLastSection(true);
    m_Table->setColumnWidth(0, 250);
    for (int i = 0; i < (int)items.size(); i++) {
        auto& it = items[i];
        m_Table->setItem(i, 0, new QTableWidgetItem(fromUtf8(it.m_Dict->GetEntryStrUtf8(it.m_UnitNo))));
        m_Table->setItem(i, 1, new QTableWidgetItem(QString::number(it.m_Dict->GetRoss()->GetUnitMeanNum(it.m_UnitNo))));
        m_Table->setItem(i, 2, new QTableWidgetItem(it.m_Dict->dictName()));
    }
    if (!items.empty()) {
        m_Table->selectRow(0);
    }
    connect(m_Table, &QTableWidget::cellDoubleClicked, this, &PocketWindow::onEdit);

    auto buttons = new QVBoxLayout();
    for (auto [name, slot] : { std::pair{ "Edit Entry", &PocketWindow::onEdit }, { "Save to file", &PocketWindow::onSave },
             { "Select in the main list", &PocketWindow::onSelectInMainList } }) {
        auto b = new QPushButton(name, this);
        connect(b, &QPushButton::clicked, this, slot);
        buttons->addWidget(b);
    }
    buttons->addStretch();
    auto left = new QVBoxLayout();
    left->addWidget(m_Table, 1);
    left->addWidget(new QLabel(QString("Number of dictionary entries: %1").arg(items.size()), this));
    auto layout = new QHBoxLayout(this);
    layout->addLayout(left, 1);
    layout->addLayout(buttons);
    resize(600, 500);
}

bool PocketWindow::contains(const RossDictionary* dict) const {
    for (auto& it : m_Items) {
        if (it.m_Dict == dict) {
            return true;
        }
    }
    return false;
}

void PocketWindow::onEdit() {
    int row = m_Table->currentRow();
    if (row < 0 || row >= (int)m_Items.size()) {
        QMessageBox::information(this, "RossDev", "No dictionary entries are highlighted in the list");
        return;
    }
    MainWindow::instance()->openArticle(m_Items[row].m_Dict, m_Items[row].m_UnitNo);
}

void PocketWindow::onSave() {
    QString path = QFileDialog::getSaveFileName(this, "Save to file", m_Title + ".txt", "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    std::ofstream out(path.toStdString(), std::ios::binary);
    for (auto& it : m_Items) {
        out << it.m_Dict->GetRoss()->GetEntryStr(it.m_UnitNo) << (int)it.m_Dict->GetRoss()->GetUnitMeanNum(it.m_UnitNo) << "\r\n";
    }
}

// marks the entries of the pocket in the word lists of their dictionaries
void PocketWindow::onSelectInMainList() {
    for (auto& it : m_Items) {
        it.m_Dict->GetRoss()->GetEntries()[it.m_UnitNo].m_bSelected = true;
    }
    MainWindow::instance()->refreshWordLists();
}

//----------------------------------------------------------------------------

FindByStrDialog::FindByStrDialog(RossDictionary* activeDict, QWidget* parent) : QDialog(parent), m_ActiveDict(activeDict) {
    setWindowTitle("Find entry by a string");
    m_String = new QLineEdit(this);
    m_AllDicts = new QRadioButton("Search in all open dictionaries", this);
    auto active = new QRadioButton("Search in the current dictionary", this);
    m_AllDicts->setChecked(true);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("A substring of the entry text:", this));
    layout->addWidget(m_String);
    layout->addWidget(m_AllDicts);
    layout->addWidget(active);
    layout->addWidget(buttons);
}

void FindByStrDialog::accept() {
    std::vector<RossDictionary*> dicts;
    if (m_AllDicts->isChecked()) {
        dicts = RossDevCore::instance().dictionaries();
    }
    else {
        dicts.push_back(m_ActiveDict);
    }
    std::string substr = toUtf8(m_String->text());
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::vector<PocketItem> items;
    for (auto dict : dicts) {
        CTempArticle A(dict->GetRoss());
        for (uint16_t UnitNo = 0; UnitNo < dict->GetRoss()->GetUnitsSize(); UnitNo++) {
            if (dict->GetRoss()->IsEmptyArticle(UnitNo)) {
                continue;
            }
            try {
                A.ReadFromDictionary(UnitNo, false, true);
                if (A.GetArticleStrUtf8().find(substr) != std::string::npos) {
                    items.push_back({ dict, UnitNo });
                }
            }
            catch (...) {
                QApplication::restoreOverrideCursor();
                QMessageBox::warning(this, "RossDev", "Errors in article " + fromUtf8(dict->GetRoss()->GetEntryStr(UnitNo)));
                QApplication::setOverrideCursor(Qt::WaitCursor);
            }
        }
    }
    QApplication::restoreOverrideCursor();
    MainWindow::instance()->openPocket(items, "Search Results for " + m_String->text());
    QDialog::accept();
}

//----------------------------------------------------------------------------

FindWrongRefsDialog::FindWrongRefsDialog(RossDictionary* activeDict, QWidget* parent) : QDialog(parent), m_ActiveDict(activeDict) {
    setWindowTitle("Search for invalid references");
    m_Field = new QLineEdit("ENG", this);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    auto layout = new QFormLayout(this);
    layout->addRow("Field Name", m_Field);
    layout->addRow(buttons);
}

// entries whose field refers to an entry (with a sense number), which is absent in all open dictionaries
void FindWrongRefsDialog::accept() {
    CDictionary* ross = m_ActiveDict->GetRoss();
    BYTE FieldNo = ross->GetFieldNoByFieldStr(toUtf8(m_Field->text()).c_str());
    if (FieldNo == ErrUChar) {
        QMessageBox::warning(this, "RossDev", "Cannot find this field");
        return;
    }
    const auto& dicts = RossDevCore::instance().dictionaries();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::vector<PocketItem> items;
    for (uint16_t UnitNo = 0; UnitNo < ross->GetUnitsSize(); UnitNo++) {
        if (ross->IsEmptyArticle(UnitNo)) {
            continue;
        }
        size_t k = ross->GetUnitStartPos(UnitNo);
        for (; k <= (size_t)ross->GetUnitLastPos(UnitNo); k++) {
            const TCortege& C = ross->GetCortege(k);
            if (C.m_FieldNo != FieldNo) {
                continue;
            }
            std::string Q = ross->WriteToString(C);
            if (Q.find("TR") != std::string::npos || Q.find("PR") != std::string::npos
                || Q.find("SR") != std::string::npos || Q.find("[") != std::string::npos) {
                continue;
            }
            if (!C.is_null(0) && ross->GetDomItemStr(C.GetItem(0)).find(" ") != std::string::npos) {
                continue;
            }
            if (C.is_null(0) || C.is_null(1)) {
                break;
            }
            std::string MeanNumStr = ross->GetDomItemStr(C.GetItem(1));
            if (MeanNumStr.length() != 1) {
                break;
            }
            BYTE MeanNum = MeanNumStr[0] - '0';
            std::string UnitStr = ross->GetDomItemStr(C.GetItem(0));
            bool found = false;
            for (auto d : dicts) {
                if (d->GetRoss()->LocateUnit(UnitStr.c_str(), MeanNum) != ErrUnitNo) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                break;
            }
        }
        if (k <= (size_t)ross->GetUnitLastPos(UnitNo)) {
            items.push_back({ m_ActiveDict, UnitNo });
        }
    }
    QApplication::restoreOverrideCursor();
    MainWindow::instance()->openPocket(items, "Invalid references");
    QDialog::accept();
}

//----------------------------------------------------------------------------

std::string askUserName(QWidget* parent, const RossDictionary* dict) {
    const auto& users = dict->GetRoss()->GetConfig().DictUsers;
    for (int trial = 0; trial < 3; trial++) {
        bool ok = false;
        QString login = QInputDialog::getText(parent, dict->dictName(),
            "Input your login (\"guest\" - for ReadOnly):", QLineEdit::Normal, "guest", &ok).trimmed();
        if (!ok || login.isEmpty() || login == "guest") {
            return "guest";
        }
        for (auto& u : users) {
            if (toUtf8(login) == u) {
                return u;
            }
        }
        QMessageBox::warning(parent, "RossDev", "Your login is incorrect");
    }
    QMessageBox::warning(parent, "RossDev", "Cannot find your login in the login list. Logging as guest...");
    return "guest";
}
