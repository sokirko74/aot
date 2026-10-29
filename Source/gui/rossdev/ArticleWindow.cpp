#include "ArticleWindow.h"
#include "RossDictionary.h"
#include "MainWindow.h"
#include "HierarchyWindow.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBlock>
#include <QVBoxLayout>

ArticleHighlighter::ArticleHighlighter(QTextDocument* doc, RossDictionary* dict) : QSyntaxHighlighter(doc), m_Dict(dict) {
}

static bool isTokenChar(QChar c) {
    return c.unicode() > 127 || c.isLetterOrNumber();
}

// colors: entries of open dictionaries are blue, field names are gray
// (items of metalanguage domains are not colored, as in the MFC version, where domain colors were lost)
void ArticleHighlighter::highlightBlock(const QString& text) {
    QTextCharFormat entryFormat;
    entryFormat.setForeground(QColor(0, 0, 255));
    QTextCharFormat fieldFormat;
    fieldFormat.setForeground(QColor(90, 90, 90));
    for (int x = 0; x < text.length(); ) {
        int end = x;
        while (end < text.length() && isTokenChar(text[end])) {
            end++;
        }
        if (end == x) {
            x++;
            continue;
        }
        std::string word = toUtf8(text.mid(x, end - x));
        auto it = m_Dict->m_BasicDomItems.find(word);
        if (it == m_Dict->m_BasicDomItems.end() || m_Dict->GetRoss()->m_Domens[it->second].GetDomStr() == "D_1") {
            if (RossDevCore::instance().isRossEntry(word)) {
                setFormat(x, end - x, entryFormat);
            }
            else if (m_Dict->m_Fields.count(word)) {
                setFormat(x, end - x, fieldFormat);
            }
        }
        x = end;
    }
}

//----------------------------------------------------------------------------

static bool canBeEntrySymbol(QChar c) {
    return (c.unicode() < 128 && c.isLetter()) || c.unicode() > 127 || c == '_' || c == '-';
}

// opens the article of the clicked word (the sense number is the first digit after the word)
void ArticleEdit::mouseDoubleClickEvent(QMouseEvent* e) {
    QPlainTextEdit::mouseDoubleClickEvent(e);
    QTextCursor cursor = cursorForPosition(e->pos());
    QString line = cursor.block().text();
    int pos = cursor.positionInBlock();
    if (pos >= line.length() || !canBeEntrySymbol(line[pos])) {
        return;
    }
    int start = pos;
    while (start > 0 && canBeEntrySymbol(line[start - 1])) {
        start--;
    }
    int end = pos;
    while (end < line.length() && canBeEntrySymbol(line[end])) {
        end++;
    }
    std::string word = toUtf8(line.mid(start, end - start));
    BYTE meanNum = 1;
    for (int i = end; i < line.length(); i++) {
        if (line[i].isDigit()) {
            meanNum = line[i].digitValue();
            break;
        }
    }
    for (auto d : RossDevCore::instance().dictionaries()) {
        uint16_t unitNo = d->GetRoss()->LocateUnit(word.c_str(), meanNum);
        if (unitNo != ErrUnitNo) {
            MainWindow::instance()->openArticle(d, unitNo);
            return;
        }
    }
}

//----------------------------------------------------------------------------

FieldHintPanel::FieldHintPanel(RossDictionary* dict, QWidget* parent) : QWidget(parent), m_Dict(dict) {
    m_Title = new QLabel(this);
    m_Title->setWordWrap(true);
    m_Signats = new QListWidget(this);
    m_Signats->setToolTip("Signatures of the field (sequences of domains)");
    m_Signats->setMaximumHeight(120);
    m_Domains = new QComboBox(this);
    m_Domains->setToolTip("Domains of the signature");
    m_Filter = new QLineEdit(this);
    m_Filter->setPlaceholderText("Filter");
    m_Items = new QListWidget(this);
    m_Items->setToolTip("Double click inserts the item into the article");
    connect(m_Signats, &QListWidget::currentRowChanged, this, &FieldHintPanel::onSignatChanged);
    connect(m_Domains, &QComboBox::currentIndexChanged, this, &FieldHintPanel::onDomainChanged);
    connect(m_Filter, &QLineEdit::textChanged, this, &FieldHintPanel::onFilterChanged);
    connect(m_Items, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item->flags() & Qt::ItemIsEnabled) {
            emit insertText(item->text() + " ");
        }
    });
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_Title);
    layout->addWidget(m_Signats);
    layout->addWidget(m_Domains);
    layout->addWidget(m_Filter);
    layout->addWidget(m_Items, 1);
    showField(QString());
}

// the field number by a field name with an actant number ("GF1", "SF2(1)", "GF*")
static BYTE findField(const CDictionary* ross, QString name) {
    BYTE fieldNo = ross->GetFieldNoByFieldStr(toUtf8(name).c_str());
    if (fieldNo != ErrUChar) {
        return fieldNo;
    }
    int bracket = name.indexOf('(');
    if (bracket != -1) {
        name = name.left(bracket);
    }
    while (!name.isEmpty() && (name.back().isDigit() || name.back() == '*')) {
        name.chop(1);
    }
    return ross->GetFieldNoByFieldStr(toUtf8(name).c_str());
}

void FieldHintPanel::showField(const QString& fieldName) {
    if (fieldName == m_FieldName && !fieldName.isEmpty()) {
        return;
    }
    m_FieldName = fieldName;
    m_Filter->clear();
    const CDictionary* ross = m_Dict->GetRoss();
    m_FieldNo = fieldName.isEmpty() ? ErrUChar : findField(ross, fieldName);
    m_Signats->clear();
    if (m_FieldNo == ErrUChar) {
        m_Title->setText(fieldName.isEmpty() ? "Put the cursor into a field" : "Unknown field " + fieldName);
        onSignatChanged(-1);
        return;
    }
    m_Title->setText("<b>" + fieldName + "</b>");
    for (auto& signat : ross->Fields[m_FieldNo].m_Signats) {
        QStringList parts;
        for (auto& item : signat.GetDomsWithDelims()) {
            const CDomen& D = ross->m_Domens[item.m_DomNo];
            // a delimiter is shown as it is
            parts << (D.DomainIsDelim() && !D.IsEmpty() ? fromUtf8(D.GetFirstDomStr()) : fromUtf8(D.GetDomStr()));
        }
        m_Signats->addItem(parts.join(" "));
    }
    m_Signats->setCurrentRow(0);
}

void FieldHintPanel::onSignatChanged(int row) {
    m_Domains->blockSignals(true);
    m_Domains->clear();
    if (m_FieldNo != ErrUChar && row >= 0 && row < (int)m_Dict->GetRoss()->Fields[m_FieldNo].m_Signats.size()) {
        const CDictionary* ross = m_Dict->GetRoss();
        for (auto& item : ross->Fields[m_FieldNo].m_Signats[row].GetDomsWithDelims()) {
            const CDomen& D = ross->m_Domens[item.m_DomNo];
            if (D.DomainIsDelim()) {
                continue;
            }
            m_Domains->addItem(fromUtf8(D.GetDomStr()), item.m_DomNo);
            // the parts of a union domain
            for (BYTE part : D.GetParts()) {
                m_Domains->addItem("    " + fromUtf8(ross->m_Domens[part].GetDomStr()), part);
            }
        }
    }
    m_Domains->blockSignals(false);
    onDomainChanged(m_Domains->currentIndex());
}

void FieldHintPanel::onDomainChanged(int index) {
    m_Items->clear();
    if (index < 0) {
        return;
    }
    const CDictionary* ross = m_Dict->GetRoss();
    const CDomen& D = ross->m_Domens[m_Domains->itemData(index).toInt()];
    // a union domain contains the items of its parts
    std::vector<const CDomen*> domains;
    if (D.GetDomainSource() == dsUnion) {
        for (BYTE part : D.GetParts()) {
            domains.push_back(&ross->m_Domens[part]);
        }
    }
    else {
        domains.push_back(&D);
    }
    QStringList items;
    bool freeText = false;
    for (auto d : domains) {
        if (d->IsFree || d->GetDomainSource() == dsText) {
            freeText = true;
            continue;
        }
        for (auto& s : d->GetAllStrings()) {
            items << fromUtf8(s);
        }
    }
    if (freeText) {
        auto item = new QListWidgetItem("(free text)", m_Items);
        item->setFlags(Qt::NoItemFlags);
    }
    items.sort();
    items.removeDuplicates();
    m_Items->addItems(items);
    onFilterChanged(m_Filter->text());
}

void FieldHintPanel::onFilterChanged(const QString& text) {
    for (int i = 0; i < m_Items->count(); i++) {
        auto item = m_Items->item(i);
        item->setHidden(!text.isEmpty() && (item->flags() & Qt::ItemIsEnabled) && !item->text().contains(text, Qt::CaseInsensitive));
    }
}

//----------------------------------------------------------------------------

ArticleWindow::ArticleWindow(RossDictionary* dict, uint16_t unitNo, QWidget* parent)
    : QWidget(parent), m_Dict(dict), m_UnitNo(unitNo), m_Article(dict->GetRoss()) {
    setAttribute(Qt::WA_DeleteOnClose);

    m_Edit = new ArticleEdit(this);
    QFont f("Courier New");
    f.setStyleHint(QFont::Monospace);
    f.setPointSize(11);
    m_Edit->setFont(f);
    m_Edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    new ArticleHighlighter(m_Edit->document(), dict);

    m_FieldScroll = new QScrollBar(Qt::Vertical, this);
    m_FieldScroll->setToolTip("Go to a field of the article");
    connect(m_FieldScroll, &QScrollBar::valueChanged, this, &ArticleWindow::setFocusToField);

    auto buttons = new QVBoxLayout();
    auto addButton = [&](const char* title, const char* tip, void (ArticleWindow::*slot)()) {
        auto b = new QPushButton(title, this);
        b->setToolTip(tip);
        connect(b, &QPushButton::clicked, this, slot);
        buttons->addWidget(b);
        return b;
    };
    addButton("Text", "Edit the article as a plain text", &ArticleWindow::onTextMode);
    addButton("Mark Up", "Check the article and format it", &ArticleWindow::onMarkup);
    auto saveBtn = new QPushButton("Save", this);
    saveBtn->setToolTip("Write the article to the dictionary (Ctrl+S)");
    saveBtn->setShortcut(QKeySequence::Save);
    connect(saveBtn, &QPushButton::clicked, this, [this]() { saveArticle(); });
    buttons->addWidget(saveBtn);
    addButton("Add Field", "Add new fields to the end of the article", &ArticleWindow::onAddField);
    addButton("Delete Field", "Delete the current field", &ArticleWindow::onDeleteField);
    m_Hint = new FieldHintPanel(dict, this);
    m_Hint->setMinimumWidth(220);
    buttons->addWidget(m_Hint, 1);
    connect(m_Hint, &FieldHintPanel::insertText, this, [this](const QString& text) {
        // items are separated by spaces
        QTextCursor cursor = m_Edit->textCursor();
        int pos = cursor.positionInBlock();
        if (pos > 0 && !cursor.block().text()[pos - 1].isSpace()) {
            m_Edit->insertPlainText(" ");
        }
        m_Edit->insertPlainText(text);
        m_Edit->setFocus();
    });
    connect(m_Edit, &QPlainTextEdit::cursorPositionChanged, this, [this]() { m_Hint->showField(currentFieldName()); });

    auto layout = new QHBoxLayout(this);
    layout->addWidget(m_Edit, 1);
    layout->addWidget(m_FieldScroll);
    layout->addLayout(buttons);

    updateTitle();
    m_Article.ReadFromDictionary(unitNo, true, false);
    setText(fromUtf8(m_Article.GetArticleStrUtf8()));
    if (markout(false) && !m_Article.m_Fields.empty()) {
        setFocusToField(0);
    }
    resize(700, 500);
}

void ArticleWindow::updateTitle() {
    setWindowTitle(QString("%1:   %2%3").arg(m_Dict->dictName(), fromUtf8(m_Dict->GetEntryStrUtf8(m_UnitNo)))
        .arg(m_Dict->GetRoss()->GetUnitMeanNum(m_UnitNo)));
}

void ArticleWindow::setText(const QString& text) {
    m_Edit->setPlainText(text);
    m_Edit->document()->setModified(false);
}

void ArticleWindow::updateFieldScroll() {
    m_FieldScroll->blockSignals(true);
    m_FieldScroll->setRange(0, m_Article.m_Fields.empty() ? 0 : (int)m_Article.m_Fields.size() - 1);
    m_FieldScroll->setPageStep(1);
    m_FieldScroll->blockSignals(false);
    m_FieldScroll->show();
}

bool ArticleWindow::markout(bool showError) {
    try {
        m_Article.ReadFromUtf8String(toUtf8(m_Edit->toPlainText()).c_str());
        bool modified = m_Edit->document()->isModified();
        QString text = fromUtf8(m_Article.GetArticleStrUtf8());
        if (text != m_Edit->toPlainText()) {
            setText(text);
            m_Edit->document()->setModified(modified);
        }
        updateFieldScroll();
    }
    catch (std::exception& e) {
        if (showError) {
            QMessageBox::critical(this, "RossDev", fromUtf8(e.what()));
        }
        return false;
    }
    return true;
}

void ArticleWindow::onMarkup() {
    if (markout() && !m_Article.m_Fields.empty()) {
        m_FieldScroll->setValue(0);
        setFocusToField(0);
    }
}

void ArticleWindow::onTextMode() {
    m_FieldScroll->hide();
    m_Edit->setFocus();
}

// selects the lines of the field
void ArticleWindow::setFocusToField(int fieldNo) {
    if (fieldNo < 0 || fieldNo >= (int)m_Article.m_Fields.size()) {
        return;
    }
    const CTextField& field = m_Article.m_Fields[fieldNo];
    QTextDocument* doc = m_Edit->document();
    QTextBlock startBlock = doc->findBlockByNumber(field.StartLine);
    QTextBlock endBlock = doc->findBlockByNumber(field.EndLine);
    if (!startBlock.isValid() || !endBlock.isValid()) {
        return;
    }
    QTextCursor cursor(doc);
    cursor.setPosition(endBlock.position() + endBlock.length() - 1);
    m_Edit->setTextCursor(cursor);
    m_Edit->ensureCursorVisible();
    cursor.setPosition(startBlock.position(), QTextCursor::KeepAnchor);
    m_Edit->setTextCursor(cursor);
    m_Edit->setFocus();
}

// the field of a line is written at the beginning of the line or of the previous lines ("GF1     = ...")
QString ArticleWindow::currentFieldName() const {
    for (QTextBlock b = m_Edit->textCursor().block(); b.isValid(); b = b.previous()) {
        QString line = b.text();
        if (line.isEmpty() || line[0].isSpace()) {
            continue;
        }
        int eq = line.indexOf('=');
        return eq == -1 ? QString() : line.left(eq).trimmed();
    }
    return QString();
}

// adds empty fields to the end of the article (CFieldList in the MFC version)
void ArticleWindow::onAddField() {
    const CDictionary* ross = m_Dict->GetRoss();
    QDialog dlg(this);
    dlg.setWindowTitle("Add fields");
    auto list = new QListWidget(&dlg);
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    for (size_t i = 0; i < ross->Fields.size(); i++) {
        for (int leaf = 0; leaf < 8; leaf++) {
            if (leaf == 0 || ross->Fields[i].IsApplicToActant) {
                QString name = fromUtf8(ross->Fields[i].FieldStr);
                if (leaf > 0) {
                    name += QString::number(leaf);
                }
                list->addItem(name);
            }
        }
    }
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(list, &QListWidget::itemDoubleClicked, &dlg, &QDialog::accept);
    auto layout = new QVBoxLayout(&dlg);
    layout->addWidget(list);
    layout->addWidget(buttons);
    dlg.resize(250, 500);
    if (dlg.exec() != QDialog::Accepted || list->selectedItems().isEmpty()) {
        return;
    }
    QTextCursor cursor(m_Edit->document());
    cursor.movePosition(QTextCursor::End);
    if (!m_Edit->toPlainText().isEmpty() && !m_Edit->toPlainText().endsWith('\n')) {
        cursor.insertText("\n");
    }
    QStringList lines;
    for (int i = 0; i < list->count(); i++) {
        if (list->item(i)->isSelected()) {
            lines << QString("%1= ").arg(list->item(i)->text(), -8);
        }
    }
    cursor.insertText(lines.join("\n"));
    // the cursor is at the value of the first added field
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, lines.size() - 1);
    cursor.movePosition(QTextCursor::EndOfBlock);
    m_Edit->setTextCursor(cursor);
    m_Edit->setFocus();
}

void ArticleWindow::onDeleteField() {
    if (!markout() || m_Article.m_Fields.empty()) {
        return;
    }
    int fieldNo = std::min(m_FieldScroll->value(), (int)m_Article.m_Fields.size() - 1);
    const CTextField& field = m_Article.m_Fields[fieldNo];
    QTextDocument* doc = m_Edit->document();
    QTextBlock startBlock = doc->findBlockByNumber(field.StartLine);
    QTextBlock endBlock = doc->findBlockByNumber(field.EndLine);
    QTextCursor cursor(doc);
    cursor.setPosition(startBlock.position());
    cursor.setPosition(std::min(endBlock.position() + endBlock.length(), doc->characterCount() - 1), QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    if (markout() && !m_Article.m_Fields.empty()) {
        fieldNo = std::min(fieldNo, (int)m_Article.m_Fields.size() - 1);
        m_FieldScroll->setValue(fieldNo);
        setFocusToField(fieldNo);
    }
}

bool ArticleWindow::isModified() {
    if (!markout(false)) {
        return true;
    }
    return m_Article.IsModified();
}

// writes the article to the dictionary (CArticleDoc::Serialize in the old MFC version)
bool ArticleWindow::saveArticle() {
    if (!markout()) {
        return false;
    }
    CHierarchyEnum hierarchyType;
    if (isHierarchyEntry(m_Dict, m_UnitNo, hierarchyType)) {
        HierarchyGraph graph;
        graph.read(m_Dict, m_Article);
        if (!graph.m_Errors.isEmpty()) {
            QMessageBox::critical(this, "RossDev", "The hierarchy is wrong:\n" + graph.m_Errors.join("\n"));
            return false;
        }
        if (!graph.m_Warnings.isEmpty()
            && QMessageBox::question(this, "RossDev", graph.m_Warnings.join("\n") + "\n\nSave the hierarchy?") != QMessageBox::Yes) {
            return false;
        }
    }
    try {
        m_Article.WriteToDictionary();
        CDictionary* ross = m_Dict->GetRoss();
        if (!m_Dict->author().empty()) {
            ross->SetUnitEditor(m_UnitNo, m_Dict->author().c_str());
        }
        ross->SetUnitCurrentTime(m_UnitNo);
        // the article is reread from the dictionary to know what has been really written
        m_Article.ReadFromDictionary(m_UnitNo, true, false);
        setText(fromUtf8(m_Article.GetArticleStrUtf8()));
        updateFieldScroll();
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "RossDev", QString("Cannot write the article: ") + fromUtf8(e.what()));
        return false;
    }
    m_Dict->setModified(true);
    emit articleSaved(m_Dict, m_UnitNo);
    return true;
}

bool ArticleWindow::maybeSave() {
    bool parsed = markout(false);
    if (parsed && !m_Article.IsModified()) {
        return true;
    }
    if (!parsed) {
        auto answer = QMessageBox::question(this, "RossDev",
            QString("The entry \"%1\" contains errors. Close without saving?").arg(windowTitle()),
            QMessageBox::Ok | QMessageBox::Cancel);
        return answer == QMessageBox::Ok;
    }
    auto answer = QMessageBox::question(this, "RossDev",
        QString("Save changes to \"%1\"?").arg(windowTitle()),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (answer == QMessageBox::Cancel) {
        return false;
    }
    return answer == QMessageBox::No || saveArticle();
}

void ArticleWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}
