#pragma once

#include "seman/SemanLib/SemGraph.h"

#include <QWidget>

#include <functional>

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QTextBrowser;
class SemGraphView;

// the semantic analysis and the translation of a sentence with the dictionaries of the editor
// (CSemanticStrView + semstruct.tcl in the MFC version)
class SemanticWindow : public QWidget {
    Q_OBJECT
public:
    explicit SemanticWindow(QWidget* parent = nullptr);
    ~SemanticWindow() override;

    void setText(const QString& text);

public slots:
    void onSynan();
    void onSeman();
    void onTransfer();
    void onEngSynthesis();
    void onRusSynthesis();
    void onMemo();
    void onClearMemo();

private:
    // the analysis steps that have been done for the current text
    enum Stage { NoStage, SynanStage, SemanStage, TransferStage };
    Stage m_Stage = NoStage;
    CSemGraph m_Graph;

    QPlainTextEdit* m_Text;
    QSpinBox* m_TreeVariant;
    QSpinBox* m_ClauseVariant;
    QSpinBox* m_PanicCount;
    QComboBox* m_Domain;
    QLineEdit* m_Prohibited;
    QComboBox* m_GraphType;
    QPushButton* m_TransferBtn;
    QPushButton* m_EngSynthBtn;
    QPushButton* m_RusSynthBtn;
    SemGraphView* m_View;
    QTextBrowser* m_Selection;
    QTabWidget* m_Info;
    QPlainTextEdit* m_ClauseProps;
    QPlainTextEdit* m_TreeVariantProps;
    QPlainTextEdit* m_Statistics;
    QPlainTextEdit* m_VariantsStatistics;
    QPlainTextEdit* m_Result;

    // runs the function in a thread with a modal progress dialog, false if it throws an exception
    bool run(const QString& title, const std::function<void()>& f);
    std::string text() const;
    void openAllDictionaries();
    void setStage(Stage stage);
    void showGraph();
    void showAttrs(const QString& title, const std::vector<CSemGraphAttr>& attrs);
    void openArticle(int nodeNo);
};
