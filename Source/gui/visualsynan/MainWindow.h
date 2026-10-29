#pragma once

#include "morph_dict/common/utilit.h"

#include <QMainWindow>
#include <map>
#include <memory>

class CSyntaxHolder;
class QAction;
class QMdiArea;
class SynanWindow;

// the MDI frame (CMainFrame + CVisualSynanApp in the MFC version)
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(MorphLanguageEnum language, QWidget* parent = nullptr);
    ~MainWindow() override;

    // loads morphology and syntax of the language, if they are not loaded yet
    CSyntaxHolder* getHolder(MorphLanguageEnum language);
    void openFile(const QString& path);

public slots:
    void onNew();
    void onBuildFromFile();
    void onRunSyntax();
    void onBuildRels();
    void onKillHomonymsMode();
    void onShowMessages(bool show);
    void onAbout();

private:
    QMdiArea* m_MdiArea = nullptr;
    QAction* m_RussianAction = nullptr;
    QAction* m_GermanAction = nullptr;
    QAction* m_ShowMessagesAction = nullptr;
    MorphLanguageEnum m_Language;
    std::map<MorphLanguageEnum, std::unique_ptr<CSyntaxHolder>> m_Holders;

    SynanWindow* currentSynanWindow() const;
    SynanWindow* addSynanWindow(bool withEditor, const QString& title);
    void setLanguage(MorphLanguageEnum language);
};
