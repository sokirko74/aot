// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "SentencesCollection.h"
#include "synan/RusSynanLib/RusSyntaxOpt.h"
#include "synan/GerSynanLib/GerSyntaxOpt.h"
#include "synan/EngSynanLib/EngSyntaxOpt.h"


CSyntaxOpt *NewOptions(MorphLanguageEnum langua) {
    if (langua == morphGerman)
        return new CGerSyntaxOpt(langua);

    if (langua == morphEnglish)
        return new CEngSyntaxOpt(langua);

    return new CRusSyntaxOpt(morphRussian);
}

CSentencesCollection::CSentencesCollection() {
    m_bDeleteEqualPrimitiveClauseVariants = false;
    m_bEnableProgressBar = false;
    m_bLogProcessedSentence = false;
    m_pSyntaxOptions = 0;
}

void CSentencesCollection::ClearOptions() {
    if (m_pSyntaxOptions) {
        m_pSyntaxOptions->DestroyOptions();
        delete m_pSyntaxOptions;
        m_pSyntaxOptions = 0;
    };
};

void CSentencesCollection::ClearSentences() {
    for (int i = 0; i < m_vectorSents.size(); i++)
        delete m_vectorSents[i];

    m_vectorSents.clear();
};


CSentencesCollection::~CSentencesCollection() {
    ClearSentences();
    ClearOptions();
}


bool CSentencesCollection::CreateOptions(MorphLanguageEnum langua) {
    try {
        ClearOptions();
        m_pSyntaxOptions = NewOptions(langua);
        assert (m_pSyntaxOptions);
    }
    catch (...) {
        return false;
    }
    return true;
}

void CSentencesCollection::InitializeProcesser() {
    m_pSyntaxOptions->InitializeOptions();
}


bool AreEqualByWords(const CMorphVariant &_X1, const CMorphVariant &_X2) {
    if (_X1.GetUnitsCount() != _X2.GetUnitsCount())
        return false;

    for (int i = 0; i < _X1.GetUnitsCount(); i++)
        if ((_X1.GetUnitType(i) != EClause)
            || (_X2.GetUnitType(i) != EClause)
                )
            if (_X1.GetHomNum(i) != _X2.GetHomNum(i))
                return false;

    return true;
}

bool CSentencesCollection::ProcessData(const CLemmatizedText *piPlmLine) {
    try {
        bool bRes = ReadAndProcessSentences(piPlmLine);

        if (m_bDeleteEqualPrimitiveClauseVariants) {
            for (long i = 0; i < m_vectorSents.size(); i++) {
                CSentence &Clauses = *(m_vectorSents[i]);

                for (long ClauseNo = 0; ClauseNo < Clauses.GetClausesCount(); ClauseNo++) {
                    CClause &C = Clauses.GetClause(ClauseNo);
                    for (SVI j = C.m_SynVariants.begin(); j != C.m_SynVariants.end(); j++) {
                        SVI k = j;
                        k++;


                        while (k != C.m_SynVariants.end())
                            if (AreEqualByWords(*j, *k)) {
                                k = C.m_SynVariants.erase(k);
                            } else
                                k++;
                    };
                };
            };
        };


        if (!bRes)
            return false;


    }
    catch (...) {
        return false;
    }

    return true;
}


bool CSentencesCollection::ReadAndProcessSentences(const CLemmatizedText* text) {
    if (!m_pSyntaxOptions->IsValid())
        return false;

    if (m_bEnableProgressBar)
        LOGD << "Starting Syntax";

    time_t t1;
    time(&t1);

    for (size_t LineNo = 0; LineNo < text->m_LemWords.size();) {
        CSentence *S = m_pSyntaxOptions->NewSentence();
        if (!S) {
            throw CExpc("Cannot allocate space for the new sentence!");
        };
        assert(S->GetOpt() != nullptr);
        bool bResult = S->GetNextSentence(text, LineNo);

        if (m_bLogProcessedSentence) {
            LOGD << S->GetSentenceBeginStr();
        };

        if (!bResult) {
            //  a parse error occurs
            delete S;
            return false;
        };

        if (S->m_Words.empty()) {
            // no not-empty sentence can be found, we are at the end of the text
            delete S;
            break;
        }


        try {
            if (!S->CreateSyntaxStructure()) {
                delete S;
                return false;
            };
            if (m_bEnableProgressBar) {
                if (S->m_bPanicMode)
                    printf("    found a \"panic\" sentence\n");
            };
            m_vectorSents.push_back(S);
        }
        catch (...) {
            if (m_bEnableProgressBar) {
                printf("\n");
                if (!S->GetWords().empty())
                    printf("exception at offset %i\n", S->GetWords()[0].m_GraphematicalUnitOffset);
            };

            delete S;
            m_pSyntaxOptions->OutputErrorString("An exception in Synan occurred!");
            return false;
        };


    }

    if (m_bEnableProgressBar) {
        time_t t2;
        time(&t2);
        LOGD << "Finish             ";
        int seconds = t2 - t1;
        LOGD << "Time : " << seconds;
        LOGD << "Count of tokens : " << text->m_LemWords.size();
        if (seconds > 0)
            LOGD << "The speed is " << text->m_LemWords.size() / seconds << " tokens pro second";
    };

    return true;
}

void CSentencesCollection::put_SilentMode(bool newVal) {
    m_pSyntaxOptions->m_bSilentMode = newVal;
}

void CSentencesCollection::SetLemmatizer(const CLemmatizer *R) {
    m_pSyntaxOptions->SetLemmatizerWeak(R);
}

void CSentencesCollection::SetOborDic(const CDictionary *D) {
    m_pSyntaxOptions->SetOborDictionary(D);
}

void CSentencesCollection::SetEnableAllThesauri(bool Value) {
    m_pSyntaxOptions->m_bEnableCompThesaurus = Value;
    m_pSyntaxOptions->m_bEnableLocThesaurus = Value;
    m_pSyntaxOptions->m_bEnableFinThesaurus = Value;
    m_pSyntaxOptions->m_bEnableOmniThesaurus = Value;
}
