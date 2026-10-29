#include "MainWindow.h"
#include "SynanView.h"

#include "synan/SynanLib/SyntaxHolder.h"

#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>

MainWindow::MainWindow(MorphLanguageEnum language, QWidget* parent) : QMainWindow(parent), m_Language(language) {
    setWindowTitle("VisualSynan");
    m_MdiArea = new QMdiArea(this);
    m_MdiArea->setViewMode(QMdiArea::TabbedView);
    m_MdiArea->setTabsClosable(true);
    m_MdiArea->setTabsMovable(true);
    setCentralWidget(m_MdiArea);
    connect(m_MdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow* w) {
        if (w) {
            if (auto s = qobject_cast<SynanWindow*>(w->widget())) {
                statusBar()->showMessage(s->workTime());
            }
        }
    });

    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New", QKeySequence::New, this, &MainWindow::onNew);
    fileMenu->addAction("&Build syntax from text file...", QKeySequence::Open, this, &MainWindow::onBuildFromFile);
    fileMenu->addAction("&Close", QKeySequence::Close, m_MdiArea, &QMdiArea::closeActiveSubWindow);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

    auto syntaxMenu = menuBar()->addMenu("&Syntax");
    auto runAction = syntaxMenu->addAction(style()->standardIcon(QStyle::SP_MediaPlay), "&Run syntax", QKeySequence(Qt::Key_F5), this, &MainWindow::onRunSyntax);
    syntaxMenu->addSeparator();
    auto languages = new QActionGroup(this);
    m_RussianAction = syntaxMenu->addAction("&Russian", this, [this]() { setLanguage(morphRussian); });
    m_GermanAction = syntaxMenu->addAction("&German", this, [this]() { setLanguage(morphGerman); });
    for (auto a : { m_RussianAction, m_GermanAction }) {
        a->setCheckable(true);
        languages->addAction(a);
    }
    (m_Language == morphGerman ? m_GermanAction : m_RussianAction)->setChecked(true);
    syntaxMenu->addSeparator();
    syntaxMenu->addAction("Build best &rels", this, &MainWindow::onBuildRels);

    auto optionsMenu = menuBar()->addMenu("&Options");
    optionsMenu->addAction("&Kill Homonyms Mode...", this, &MainWindow::onKillHomonymsMode);
    m_ShowMessagesAction = optionsMenu->addAction("Show Error Messages", this, &MainWindow::onShowMessages);
    m_ShowMessagesAction->setCheckable(true);
    m_ShowMessagesAction->setChecked(true);

    auto windowMenu = menuBar()->addMenu("&Window");
    auto tabbed = windowMenu->addAction("&Tabs");
    tabbed->setCheckable(true);
    tabbed->setChecked(true);
    connect(tabbed, &QAction::toggled, this, [this](bool on) {
        m_MdiArea->setViewMode(on ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
    });
    windowMenu->addAction("&Cascade", m_MdiArea, &QMdiArea::cascadeSubWindows);
    windowMenu->addAction("T&ile", m_MdiArea, &QMdiArea::tileSubWindows);

    auto helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About VisualSynan...", this, &MainWindow::onAbout);

    auto toolBar = addToolBar("Main");
    toolBar->addAction(runAction);
    toolBar->addSeparator();
    toolBar->addAction(m_RussianAction);
    toolBar->addAction(m_GermanAction);

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow() {
    for (auto& [l, h] : m_Holders) {
        h->ClearHolder();
    }
}

CSyntaxHolder* MainWindow::getHolder(MorphLanguageEnum language) {
    auto it = m_Holders.find(language);
    if (it != m_Holders.end()) {
        return it->second.get();
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    statusBar()->showMessage("Loading syntax for " + QString::fromStdString(GetStringByLanguage(language)) + "...");
    QApplication::processEvents();
    try {
        GlobalLoadMorphHolder(language);
        auto holder = std::make_unique<CSyntaxHolder>(language);
        holder->LoadSyntax();
        holder->m_Synan.put_SilentMode(!m_ShowMessagesAction->isChecked());
        it = m_Holders.emplace(language, std::move(holder)).first;
        statusBar()->showMessage("Ready");
    }
    catch (std::exception& e) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage("Cannot load syntax");
        QMessageBox::critical(this, "VisualSynan", QString("Cannot load syntax: ") + QString::fromUtf8(e.what()));
        return nullptr;
    }
    QApplication::restoreOverrideCursor();
    return it->second.get();
}

void MainWindow::setLanguage(MorphLanguageEnum language) {
    m_Language = language;
    getHolder(language);
}

SynanWindow* MainWindow::currentSynanWindow() const {
    auto sub = m_MdiArea->activeSubWindow();
    return sub ? qobject_cast<SynanWindow*>(sub->widget()) : nullptr;
}

SynanWindow* MainWindow::addSynanWindow(bool withEditor, const QString& title) {
    auto w = new SynanWindow(m_MdiArea, withEditor);
    w->setWindowTitle(title);
    connect(w, &SynanWindow::statusMessage, this, [this](const QString& s) { statusBar()->showMessage(s); });
    auto sub = m_MdiArea->addSubWindow(w);
    sub->showMaximized();
    return w;
}

void MainWindow::onNew() {
    static int count = 0;
    addSynanWindow(true, QString("Text %1").arg(++count));
}

void MainWindow::onBuildFromFile() {
    QString path = QFileDialog::getOpenFileName(this, "Build syntax from text file", "", "Text files (*.txt);;All files (*)");
    if (!path.isEmpty()) {
        openFile(path);
    }
}

void MainWindow::openFile(const QString& path) {
    CSyntaxHolder* holder = getHolder(m_Language);
    if (!holder) {
        return;
    }
    auto w = addSynanWindow(false, QFileInfo(path).fileName());
    w->runSyntaxOnFile(*holder, path);
}

void MainWindow::onRunSyntax() {
    SynanWindow* w = currentSynanWindow();
    if (!w || !w->hasEditor()) {
        return;
    }
    if (CSyntaxHolder* holder = getHolder(m_Language)) {
        w->runSyntax(*holder);
    }
}

// the relations of the last processed text
void MainWindow::onBuildRels() {
    CSyntaxHolder* holder = getHolder(m_Language);
    if (!holder) {
        return;
    }
    QString report;
    try {
        report = BuildRels(holder->m_Synan);
    }
    catch (...) {
        QMessageBox::critical(this, "VisualSynan", "Build-rels error!");
        return;
    }
    auto edit = new QPlainTextEdit(m_MdiArea);
    edit->setReadOnly(true);
    edit->setPlainText(report);
    edit->setWindowTitle("Relations");
    edit->setAttribute(Qt::WA_DeleteOnClose);
    m_MdiArea->addSubWindow(edit)->showMaximized();
}

void MainWindow::onKillHomonymsMode() {
    CSyntaxHolder* holder = getHolder(m_Language);
    if (!holder) {
        return;
    }
    QStringList items = { "Don't Kill Homonyms", "Coverage Kill Homonyms" };
    bool ok = false;
    QString item = QInputDialog::getItem(this, "Kill Homonyms Mode", "Mode:", items,
        holder->m_Synan.GetOpt()->m_KillHomonymsMode == CoverageKillHomonyms ? 1 : 0, false, &ok);
    if (ok) {
        holder->m_Synan.SetKillHomonymsMode(item == items[1] ? CoverageKillHomonyms : DontKillHomonyms);
    }
}

void MainWindow::onShowMessages(bool show) {
    for (auto& [l, h] : m_Holders) {
        h->m_Synan.put_SilentMode(!show);
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About VisualSynan",
        "VisualSynan\n\nShows the results of the syntax analysis (aot.ru)\nCopyright (C) Dialing 1999");
}
