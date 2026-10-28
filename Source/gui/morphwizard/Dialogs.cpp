#include "Dialogs.h"
#include "ProgressMeter.h"
#include "QtUtil.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QVBoxLayout>

//----------------------------------------------------------------------------
// ChangeAllDialog
//----------------------------------------------------------------------------
ChangeAllDialog::ChangeAllDialog(const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& found_paradigms,
    uint16_t oldAccentModelNo, uint16_t newAccentModelNo, QWidget* parent)
    : QDialog(parent), m_wizard(wizard), m_found_paradigms(found_paradigms),
    m_oldAccentModelNo(oldAccentModelNo), m_newAccentModelNo(newAccentModelNo)
{
    setWindowTitle("Change All Paradigms");
    auto layout = new QVBoxLayout(this);

    m_list = new QTableWidget(0, 2, this);
    m_list->setHorizontalHeaderLabels({ "Model No", "Paradigms" });
    m_list->horizontalHeader()->setStretchLastSection(true);
    m_list->verticalHeader()->hide();
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(m_list);

    m_sameAccentRadio = new QRadioButton("the same accent model", this);
    m_anyAccentRadio = new QRadioButton("any accent models", this);
    auto modelsGroup = new QButtonGroup(this);
    modelsGroup->addButton(m_sameAccentRadio, 0);
    modelsGroup->addButton(m_anyAccentRadio, 1);
    layout->addWidget(m_sameAccentRadio);
    layout->addWidget(m_anyAccentRadio);

    auto accentsBox = new QGroupBox("Set Accents", this);
    auto accentsLayout = new QVBoxLayout(accentsBox);
    auto accentsGroup = new QButtonGroup(this);
    const char* accentLabels[3] = { "Keep old accents", "Fill nonaccented wordforms", "Use new accents" };
    for (int i = 0; i < 3; ++i) {
        m_accentRadio[i] = new QRadioButton(accentLabels[i], accentsBox);
        accentsGroup->addButton(m_accentRadio[i], i);
        accentsLayout->addWidget(m_accentRadio[i]);
    }
    layout->addWidget(accentsBox);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    m_anyAccentModels = 0;
    size_t count = 0;
    for (auto& p : m_found_paradigms) {
        if (p->second.m_AccentModelNo != m_oldAccentModelNo) {
            m_anyAccentModels = 1;
            ++count;
        }
    }

    if (m_anyAccentModels == 0 || count == m_found_paradigms.size()) {
        m_sameAccentRadio->setEnabled(false);
        m_anyAccentRadio->setEnabled(false);
    }

    if (m_anyAccentModels == 0 && m_oldAccentModelNo == m_newAccentModelNo) {
        m_usedAccentModel = 2;
        for (auto r : m_accentRadio) {
            r->setEnabled(false);
        }
    }

    (m_anyAccentModels == 0 ? m_sameAccentRadio : m_anyAccentRadio)->setChecked(true);
    m_accentRadio[m_usedAccentModel]->setChecked(true);

    connect(modelsGroup, &QButtonGroup::idClicked, this, [this](int id) {
        m_anyAccentModels = id;
        updateParadigmList();
    });

    updateParadigmList();
    resize(400, 550);
}

void ChangeAllDialog::accept() {
    for (int i = 0; i < 3; ++i) {
        if (m_accentRadio[i]->isChecked()) {
            m_usedAccentModel = i;
        }
    }
    QDialog::accept();
}

void ChangeAllDialog::updateParadigmList() {
    m_list->setRowCount(0);
    int count = 0;
    for (auto& p : m_found_paradigms) {
        if (m_anyAccentModels == 0 && p->second.m_AccentModelNo != m_oldAccentModelNo) {
            continue;
        }
        m_list->insertRow(count);
        m_list->setItem(count, 0, new QTableWidgetItem(
            QString("%1 (%2)").arg(p->second.m_FlexiaModelNo).arg(p->second.m_AccentModelNo)));

        std::string lemma = m_wizard.get_lemm_string(p);
        std::string prefixSet = m_wizard.get_prefix_set(p);
        if (!prefixSet.empty()) {
            lemma = prefixSet + '|' + lemma;
        }
        MakeLowerUtf8(lemma);
        m_list->setItem(count, 1, new QTableWidgetItem(fromUtf8(lemma)));
        ++count;
    }
    m_list->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Paradigms: %1").arg(count)));
}


//----------------------------------------------------------------------------
// AccentWizardDialog
//----------------------------------------------------------------------------
AccentWizardDialog::AccentWizardDialog(MorphoWizard& wizard, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Accent Wizard");

    size_t allNonaccParaCount = 0;
    size_t partialAccParaCount = 0;
    {
        WizardProgressMeter meter(wizard);
        for (auto it = wizard.m_LemmaToParadigm.begin(); it != wizard.m_LemmaToParadigm.end(); it++) {
            if (wizard.HasUnknownAccents(it)) {
                ++allNonaccParaCount;
                if (wizard.IsPartialAccented(it)) {
                    ++partialAccParaCount;
                }
            }
        }
    }

    auto layout = new QVBoxLayout(this);
    auto form = new QFormLayout();
    m_partialRadio = new QRadioButton("Partial accented paradigms", this);
    m_allRadio = new QRadioButton("All nonaccented paradigms", this);
    auto partialCount = new QLineEdit(QString::number(partialAccParaCount), this);
    partialCount->setReadOnly(true);
    auto allCount = new QLineEdit(QString::number(allNonaccParaCount), this);
    allCount->setReadOnly(true);
    form->addRow(m_partialRadio, partialCount);
    form->addRow(m_allRadio, allCount);
    layout->addLayout(form);

    auto buttons = new QDialogButtonBox(this);
    auto startButton = buttons->addButton("Start", QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    m_partialRadio->setChecked(true);
    if (partialAccParaCount == 0) {
        m_partialRadio->setEnabled(false);
        m_allRadio->setChecked(true);
        if (allNonaccParaCount == 0) {
            m_allRadio->setEnabled(false);
            startButton->hide();
        }
    }
}

bool AccentWizardDialog::allNonaccentedPara() const {
    return m_allRadio->isChecked();
}
