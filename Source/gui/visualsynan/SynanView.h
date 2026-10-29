#pragma once

#include "VisualSentences.h"

#include <QAbstractScrollArea>
#include <QWidget>

class CSyntaxHolder;
class QPlainTextEdit;

// draws the sentences (CVisualSynanView in the MFC version)
class SynanView : public QAbstractScrollArea {
    Q_OBJECT
public:
    explicit SynanView(QWidget* parent = nullptr);

    CVisualSentences& sentences() { return m_Sentences; }
    // recalculates coordinates of all sentences for the current width
    void recalculate();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool viewportEvent(QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;

private:
    CVisualSentences m_Sentences;

    QPoint toDocument(const QPoint& viewportPoint) const;
    void updateScrollBars();
};

// one document: the text editor and the view of syntax results (CChildFrame + CVisualSynanDoc)
class SynanWindow : public QWidget {
    Q_OBJECT
public:
    // a new document with a text editor, or a document built from a text file (without editor)
    explicit SynanWindow(QWidget* parent = nullptr, bool withEditor = true);

    bool hasEditor() const { return m_Edit != nullptr; }
    // runs synan on the text of the editor (or on the selected part of it)
    bool runSyntax(CSyntaxHolder& holder);
    bool runSyntaxOnFile(CSyntaxHolder& holder, const QString& path);
    QString workTime() const { return m_WorkTimeStr; }

signals:
    void statusMessage(const QString& message);

private:
    QPlainTextEdit* m_Edit = nullptr;
    SynanView* m_View = nullptr;
    QString m_WorkTimeStr;

    bool processInput(CSyntaxHolder& holder, const std::string& input, bool bFile);
};
