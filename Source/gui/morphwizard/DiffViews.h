#pragma once

#include <QWidget>

class QPlainTextEdit;
class QSpinBox;
class WizardView;

// shows differences between two flexia models (CParadigmDifferences in the MFC version)
class ParadigmDiffView : public QWidget {
    Q_OBJECT
public:
    ParadigmDiffView(WizardView* parent);
private slots:
    void onAnalyse();
private:
    WizardView* m_pParent;
    QSpinBox* m_Paradigm1;
    QSpinBox* m_Paradigm2;
    QPlainTextEdit* m_Results;
};


// shows differences between two accent models (CAccentModelDiff in the MFC version)
class AccentModelDiffView : public QWidget {
    Q_OBJECT
public:
    AccentModelDiffView(WizardView* parent);
private slots:
    void onAnalyse();
private:
    WizardView* m_pParent;
    QSpinBox* m_AccentModel1;
    QSpinBox* m_AccentModel2;
    QPlainTextEdit* m_Results;
};
