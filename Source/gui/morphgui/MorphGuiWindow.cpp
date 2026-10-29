#include "MorphGuiWindow.h"

#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStyle>
#include <QTableWidget>
#include <QVBoxLayout>

// interface languages, the ids of the radio buttons
enum {
    InLangAll = 0,
    InLangRussian = 1,
    InLangEnglish = 2,
    InLangGerman = 3
};

enum {
    ColumnLanguage = 0,
    ColumnStatus,
    ColumnType,
    ColumnNorm,
    ColumnForm,
    ColumnId,
    ColumnHomo,
    ColumnWord,
    ColumnAncode,
    ColumnAccents,
    ColumnsCount
};

static QString fromUtf8(const std::string& s) {
    return QString::fromUtf8(s.c_str());
}

// the lemmatizer works with single-byte strings
static QString fromSingleByte(MorphLanguageEnum langua, const std::string& s) {
    return fromUtf8(convert_to_utf8(s, langua));
}

static QString getStrByLanguage(MorphLanguageEnum langua) {
    switch (langua) {
        case morphRussian: return "Rus";
        case morphEnglish: return "Eng";
        case morphGerman: return "Ger";
        default: return "";
    }
}

// all forms of the paradigm equal to the input word, the accented vowel is in upper case
static QString getAccentedForms(const CFormInfo& paradigm, const QString& form, MorphLanguageEnum langua) {
    QString result;
    QString upperForm = form.toUpper();
    for (uint16_t i = 0; i < paradigm.GetCount(); i++) {
        QString s = fromSingleByte(langua, paradigm.GetWordForm(i));
        if (s != upperForm) {
            continue;
        }
        BYTE accentedCharNo = paradigm.GetAccentedVowel(i);
        if (accentedCharNo != 255 && accentedCharNo < s.length()) {
            s = s.toLower();
            s[accentedCharNo] = s[accentedCharNo].toUpper();
            result += s + ";";
        }
    }
    return result;
}

MorphGuiWindow::MorphGuiWindow(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Morphology");

    auto top = new QHBoxLayout();
    {
        auto left = new QVBoxLayout();
        left->addWidget(new QLabel("Word to find"));
        m_Source = new QLineEdit();
        connect(m_Source, &QLineEdit::returnPressed, this, &MorphGuiWindow::onFind);
        left->addWidget(m_Source);
        top->addLayout(left, 1);

        auto buttons = new QVBoxLayout();
        auto findBtn = new QPushButton(style()->standardIcon(QStyle::SP_FileDialogContentsView), "Find");
        findBtn->setToolTip("Find");
        findBtn->setDefault(true);
        connect(findBtn, &QPushButton::clicked, this, &MorphGuiWindow::onFind);
        auto exitBtn = new QPushButton(style()->standardIcon(QStyle::SP_DialogCloseButton), "Exit");
        exitBtn->setToolTip("Exit");
        connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);
        buttons->addWidget(findBtn);
        buttons->addWidget(exitBtn);
        top->addLayout(buttons);
    }

    auto middle = new QHBoxLayout();
    {
        m_Results = new QTableWidget(0, ColumnsCount);
        m_Results->setHorizontalHeaderLabels({ "Lang", "Status", "Type", "Normal form", "Information",
            "ID / Source", "HomoWeight", "WordWeight", "Ancode", "Accents" });
        m_Results->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_Results->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_Results->verticalHeader()->setVisible(false);
        m_Results->horizontalHeader()->setStretchLastSection(true);
        auto results = new QVBoxLayout();
        results->addWidget(new QLabel("Results"));
        results->addWidget(m_Results);
        middle->addLayout(results, 1);

        auto subjectBox = new QGroupBox("Subject:");
        auto subjectLayout = new QVBoxLayout(subjectBox);
        m_Subjects = new QButtonGroup(this);
        for (auto [name, subj] : { std::pair{ "Finance", subjFinance }, { "Computer", subjComputer }, { "Literature", subjLiterature } }) {
            auto b = new QRadioButton(name);
            m_Subjects->addButton(b, subj);
            subjectLayout->addWidget(b);
        }
        m_Subjects->button(subjFinance)->setChecked(true);
        subjectLayout->addStretch();
        connect(m_Subjects, &QButtonGroup::idClicked, this, &MorphGuiWindow::onSubjectChanged);
        middle->addWidget(subjectBox);
    }

    auto bottom = new QHBoxLayout();
    {
        m_Languages = new QButtonGroup(this);
        for (auto [name, id] : { std::pair{ "All", InLangAll }, { "Russian", InLangRussian }, { "English", InLangEnglish }, { "German", InLangGerman } }) {
            auto b = new QRadioButton(name);
            m_Languages->addButton(b, id);
            bottom->addWidget(b);
        }
        m_Languages->button(InLangAll)->setChecked(true);
        bottom->addStretch();
    }

    auto layout = new QVBoxLayout(this);
    layout->addLayout(top);
    layout->addLayout(middle, 1);
    layout->addLayout(bottom);
    resize(1100, 500);
}

bool MorphGuiWindow::loadMorphology(std::string& error) {
    if (!IsRmlRegistered(error)) {
        return false;
    }
    try {
        m_Russian.LoadMorphology(morphRussian);
        m_Russian.m_pLemmatizer->LoadStatisticRegistry(subjFinance);
        m_Russian.m_pLemmatizer->m_bUseStatistic = true;
        m_English.LoadMorphology(morphEnglish);
        m_German.LoadMorphology(morphGerman);
    }
    catch (std::exception& e) {
        error = e.what();
        return false;
    }
    return true;
}

CMorphanHolder& MorphGuiWindow::getHolder(MorphLanguageEnum langua) {
    switch (langua) {
        case morphRussian: return m_Russian;
        case morphEnglish: return m_English;
        case morphGerman: return m_German;
        default: throw CExpc("Unknown language");
    }
}

bool MorphGuiWindow::buildParadigmsForOneLanguage(MorphLanguageEnum langua, const QString& src, bool onlyDigits) {
    const CAgramtab* gramtab = getHolder(langua).m_pGramTab;
    const CLemmatizer* lemmatizer = getHolder(langua).m_pLemmatizer;
    if (lemmatizer == nullptr) {
        return false;
    }

    std::vector<CFormInfo> paradigms;
    if (onlyDigits) {
        CFormInfo p;
        if (lemmatizer->CreateParadigmFromID(src.toUInt(), p)) {
            paradigms.push_back(p);
        }
    }
    else {
        std::string s8 = convert_from_utf8(src.toUtf8().constData(), langua);
        lemmatizer->CreateParadigmCollection(false, s8, src[0].isUpper(), true, paradigms);
    }

    bool foundSomething = false;
    for (auto& paradigm : paradigms) {
        int row = m_Results->rowCount();
        m_Results->insertRow(row);
        auto setText = [&](int column, const QString& text) {
            m_Results->setItem(row, column, new QTableWidgetItem(text));
        };
        setText(ColumnLanguage, getStrByLanguage(langua));
        setText(ColumnStatus, paradigm.m_bFound ? "Found" : "Predicted");
        foundSomething |= paradigm.m_bFound;
        if (paradigm.m_bFound) {
            setText(ColumnId, QString::number(paradigm.GetParadigmId()));
            try {
                setText(ColumnHomo, QString::number(paradigm.GetHomonymWeight()));
            }
            catch (...) {
                setText(ColumnHomo, "not found");
            }
            setText(ColumnWord, QString::number(paradigm.GetWordWeight()));
            setText(ColumnAccents, getAccentedForms(paradigm, src, langua));
        }
        else {
            setText(ColumnId, fromSingleByte(langua, paradigm.GetSrcNorm()));
        }

        std::string typeAncode = paradigm.GetCommonAncode();
        if (!typeAncode.empty()) {
            grammems_mask_t grammems;
            gramtab->GetGrammems(typeAncode.c_str(), grammems);
            setText(ColumnType, fromUtf8(gramtab->GrammemsToStr(grammems)));
        }

        setText(ColumnNorm, fromSingleByte(langua, paradigm.GetWordForm(0)));
        QString ancode;
        try {
            ancode = fromUtf8(paradigm.GetSrcAncode());
        }
        catch (...) {
            ancode = fromUtf8(paradigm.GetAncode(0));
        }
        setText(ColumnAncode, ancode);
        setText(ColumnForm, fromUtf8(paradigm.GetGramInfoStr(gramtab)));
    }
    return foundSomething;
}

void MorphGuiWindow::onFind() {
    QString src = m_Source->text().trimmed();
    if (src.isEmpty()) {
        return;
    }
    m_Results->setRowCount(0);

    std::vector<MorphLanguageEnum> languages;
    switch (m_Languages->checkedId()) {
        case InLangRussian: languages = { morphRussian }; break;
        case InLangEnglish: languages = { morphEnglish }; break;
        case InLangGerman: languages = { morphGerman }; break;
        default: languages = { morphRussian, morphGerman, morphEnglish }; break;
    }

    // if the input consists only of digits, it is a paradigm id
    bool onlyDigits = std::all_of(src.begin(), src.end(), [](QChar c) { return c.isDigit(); });

    bool foundSomething = false;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (auto langua : languages) {
        try {
            if (!onlyDigits) {
                try {
                    if (!CheckLanguage(src.toUtf8().toStdString(), langua)) {
                        continue;
                    }
                }
                catch (convert_exception&) {
                    continue;
                }
            }
            if (buildParadigmsForOneLanguage(langua, src, onlyDigits)) {
                foundSomething = true;
            }
        }
        catch (...) {
        }
    }

    // predicted paradigms are shown only if nothing was found in dictionaries
    if (foundSomething) {
        for (int i = m_Results->rowCount() - 1; i >= 0; --i) {
            if (m_Results->item(i, ColumnStatus)->text() == "Predicted") {
                m_Results->removeRow(i);
            }
        }
    }
    m_Results->resizeColumnsToContents();
    QApplication::restoreOverrideCursor();
}

void MorphGuiWindow::onSubjectChanged(int subject) {
    m_Russian.m_pLemmatizer->LoadStatisticRegistry((SubjectEnum)subject);
    m_English.m_pLemmatizer->LoadStatisticRegistry((SubjectEnum)subject);
}
