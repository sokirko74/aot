#pragma once

#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/TempArticle.h"

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QScrollBar;
class RossDictionary;

// colors entries of open dictionaries and field names (CSyntaxColorizer in the MFC version)
class ArticleHighlighter : public QSyntaxHighlighter {
public:
    ArticleHighlighter(QTextDocument* doc, RossDictionary* dict);

protected:
    void highlightBlock(const QString& text) override;

private:
    RossDictionary* m_Dict;
};

// the text of an article: a double click opens the article of the clicked word
class ArticleEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    using QPlainTextEdit::QPlainTextEdit;

protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
};

// shows possible values of the field under the cursor: its signatures and items of their domains
class FieldHintPanel : public QWidget {
    Q_OBJECT
public:
    FieldHintPanel(RossDictionary* dict, QWidget* parent);
    // shows the field (a field name with an actant number, for example "GF1"), does nothing if it is the same
    void showField(const QString& fieldName);

signals:
    void insertText(const QString& text);

private:
    RossDictionary* m_Dict;
    QString m_FieldName;
    BYTE m_FieldNo = ErrUChar;
    QLabel* m_Title;
    QListWidget* m_Signats;
    QComboBox* m_Domains;
    QLineEdit* m_Filter;
    QListWidget* m_Items;

    void onSignatChanged(int row);
    void onDomainChanged(int index);
    void onFilterChanged(const QString& text);
};

// one dictionary article (CArticleDoc + CArticleView in the MFC version)
class ArticleWindow : public QWidget {
    Q_OBJECT
public:
    ArticleWindow(RossDictionary* dict, uint16_t unitNo, QWidget* parent = nullptr);

    RossDictionary* dictionary() const { return m_Dict; }
    uint16_t unitNo() const { return m_UnitNo; }
    // parses the text, false if it contains errors
    bool markout(bool showError = true);
    bool isModified();
    // writes the article to the dictionary
    bool saveArticle();
    // asks the user to save changes, false if the window must not be closed
    bool maybeSave();

public slots:
    void onMarkup();
    void onTextMode();
    void onDeleteField();
    void onAddField();
    void setFocusToField(int fieldNo);

signals:
    // the article has been written to the dictionary
    void articleSaved(RossDictionary* dict, uint16_t unitNo);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    RossDictionary* m_Dict;
    uint16_t m_UnitNo;
    CTempArticle m_Article;
    ArticleEdit* m_Edit = nullptr;
    QScrollBar* m_FieldScroll = nullptr;
    FieldHintPanel* m_Hint = nullptr;

    void updateTitle();
    void setText(const QString& text);
    void updateFieldScroll();
    // the name of the field of the line with the cursor
    QString currentFieldName() const;
};
