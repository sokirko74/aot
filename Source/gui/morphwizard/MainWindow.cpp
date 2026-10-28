#include "MainWindow.h"
#include "SlfEditor.h"
#include "WizardView.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>

const int MaxRecentFiles = 4;

MainWindow::MainWindow(bool usePredict)
    : m_UsePredict(usePredict)
{
    setWindowTitle("MorphWizard");
    m_MdiArea = new QMdiArea(this);
    m_MdiArea->setViewMode(QMdiArea::TabbedView);
    m_MdiArea->setTabsClosable(true);
    m_MdiArea->setTabsMovable(true);
    m_MdiArea->setDocumentMode(true);
    setCentralWidget(m_MdiArea);
    connect(m_MdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);

    createMenus();
    statusBar()->showMessage("Ready");
    updateActions();
}

void MainWindow::createMenus() {
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open...", QKeySequence::Open, this, &MainWindow::onOpen);
    m_SaveAction = fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::onSave);
    m_CloseAction = fileMenu->addAction("&Close", QKeySequence::Close, this, &MainWindow::onClose);
    fileMenu->addSeparator();
    m_RecentFilesMenu = fileMenu->addMenu("Recent Files");
    connect(m_RecentFilesMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFilesMenu);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

    // the tools are applied to the current dictionary
    m_ToolsMenu = menuBar()->addMenu("&Tools");
    auto addTool = [this](const char* title, void (WizardView::*slot)()) {
        m_ToolsMenu->addAction(title, this, [this, slot]() {
            if (auto v = currentWizardView()) {
                (v->*slot)();
            }
        });
    };
    addTool("Save current list of words to file", &WizardView::onToolsSaveListFile);
    addTool("Paradigm Export", &WizardView::onToolsExport);
    addTool("Find paradigms by list of words", &WizardView::onToolsSelectByFile);
    addTool("Paradigm Import", &WizardView::onToolsImport);
    addTool("Pack", &WizardView::onToolsPack);
    addTool("Set Paradigm No", &WizardView::onToolsSetParaNo);
    addTool("Set Accent Model No", &WizardView::onToolsSetAccentModelNo);
    addTool("Show paradigm differences", &WizardView::onToolsShowParadigmDifferences);
    addTool("Show accent model differences", &WizardView::onToolsShowAccentDifferences);
    addTool("Accent Wizard", &WizardView::onToolsAccentWizard);

    auto windowMenu = menuBar()->addMenu("&Window");
    auto tabbed = windowMenu->addAction("&Tabs");
    tabbed->setCheckable(true);
    tabbed->setChecked(true);
    connect(tabbed, &QAction::toggled, this, [this](bool on) {
        m_MdiArea->setViewMode(on ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
    });
    windowMenu->addAction("&Cascade", m_MdiArea, &QMdiArea::cascadeSubWindows);
    windowMenu->addAction("T&ile", m_MdiArea, &QMdiArea::tileSubWindows);
    windowMenu->addSeparator();
    windowMenu->addAction("&Next", QKeySequence::NextChild, m_MdiArea, &QMdiArea::activateNextSubWindow);
    windowMenu->addAction("&Previous", QKeySequence::PreviousChild, m_MdiArea, &QMdiArea::activatePreviousSubWindow);

    auto helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About Morphwizard...", this, &MainWindow::onAbout);
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* w) {
    if (w) {
        if (auto v = qobject_cast<WizardView*>(w->widget())) {
            m_CurrentWizardView = v;
        }
        else if (auto e = qobject_cast<SlfEditor*>(w->widget())) {
            if (e->m_pParent) {
                m_CurrentWizardView = e->m_pParent;
            }
        }
    }
    updateActions();
}

WizardView* MainWindow::currentWizardView() const {
    if (m_CurrentWizardView && wizardViews().contains(m_CurrentWizardView.data())) {
        return m_CurrentWizardView;
    }
    auto views = wizardViews();
    return views.isEmpty() ? nullptr : views.front();
}

QList<WizardView*> MainWindow::wizardViews() const {
    QList<WizardView*> res;
    for (auto w : m_MdiArea->subWindowList()) {
        if (auto v = qobject_cast<WizardView*>(w->widget())) {
            res << v;
        }
    }
    return res;
}

void MainWindow::updateActions() {
    bool hasWizard = currentWizardView() != nullptr;
    m_ToolsMenu->setEnabled(hasWizard);
    m_SaveAction->setEnabled(hasWizard);
    m_CloseAction->setEnabled(m_MdiArea->activeSubWindow() != nullptr);
}

void MainWindow::onOpen() {
    QSettings settings;
    QString dir = settings.value("LastDir").toString();
    QString path = QFileDialog::getOpenFileName(this, "Open dictionary", dir, "MorphWizard projects (*.mwz);;All files (*)");
    if (!path.isEmpty()) {
        settings.setValue("LastDir", QFileInfo(path).path());
        openFile(path);
    }
}

void MainWindow::openFile(const QString& path) {
    QString absPath = QFileInfo(path).absoluteFilePath();
    for (auto v : wizardViews()) {
        if (v->filePath() == absPath) {
            m_MdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(v->parentWidget()));
            return;
        }
    }

    bool ok = false;
    QString userName = QInputDialog::getText(this, "MorphWizard Login",
        "Please enter your login name\nor 'guest' for read-only mode", QLineEdit::Normal, "guest", &ok);
    if (!ok) {
        return;
    }

    WizardView* view = nullptr;
    try {
        statusBar()->showMessage("Loading " + absPath + "...");
        view = new WizardView(m_MdiArea, absPath, userName.trimmed(), m_UsePredict);
    }
    catch (std::exception& e) {
        statusBar()->clearMessage();
        QMessageBox::critical(this, "MorphWizard", QString("Cannot load %1:\n%2").arg(absPath, QString::fromUtf8(e.what())));
        return;
    }
    catch (...) {
        statusBar()->clearMessage();
        QMessageBox::critical(this, "MorphWizard", "Cannot load " + absPath);
        return;
    }
    statusBar()->showMessage("Loaded " + absPath, 5000);
    auto sub = m_MdiArea->addSubWindow(view);
    sub->showMaximized();
    m_MdiArea->setActiveSubWindow(sub);
    addRecentFile(absPath);
}

void MainWindow::onSave() {
    if (auto v = currentWizardView()) {
        v->save();
    }
}

void MainWindow::onClose() {
    m_MdiArea->closeActiveSubWindow();
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About Morphwizard",
        "Morphwizard\n\nAn editor of morphological dictionaries (aot.ru)\nCopyright (C) 2003");
}

void MainWindow::addRecentFile(const QString& path) {
    QSettings settings;
    QStringList files = settings.value("RecentFiles").toStringList();
    files.removeAll(path);
    files.prepend(path);
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }
    settings.setValue("RecentFiles", files);
}

void MainWindow::updateRecentFilesMenu() {
    m_RecentFilesMenu->clear();
    QSettings settings;
    QStringList files = settings.value("RecentFiles").toStringList();
    int i = 1;
    for (auto& f : files) {
        m_RecentFilesMenu->addAction(QString("&%1 %2").arg(i++).arg(f), this, [this, f]() { openFile(f); });
    }
    if (files.isEmpty()) {
        m_RecentFilesMenu->addAction("(empty)")->setEnabled(false);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // dictionaries first, they close their paradigm editors themselves
    for (auto v : wizardViews()) {
        auto sub = qobject_cast<QMdiSubWindow*>(v->parentWidget());
        m_MdiArea->setActiveSubWindow(sub);
        if (!sub->close()) {
            event->ignore();
            return;
        }
    }
    m_MdiArea->closeAllSubWindows();
    if (!m_MdiArea->subWindowList().isEmpty()) {
        event->ignore();
        return;
    }
    event->accept();
}
