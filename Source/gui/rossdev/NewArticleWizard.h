#pragma once

#include "morph_dict/lemmatizer_base_lib/Paradigm.h"

#include <QWizard>
#include <QWizardPage>

#include <string>
#include <vector>

class QLineEdit;
class QListWidget;
class RossDictionary;
class NewArticleWizard;

// creates a new entry: its name, part of speech, English translations and
// a reference article for the perfective verb (CNewArticleWizard in the MFC version)
class NewArticleWizard : public QWizard {
    Q_OBJECT
public:
    enum { PageEntry, PagePos, PageTranslations, PageRefArticles };

    NewArticleWizard(RossDictionary* dict, QWidget* parent);

    std::string entryStr() const { return m_EntryStr; }
    BYTE meanNum() const { return m_MeanNum; }
    // the article of the new entry
    std::string article() const { return m_PosArticle + m_TransArticle; }
    // the reference article for a perfective verb (empty if it is not needed)
    std::string refLemma() const { return m_RefLemma; }
    std::string refArticle() const { return m_RefArticle; }

    RossDictionary* m_Dict;
    std::string m_EntryStr;
    BYTE m_MeanNum = 1;
    std::vector<CFormInfo> m_Paradigms;
    int m_PartOfSpeech = -1;
    std::string m_PosArticle;
    std::string m_TransArticle;
    std::vector<uint32_t> m_PerfectiveIds;
    std::string m_RefLemma;
    std::string m_RefArticle;
};

class EntryPage : public QWizardPage {
    Q_OBJECT
public:
    explicit EntryPage(NewArticleWizard* wizard);
    bool validatePage() override;

private:
    NewArticleWizard* m_Wizard;
    QLineEdit* m_Entry;
    QLineEdit* m_MeanNum;
};

class PosPage : public QWizardPage {
    Q_OBJECT
public:
    explicit PosPage(NewArticleWizard* wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    NewArticleWizard* m_Wizard;
    QListWidget* m_List;
};

class TranslationsPage : public QWizardPage {
    Q_OBJECT
public:
    explicit TranslationsPage(NewArticleWizard* wizard);
    void initializePage() override;
    bool validatePage() override;
    int nextId() const override;

private:
    NewArticleWizard* m_Wizard;
    QListWidget* m_List;
};

class RefArticlesPage : public QWizardPage {
    Q_OBJECT
public:
    explicit RefArticlesPage(NewArticleWizard* wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    NewArticleWizard* m_Wizard;
    QListWidget* m_List;
};
