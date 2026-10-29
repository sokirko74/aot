#include "MainWindow.h"
#include "ArticleWindow.h"
#include "HierarchyWindow.h"
#include "SemanticWindow.h"
#include "RossDictionary.h"
#include "WordListWindow.h"

#include "seman/SemanLib/dict_type.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <QStatusBar>

#include <filesystem>

static MainWindow* g_MainWindow = nullptr;

MainWindow* MainWindow::instance() {
    return g_MainWindow;
}

MainWindow::MainWindow() {
    g_MainWindow = this;
    setWindowTitle("RossDev");
    m_MdiArea = new QMdiArea(this);
    m_MdiArea->setViewMode(QMdiArea::TabbedView);
    m_MdiArea->setTabsClosable(true);
    m_MdiArea->setTabsMovable(true);
    setCentralWidget(m_MdiArea);

    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open dictionary...", QKeySequence::Open, this, &MainWindow::onOpen);
    fileMenu->addAction("Open all dictionaries", this, &MainWindow::onOpenAllDictionaries);
    m_RecentMenu = fileMenu->addMenu("Recent dictionaries");
    fileMenu->addSeparator();
    fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::onSave);
    fileMenu->addAction("&Close", QKeySequence::Close, m_MdiArea, &QMdiArea::closeActiveSubWindow);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);
    updateRecentMenu();

    // the commands of the current dictionary (the context menu of its word list)
    m_DictionaryMenu = menuBar()->addMenu("&Dictionary");
    connect(m_DictionaryMenu, &QMenu::aboutToShow, this, [this]() {
        m_DictionaryMenu->clear();
        WordListWindow* w = currentWordList();
        if (!w && currentDictionary()) {
            w = wordList(currentDictionary());
        }
        if (!w) {
            m_DictionaryMenu->addAction("(no dictionary)")->setEnabled(false);
            return;
        }
        for (auto a : w->contextMenu()->actions()) {
            m_DictionaryMenu->addAction(a);
        }
    });

    auto hierarchyMenu = menuBar()->addMenu("&Hierarchies");
    hierarchyMenu->addAction("Semantic &relations", this, [this]() { openHierarchy(SemRel); });
    hierarchyMenu->addAction("Semantic &features", this, [this]() { openHierarchy(SemFet); });

    auto analysisMenu = menuBar()->addMenu("&Analysis");
    analysisMenu->addAction("&Semantic analysis and translation", QKeySequence(Qt::CTRL | Qt::Key_T), this, &MainWindow::openSemanticWindow);

    auto windowMenu = menuBar()->addMenu("&Window");
    auto tabbed = windowMenu->addAction("&Tabs");
    tabbed->setCheckable(true);
    tabbed->setChecked(true);
    connect(tabbed, &QAction::toggled, this, [this](bool on) {
        m_MdiArea->setViewMode(on ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
    });
    windowMenu->addAction("&Cascade", m_MdiArea, &QMdiArea::cascadeSubWindows);
    windowMenu->addAction("T&ile", m_MdiArea, &QMdiArea::tileSubWindows);
    windowMenu->addAction("&Next", QKeySequence::NextChild, m_MdiArea, &QMdiArea::activateNextSubWindow);
    windowMenu->addAction("&Previous", QKeySequence::PreviousChild, m_MdiArea, &QMdiArea::activatePreviousSubWindow);

    auto helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About RossDev...", this, &MainWindow::onAbout);

    statusBar()->showMessage("Ready");
}

QMdiSubWindow* MainWindow::addSubWindow(QWidget* w) {
    auto sub = m_MdiArea->addSubWindow(w);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    sub->show();
    w->setFocus();
    return sub;
}

template <class T>
std::vector<T*> MainWindow::windows() const {
    std::vector<T*> result;
    for (auto sub : m_MdiArea->subWindowList()) {
        if (auto w = qobject_cast<T*>(sub->widget())) {
            result.push_back(w);
        }
    }
    return result;
}

WordListWindow* MainWindow::currentWordList() const {
    auto sub = m_MdiArea->activeSubWindow();
    return sub ? qobject_cast<WordListWindow*>(sub->widget()) : nullptr;
}

RossDictionary* MainWindow::currentDictionary() const {
    auto sub = m_MdiArea->activeSubWindow();
    if (!sub) {
        return nullptr;
    }
    if (auto w = qobject_cast<WordListWindow*>(sub->widget())) {
        return w->dictionary();
    }
    if (auto w = qobject_cast<ArticleWindow*>(sub->widget())) {
        return w->dictionary();
    }
    if (auto w = qobject_cast<HierarchyWindow*>(sub->widget())) {
        return w->dictionary();
    }
    return nullptr;
}

WordListWindow* MainWindow::wordList(const RossDictionary* dict) const {
    for (auto w : windows<WordListWindow>()) {
        if (w->dictionary() == dict) {
            return w;
        }
    }
    return nullptr;
}

RossDictionary* MainWindow::openDictionary(const QString& folderOrFile, bool readOnly) {
    QString folder = folderOrFile;
    if (QFileInfo(folder).isFile()) {
        folder = QFileInfo(folder).absolutePath();
    }
    folder = QDir(folder).absolutePath();
    if (!QFileInfo::exists(folder + "/config.rcf")) {
        QMessageBox::critical(this, "RossDev", "Cannot find config.rcf in " + folder);
        return nullptr;
    }
    std::string folder_u8 = QFile::encodeName(folder).toStdString();
    if (RossDictionary* dict = RossDevCore::instance().findDictionaryByFolder(folder_u8)) {
        if (auto w = wordList(dict)) {
            m_MdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(w->parentWidget()));
        }
        return dict;
    }
    auto dict = new RossDictionary(this);
    try {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        statusBar()->showMessage("Loading " + folder + "...");
        dict->load(folder_u8, readOnly);
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage("Ready");
    }
    catch (std::exception& e) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage("Ready");
        QMessageBox::critical(this, "RossDev", QString("Cannot open %1: %2").arg(folder, fromUtf8(e.what())));
        delete dict;
        return nullptr;
    }
    if (!dict->isReadOnly()) {
        dict->setAuthor(askUserName(this, dict));
    }
    RossDevCore::instance().addDictionary(dict);
    auto w = new WordListWindow(dict, m_MdiArea);
    addSubWindow(w);
    addRecentFolder(folder);
    return dict;
}

void MainWindow::onOpen() {
    QSettings settings;
    QString dir = settings.value("lastFolder", qEnvironmentVariable("RML") + "/Dicts").toString();
    QString path = QFileDialog::getOpenFileName(this, "Open dictionary", dir, "Dictionary config (config.rcf);;All files (*)");
    if (path.isEmpty()) {
        return;
    }
    settings.setValue("lastFolder", QFileInfo(path).absolutePath() + "/..");
    openDictionary(path);
}

// opens all dictionaries used by the semantic analysis (read only)
void MainWindow::onOpenAllDictionaries() {
    QStringList errors;
    for (int i = 0; i < NoneRoss; i++) {
        DictTypeEnum type = (DictTypeEnum)i;
        if (RossDevCore::instance().findDictionary(type)) {
            continue;
        }
        QString folder;
        try {
            folder = QFile::decodeName(GetRossPath(type).c_str());
        }
        catch (std::exception& e) {
            errors << fromUtf8(e.what());
            continue;
        }
        if (!QFileInfo::exists(folder + "/config.rcf")) {
            errors << "Cannot find " + folder + "/config.rcf";
            continue;
        }
        openDictionary(folder, true);
    }
    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "RossDev", "Some dictionaries are not opened:\n" + errors.join("\n"));
    }
}

void MainWindow::addRecentFolder(const QString& folder) {
    QSettings settings;
    QStringList recent = settings.value("recentDictionaries").toStringList();
    recent.removeAll(folder);
    recent.prepend(folder);
    while (recent.size() > 10) {
        recent.removeLast();
    }
    settings.setValue("recentDictionaries", recent);
    updateRecentMenu();
}

void MainWindow::updateRecentMenu() {
    m_RecentMenu->clear();
    QStringList recent = QSettings().value("recentDictionaries").toStringList();
    int i = 1;
    for (auto& f : recent) {
        m_RecentMenu->addAction(QString("&%1 %2").arg(i++).arg(f), this, [this, f]() { openDictionary(f); });
    }
    if (recent.isEmpty()) {
        m_RecentMenu->addAction("(empty)")->setEnabled(false);
    }
}

void MainWindow::openArticle(RossDictionary* dict, uint16_t unitNo) {
    for (auto sub : m_MdiArea->subWindowList()) {
        if (auto a = qobject_cast<ArticleWindow*>(sub->widget())) {
            if (a->dictionary() == dict && a->unitNo() == unitNo) {
                m_MdiArea->setActiveSubWindow(sub);
                return;
            }
        }
    }
    try {
        auto a = new ArticleWindow(dict, unitNo, m_MdiArea);
        connect(a, &ArticleWindow::articleSaved, this, &MainWindow::onArticleSaved);
        addSubWindow(a);
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "RossDev", QString("Cannot open the article: ") + fromUtf8(e.what()));
    }
}

void MainWindow::onArticleSaved(RossDictionary* dict, uint16_t unitNo) {
    if (auto w = wordList(dict)) {
        w->update();
    }
    CHierarchyEnum type;
    if (isHierarchyEntry(dict, unitNo, type)) {
        for (auto h : windows<HierarchyWindow>()) {
            if (h->dictionary() == dict && h->hierarchyType() == type) {
                h->refresh();
            }
        }
    }
    statusBar()->showMessage(QString("The article %1 is written to %2 (in memory, save the dictionary to write it to disk)")
        .arg(fromUtf8(dict->GetEntryStrUtf8(unitNo)), dict->dictName()), 10000);
}

void MainWindow::openHierarchy(CHierarchyEnum type) {
    RossDictionary* dict = RossDevCore::instance().findDictionary(Ross);
    if (!dict) {
        dict = openDictionary(QFile::decodeName(GetRossPath(Ross).c_str()));
        if (!dict) {
            return;
        }
    }
    for (auto sub : m_MdiArea->subWindowList()) {
        if (auto h = qobject_cast<HierarchyWindow*>(sub->widget())) {
            if (h->dictionary() == dict && h->hierarchyType() == type) {
                h->refresh();
                m_MdiArea->setActiveSubWindow(sub);
                return;
            }
        }
    }
    addSubWindow(new HierarchyWindow(dict, type, m_MdiArea));
}

void MainWindow::openSemanticWindow() {
    if (auto w = windows<SemanticWindow>(); !w.empty()) {
        m_MdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(w[0]->parentWidget()));
        return;
    }
    addSubWindow(new SemanticWindow(m_MdiArea));
}

void MainWindow::openPocket(const std::vector<PocketItem>& items, const QString& title) {
    addSubWindow(new PocketWindow(items, title, m_MdiArea));
}

void MainWindow::openReport(const QString& text, const QString& title) {
    auto edit = new QPlainTextEdit(m_MdiArea);
    edit->setReadOnly(true);
    QFont f("Courier New");
    f.setStyleHint(QFont::Monospace);
    edit->setFont(f);
    edit->setPlainText(title + "\n\n" + text);
    edit->setWindowTitle(title);
    edit->setAttribute(Qt::WA_DeleteOnClose);
    addSubWindow(edit);
}

bool MainWindow::hasWindowsWithUnitNos(const RossDictionary* dict) const {
    for (auto a : windows<ArticleWindow>()) {
        if (a->dictionary() == dict) {
            return true;
        }
    }
    for (auto p : windows<PocketWindow>()) {
        if (p->contains(dict)) {
            return true;
        }
    }
    for (auto h : windows<HierarchyWindow>()) {
        if (h->dictionary() == dict) {
            return true;
        }
    }
    return false;
}

void MainWindow::refreshWordLists() {
    for (auto w : windows<WordListWindow>()) {
        w->update();
    }
}

bool MainWindow::saveDictionary(RossDictionary* dict) {
    try {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        dict->save();
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage("Saved " + dict->dictName() + " to " + fromUtf8(dict->folder()), 10000);
    }
    catch (std::exception& e) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, "RossDev", fromUtf8(e.what()));
        return false;
    }
    return true;
}

// saves the article in the article window, otherwise the dictionary
void MainWindow::onSave() {
    auto sub = m_MdiArea->activeSubWindow();
    if (!sub) {
        return;
    }
    if (auto a = qobject_cast<ArticleWindow*>(sub->widget())) {
        a->saveArticle();
        return;
    }
    if (RossDictionary* dict = currentDictionary()) {
        saveDictionary(dict);
    }
}

bool MainWindow::closeDictionary(RossDictionary* dict, WordListWindow*) {
    if (hasWindowsWithUnitNos(dict)) {
        QMessageBox::warning(this, "RossDev", "Close all articles, entry lists and hierarchies of this dictionary!");
        return false;
    }
    if (dict->isModified()) {
        if (dict->isReadOnly()) {
            if (QMessageBox::question(this, "RossDev",
                QString("The dictionary %1 is read only, its changes will be lost. Close it?").arg(dict->dictName())) != QMessageBox::Yes) {
                return false;
            }
        }
        else {
            auto answer = QMessageBox::question(this, "RossDev", QString("Save changes to %1?").arg(dict->dictName()),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (answer == QMessageBox::Cancel || (answer == QMessageBox::Yes && !saveDictionary(dict))) {
                return false;
            }
        }
    }
    RossDevCore::instance().removeDictionary(dict);
    dict->deleteLater();
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // articles and pockets first, then dictionaries
    for (auto sub : m_MdiArea->subWindowList()) {
        if (!qobject_cast<WordListWindow*>(sub->widget()) && !sub->close()) {
            event->ignore();
            return;
        }
    }
    for (auto sub : m_MdiArea->subWindowList()) {
        if (!sub->close()) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About RossDev",
        "RossDev\n\nAn editor of structural (semantic) dictionaries (aot.ru)\n"
        "Copyright (C) Dialing 1999");
}
