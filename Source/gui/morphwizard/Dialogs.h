#pragma once

#include "morph_dict/morph_wizard/wizard.h"

#include <QDialog>

class QRadioButton;
class QTableWidget;

// asks how to change all paradigms with the same flexia model (CChangeAllDlg in the MFC version)
class ChangeAllDialog : public QDialog {
    Q_OBJECT
public:
    // 0 - only the same accent model, 1 - any accent models
    int m_anyAccentModels = 0;
    // 0 - keep old accents, 1 - fill nonaccented word forms, 2 - use new accents
    int m_usedAccentModel = 1;

    ChangeAllDialog(const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& found_paradigms,
        uint16_t oldAccentModelNo, uint16_t newAccentModelNo, QWidget* parent = nullptr);

    void accept() override;

private:
    void updateParadigmList();

    const MorphoWizard& m_wizard;
    const std::vector<lemma_iterator_t>& m_found_paradigms;
    int m_oldAccentModelNo;
    int m_newAccentModelNo;

    QTableWidget* m_list;
    QRadioButton* m_sameAccentRadio;
    QRadioButton* m_anyAccentRadio;
    QRadioButton* m_accentRadio[3];
};


// starts editing nonaccented paradigms one by one (CAccentWizardDlg in the MFC version)
class AccentWizardDialog : public QDialog {
    Q_OBJECT
public:
    AccentWizardDialog(MorphoWizard& wizard, QWidget* parent = nullptr);
    bool allNonaccentedPara() const;
private:
    QRadioButton* m_partialRadio;
    QRadioButton* m_allRadio;
};
