#pragma once

#include "Dialogs.h"

#include "seman/SemanLib/HierarchyHolder.h"

#include <QMainWindow>

#include <vector>

class ArticleWindow;
class QMdiArea;
class QMdiSubWindow;
class QMenu;
class RossDictionary;
class WordListWindow;

// the MDI frame (CMainFrame + CRossDevApp in the MFC version)
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    static MainWindow* instance();

    // opens the dictionary from the folder (with config.rcf), activates it, if it is already open
    RossDictionary* openDictionary(const QString& folder, bool readOnly = false);
    void openArticle(RossDictionary* dict, uint16_t unitNo);
    void openPocket(const std::vector<PocketItem>& items, const QString& title);
    void openReport(const QString& text, const QString& title);
    // opens the hierarchy of Ross (opens Ross itself, if it is not open)
    void openHierarchy(CHierarchyEnum type);
    // there is only one window of the semantic analysis, since the analysis is shared
    void openSemanticWindow();
    // are there articles or pockets with units of the dictionary
    bool hasWindowsWithUnitNos(const RossDictionary* dict) const;
    void refreshWordLists();
    // asks to save the dictionary and closes it, false if it must stay open
    bool closeDictionary(RossDictionary* dict, WordListWindow* wordList);
    WordListWindow* wordList(const RossDictionary* dict) const;

public slots:
    void onOpen();
    void onOpenAllDictionaries();
    void onSave();
    void onAbout();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QMdiArea* m_MdiArea;
    QMenu* m_RecentMenu;
    QMenu* m_DictionaryMenu;

    QMdiSubWindow* addSubWindow(QWidget* w);
    template <class T>
    std::vector<T*> windows() const;
    WordListWindow* currentWordList() const;
    // the dictionary of the active window (word list or article)
    RossDictionary* currentDictionary() const;
    bool saveDictionary(RossDictionary* dict);
    void addRecentFolder(const QString& folder);
    void updateRecentMenu();
    void onArticleSaved(RossDictionary* dict, uint16_t unitNo);
};
