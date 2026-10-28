#pragma once

#include "morph_dict/morph_wizard/wizard.h"

#include <QPlainTextEdit>
#include <QSet>
#include <QSyntaxHighlighter>
#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class WizardView;

// colors parts of speech, grammems and word forms without accents (CSyntaxColorizer in the MFC version)
class SlfHighlighter : public QSyntaxHighlighter {
public:
    SlfHighlighter(QTextDocument* doc, const MorphoWizard& wizard);
protected:
    void highlightBlock(const QString& text) override;
private:
    QSet<QString> m_Poses;
    QSet<QString> m_Grammems;
};


// the text editor of a paradigm (CColorRichEditView in the MFC version)
//  double click puts an accent after the clicked char,
//  F3 copies the accent of the current line to the following lines
class ParadigmEdit : public QPlainTextEdit {
public:
    ParadigmEdit(QWidget* parent);
    void setAccentFromThisLineToTheEndOfParadigm();
protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
private:
    void setAccentByIndex(int ind);
};


// an editor of one paradigm (CSLFDocument + CSLFView in the MFC version)
class SlfEditor : public QWidget {
    Q_OBJECT
public:
    SlfEditor(WizardView* parent);
    ~SlfEditor() override;

    WizardView* m_pParent;
    bool m_bShouldBeClosed = false;
    bool m_bRunAccentWizard = false;
    int m_GotoLine = -1;

    CParadigmInfo m_Paradigm;
    QString m_CommonGrammems;
    QString m_ParadigmComments;
    QString m_Prefixes;
    QString m_ParadigmText;

    // initializes the controls from the fields above
    void open(bool runAccentWizard = false);

    std::string getSavedLemma() const;
    std::string getLemmaUtf8() const;
    std::string getBase() const;

    // reads controls into fields (save = true) or writes fields to controls (save = false)
    void updateData(bool save = true);
    bool isModified();
    // tries to write the paradigm to the dictionary, returns false if the user wants to continue editing
    bool saveModified();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onOk();
    void onCancel();
    void onChangeAll();
    void onAddPrefix();
    void onChangeParadigm();
    void onPosDoubleClicked();
    void onGramDoubleClicked();

private:
    MorphoWizard* GetWizard() const;
    void updateSavedParadigm();
    void updateTypeGrammemsCombo();
    void createControls();

    QString m_SaveParadigmText;
    QString m_SaveCommonGrammems;
    QString m_SavePrefixes;
    CParadigmInfo m_SaveParadigm;

    ParadigmEdit* m_Edit = nullptr;
    QComboBox* m_TypeGrammems = nullptr;
    QLineEdit* m_Comments = nullptr;
    QLineEdit* m_PrefixesEdit = nullptr;
    QListWidget* m_PosList = nullptr;
    QListWidget* m_GramList = nullptr;
    QLabel* m_BaseLabel = nullptr;
    QLabel* m_PrdNoLabel = nullptr;
    QLabel* m_AccMdlNoLabel = nullptr;
};
