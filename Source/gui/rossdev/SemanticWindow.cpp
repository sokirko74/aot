#include "SemanticWindow.h"
#include "MainWindow.h"
#include "RossDictionary.h"
#include "SemGraphView.h"

#include <QApplication>
#include <QComboBox>
#include <QEventLoop>
#include <QFile>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <thread>

static QPlainTextEdit* createReadOnlyText(QWidget* parent) {
    auto t = new QPlainTextEdit(parent);
    t->setReadOnly(true);
    return t;
}

SemanticWindow::SemanticWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Semantic analysis");

    m_Text = new QPlainTextEdit(this);
    m_Text->setPlaceholderText("A Russian sentence. \"text ## lemma1/lemma2;...\" replaces lemmas in the synthesis");
    m_Text->setMaximumHeight(70);
    m_Text->setPlainText(QSettings().value("semanticText").toString());

    auto buttons = new QHBoxLayout();
    auto addButton = [&](const QString& name, const QString& tip, void (SemanticWindow::*slot)()) {
        auto b = new QPushButton(name, this);
        b->setToolTip(tip);
        connect(b, &QPushButton::clicked, this, slot);
        buttons->addWidget(b);
        return b;
    };
    addButton("Synan", "Show syntax relations of the first sentence", &SemanticWindow::onSynan);
    auto semanBtn = addButton("Seman", "The semantic analysis (Ctrl+Enter)", &SemanticWindow::onSeman);
    semanBtn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));
    m_TransferBtn = addButton("Transfer", "Build the English semantic structure", &SemanticWindow::onTransfer);
    m_EngSynthBtn = addButton("EngSynt", "Build the English sentence", &SemanticWindow::onEngSynthesis);
    m_RusSynthBtn = addButton("RusSynt", "Build the Russian sentence by the semantic structure", &SemanticWindow::onRusSynthesis);
    addButton("Memo", "Remember the Russian structure (for the next sentences)", &SemanticWindow::onMemo);
    addButton("Clear Memo", "Forget remembered structures", &SemanticWindow::onClearMemo);
    buttons->addStretch(1);

    // the options of CSemOptions
    auto options = new QHBoxLayout();
    auto addOption = [&](const QString& name, QWidget* w, const QString& tip) {
        auto label = new QLabel(name, this);
        label->setToolTip(tip);
        w->setToolTip(tip);
        options->addWidget(label);
        options->addWidget(w);
    };
    m_TreeVariant = new QSpinBox(this);
    m_TreeVariant->setRange(-1, 100000);
    m_TreeVariant->setSpecialValueText("best");
    m_TreeVariant->setValue(-1);
    addOption("Variant:", m_TreeVariant, "The number of the tree variant, \"best\" for the best one");
    m_ClauseVariant = new QSpinBox(this);
    m_ClauseVariant->setRange(-1, 100000);
    m_ClauseVariant->setSpecialValueText("all");
    m_ClauseVariant->setValue(-1);
    addOption("Clause variant:", m_ClauseVariant, "The number of the morphological clause variant, \"all\" to try all of them");
    m_PanicCount = new QSpinBox(this);
    m_PanicCount->setRange(1, 10000000);
    m_PanicCount->setValue(20000);
    addOption("Panic count:", m_PanicCount, "The maximal count of tree variants");
    m_Domain = new QComboBox(this);
    m_Domain->addItem("общ", "общ");
    m_Domain->addItem("фин", "фин");
    m_Domain->addItem("комп", "информ");
    addOption("Topic:", m_Domain, "The subject domain");
    m_Prohibited = new QLineEdit(this);
    m_Prohibited->setPlaceholderText("баня1,папа2");
    addOption("Prohibited:", m_Prohibited, "Prohibited lexical variants (entry and sense number), separated by commas");
    m_GraphType = new QComboBox(this);
    m_GraphType->addItem("Main graph");
    m_GraphType->addItem("Clauses");
    connect(m_GraphType, &QComboBox::currentIndexChanged, this, [this]() { showGraph(); });
    options->addWidget(m_GraphType);
    options->addStretch(1);

    m_View = new SemGraphView(this);
    connect(m_View, &SemGraphView::nodeSelected, this, [this](int no) {
        auto& nodes = m_GraphType->currentIndex() == 1 ? m_Graph.m_ClauseNodes : m_Graph.m_Nodes;
        showAttrs(fromUtf8(nodes[no].m_Label), nodes[no].m_Attrs);
    });
    connect(m_View, &SemGraphView::edgeSelected, this, [this](int no) {
        auto& edges = m_GraphType->currentIndex() == 1 ? m_Graph.m_ClauseEdges : m_Graph.m_Edges;
        showAttrs(fromUtf8(edges[no].m_Label), edges[no].m_Attrs);
    });
    connect(m_View, &SemGraphView::selectionCleared, this, [this]() { m_Selection->setHtml("Click a node or a relation"); });
    connect(m_View, &SemGraphView::nodeDoubleClicked, this, &SemanticWindow::openArticle);

    m_Info = new QTabWidget(this);
    m_Selection = new QTextBrowser(this);
    m_Selection->setHtml("Click a node or a relation. A double click opens the article of the node.");
    m_ClauseProps = createReadOnlyText(this);
    m_TreeVariantProps = createReadOnlyText(this);
    m_Statistics = createReadOnlyText(this);
    m_VariantsStatistics = createReadOnlyText(this);
    m_Info->addTab(m_Selection, "Selection");
    m_Info->addTab(m_ClauseProps, "Clauses");
    m_Info->addTab(m_TreeVariantProps, "Tree variant");
    m_Info->addTab(m_Statistics, "Statistics");
    m_Info->addTab(m_VariantsStatistics, "Variants");

    m_Result = createReadOnlyText(this);
    m_Result->setMaximumHeight(60);
    m_Result->setPlaceholderText("The result of the synthesis");

    auto splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_View);
    splitter->addWidget(m_Info);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({700, 300});

    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_Text);
    layout->addLayout(buttons);
    layout->addLayout(options);
    layout->addWidget(splitter, 1);
    layout->addWidget(m_Result);
    setStage(NoStage);
    resize(1000, 700);
}

SemanticWindow::~SemanticWindow() {
    QSettings().setValue("semanticText", m_Text->toPlainText());
}

void SemanticWindow::setText(const QString& text) {
    m_Text->setPlainText(text);
}

std::string SemanticWindow::text() const {
    return toUtf8(m_Text->toPlainText().simplified());
}

void SemanticWindow::setStage(Stage stage) {
    m_Stage = stage;
    m_TransferBtn->setEnabled(stage >= SemanStage);
    m_RusSynthBtn->setEnabled(stage >= SemanStage);
    m_EngSynthBtn->setEnabled(stage >= TransferStage);
}

void SemanticWindow::openAllDictionaries() {
    // the analysis uses all dictionaries, so they are opened (read only) to see their articles
    // (OpenAllRosses in the MFC version); the editor loads a dictionary only when it is opened
    MainWindow::instance()->onOpenAllDictionaries();
    if (auto sub = qobject_cast<QMdiSubWindow*>(parentWidget())) {
        sub->mdiArea()->setActiveSubWindow(sub);
    }
}

bool SemanticWindow::run(const QString& title, const std::function<void()>& f) {
    QProgressDialog progress(title + "...", QString(), 0, 0, this);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);
    progress.show();
    QEventLoop loop;
    std::string error;
    std::thread worker([&]() {
        try {
            f();
        }
        catch (std::exception& e) {
            error = e.what();
        }
        catch (...) {
            error = "unknown error";
        }
        QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
    });
    loop.exec();
    worker.join();
    progress.close();
    if (!error.empty()) {
        QMessageBox::critical(this, "RossDev", title + " failed: " + fromUtf8(error));
        return false;
    }
    return true;
}

void SemanticWindow::onSynan() {
    std::string t = text();
    if (t.empty()) {
        return;
    }
    openAllDictionaries();
    int clauseVariant = m_ClauseVariant->value();
    auto& builder = RossDevCore::instance().semBuilder();
    bool ok = false;
    if (!run("Syntax analysis", [&]() {
        ok = builder.m_RusStr.BuildSyntaxNodesByText(t, clauseVariant);
        if (ok) {
            builder.m_RusStr.GetSyntaxGraph(m_Graph);
        }
    })) {
        return;
    }
    if (!ok) {
        QMessageBox::warning(this, "RossDev", "Cannot build the syntax structure");
        return;
    }
    setStage(SynanStage);
    m_Result->clear();
    showGraph();
}

void SemanticWindow::onSeman() {
    std::string t = text();
    if (t.empty()) {
        return;
    }
    CSemOptions opts;
    opts.m_TreeVariantIndex = m_TreeVariant->value();
    opts.m_ClauseVariantsCombinationIndex = m_ClauseVariant->value();
    opts.m_PanicTreeVariantCount = m_PanicCount->value();
    opts.m_Domain = toUtf8(m_Domain->currentData().toString());
    for (auto& l : m_Prohibited->text().split(',', Qt::SkipEmptyParts)) {
        opts.m_ProhibitedLexVars.push_back(toUtf8(l.trimmed()));
    }
    std::string lemmasToReplace;
    size_t index = t.find("##");
    if (index != std::string::npos) {
        lemmasToReplace = t.substr(index + 2);
        t.erase(index);
    }
    openAllDictionaries();
    auto& builder = RossDevCore::instance().semBuilder();
    setStage(NoStage);
    if (!run("Semantic analysis", [&]() {
        builder.m_RusStr.SetLemmasToReplace(lemmasToReplace);
        builder.FindSituations(t, opts);
        builder.m_RusStr.GetGraph(m_Graph);
        builder.m_RusStr.GetClauseGraph(m_Graph);
    })) {
        m_View->clearGraph();
        return;
    }
    setStage(SemanStage);
    m_Result->clear();
    showGraph();
}

void SemanticWindow::onTransfer() {
    auto& builder = RossDevCore::instance().semBuilder();
    if (!run("Transfer", [&]() {
        builder.TranslateToEnglish();
        builder.m_EngStr.GetGraph(m_Graph);
    })) {
        return;
    }
    setStage(TransferStage);
    m_GraphType->setCurrentIndex(0);
    showGraph();
}

void SemanticWindow::onEngSynthesis() {
    std::string sentence;
    auto& builder = RossDevCore::instance().semBuilder();
    if (run("English synthesis", [&]() { builder.BuildSentence(sentence); })) {
        m_Result->setPlainText(fromUtf8(sentence));
    }
}

void SemanticWindow::onRusSynthesis() {
    std::string sentence;
    bool ok = false;
    auto& builder = RossDevCore::instance().semBuilder();
    if (run("Russian synthesis", [&]() { ok = builder.SyntRusSentence(sentence); })) {
        m_Result->setPlainText(ok ? fromUtf8(sentence) : "(the Russian synthesis failed)");
    }
}

void SemanticWindow::onMemo() {
    if (m_Stage < SemanStage) {
        return;
    }
    RossDevCore::instance().semBuilder().SaveThisSentence();
}

void SemanticWindow::onClearMemo() {
    RossDevCore::instance().semBuilder().ClearSavedSentences();
}

void SemanticWindow::showGraph() {
    bool clauses = m_GraphType->currentIndex() == 1;
    if (clauses) {
        m_View->setGraph(m_Graph.m_ClauseNodes, m_Graph.m_ClauseEdges);
    }
    else {
        m_View->setGraph(m_Graph.m_Nodes, m_Graph.m_Edges);
    }
    m_ClauseProps->setPlainText(fromUtf8(m_Graph.m_ClauseProperties));
    m_TreeVariantProps->setPlainText(fromUtf8(m_Graph.m_TreeVariantProperties));
    m_Statistics->setPlainText(fromUtf8(m_Graph.m_Statistics));
    m_VariantsStatistics->setPlainText(fromUtf8(m_Graph.m_VariantsStatistics));
    m_Selection->setHtml(QString("%1 nodes, %2 relations. Click a node or a relation, a double click opens the article of the node.")
        .arg(clauses ? m_Graph.m_ClauseNodes.size() : m_Graph.m_Nodes.size())
        .arg(clauses ? m_Graph.m_ClauseEdges.size() : m_Graph.m_Edges.size()));
}

void SemanticWindow::showAttrs(const QString& title, const std::vector<CSemGraphAttr>& attrs) {
    QString html = "<h3>" + title.toHtmlEscaped() + "</h3>";
    for (auto& a : attrs) {
        html += "<b>" + fromUtf8(a.m_Title).toHtmlEscaped() + "</b><pre>" + fromUtf8(a.m_Value).trimmed().toHtmlEscaped() + "</pre>";
    }
    m_Selection->setHtml(html);
    m_Info->setCurrentWidget(m_Selection);
}

void SemanticWindow::openArticle(int nodeNo) {
    if (m_GraphType->currentIndex() == 1 || nodeNo >= (int)m_Graph.m_Nodes.size()) {
        return;
    }
    const CSemGraphNode& node = m_Graph.m_Nodes[nodeNo];
    if (node.m_DictType == NoneRoss || node.m_UnitNo == ErrUnitNo) {
        return;
    }
    RossDictionary* dict = RossDevCore::instance().findDictionary(node.m_DictType);
    if (!dict) {
        dict = MainWindow::instance()->openDictionary(QFile::decodeName(GetRossPath(node.m_DictType).c_str()), true);
    }
    if (dict && node.m_UnitNo < dict->GetRoss()->GetUnitsSize()) {
        MainWindow::instance()->openArticle(dict, node.m_UnitNo);
    }
}
