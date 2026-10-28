#include "WizardView.h"
#include "DiffViews.h"
#include "Dialogs.h"
#include "ProgressMeter.h"
#include "SlfEditor.h"
#include "QtUtil.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>

#include <plog/Log.h>

#include <algorithm>
#include <fstream>
#include <set>

const int HISTORY_SIZE = 10;

enum SearchTypeEnum {
    FindByLemma = 0,
    FindByGrammems,
    FindByUserName,
    FindByWordForm,
    FindByGramCode,
    FindByAccentModel,
    FindByParadigmNo
};

static void errorMessage(QWidget* parent, const QString& s) {
    QMessageBox::critical(parent, "MorphWizard", s);
}

static std::string lemmaWithPrefixes(const MorphoWizard& wizard, const_lemma_iterator_t it, bool withAccents) {
    std::string lemma = withAccents ? wizard.get_lemm_string_with_accents(it) : wizard.get_lemm_string(it);
    std::string prefixSet = wizard.get_prefix_set(it);
    if (!prefixSet.empty()) {
        lemma = prefixSet + '|' + lemma;
    }
    MakeLowerUtf8(lemma);
    return lemma;
}

//----------------------------------------------------------------------------
// FoundParadigmsModel
//----------------------------------------------------------------------------
void FoundParadigmsModel::setRows(const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& paradigms) {
    beginResetModel();
    m_Rows.clear();
    m_Rows.reserve(paradigms.size());
    for (auto& it : paradigms) {
        std::array<QString, ColumnsCount> row;
        row[ParadigmNoColumn] = QString("%1 (%2)").arg(it->second.m_FlexiaModelNo).arg(it->second.m_AccentModelNo);
        row[LemmaColumn] = fromUtf8(lemmaWithPrefixes(wizard, it, true));
        row[GrammemsColumn] = fromUtf8(wizard.get_pos_string(it) + " " + wizard.get_grammem_string(it));
        row[TypeGrammemsColumn] = fromUtf8(wizard.get_common_grammems_string(it));
        m_Rows.push_back(row);
    }
    endResetModel();
}

void FoundParadigmsModel::clear() {
    beginResetModel();
    m_Rows.clear();
    endResetModel();
}

int FoundParadigmsModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)m_Rows.size();
}

int FoundParadigmsModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : ColumnsCount;
}

QVariant FoundParadigmsModel::data(const QModelIndex& index, int role) const {
    if (role != Qt::DisplayRole || !index.isValid() || index.row() >= (int)m_Rows.size()) {
        return QVariant();
    }
    return m_Rows[index.row()][index.column()];
}

QVariant FoundParadigmsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }
    switch (section) {
        case ParadigmNoColumn: return "ParadigmNo";
        case LemmaColumn: return "Lemma";
        case GrammemsColumn: return "Grammems";
        case TypeGrammemsColumn: return "Type";
    }
    return QVariant();
}


//----------------------------------------------------------------------------
// WizardView
//----------------------------------------------------------------------------
WizardView::WizardView(QMdiArea* mdiArea, const QString& mwzPath, const QString& userName, bool usePredict, QWidget* parent)
    : QWidget(parent), m_FilePath(mwzPath), m_MdiArea(mdiArea)
{
    m_Wizard = std::make_unique<MorphoWizard>();
    {
        WizardProgressMeter meter(*m_Wizard);
        m_Wizard->load_wizard(toUtf8(mwzPath), toUtf8(userName), usePredict, false);
        // load_mrd_json ignores bCreatePrediction, the index is created only in pack()
        if (usePredict) {
            meter.SetInfo("Creating prediction index...");
            m_Wizard->m_Predictor.CreateIndex();
        }
    }
    loadHistory();
    createControls(m_Wizard->m_ReadOnly);
    updateTitle();
}

WizardView::~WizardView() {
    saveHistory();
    for (auto& e : m_Editors) {
        if (e) {
            e->m_pParent = nullptr;
        }
    }
}

void WizardView::updateTitle() {
    QString title = QFileInfo(m_FilePath).fileName();
    if (m_Wizard->m_ReadOnly) {
        title += " [read only]";
    }
    if (m_Wizard->is_changed()) {
        title += " *";
    }
    setWindowTitle(title);
}

void WizardView::createControls(bool readOnly) {
    auto mainLayout = new QVBoxLayout(this);
    auto splitter = new QSplitter(this);

    // the left panel: search, edit and remove paradigms
    auto editBox = new QGroupBox("EDIT/REMOVE PARADIGM", splitter);
    auto editLayout = new QVBoxLayout(editBox);
    {
        auto line = new QHBoxLayout();
        auto findBtn = new QPushButton("FIND", editBox);
        m_FindWhat = new QComboBox(editBox);
        m_FindWhat->setEditable(true);
        m_FindWhat->setInsertPolicy(QComboBox::NoInsert);
        m_FindWhat->addItems(m_LastQueries);
        m_FindWhat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        line->addWidget(findBtn);
        line->addWidget(m_FindWhat, 1);
        editLayout->addLayout(line);
        connect(findBtn, &QPushButton::clicked, this, &WizardView::onFind);
        connect(m_FindWhat->lineEdit(), &QLineEdit::returnPressed, this, &WizardView::onFind);
    }
    {
        auto line = new QHBoxLayout();
        m_SearchType = new QButtonGroup(this);
        const std::pair<const char*, int> types[] = {
            { "lemma", FindByLemma },
            { "pos+grammem", FindByGrammems },
            { "user", FindByUserName },
            { "word form", FindByWordForm },
            { "gramcode", FindByGramCode },
            { "accent model", FindByAccentModel },
            { "paradigm no", FindByParadigmNo }
        };
        for (auto& [label, id] : types) {
            auto r = new QRadioButton(label, editBox);
            m_SearchType->addButton(r, id);
            line->addWidget(r);
        }
        m_SearchType->button(FindByLemma)->setChecked(true);
        line->addStretch();
        editLayout->addLayout(line);
    }
    m_FoundModel = new FoundParadigmsModel(this);
    m_FoundList = new QTableView(editBox);
    m_FoundList->setModel(m_FoundModel);
    m_FoundList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_FoundList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_FoundList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_FoundList->verticalHeader()->hide();
    m_FoundList->verticalHeader()->setDefaultSectionSize(m_FoundList->fontMetrics().height() + 4);
    m_FoundList->horizontalHeader()->setStretchLastSection(true);
    m_FoundList->horizontalHeader()->setSectionsClickable(true);
    m_FoundList->setColumnWidth(FoundParadigmsModel::ParadigmNoColumn, 90);
    m_FoundList->setColumnWidth(FoundParadigmsModel::LemmaColumn, 180);
    m_FoundList->setColumnWidth(FoundParadigmsModel::GrammemsColumn, 210);
    editLayout->addWidget(m_FoundList, 1);
    connect(m_FoundList, &QTableView::doubleClicked, this, &WizardView::onEdit);
    connect(m_FoundList->horizontalHeader(), &QHeaderView::sectionClicked, this, &WizardView::onFoundHeaderClicked);
    {
        auto line = new QHBoxLayout();
        auto removeBtn = new QPushButton("REMOVE", editBox);
        auto editBtn = new QPushButton("EDIT", editBox);
        auto commentsBtn = new QPushButton("COMMENTS", editBox);
        m_Filter = new QLineEdit(editBox);
        m_Filter->setToolTip("A paradigm number or a filter expression");
        line->addWidget(removeBtn);
        line->addWidget(editBtn);
        line->addWidget(commentsBtn);
        line->addWidget(new QLabel("Filter:", editBox));
        line->addWidget(m_Filter, 1);
        editLayout->addLayout(line);
        connect(removeBtn, &QPushButton::clicked, this, &WizardView::onRemove);
        connect(editBtn, &QPushButton::clicked, this, &WizardView::onEdit);
        connect(commentsBtn, &QPushButton::clicked, this, &WizardView::onSetParadigmComments);
        connect(m_Filter, &QLineEdit::returnPressed, this, &WizardView::onFind);
    }

    // the right panel: predict and add paradigms
    auto addBox = new QGroupBox("ADD PARADIGM", splitter);
    auto addLayout = new QVBoxLayout(addBox);
    {
        auto line = new QHBoxLayout();
        auto predictBtn = new QPushButton("PREDICT", addBox);
        m_PredictWhat = new QLineEdit(addBox);
        m_PredictWhat->setToolTip("A new lemma; use '#' to separate a prefix, '|' to set prefixes of a new paradigm");
        line->addWidget(predictBtn);
        line->addWidget(m_PredictWhat, 1);
        if (readOnly) {
            auto readOnlyLabel = new QLabel("READ ONLY", addBox);
            readOnlyLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
            line->addWidget(readOnlyLabel);
        }
        addLayout->addLayout(line);
        connect(predictBtn, &QPushButton::clicked, this, &WizardView::onPredict);
        connect(m_PredictWhat, &QLineEdit::returnPressed, this, &WizardView::onPredict);
        // the lemma is always in upper case
        connect(m_PredictWhat, &QLineEdit::textEdited, this, [this](const QString& s) {
            int pos = m_PredictWhat->cursorPosition();
            m_PredictWhat->setText(s.toUpper());
            m_PredictWhat->setCursorPosition(pos);
        });
    }
    {
        auto line = new QHBoxLayout();
        m_MinimalFrequence = new QSpinBox(addBox);
        m_MinimalFrequence->setRange(0, 1000000);
        m_MinimalFrequence->setValue(2);
        m_PredictSuffLength = new QSpinBox(addBox);
        m_PredictSuffLength->setRange(MinPredictSuffixLength, MaxPredictSuffixLength);
        m_PredictSuffLength->setValue(2);
        line->addWidget(new QLabel("Min. frequence:", addBox));
        line->addWidget(m_MinimalFrequence);
        line->addWidget(new QLabel("Suffix Length:", addBox));
        line->addWidget(m_PredictSuffLength);
        line->addStretch();
        addLayout->addLayout(line);
    }
    m_PredictedList = new QTableWidget(0, 8, addBox);
    {
        // the column order is defined by TLemmPredictSortEnum
        m_PredictedList->setHorizontalHeaderLabels({ "Grammems", "Type", "Source lemma", "Freq", "Innernumber", "ParadigmNo", "Prefix", "Forms Count" });
        m_PredictedList->setColumnHidden((int)TLemmPredictSortEnum::Index, true);
        m_PredictedList->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_PredictedList->setSelectionMode(QAbstractItemView::SingleSelection);
        m_PredictedList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_PredictedList->verticalHeader()->hide();
        m_PredictedList->verticalHeader()->setDefaultSectionSize(m_PredictedList->fontMetrics().height() + 4);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::Grammems, 140);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::TypeDict, 60);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::Lemma, 140);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::Freq, 60);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::Paradigm, 80);
        m_PredictedList->setColumnWidth((int)TLemmPredictSortEnum::LemmaPrefix, 60);
        m_PredictedList->horizontalHeader()->setStretchLastSection(true);
        addLayout->addWidget(m_PredictedList, 1);
        connect(m_PredictedList, &QTableWidget::cellDoubleClicked, this, &WizardView::onAdd);
        connect(m_PredictedList->horizontalHeader(), &QHeaderView::sectionClicked, this, &WizardView::onPredictedHeaderClicked);
    }
    {
        auto line = new QHBoxLayout();
        auto addBtn = new QPushButton("ADD", addBox);
        m_OnlyMainPartOfSpeeches = new QCheckBox("Only main part of speeches", addBox);
        m_OnlyMainPartOfSpeeches->setChecked(true);
        line->addWidget(addBtn);
        line->addWidget(m_OnlyMainPartOfSpeeches);
        line->addStretch();
        addLayout->addLayout(line);
        connect(addBtn, &QPushButton::clicked, this, &WizardView::onAdd);
    }

    splitter->addWidget(editBox);
    splitter->addWidget(addBox);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    m_Status = new QPlainTextEdit(this);
    m_Status->setReadOnly(true);
    m_Status->setFocusPolicy(Qt::NoFocus);
    m_Status->setMaximumHeight(m_Status->fontMetrics().height() * 4);
    mainLayout->addWidget(m_Status);

    m_FindWhat->setFocus();
}

void WizardView::echo(const QString& s) {
    m_Status->setPlainText(s);
}

//----------------------------------------------------------------------------
// query history
//----------------------------------------------------------------------------
void WizardView::loadHistory() {
    QSettings settings;
    m_LastQueries = settings.value("History").toStringList();
    while (m_LastQueries.size() > HISTORY_SIZE) {
        m_LastQueries.removeLast();
    }
}

void WizardView::saveHistory() const {
    QSettings settings;
    settings.setValue("History", m_LastQueries);
}

void WizardView::changeHistory(const QString& query) {
    if (!m_LastQueries.isEmpty() && m_LastQueries.front() == query) {
        return;
    }
    m_LastQueries.removeAll(query);
    m_LastQueries.push_front(query);
    while (m_LastQueries.size() > HISTORY_SIZE) {
        m_LastQueries.removeLast();
    }
    m_FindWhat->blockSignals(true);
    m_FindWhat->clear();
    m_FindWhat->addItems(m_LastQueries);
    m_FindWhat->setCurrentIndex(0);
    m_FindWhat->blockSignals(false);
}

//----------------------------------------------------------------------------
// find
//----------------------------------------------------------------------------
void WizardView::filterFoundParadigms() {
    std::string flt_str = toUtf8(m_Filter->text().trimmed());
    if (flt_str.empty()) {
        return;
    }
    if (isdigit((BYTE)flt_str[0])) {
        if (strspn(flt_str.c_str(), "0123456789") != flt_str.length()) {
            errorMessage(this, "Bad format for paradigm No in filter");
            return;
        }
        size_t flexiaModel = atoi(flt_str.c_str());
        if (flexiaModel >= m_Wizard->m_FlexiaModels.size()) {
            errorMessage(this, "Paradigm No in filter is too high ");
            return;
        }
        std::vector<lemma_iterator_t> filtered;
        for (auto& p : m_FoundParadigms) {
            if (p->second.m_FlexiaModelNo == flexiaModel) {
                filtered.push_back(p);
            }
        }
        m_FoundParadigms = filtered;
    }
    else {
        if (!m_Wizard->Filter(flt_str, m_FoundParadigms)) {
            errorMessage(this, "Not valid filter!");
        }
    }
}

void WizardView::showFoundParadigms() {
    m_FoundModel->setRows(*m_Wizard, m_FoundParadigms);
    echo(QString("Found %1 paradigms").arg(m_FoundParadigms.size()));
}

void WizardView::onFind() {
    try {
        m_FoundModel->clear();
        m_FoundParadigms.clear();
        m_FindWhat->setFocus();

        QString find_what = m_FindWhat->currentText().trimmed();
        if (find_what.isEmpty()) {
            return;
        }
        changeHistory(find_what);
        std::string find_what_u8 = toUtf8(find_what);

        WizardProgressMeter meter(*m_Wizard);
        switch (m_SearchType->checkedId()) {
            case FindByLemma:
                m_FoundParadigms = m_Wizard->find_lemm(find_what_u8, false);
                break;
            case FindByParadigmNo:
                if (isdigit((BYTE)find_what_u8[0])) {
                    m_Wizard->find_lemm_by_prdno(atoi(find_what_u8.c_str()), m_FoundParadigms);
                }
                break;
            case FindByGrammems:
                m_Wizard->find_lemm_by_grammem(find_what_u8, m_FoundParadigms);
                break;
            case FindByWordForm:
                MakeUpperUtf8(find_what_u8);
                m_FoundParadigms = m_Wizard->find_wordforms(find_what_u8);
                break;
            case FindByGramCode:
                m_Wizard->find_ancodes(find_what_u8, m_FoundParadigms);
                break;
            case FindByUserName:
                m_Wizard->find_lemm_by_user(find_what_u8, m_FoundParadigms);
                break;
            case FindByAccentModel:
                if (isdigit((BYTE)find_what_u8[0])) {
                    m_Wizard->find_lemm_by_accent_model(atoi(find_what_u8.c_str()), m_FoundParadigms);
                }
                break;
        }

        filterFoundParadigms();
        showFoundParadigms();
    }
    catch (std::exception& c) {
        errorMessage(this, fromUtf8(c.what()));
    }
    catch (...) {
        errorMessage(this, "Search failed");
    }
}

int WizardView::selectedFoundIndex() const {
    auto rows = m_FoundList->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        return -1;
    }
    int index = rows.front().row();
    return index < (int)m_FoundParadigms.size() ? index : -1;
}

void WizardView::onFoundHeaderClicked(int column) {
    if (column == FoundParadigmsModel::ParadigmNoColumn) {
        std::stable_sort(m_FoundParadigms.begin(), m_FoundParadigms.end(),
            [](const lemma_iterator_t& a, const lemma_iterator_t& b) { return a->second.m_FlexiaModelNo < b->second.m_FlexiaModelNo; });
        showFoundParadigms();
    }
    else if (column == FoundParadigmsModel::LemmaColumn) {
        std::stable_sort(m_FoundParadigms.begin(), m_FoundParadigms.end(),
            [](const lemma_iterator_t& a, const lemma_iterator_t& b) { return a->first < b->first; });
        showFoundParadigms();
    }
}

void WizardView::onRemove() {
    try {
        int index = selectedFoundIndex();
        if (index == -1) {
            echo("No item selected");
            return;
        }
        m_Wizard->remove_lemm(m_FoundParadigms.at(index));
        onFind();
        echo("Word removed");
        updateTitle();
    }
    catch (std::exception& e) {
        errorMessage(this, fromUtf8(e.what()));
    }
}

void WizardView::onEdit() {
    int index = selectedFoundIndex();
    if (index == -1) {
        echo("No item selected");
        return;
    }
    echo("Editing");
    openExistingParadigm(m_FoundParadigms[index]);
}

void WizardView::onSetParadigmComments() {
    int index = selectedFoundIndex();
    if (index == -1) {
        errorMessage(this, "No item selected");
        echo("No item selected");
        return;
    }
    int modelNo = m_FoundParadigms[index]->second.m_FlexiaModelNo;
    std::string& comments = m_Wizard->m_FlexiaModels[modelNo].m_Comments;
    bool ok = false;
    QString s = QInputDialog::getText(this, "MorphWizard", QString("Input comments to paradigm No:%1").arg(modelNo),
        QLineEdit::Normal, fromUtf8(comments), &ok);
    if (ok && !m_Wizard->m_ReadOnly) {
        comments = toUtf8(s);
    }
}

//----------------------------------------------------------------------------
// predict and add
//----------------------------------------------------------------------------
void WizardView::onPredict() {
    try {
        m_FoundModel->clear();
        m_PredictedList->setRowCount(0);

        QString predict_what = m_PredictWhat->text();
        std::string lemma = toUtf8(predict_what);
        qsizetype i = predict_what.indexOf('#');
        if (i != -1) {
            QString prefix = predict_what.left(i);
            predict_what = predict_what.mid(i + 1);
            lemma = toUtf8(prefix + predict_what);
        }

        if (!m_Wizard->find_lemm(lemma, false).empty()) {
            QMessageBox::information(this, "MorphWizard", "The dictionary contains already this word!");
        }
        m_LemmaToPredict = toUtf8(predict_what);
        m_PredictedParadigms = m_Wizard->m_Predictor.predict_lemm(
            m_LemmaToPredict,
            m_PredictSuffLength->value(),
            m_MinimalFrequence->value(),
            m_OnlyMainPartOfSpeeches->isChecked(),
            (TLemmPredictSortEnum)m_SortPredictColumn);

        if (m_PredictedParadigms.empty()) {
            errorMessage(this, "No variants found");
            return;
        }

        m_PredictedList->setRowCount((int)m_PredictedParadigms.size());
        for (size_t i = 0; i < m_PredictedParadigms.size(); i++) {
            const CPredictSuffix& S = m_PredictedParadigms[i];
            const CFlexiaModel& P = m_Wizard->m_FlexiaModels[S.m_FlexiaModelNo];
            auto set = [this, i](TLemmPredictSortEnum column, const QString& s) {
                m_PredictedList->setItem((int)i, (int)column, new QTableWidgetItem(s));
            };
            set(TLemmPredictSortEnum::Grammems, fromUtf8(m_Wizard->get_pos_string_and_grammems(S.m_SourceLemmaAncode)));
            set(TLemmPredictSortEnum::TypeDict, fromUtf8(m_Wizard->get_grammem_string(S.m_SourceCommonAncode)));
            set(TLemmPredictSortEnum::Lemma, fromUtf8(S.m_SourceLemma));
            set(TLemmPredictSortEnum::Freq, QString::number(S.m_Frequence));
            set(TLemmPredictSortEnum::Index, QString::number(i));
            set(TLemmPredictSortEnum::Paradigm, QString::number(S.m_FlexiaModelNo));
            set(TLemmPredictSortEnum::LemmaPrefix, fromUtf8(S.m_PrefixSetStr));
            set(TLemmPredictSortEnum::FormsCount, QString::number(P.m_Flexia.size()));
        }
        onFind();
        m_PredictWhat->setFocus();
    }
    catch (std::exception& e) {
        errorMessage(this, fromUtf8(e.what()));
    }
}

void WizardView::onPredictedHeaderClicked(int column) {
    m_SortPredictColumn = column;
    onPredict();
}

void WizardView::openWindow(QWidget* w, QSize size) {
    QMdiSubWindow* sub = m_MdiArea->addSubWindow(w);
    sub->resize(size);
    w->show();
    m_MdiArea->setActiveSubWindow(sub);
}

SlfEditor* WizardView::newSlfEditor() {
    return new SlfEditor(this);
}

void WizardView::registerEditor(SlfEditor* editor) {
    m_Editors.push_back(editor);
}

void WizardView::unregisterEditor(SlfEditor* editor) {
    m_Editors.erase(std::remove(m_Editors.begin(), m_Editors.end(), editor), m_Editors.end());
}

void WizardView::onAdd() {
    try {
        auto rows = m_PredictedList->selectionModel()->selectedRows();
        QString predict_what = m_PredictWhat->text();
        auto editor = newSlfEditor();
        if (rows.isEmpty()) {
            editor->m_ParadigmText = predict_what + " ";
        }
        else {
            size_t ind = rows.front().row();
            auto slf = m_Wizard->create_slf_for_lemm(m_LemmaToPredict, m_PredictedParadigms[ind].m_FlexiaModelNo, 50);
            editor->m_ParadigmText = fromUtf8(slf);
            editor->m_CommonGrammems = fromUtf8(m_Wizard->get_grammem_string(m_PredictedParadigms[ind].m_SourceCommonAncode));
            qsizetype i = predict_what.indexOf('|');
            if (i != -1) {
                editor->m_Prefixes = predict_what.left(i);
            }
        }
        editor->open();
        openWindow(editor, QSize(800, 600));
    }
    catch (std::exception& e) {
        echo(fromUtf8(e.what()));
    }
}

bool WizardView::openExistingParadigm(lemma_iterator_t it, bool runAccentWizard) {
    try {
        if (it == m_Wizard->m_LemmaToParadigm.end()) {
            return false;
        }
        std::string common_grammems, prefixes;
        std::string slf = m_Wizard->get_slf_string(it, common_grammems, prefixes, 50);

        auto editor = newSlfEditor();
        editor->m_Paradigm = it->second;
        editor->m_ParadigmText = fromUtf8(slf);
        editor->m_CommonGrammems = fromUtf8(common_grammems);
        editor->m_Prefixes = fromUtf8(prefixes);
        editor->open(runAccentWizard);
        openWindow(editor, QSize(800, 600));
        return true;
    }
    catch (std::exception& e) {
        echo(fromUtf8(e.what()));
    }
    return false;
}

//----------------------------------------------------------------------------
// saving and closing
//----------------------------------------------------------------------------
bool WizardView::isModified() const {
    if (m_Wizard->is_changed()) {
        return true;
    }
    for (auto& e : m_Editors) {
        if (e) {
            e->updateData();
            if (e->isModified()) {
                return true;
            }
        }
    }
    return false;
}

bool WizardView::save() {
    if (m_Wizard->m_ReadOnly) {
        QMessageBox::information(this, "MorphWizard", "Wizard is readonly, cannot be saved");
        return true;
    }
    try {
        WizardProgressMeter meter(*m_Wizard);
        m_Wizard->save_mrd();
    }
    catch (std::exception& e) {
        errorMessage(this, fromUtf8(e.what()));
        return false;
    }
    updateTitle();
    return true;
}

bool WizardView::saveModified() {
    QStringList modifiedLemmas;
    for (auto& e : m_Editors) {
        if (e) {
            e->updateData();
            if (e->isModified()) {
                modifiedLemmas << fromUtf8(e->getLemmaUtf8());
            }
        }
    }
    bool hasModifiedSlf = !modifiedLemmas.isEmpty();
    if (!hasModifiedSlf && !m_Wizard->is_changed()) {
        return true;
    }

    QString msg;
    if (hasModifiedSlf) {
        msg = windowTitle() + "\n\nSave all ";
        if (m_Wizard->is_changed()) {
            msg += "changes\nincluding ";
        }
        msg += "revised paradigms:\n\n" + modifiedLemmas.join(",\n") + "?";
    }
    else {
        msg = windowTitle() + "\n\nSave all changes?";
    }
    auto rn = QMessageBox::question(this, "MorphWizard", msg, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (rn == QMessageBox::Cancel) {
        return false;
    }
    if (rn == QMessageBox::Yes) {
        if (hasModifiedSlf) {
            for (auto& e : m_Editors) {
                if (e) {
                    e->saveModified();
                }
            }
        }
        if (!save()) {
            return false;
        }
    }
    return true;
}

void WizardView::closeEvent(QCloseEvent* event) {
    if (!saveModified()) {
        event->ignore();
        return;
    }
    // close all windows of this dictionary without questions
    auto editors = m_Editors;
    for (auto& e : editors) {
        if (e) {
            e->m_bShouldBeClosed = true;
            if (auto sub = qobject_cast<QMdiSubWindow*>(e->parentWidget())) {
                sub->close();
            }
        }
    }
    for (auto& w : m_ChildWindows) {
        if (w) {
            if (auto sub = qobject_cast<QMdiSubWindow*>(w->parentWidget())) {
                sub->close();
            }
        }
    }
    event->accept();
}

//----------------------------------------------------------------------------
// accent wizard
//----------------------------------------------------------------------------
void WizardView::onToolsAccentWizard() {
    AccentWizardDialog dlg(*m_Wizard, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_PartialAccentedPara = !dlg.allNonaccentedPara();
        onNextNonAccentedPara();
    }
}

void WizardView::onNextNonAccentedPara() {
    if (!openExistingParadigm(findNonAccentedPara(m_PartialAccentedPara), true)) {
        echo("No more paradigms without accents");
    }
}

lemma_iterator_t WizardView::findNonAccentedPara(bool partialAccentedPara) {
    auto it = m_Wizard->m_LemmaToParadigm.begin();
    for (; it != m_Wizard->m_LemmaToParadigm.end(); it++) {
        if (m_Wizard->HasUnknownAccents(it)) {
            if (m_Wizard->IsPartialAccented(it) || !partialAccentedPara) {
                return it;
            }
        }
    }
    return it;
}

//----------------------------------------------------------------------------
// tools
//----------------------------------------------------------------------------
void WizardView::onToolsSaveListFile() {
    QString path = QFileDialog::getSaveFileName(this, "Save current list of words", "wordlist.txt", "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    FILE* fp = openFile(path, "wb");
    if (!fp) {
        errorMessage(this, "Cannot open file " + path);
        return;
    }
    for (auto& p : m_FoundParadigms) {
        std::string lemma = lemmaWithPrefixes(*m_Wizard, p, true);
        std::string common_grams = "* " + m_Wizard->get_grammem_string(p->second.GetCommonAncodeCopy());
        std::string s = lemma + ";" + common_grams + ";" + m_Wizard->get_pos_string(p) + " " + m_Wizard->get_grammem_string(p);
        fprintf(fp, "%s\n", s.c_str());
    }
    fclose(fp);
}

void WizardView::onToolsExport() {
    QString path = QFileDialog::getSaveFileName(this, "Paradigm Export", "paradigms.slf", "Slf files (*.slf);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    try {
        FILE* fp = openFile(path, "wb");
        if (!fp) {
            errorMessage(this, "Cannot open file " + path);
            return;
        }
        {
            ProgressMeter meter;
            meter.SetMaxPos((uint32_t)m_FoundParadigms.size(), 100);
            meter.SetInfo(toUtf8("Exporting " + path + "...").c_str());

            m_Wizard->set_to_delete_false();
            for (auto& p : m_FoundParadigms) {
                CDumpParadigm P(m_Wizard.get());
                P.m_SlfStr = m_Wizard->get_slf_string(p, P.m_TypeGrammemsStr, P.m_PrefixesStr, 50);
                if (p->second.m_PrefixSetNo != UnknownPrefixSetNo) {
                    P.m_PrefixesStr = m_Wizard->get_prefix_set(p);
                }
                if (!p->second.GetCommonAncodeCopy().empty()) {
                    P.m_TypeGrammemsStr = m_Wizard->get_grammem_string(p->second.GetCommonAncodeCopy());
                }
                if (p->second.m_SessionNo != UnknownSessionNo) {
                    P.m_Session = m_Wizard->get_session(p->second.m_SessionNo);
                }
                std::string s = P.ToStringUtf8();
                fwrite(s.data(), 1, s.size(), fp);
                p->second.m_bToDelete = true;
                meter.AddPos();
            }
        }
        fclose(fp);

        if (QMessageBox::question(this, "MorphWizard", "Delete all exported paradigms?") == QMessageBox::Yes) {
            m_Wizard->delete_checked_lemms();
            onFind();
            updateTitle();
        }
    }
    catch (std::exception& e) {
        errorMessage(this, "Some error has occurred: " + fromUtf8(e.what()));
    }
}

void WizardView::onToolsImport() {
    QString path = QFileDialog::getOpenFileName(this, "Paradigm Import", "", "Slf files (*.slf);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    QMessageBox ask(QMessageBox::Question, "MorphWizard", "Test the file or import it?", QMessageBox::NoButton, this);
    auto testBtn = ask.addButton("Test", QMessageBox::AcceptRole);
    auto importBtn = ask.addButton("Import", QMessageBox::AcceptRole);
    ask.addButton(QMessageBox::Cancel);
    ask.exec();
    if (ask.clickedButton() != testBtn && ask.clickedButton() != importBtn) {
        return;
    }
    bool bTestMode = ask.clickedButton() == testBtn;

    std::string pathUtf8 = toUtf8(path);
    int line_no = 0;
    m_Wizard->m_bFullTrace = false;
    try {
        FILE* fp = openFile(path, "rb");
        if (!fp) {
            throw CExpc("Cannot open file");
        }
        LOGI << "import file " << pathUtf8;

        int paradigmCount = 0;
        std::string errors;
        {
            ProgressMeter meter;
            meter.SetFileMaxPos(fp);
            meter.SetInfo(toUtf8((bTestMode ? "Testing file " : "Importing file ") + path).c_str());

            bool bError;
            CDumpParadigm P(m_Wizard.get());
            while (P.ReadNextParadigmFromFile(fp, line_no, bError, errors)) {
                if (bError) {
                    continue;
                }
                int line_no_err = 0;
                paradigmCount++;
                try {
                    if (bTestMode) {
                        m_Wizard->check_slf(P.m_SlfStr, line_no_err);
                    }
                    else {
                        uint16_t SessionNo = m_Wizard->RegisterSession(P.m_Session);
                        m_Wizard->add_lemma_to_dict(P.m_SlfStr, P.m_TypeGrammemsStr, P.m_PrefixesStr, line_no_err, SessionNo);
                    }
                    meter.SetFilePos();
                }
                catch (std::exception& c) {
                    errors += Format("%s (%s:%i) \n", c.what(), pathUtf8.c_str(), P.m_FirstSlfLineNo + line_no_err);
                }
            }
        }
        fclose(fp);

        if (!errors.empty()) {
            QString errorFile = QFileInfo(path).path() + "/" + QFileInfo(path).completeBaseName() + ".err";
            FILE* errFp = openFile(errorFile, "wb");
            if (errFp) {
                fwrite(errors.data(), 1, errors.size(), errFp);
                fclose(errFp);
                errorMessage(this, "Errors were written to file " + errorFile);
            }
            else {
                errorMessage(this, "Cannot write errors to " + errorFile);
            }
        }
        else {
            QMessageBox::information(this, "Confirmation", QString("Successfully %1 %2 paradigms from \"%3\"")
                .arg(bTestMode ? "tested" : "imported").arg(paradigmCount).arg(path));
        }
    }
    catch (std::exception& e) {
        errorMessage(this, QString("Some error has occurred (file=%1, line=%2): %3").arg(path).arg(line_no).arg(fromUtf8(e.what())));
    }
    m_Wizard->m_bFullTrace = true;
    updateTitle();
}

//  Finds words by a list in a file. Each line has the format <Lemma>;<TypeGrammems>;<MorphPattern>
void WizardView::onToolsSelectByFile() {
    QString path = QFileDialog::getOpenFileName(this, "Find paradigms by list of words", "", "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
#ifdef _WIN32
    std::ifstream inp(path.toStdWString());
#else
    std::ifstream inp(QFile::encodeName(path).toStdString());
#endif
    if (!inp.is_open()) {
        errorMessage(this, "Cannot open file " + path);
        return;
    }
    std::string strNotFound;
    m_FoundParadigms.clear();
    std::string line;
    while (std::getline(inp, line)) {
        Trim(line);
        if (line.empty()) {
            continue;
        }
        auto items = split_string(line, ';');
        if (items.size() != 3) {
            errorMessage(this, "cannot get lemma from " + fromUtf8(line) + "; The format should be <Lemma>;<TypeGrammems>;<MorphPattern>");
            break;
        }
        std::string lemma = items[0];
        std::string grams = items[1];
        std::string posStr = items[2];
        Trim(lemma);

        std::string typeAncode;
        Trim(grams);
        if (grams != "*") {
            typeAncode = m_Wizard->m_pGramTab->GetFirstAncodeByPattern(grams);
            if (typeAncode.empty()) {
                errorMessage(this, "cannot process type grammems " + fromUtf8(grams));
                break;
            }
        }

        Trim(posStr);
        std::string firstCode = m_Wizard->m_pGramTab->GetFirstAncodeByPattern(posStr);
        if (firstCode.empty()) {
            errorMessage(this, "cannot process morph. pattern " + fromUtf8(posStr));
            break;
        }

        MakeUpperUtf8(lemma);
        bool bFound = false;
        for (auto& r : m_Wizard->find_lemm(lemma, true)) {
            std::string str_pos = m_Wizard->m_FlexiaModels[r->second.m_FlexiaModelNo].get_first_code();
            if (r->second.GetCommonAncodeCopy() == typeAncode && firstCode == str_pos
                && std::find(m_FoundParadigms.begin(), m_FoundParadigms.end(), r) == m_FoundParadigms.end()) {
                m_FoundParadigms.push_back(r);
                bFound = true;
            }
        }
        if (!bFound) {
            strNotFound += "Not found: " + line + "\n";
        }
    }

    filterFoundParadigms();
    showFoundParadigms();
    if (!strNotFound.empty()) {
        echo(fromUtf8(strNotFound));
    }
}

void WizardView::onToolsPack() {
    {
        WizardProgressMeter meter(*m_Wizard);
        meter.SetInfo("Packing...");
        m_Wizard->pack();
    }
    m_FoundModel->clear();
    m_FoundParadigms.clear();
    updateTitle();
    QMessageBox::information(this, "MorphWizard", "Packing is finished!");
}

void WizardView::onToolsSetParaNo() {
    if (m_FoundParadigms.empty()) {
        errorMessage(this, "Cannot set paradigm No to an empty found list");
        return;
    }
    int index = selectedFoundIndex();
    int paradigmNo = index == -1 ? 0 : m_FoundParadigms[index]->second.m_FlexiaModelNo;
    bool ok = false;
    paradigmNo = QInputDialog::getInt(this, "MorphWizard", "Input paradigm No:", paradigmNo, 0, 0xffff, 1, &ok);
    if (!ok) {
        return;
    }
    if (paradigmNo >= (int)m_Wizard->m_FlexiaModels.size()) {
        errorMessage(this, QString("Paradigm no %1 is not found").arg(paradigmNo));
        return;
    }
    if (QMessageBox::question(this, "MorphWizard",
        QString("The program ascribes all found words to paradigm %1 . Continue?").arg(paradigmNo),
        QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
        return;
    }

    const CFlexiaModel& new_par = m_Wizard->m_FlexiaModels[paradigmNo];
    std::string new_flexia = new_par.get_first_flex();
    size_t errorsCount = 0;
    for (auto& p : m_FoundParadigms) {
        if (p->second.m_FlexiaModelNo == paradigmNo) {
            continue;
        }
        std::string lemma = m_Wizard->get_lemm_string(p);
        if (lemma.length() >= new_flexia.length() && lemma.substr(lemma.length() - new_flexia.length()) == new_flexia) {
            m_Wizard->change_prd_info(p->second, lemma, (uint16_t)paradigmNo, UnknownAccentModelNo, true);
        }
        else {
            errorsCount++;
        }
    }
    if (errorsCount > 0) {
        errorMessage(this, QString("This last operation cannot change lemma in %1 cases.").arg(errorsCount));
    }
    showFoundParadigms();
    updateTitle();
}

void WizardView::onToolsSetAccentModelNo() {
    if (m_FoundParadigms.empty()) {
        errorMessage(this, "Cannot set accent model to an empty found list");
        return;
    }
    int index = selectedFoundIndex();
    int accentModelNo = index == -1 ? 0 : m_FoundParadigms[index]->second.m_AccentModelNo;
    bool ok = false;
    accentModelNo = QInputDialog::getInt(this, "MorphWizard", "Input accent model No:", accentModelNo, 0, 0xffff, 1, &ok);
    if (!ok) {
        return;
    }
    if (accentModelNo >= (int)m_Wizard->m_AccentModels.size()) {
        errorMessage(this, QString("accent model no %1 is not found").arg(accentModelNo));
        return;
    }
    std::set<std::pair<uint16_t, uint16_t>> flexia_model_to_accent_model;
    for (auto const& p : m_Wizard->m_LemmaToParadigm) {
        flexia_model_to_accent_model.insert({ p.second.m_FlexiaModelNo, p.second.m_AccentModelNo });
    }
    for (auto const& p : m_FoundParadigms) {
        auto flexia_model_no = p->second.m_FlexiaModelNo;
        if (flexia_model_to_accent_model.find({ flexia_model_no, (uint16_t)accentModelNo }) == flexia_model_to_accent_model.end()) {
            errorMessage(this, QString("This operation would set accent model no %1 to a word with flexia model no %2, "
                "this association is unknown. The operation was canceled ").arg(accentModelNo).arg(flexia_model_no));
            return;
        }
    }
    if (QMessageBox::question(this, "MorphWizard",
        QString("The program ascribes all found words to accent model %1 . Continue?").arg(accentModelNo),
        QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
        return;
    }
    for (auto& p : m_FoundParadigms) {
        p->second.m_AccentModelNo = (uint16_t)accentModelNo;
    }
    showFoundParadigms();
}

void WizardView::onToolsShowParadigmDifferences() {
    auto w = new ParadigmDiffView(this);
    m_ChildWindows.push_back(w);
    openWindow(w, QSize(700, 450));
}

void WizardView::onToolsShowAccentDifferences() {
    auto w = new AccentModelDiffView(this);
    m_ChildWindows.push_back(w);
    openWindow(w, QSize(700, 450));
}
