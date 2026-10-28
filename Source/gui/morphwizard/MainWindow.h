#pragma once

#include <QMainWindow>
#include <QPointer>

class QAction;
class QMdiArea;
class QMdiSubWindow;
class QMenu;
class WizardView;

// the MDI frame (CMainFrame + CMorphwizardApp in the MFC version)
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(bool usePredict);
    void openFile(const QString& path);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onOpen();
    void onSave();
    void onClose();
    void onAbout();
    void onSubWindowActivated(QMdiSubWindow* w);
    void updateRecentFilesMenu();

private:
    WizardView* currentWizardView() const;
    QList<WizardView*> wizardViews() const;
    void addRecentFile(const QString& path);
    void createMenus();
    void updateActions();

    bool m_UsePredict;
    QMdiArea* m_MdiArea;
    QPointer<WizardView> m_CurrentWizardView;
    QMenu* m_RecentFilesMenu;
    QMenu* m_ToolsMenu;
    QAction* m_SaveAction;
    QAction* m_CloseAction;
};
