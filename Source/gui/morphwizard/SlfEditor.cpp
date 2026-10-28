#include "SlfEditor.h"
#include "Dialogs.h"
#include "WizardView.h"
#include "QtUtil.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QRegularExpression>
#include <QSplitter>
#include <QTextBlock>
#include <QTimer>
#include <QVBoxLayout>

static void errorMessage(QWidget* parent, const QString& s) {
    QMessageBox::critical(parent, "MorphWizard", s);
}

static QString firstWord(const QString& text) {
    QString t = text.trimmed();
    qsizetype i = 0;
    while (i < t.length() && !t[i].isSpace()) {
        ++i;
    }
    return t.left(i);
}

//----------------------------------------------------------------------------
// SlfHighlighter
//----------------------------------------------------------------------------
SlfHighlighter::SlfHighlighter(QTextDocument* doc, const MorphoWizard& wizard)
    : QSyntaxHighlighter(doc)
{
    for (auto& s : const_cast<MorphoWizard&>(wizard).get_poses()) {
        m_Poses.insert(fromUtf8(s));
    }
    for (auto& s : const_cast<MorphoWizard&>(wizard).get_grammems()) {
        m_Grammems.insert(fromUtf8(s));
    }
}

static bool isKeywordChar(QChar c) {
    return c.isLetter() || c == '_';
}

void SlfHighlighter::highlightBlock(const QString& text) {
    // the word form: bold, with a background if it has no accent
    qsizetype wordStart = 0;
    while (wordStart < text.length() && text[wordStart] == ' ') {
        ++wordStart;
    }
    if (wordStart < text.length()) {
        qsizetype i = wordStart;
        bool hasAccent = false;
        for (; i < text.length() && text[i] != ' '; ++i) {
            if (text[i] == '\'') {
                hasAccent = true;
            }
        }
        QTextCharFormat f;
        f.setFontWeight(QFont::Bold);
        if (!hasAccent) {
            f.setBackground(QColor(255, 230, 230));
            f.setForeground(Qt::black);
        }
        setFormat((int)wordStart, (int)(i - wordStart), f);
    }

    // parts of speech and grammems
    const qsizetype MaxKeyWordLen = 50;
    for (qsizetype x = 0; x < text.length(); ++x) {
        qsizetype end = x;
        while (end < text.length() && isKeywordChar(text[end])) {
            ++end;
        }
        if (end == x) {
            continue;
        }
        if (end - x < MaxKeyWordLen) {
            QString word = text.mid(x, end - x);
            QTextCharFormat f;
            if (m_Poses.contains(word)) {
                f.setForeground(QColor(255, 0, 0));
                setFormat((int)x, (int)(end - x), f);
            }
            else if (m_Grammems.contains(word)) {
                f.setForeground(QColor(0, 0, 255));
                setFormat((int)x, (int)(end - x), f);
            }
        }
        x = end;
    }
}


//----------------------------------------------------------------------------
// ParadigmEdit
//----------------------------------------------------------------------------
ParadigmEdit::ParadigmEdit(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(11);
    setFont(f);
}

void ParadigmEdit::setAccentByIndex(int ind) {
    QString text = toPlainText();
    if (ind < 0 || ind >= text.length() - 1) {
        return;
    }
    int wordStart = ind;
    while (wordStart > 0 && !text[wordStart].isSpace()) {
        wordStart--;
    }
    int wordEnd = ind;
    while (wordEnd < text.length() && !text[wordEnd].isSpace()) {
        wordEnd++;
    }

    QTextCursor c = textCursor();
    c.beginEditBlock();
    int accOld = (int)text.indexOf('\'', wordStart);
    if (accOld >= 0 && accOld < wordEnd) {
        if (accOld < ind) {
            --ind;
        }
        c.setPosition(accOld);
        c.setPosition(accOld + 1, QTextCursor::KeepAnchor);
        c.removeSelectedText();
    }
    c.setPosition(ind + 1);
    c.insertText("'");
    c.endEditBlock();
}

void ParadigmEdit::mouseDoubleClickEvent(QMouseEvent* e) {
    // find the char under the mouse pointer, not the nearest char boundary
    QTextCursor c = cursorForPosition(e->position().toPoint());
    int ind = c.position();
    if (cursorRect(c).left() > e->position().toPoint().x()) {
        --ind;
    }
    setAccentByIndex(ind);
}

void ParadigmEdit::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_F3) {
        setAccentFromThisLineToTheEndOfParadigm();
        return;
    }
    QPlainTextEdit::keyPressEvent(e);
}

// copies the accent position of the current word form to all word forms below it
void ParadigmEdit::setAccentFromThisLineToTheEndOfParadigm() {
    QTextBlock current = textCursor().block();
    int line_no = current.blockNumber();
    QString word = current.text();
    qsizetype space = word.indexOf(' ');
    if (space != -1) {
        word = word.left(space);
    }
    qsizetype acc = word.indexOf('\'');
    if (acc <= 0) {
        return;
    }

    QStringList lines = toPlainText().split('\n');
    for (qsizetype i = line_no + 1; i < lines.size(); ++i) {
        qsizetype space_index = lines[i].indexOf(QRegularExpression("[ \t]"));
        if (space_index == -1) {
            continue;
        }
        QString wrd = lines[i].left(space_index);
        wrd.remove('\'');
        wrd.insert(std::min(acc, wrd.length()), '\'');
        lines[i] = wrd + lines[i].mid(space_index);
    }

    int pos = textCursor().position();
    QTextCursor c = textCursor();
    c.beginEditBlock();
    c.select(QTextCursor::Document);
    c.insertText(lines.join('\n'));
    c.endEditBlock();
    c.setPosition(std::min(pos, (int)toPlainText().length()));
    setTextCursor(c);
}


//----------------------------------------------------------------------------
// SlfEditor
//----------------------------------------------------------------------------
SlfEditor::SlfEditor(WizardView* parent)
    : m_pParent(parent)
{
    m_pParent->registerEditor(this);
}

SlfEditor::~SlfEditor() {
    if (m_pParent) {
        m_pParent->unregisterEditor(this);
    }
}

MorphoWizard* SlfEditor::GetWizard() const {
    return m_pParent->GetWizard();
}

std::string SlfEditor::getSavedLemma() const {
    // without accents: MorphoWizard::find_lemm cannot find an accented utf-8 lemma
    // (it compares a byte position with a char position of the accent)
    std::string lemma = toUtf8(firstWord(m_SaveParadigmText).remove('\''));
    MakeUpperUtf8(lemma);
    return lemma;
}

std::string SlfEditor::getLemmaUtf8() const {
    std::string lemma = toUtf8(firstWord(m_ParadigmText).remove('\''));
    MakeUpperUtf8(lemma);
    return lemma;
}

std::string SlfEditor::getBase() const {
    std::string lemma = getLemmaUtf8();
    if (m_Paradigm.m_FlexiaModelNo != UnknownFlexiaModelNo) {
        std::string flexia = GetWizard()->m_FlexiaModels[m_Paradigm.m_FlexiaModelNo].get_first_flex();
        if (flexia.length() <= lemma.length()) {
            return lemma.substr(0, lemma.length() - flexia.length());
        }
    }
    return "";
}

void SlfEditor::createControls() {
    auto layout = new QVBoxLayout(this);

    // the first line
    auto top = new QHBoxLayout();
    top->addWidget(new QLabel("Type grammems:", this));
    m_TypeGrammems = new QComboBox(this);
    m_TypeGrammems->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    top->addWidget(m_TypeGrammems);
    top->addWidget(new QLabel("Comments:", this));
    m_Comments = new QLineEdit(this);
    m_Comments->setReadOnly(true);
    top->addWidget(m_Comments, 1);
    auto fixAccentBtn = new QPushButton("Fix Accent", this);
    fixAccentBtn->setToolTip("Copy the accent of the current line to the following lines (F3)");
    auto okBtn = new QPushButton("OK", this);
    okBtn->setDefault(true);
    auto cancelBtn = new QPushButton("Cancel", this);
    top->addWidget(fixAccentBtn);
    top->addWidget(okBtn);
    top->addWidget(cancelBtn);
    layout->addLayout(top);

    // the text and the lists of parts of speech and grammems
    auto splitter = new QSplitter(this);
    m_Edit = new ParadigmEdit(splitter);
    new SlfHighlighter(m_Edit->document(), *GetWizard());
    splitter->addWidget(m_Edit);

    auto right = new QWidget(splitter);
    auto rightLayout = new QGridLayout(right);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    auto changeParadigmBtn = new QPushButton("Set Para No", right);
    auto changeAllBtn = new QPushButton("Change All", right);
    rightLayout->addWidget(changeAllBtn, 0, 0);
    rightLayout->addWidget(changeParadigmBtn, 0, 1);
    rightLayout->addWidget(new QLabel("POS", right), 1, 0);
    rightLayout->addWidget(new QLabel("Grammems", right), 1, 1);
    m_PosList = new QListWidget(right);
    m_PosList->setSortingEnabled(true);
    m_GramList = new QListWidget(right);
    m_GramList->setSortingEnabled(true);
    rightLayout->addWidget(m_PosList, 2, 0);
    rightLayout->addWidget(m_GramList, 2, 1);
    splitter->addWidget(right);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter, 1);

    // the last line
    auto bottom = new QHBoxLayout();
    bottom->addWidget(new QLabel("Prefixes:", this));
    m_PrefixesEdit = new QLineEdit(this);
    bottom->addWidget(m_PrefixesEdit, 1);
    auto addPrefixBtn = new QPushButton("Add Prefix", this);
    bottom->addWidget(addPrefixBtn);
    bottom->addSpacing(20);
    auto createInfoLabel = [this]() {
        auto l = new QLabel(this);
        l->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        l->setMinimumWidth(50);
        l->setTextInteractionFlags(Qt::TextSelectableByMouse);
        return l;
    };
    bottom->addWidget(new QLabel("Base", this));
    m_BaseLabel = createInfoLabel();
    bottom->addWidget(m_BaseLabel);
    bottom->addWidget(new QLabel("Flex", this));
    m_PrdNoLabel = createInfoLabel();
    bottom->addWidget(m_PrdNoLabel);
    bottom->addWidget(new QLabel("Accent", this));
    m_AccMdlNoLabel = createInfoLabel();
    bottom->addWidget(m_AccMdlNoLabel);
    layout->addLayout(bottom);

    for (auto& s : GetWizard()->get_poses()) {
        m_PosList->addItem(fromUtf8(s));
    }
    for (auto& s : GetWizard()->get_grammems()) {
        m_GramList->addItem(fromUtf8(s) + ",");
    }
    QStringList typeGrammems;
    for (auto& s : GetWizard()->get_type_grammems()) {
        typeGrammems << fromUtf8(s);
    }
    typeGrammems << "";
    typeGrammems.sort();
    m_TypeGrammems->addItems(typeGrammems);

    connect(okBtn, &QPushButton::clicked, this, &SlfEditor::onOk);
    connect(cancelBtn, &QPushButton::clicked, this, &SlfEditor::onCancel);
    connect(fixAccentBtn, &QPushButton::clicked, this, [this]() {
        m_Edit->setAccentFromThisLineToTheEndOfParadigm();
        m_Edit->setFocus();
    });
    connect(changeAllBtn, &QPushButton::clicked, this, &SlfEditor::onChangeAll);
    connect(changeParadigmBtn, &QPushButton::clicked, this, &SlfEditor::onChangeParadigm);
    connect(addPrefixBtn, &QPushButton::clicked, this, &SlfEditor::onAddPrefix);
    connect(m_PosList, &QListWidget::itemDoubleClicked, this, &SlfEditor::onPosDoubleClicked);
    connect(m_GramList, &QListWidget::itemDoubleClicked, this, &SlfEditor::onGramDoubleClicked);
}

void SlfEditor::updateTypeGrammemsCombo() {
    int index = m_TypeGrammems->findText(m_CommonGrammems);
    if (index == -1) {
        if (m_TypeGrammems->count() != 0) {
            errorMessage(this, "Bad type grammems: " + m_CommonGrammems);
        }
    }
    else {
        m_TypeGrammems->setCurrentIndex(index);
    }
}

void SlfEditor::updateData(bool save) {
    if (!m_Edit) {
        return;
    }
    if (save) {
        m_ParadigmText = m_Edit->toPlainText();
        m_CommonGrammems = m_TypeGrammems->currentText();
        m_Prefixes = m_PrefixesEdit->text();
    }
    else {
        m_Edit->setPlainText(m_ParadigmText);
        updateTypeGrammemsCombo();
        m_PrefixesEdit->setText(m_Prefixes);
        m_Comments->setText(m_ParadigmComments);
        m_PrdNoLabel->setText(QString::number(m_Paradigm.m_FlexiaModelNo));
        m_AccMdlNoLabel->setText(QString::number(m_Paradigm.m_AccentModelNo));
        m_BaseLabel->setText(fromUtf8(getBase()));
    }
}

void SlfEditor::updateSavedParadigm() {
    m_ParadigmText = m_ParadigmText.trimmed() + "\n";
    if (m_ParadigmText == "\n") {
        m_ParadigmText.clear();
    }
    m_SaveParadigmText = m_ParadigmText;
    m_SaveParadigm = m_Paradigm;
    m_SaveCommonGrammems = m_CommonGrammems;
    m_SavePrefixes = m_Prefixes;
    if (m_Paradigm.m_FlexiaModelNo != UnknownFlexiaModelNo) {
        m_ParadigmComments = fromUtf8(GetWizard()->m_FlexiaModels[m_Paradigm.m_FlexiaModelNo].m_Comments);
    }
    QString title = m_SaveParadigmText.isEmpty() ? QString("new paradigm") : fromUtf8(getSavedLemma());
    setWindowTitle(title);
}

void SlfEditor::open(bool runAccentWizard) {
    m_bRunAccentWizard = runAccentWizard;
    m_ParadigmText.remove('\r');
    // a new paradigm is "modified" anyway, since its lemma cannot be found in the dictionary
    updateSavedParadigm();
    createControls();
    updateData(false);

    if (m_GotoLine != -1) {
        QTextBlock b = m_Edit->document()->findBlockByNumber(m_GotoLine);
        if (b.isValid()) {
            QTextCursor c(b);
            c.select(QTextCursor::LineUnderCursor);
            m_Edit->setTextCursor(c);
        }
    }
    else {
        m_Edit->moveCursor(QTextCursor::Start);
    }
    m_Edit->setFocus();
}

bool SlfEditor::isModified() {
    if (m_bShouldBeClosed) {
        return false;
    }

    // if the text of the saved paradigm and the edited paradigm differ, then "it was modified"
    if (m_SaveParadigmText.trimmed() != m_ParadigmText.trimmed()
        || m_SaveCommonGrammems != m_CommonGrammems
        || m_SavePrefixes != m_Prefixes) {
        return true;
    }

    // if a new lemma was edited, then there is no sense to search it in the dictionary
    if (m_SaveParadigmText.isEmpty()) {
        return true;
    }

    // the saved lemma can be deleted while an editor for this lemma is still open.
    // In this case we should say to the user that this paradigm "was modified"
    // and propose to insert it into the dictionary.
    for (auto& r : GetWizard()->find_lemm(getSavedLemma(), false)) {
        if (r->second == m_SaveParadigm) {
            return false;
        }
    }
    return true;
}

bool SlfEditor::saveModified() {
    if (!isModified()) {
        return true;
    }

    m_ParadigmText = m_ParadigmText.trimmed();
    if (m_ParadigmText.isEmpty()) {
        return true;
    }

    try {
        std::string slf = toUtf8(m_ParadigmText);
        std::string common_grammems = toUtf8(m_CommonGrammems);
        std::string prefixes = toUtf8(m_Prefixes);

        //  try to parse a paradigm (waiting for an exception otherwise)
        GetWizard()->check_slf(slf, m_GotoLine);
        if (!GetWizard()->check_common_grammems(common_grammems)) {
            throw CExpc("Bad type grammems");
        }
        if (!GetWizard()->check_prefixes(prefixes)) {
            throw CExpc("Bad prefixes");
        }

        // deleting the old lemma
        if (!m_SaveParadigmText.isEmpty()) {
            for (auto& r : GetWizard()->find_lemm(getSavedLemma(), false)) {
                if (r->second == m_SaveParadigm) {
                    GetWizard()->remove_lemm(r);
                    break;
                }
            }
        }

        // writing
        m_Paradigm = GetWizard()->add_lemma_to_dict(slf, common_grammems, prefixes, m_GotoLine);
        updateSavedParadigm();
        m_pParent->updateTitle();
    }
    catch (std::exception& e) {
        auto mess = QString("%1 at line %2\nExit without save?").arg(fromUtf8(e.what())).arg(m_GotoLine);
        if (QMessageBox::question(this, "MorphWizard", mess, QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void SlfEditor::closeEvent(QCloseEvent* event) {
    updateData();
    if (isModified()) {
        if (QMessageBox::question(this, "MorphWizard", "Exit without save?", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

static void closeEditorWindow(SlfEditor* editor) {
    // the editor is inside an mdi subwindow
    if (auto sub = qobject_cast<QMdiSubWindow*>(editor->parentWidget())) {
        sub->close();
    }
    else {
        editor->close();
    }
}

void SlfEditor::onOk() {
    updateData();
    if (saveModified()) {
        WizardView* parent = m_pParent;
        if (!isModified()) {
            if (m_bRunAccentWizard) {
                QTimer::singleShot(0, parent, &WizardView::onNextNonAccentedPara);
            }
            else {
                parent->onFind();
            }
        }
        m_bShouldBeClosed = true;
        closeEditorWindow(this);
    }
}

void SlfEditor::onCancel() {
    closeEditorWindow(this);
}

void SlfEditor::onPosDoubleClicked() {
    auto item = m_PosList->currentItem();
    if (!item) {
        return;
    }
    m_Edit->insertPlainText(item->text() + " ");
    m_Edit->setFocus();
}

void SlfEditor::onGramDoubleClicked() {
    auto item = m_GramList->currentItem();
    if (!item) {
        return;
    }
    m_Edit->insertPlainText(item->text());
    m_Edit->setFocus();
}

void SlfEditor::onAddPrefix() {
    updateData();
    bool ok = false;
    QString prefix = QInputDialog::getText(this, "MorphWizard", "Input prefix:", QLineEdit::Normal, "", &ok);
    if (!ok || prefix.isEmpty()) {
        return;
    }
    QString new_slf;
    for (QString line : m_ParadigmText.split('\n')) {
        line = line.trimmed();
        if (line.isEmpty()) {
            continue;
        }
        new_slf += prefix + line + "\n";
    }
    m_ParadigmText = new_slf;
    updateData(false);
}

void SlfEditor::onChangeParadigm() {
    updateData();
    bool ok = false;
    QString ss = QInputDialog::getText(this, "MorphWizard", "Input prototype lemma or paradigm number: ", QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }
    std::string s = toUtf8(ss.trimmed());
    MakeUpperUtf8(s);
    if (s.empty()) {
        return;
    }

    size_t ParadigmNo;
    if (isdigit((BYTE)s[0])) {
        ParadigmNo = atoi(s.c_str());
    }
    else {
        auto found_paradigms = GetWizard()->find_lemm(s, false);
        if (found_paradigms.empty()) {
            errorMessage(this, fromUtf8(s) + " is not found");
            return;
        }
        ParadigmNo = found_paradigms[0]->second.m_FlexiaModelNo;
    }
    if (ParadigmNo >= GetWizard()->m_FlexiaModels.size()) {
        errorMessage(this, QString("Paradigm no %1 is not found").arg(ParadigmNo));
        return;
    }

    std::string Lemma = getLemmaUtf8();
    const CFlexiaModel& new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
    if (m_Paradigm.m_FlexiaModelNo != UnknownFlexiaModelNo) {
        std::string new_flexia = new_par.get_first_flex();
        std::string lemma_suffix = Lemma.length() >= new_flexia.length() ? Lemma.substr(Lemma.length() - new_flexia.length()) : "";
        MakeUpperUtf8(lemma_suffix);
        if (new_flexia != lemma_suffix) {
            errorMessage(this, "Your choice will change lemma of the edited word. Is is not allowed.");
            return;
        }
    }
    GetWizard()->change_prd_info(m_Paradigm, Lemma, (uint16_t)ParadigmNo, UnknownAccentModelNo, true);
    auto paradigm = GetWizard()->mrd_to_slf(Lemma, new_par, m_Paradigm.m_AccentModelNo, m_Paradigm.m_AuxAccent, 50);
    m_ParadigmText = fromUtf8(paradigm).remove('\r');
    updateData(false);
}

void SlfEditor::onChangeAll() {
    updateData();

    CParadigmInfo paraOld = m_SaveParadigm;
    WizardView* parent = m_pParent;

    if (isModified()) {
        if (!saveModified()) {
            return;
        }
        if (isModified()) {
            m_bShouldBeClosed = true;
            closeEditorWindow(this);
            return;
        }
    }

    uint16_t ParadigmNo = m_Paradigm.m_FlexiaModelNo;
    uint16_t AccentMdlNo = m_Paradigm.m_AccentModelNo;
    if (ParadigmNo >= GetWizard()->m_FlexiaModels.size()) {
        return;
    }

    std::vector<lemma_iterator_t> found_paradigms;
    GetWizard()->find_lemm_by_prdno(paraOld.m_FlexiaModelNo, found_paradigms);

    // remove the template paradigm from the list (only accents were changed)
    if (paraOld.m_FlexiaModelNo == ParadigmNo) {
        for (size_t i = 0; i < found_paradigms.size(); ++i) {
            if (found_paradigms[i]->second == m_Paradigm && found_paradigms[i]->first == getSavedLemma()) {
                found_paradigms.erase(found_paradigms.begin() + i);
                break;
            }
        }
    }

    if (!found_paradigms.empty()) {
        ChangeAllDialog dlg(*GetWizard(), found_paradigms, paraOld.m_AccentModelNo, AccentMdlNo, this);
        if (dlg.exec() == QDialog::Accepted) {
            if (dlg.m_usedAccentModel == 0) {
                AccentMdlNo = paraOld.m_AccentModelNo;
            }
            bool keepAccentModel = (dlg.m_usedAccentModel != 2);
            const CFlexiaModel& new_par = GetWizard()->m_FlexiaModels[ParadigmNo];
            size_t ErrorsCount = 0;
            for (auto& p : found_paradigms) {
                if (dlg.m_anyAccentModels == 0 && p->second.m_AccentModelNo != paraOld.m_AccentModelNo) {
                    continue;
                }
                std::string new_flexia = new_par.get_first_flex();
                std::string lemma = GetWizard()->get_lemm_string(p);
                if (lemma.length() >= new_flexia.length() && lemma.substr(lemma.length() - new_flexia.length()) == new_flexia) {
                    GetWizard()->change_prd_info(p->second, lemma, ParadigmNo, AccentMdlNo, keepAccentModel);
                }
                else {
                    ErrorsCount++;
                }
            }
            if (ErrorsCount > 0) {
                errorMessage(this, QString("This last operation cannot change lemma in %1 cases.").arg(ErrorsCount));
            }
        }
    }

    if (m_bRunAccentWizard) {
        QTimer::singleShot(0, parent, &WizardView::onNextNonAccentedPara);
    }
    else {
        parent->onFind();
    }
    m_bShouldBeClosed = true;
    closeEditorWindow(this);
}
