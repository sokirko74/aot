#include "NewArticleWizard.h"
#include "RossDictionary.h"

#include "dicts/BinaryDictsLib/SetOfWordPairs.h"
#include "morph_dict/agramtab/RusGramTab.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QVBoxLayout>

NewArticleWizard::NewArticleWizard(RossDictionary* dict, QWidget* parent) : QWizard(parent), m_Dict(dict) {
    setWindowTitle("New entry");
    setPage(PageEntry, new EntryPage(this));
    setPage(PagePos, new PosPage(this));
    setPage(PageTranslations, new TranslationsPage(this));
    setPage(PageRefArticles, new RefArticlesPage(this));
    setOption(QWizard::HaveFinishButtonOnEarlyPages, false);
}

static CTranslatorHolder* translator() {
    return RossDevCore::instance().rossesHolder();
}

//----------------------------------------------------------------------------

EntryPage::EntryPage(NewArticleWizard* wizard) : QWizardPage(wizard), m_Wizard(wizard) {
    setTitle("Dict entry");
    m_Entry = new QLineEdit(this);
    m_Entry->setMaxLength(100);
    m_MeanNum = new QLineEdit("1", this);
    m_MeanNum->setEnabled(false);
    auto layout = new QFormLayout(this);
    layout->addRow("Entry name:", m_Entry);
    layout->addRow("Sense No:", m_MeanNum);
}

bool EntryPage::validatePage() {
    QString entry = m_Entry->text().trimmed();
    if (entry.isEmpty()) {
        QMessageBox::warning(this, "RossDev", "The text entry cannot be empty");
        return false;
    }
    std::string entry_u8 = toUtf8(entry);
    CDictionary* ross = m_Wizard->m_Dict->GetRoss();
    BYTE MeanNum = 1;
    uint16_t UnitNo = ross->LocateUnit(entry_u8.c_str(), MeanNum);
    if (UnitNo != ErrUnitNo) {
        if (QMessageBox::question(this, "RossDev", "An entry with the same name already exists. Add a new sense?",
            QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
            return false;
        }
        size_t k = UnitNo + 1;
        for (; k < ross->GetUnitsSize(); k++) {
            if (entry_u8 != ross->GetEntryStr(k)) {
                break;
            }
        }
        MeanNum = ross->GetUnitMeanNum(k - 1) + 1;
    }
    m_MeanNum->setText(QString::number(MeanNum));
    m_Wizard->m_EntryStr = entry_u8;
    m_Wizard->m_MeanNum = MeanNum;

    m_Wizard->m_Paradigms.clear();
    if (const CLemmatizer* lemmatizer = translator()->GetRusLemmatizer()) {
        std::string s8 = convert_from_utf8(entry_u8.c_str(), morphRussian);
        lemmatizer->CreateParadigmCollection(true, s8, false, false, m_Wizard->m_Paradigms);
    }
    return true;
}

//----------------------------------------------------------------------------

PosPage::PosPage(NewArticleWizard* wizard) : QWizardPage(wizard), m_Wizard(wizard) {
    setTitle("Choose part of speech");
    m_List = new QListWidget(this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_List);
}

void PosPage::initializePage() {
    m_List->clear();
    auto other = new QListWidgetItem("other", m_List);
    other->setData(Qt::UserRole, -1);
    const CRusGramTab* gramtab = translator()->GetRusGramTab();
    for (auto& p : m_Wizard->m_Paradigms) {
        std::string ancode = p.GetSrcAncode();
        if (ancode.length() < 2) {
            continue;
        }
        int PartOfSpeech = gramtab->GetPartOfSpeech(ancode.c_str());
        QString line;
        switch (PartOfSpeech) {
            case NOUN: line = "СУЩ:ИГ"; break;
            case INFINITIVE: line = "ГЛ:ГГ"; break;
            case ADJ_FULL: line = "ПРИЛ:с_опр"; break;
            case ADV: line = "НАР:ОБСТ_ГР"; break;
            case PREDK: line = "НАР:нар_опр"; break;
        }
        if (!line.isEmpty()) {
            line = QString(m_Wizard->m_Dict->GetGramFetFieldName()) + " = 1 " + line;
            if (m_List->findItems(line, Qt::MatchExactly).isEmpty()) {
                auto item = new QListWidgetItem(line, m_List);
                item->setData(Qt::UserRole, PartOfSpeech);
            }
        }
    }
    m_List->setCurrentRow(0);
}

bool PosPage::validatePage() {
    m_Wizard->m_PartOfSpeech = -1;
    m_Wizard->m_PosArticle.clear();
    m_Wizard->m_PerfectiveIds.clear();
    auto item = m_List->currentItem();
    if (!item || m_List->currentRow() == 0) {
        return true;
    }
    m_Wizard->m_PosArticle = toUtf8(item->text()) + "\n";
    m_Wizard->m_PartOfSpeech = item->data(Qt::UserRole).toInt();
    // the reference article for perfective verbs
    if (m_Wizard->m_PartOfSpeech == INFINITIVE) {
        for (auto& p : m_Wizard->m_Paradigms) {
            std::vector<uint32_t> perfs;
            translator()->m_AspDict.nonperf2perf(p.GetParadigmId(), perfs);
            m_Wizard->m_PerfectiveIds.insert(m_Wizard->m_PerfectiveIds.end(), perfs.begin(), perfs.end());
        }
    }
    return true;
}

//----------------------------------------------------------------------------

TranslationsPage::TranslationsPage(NewArticleWizard* wizard) : QWizardPage(wizard), m_Wizard(wizard) {
    setTitle("English translations");
    m_List = new QListWidget(this);
    m_List->setSelectionMode(QAbstractItemView::MultiSelection);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_List);
}

void TranslationsPage::initializePage() {
    m_List->clear();
    const CRusGramTab* gramtab = translator()->GetRusGramTab();
    for (auto& p : m_Wizard->m_Paradigms) {
        if (!p.m_bFound) {
            continue;
        }
        if ((int)gramtab->GetPartOfSpeech(p.GetSrcAncode().c_str()) != m_Wizard->m_PartOfSpeech) {
            continue;
        }
        CSetOfWordPairs pairs = translator()->m_BinaryDict.TranslateIndirect(p.GetParadigmId());
        for (long k = 0; k < pairs.get_Count(); k++) {
            CFormInfo prd;
            if (!translator()->GetEngLemmatizer()->CreateParadigmFromID(pairs.GetId(k), prd)) {
                continue;
            }
            QString word = fromUtf8(prd.GetWordForm(0));
            if (m_List->findItems(word, Qt::MatchExactly).isEmpty()) {
                m_List->addItem(word);
            }
        }
    }
    m_List->sortItems();
}

bool TranslationsPage::validatePage() {
    std::string result;
    for (auto item : m_List->selectedItems()) {
        result += toUtf8(item->text().section(' ', 0, 0).toLower()) + "\n";
    }
    if (!result.empty()) {
        result = std::string(m_Wizard->m_Dict->GetEnglishFieldName()) + " = " + result;
    }
    m_Wizard->m_TransArticle = result;
    return true;
}

int TranslationsPage::nextId() const {
    return m_Wizard->m_PerfectiveIds.empty() ? -1 : NewArticleWizard::PageRefArticles;
}

//----------------------------------------------------------------------------

RefArticlesPage::RefArticlesPage(NewArticleWizard* wizard) : QWizardPage(wizard), m_Wizard(wizard) {
    setTitle("Create reference article for non-perfective verbs");
    m_List = new QListWidget(this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_List);
}

void RefArticlesPage::initializePage() {
    m_List->clear();
    for (auto id : m_Wizard->m_PerfectiveIds) {
        CFormInfo prd;
        if (translator()->GetRusLemmatizer()->CreateParadigmFromID(id, prd)) {
            m_List->addItem(fromUtf8(convert_to_utf8(prd.GetWordForm(0), morphRussian)));
        }
    }
}

bool RefArticlesPage::validatePage() {
    m_Wizard->m_RefLemma.clear();
    m_Wizard->m_RefArticle.clear();
    auto item = m_List->currentItem();
    if (item && item->isSelected()) {
        m_Wizard->m_RefLemma = toUtf8(item->text().toLower());
        m_Wizard->m_RefArticle = Format("%s = %s . # %i\n", m_Wizard->m_Dict->GetReferenceFieldName(),
            m_Wizard->m_EntryStr.c_str(), m_Wizard->m_MeanNum);
    }
    return true;
}
