#include "DiffViews.h"
#include "WizardView.h"
#include "QtUtil.h"

#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

static QSpinBox* createNumberEdit(QWidget* parent) {
    auto s = new QSpinBox(parent);
    s->setRange(0, 0xffff);
    s->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return s;
}

static QPlainTextEdit* createResults(QWidget* parent) {
    auto r = new QPlainTextEdit(parent);
    r->setReadOnly(true);
    r->setLineWrapMode(QPlainTextEdit::NoWrap);
    r->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    return r;
}

static void errorMessage(QWidget* parent, const QString& s) {
    QMessageBox::critical(parent, "MorphWizard", s);
}

//----------------------------------------------------------------------------
ParadigmDiffView::ParadigmDiffView(WizardView* parent)
    : m_pParent(parent)
{
    setWindowTitle("Paradigm differences");
    auto layout = new QVBoxLayout(this);
    auto top = new QHBoxLayout();
    m_Paradigm1 = createNumberEdit(this);
    m_Paradigm2 = createNumberEdit(this);
    auto analyse = new QPushButton("Analyze", this);
    top->addWidget(new QLabel("Paradigm No 1:", this));
    top->addWidget(m_Paradigm1);
    top->addSpacing(20);
    top->addWidget(new QLabel("Paradigm No 2:", this));
    top->addWidget(m_Paradigm2);
    top->addStretch();
    top->addWidget(analyse);
    layout->addLayout(top);
    m_Results = createResults(this);
    layout->addWidget(m_Results);
    connect(analyse, &QPushButton::clicked, this, &ParadigmDiffView::onAnalyse);
}

void ParadigmDiffView::onAnalyse() {
    MorphoWizard* w = m_pParent->GetWizard();
    int p1 = m_Paradigm1->value();
    int p2 = m_Paradigm2->value();
    for (int p : { p1, p2 }) {
        if (p >= (int)w->m_FlexiaModels.size()) {
            errorMessage(this, QString("Paradigm no %1 is not found").arg(p));
            return;
        }
    }
    m_Results->setPlainText(fromUtf8(w->show_differences_in_two_paradigms(p1, p2)).remove('\r'));
}


//----------------------------------------------------------------------------
AccentModelDiffView::AccentModelDiffView(WizardView* parent)
    : m_pParent(parent)
{
    setWindowTitle("Accent model differences");
    auto layout = new QVBoxLayout(this);
    auto top = new QHBoxLayout();
    m_AccentModel1 = createNumberEdit(this);
    m_AccentModel2 = createNumberEdit(this);
    auto analyse = new QPushButton("Analyze", this);
    top->addWidget(new QLabel("Accent Model 1:", this));
    top->addWidget(m_AccentModel1);
    top->addSpacing(20);
    top->addWidget(new QLabel("Accent Model 2:", this));
    top->addWidget(m_AccentModel2);
    top->addStretch();
    top->addWidget(analyse);
    layout->addLayout(top);
    m_Results = createResults(this);
    layout->addWidget(m_Results);
    connect(analyse, &QPushButton::clicked, this, &AccentModelDiffView::onAnalyse);
}

void AccentModelDiffView::onAnalyse() {
    MorphoWizard* w = m_pParent->GetWizard();
    int m1 = m_AccentModel1->value();
    int m2 = m_AccentModel2->value();
    for (int m : { m1, m2 }) {
        if (m >= (int)w->m_AccentModels.size()) {
            errorMessage(this, QString("Accent Model no %1 is not found").arg(m));
            return;
        }
    }
    size_t c1 = w->m_AccentModels[m1].m_Accents.size();
    size_t c2 = w->m_AccentModels[m2].m_Accents.size();
    if (c1 != c2) {
        errorMessage(this, QString("Accent Model %1 has %2 items, accent Model %3 has %4 items. Cannot compare them.")
            .arg(m1).arg(c1).arg(m2).arg(c2));
        return;
    }

    // search a real word
    std::string lemma;
    int flexia_model_no = -1;
    for (auto const& p : w->m_LemmaToParadigm) {
        if (p.second.m_AccentModelNo == m1 || p.second.m_AccentModelNo == m2) {
            lemma = p.first;
            flexia_model_no = p.second.m_FlexiaModelNo;
            break;
        }
    }
    if (flexia_model_no == -1) {
        errorMessage(this, QString("Accent Models %1 and %2 are registered, but there is no word of these models").arg(m1).arg(m2));
        return;
    }
    const CFlexiaModel& flexia_model = w->m_FlexiaModels[flexia_model_no];
    QStringList lines1 = fromUtf8(w->mrd_to_slf(lemma, flexia_model, m1, UnknownAccent, 79)).remove('\r').split('\n', Qt::SkipEmptyParts);
    QStringList lines2 = fromUtf8(w->mrd_to_slf(lemma, flexia_model, m2, UnknownAccent, 79)).remove('\r').split('\n', Qt::SkipEmptyParts);
    if (lines1.size() != lines2.size()) {
        errorMessage(this, "Failed");
        return;
    }
    QString results = "Different accents:\n";
    for (qsizetype i = 0; i < lines1.size(); ++i) {
        if (lines1[i] != lines2[i]) {
            results += lines1[i] + " != " + lines2[i] + "\n";
        }
    }
    m_Results->setPlainText(results);
}
