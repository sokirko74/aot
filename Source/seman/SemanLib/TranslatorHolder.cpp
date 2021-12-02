#include "stdafx.h"
#include "TranslatorHolder.h"

const char g_strFreqCompPath[] = "Software\\Dialing\\EngRusFreqDict\\Comp";
const char g_strFreqFinPath[] = "Software\\Dialing\\EngRusFreqDict\\Fin";
const char g_strFreqHudPath[] = "Software\\Dialing\\EngRusFreqDict\\Hud";
const char g_strAdjNounDualFreqPath[] = "Software\\Dialing\\DualFreq\\linkA";


std::string CTranslatorHolder::GetThesPath(int ThesId) const {
    switch (ThesId) {
        case LocThes  :
            return GetRegistryString("Software\\Dialing\\EngRusFreqDict\\Loc");
        case OmniThes  :
            return GetRegistryString("Software\\Dialing\\EngRusFreqDict\\Omni");
        case CompThes :
            return GetRegistryString("Software\\Dialing\\EngRusFreqDict\\Comp");
        case FinThes  :
            return GetRegistryString("Software\\Dialing\\EngRusFreqDict\\Fin");
        default :
            throw CExpc("Unknown thesaurus");
    };
}


CTranslatorHolder::CTranslatorHolder() {

    m_BinaryDictEnabled = true;
    m_LastUpdateTime = 0;
    m_bSilentMode = false;
}

CTranslatorHolder::~CTranslatorHolder() {
}


CSentencesCollection *CTranslatorHolder::GetSynan() {
    return &m_RusHolder.m_Synan;
};

const CLemmatizer *CTranslatorHolder::GetRusLemmatizer() const {
    return m_RusHolder.m_pLemmatizer;
};

const CLemmatizer *CTranslatorHolder::GetEngLemmatizer() const {
    return m_EngHolder.m_pLemmatizer;
};

const CRusGramTab *CTranslatorHolder::GetRusGramTab() const {
    return (const CRusGramTab *) m_RusHolder.m_pGramTab;
};

const CEngGramTab *CTranslatorHolder::GetEngGramTab() const {
    return (const CEngGramTab *) m_EngHolder.m_pGramTab;
};

const CLemmatizer *CTranslatorHolder::GetLemmatizer(MorphLanguageEnum langua) const {
    switch (langua) {
        case morphEnglish:
            return GetEngLemmatizer();
        case morphRussian:
            return GetRusLemmatizer();
        default        :
            return GetRusLemmatizer();
    }
};

const CAgramtab *CTranslatorHolder::GetGramTab(MorphLanguageEnum langua) const {
    switch (langua) {
        case morphEnglish:
            return GetEngGramTab();
        case morphRussian:
            return GetRusGramTab();
        default        :
            return GetRusGramTab();
    }
};

const CThesaurus *CTranslatorHolder::GetThes(int ThesId) const {
    // by default it was FinThes in Seman and LocThes in Synan, now LocThes is default everywhere
    return m_RusHolder.m_Synan.GetOpt()->GetThesByThesId(ThesId);
};

std::string CTranslatorHolder::GetThesStr(int ThesId) const {
    switch (ThesId) {
        case LocThes  :
            return "LocThes";
        case CompThes :
            return "CompThes";
        case FinThes  :
            return "FinThes";
        case OmniThes  :
            return "OmniThes";
        default       :
            return "NoneThes";
    };
};


bool CTranslatorHolder::InitBinaryDictionary() {
    try {
        m_BinaryDictEnabled = m_BinaryDict.Load();
    }
    catch (...) {
        m_BinaryDictEnabled = false;
        ErrorMessage("Cannot load binary dictionary");
    };
    return m_BinaryDictEnabled;
};

bool CTranslatorHolder::InitAspDict() {
    try {
        m_AspDictEnabled = m_AspDict.Load();
    }
    catch (...) {
        m_AspDictEnabled = false;
        ErrorMessage(_R("Cannot load словарь видовых  пар"));
    };
    return m_AspDictEnabled;

};

bool CTranslatorHolder::Init() {
    std::vector<double> ts;
    clock_t m_TimeSpan = clock();
    m_EngHolder.LoadLemmatizer(morphEnglish);
    fprintf(stderr, "morphEnglish %f\n", (double) (clock() - m_TimeSpan));
    m_TimeSpan = clock();
    if (!m_RusHolder.LoadSyntax(morphRussian))
        return false;
    fprintf(stderr, "morphRussian %f\n", (double) (clock() - m_TimeSpan));
    m_TimeSpan = clock();
    if (!InitAspDict()) {
        ErrorMessage("Cannot load asp dict");
        return false;
    }

    try {


        if (!m_CompFreq.Load(GetRegistryString(g_strFreqCompPath))) {
            ErrorMessage("Cannot load CompFreq");
            return false;
        };


        if (!m_FinFreq.Load(GetRegistryString(g_strFreqFinPath))) {
            ErrorMessage("Cannot load FinFreq");
            return false;
        };


        if (!m_HudFreq.Load(GetRegistryString(g_strFreqHudPath))) {
            ErrorMessage("Cannot load HudFreq");
            return false;;
        };


    }
    catch (...) {
        ErrorMessage("Cannot load some freq-dicts");
        return false;;
    };


    if (!InitBinaryDictionary()) {
        return false;
    };


    try {
        m_AdjNounDualFreq.Load(GetRegistryString(g_strAdjNounDualFreqPath));
    }
    catch (...) {
        ErrorMessage("Cannot load English ADJ-NOUN dict");
        return false;;
    };

    return true;
}


StringVector CTranslatorHolder::GetAspVerb(long ParadigmId, bool IsPerfective) {
    StringVector Res;
    if (!m_AspDictEnabled) return Res;


    std::vector<uint32_t> ResVector;

    if (!IsPerfective)
        m_AspDict.nonperf2perf(ParadigmId, ResVector);
    else
        m_AspDict.perf2nonperf(ParadigmId, ResVector);


    for (long i = 0; i < ResVector.size(); i++) {
        CFormInfo Prd;
        bool bRes = GetRusLemmatizer()->CreateParadigmFromID(ResVector[i], Prd);
        assert (bRes);
        Res.push_back(Prd.GetWordForm(0));
    };

    return Res;
};

StringVector CTranslatorHolder::GetAspVerb(std::string Verb, bool IsPerfective) {
    StringVector Empty;
    try {

        std::vector<CFormInfo> ParadigmCollection;
        GetRusLemmatizer()->CreateParadigmCollection(true, Verb, false, false, ParadigmCollection);

        for (int i = 0; i < ParadigmCollection.size(); i++)
            if (GetRusGramTab()->GetPartOfSpeech(ParadigmCollection[i].GetSrcAncode().c_str()) == INFINITIVE) {
                uint64_t common_grm;

                GetRusGramTab()->GetGrammems(ParadigmCollection[i].GetCommonAncode().c_str(), common_grm);

                if ((IsPerfective && (common_grm & _QM(rPerfective)))
                    || (!IsPerfective && (common_grm & _QM(rNonPerfective)))
                        )
                    return GetAspVerb(ParadigmCollection[i].GetParadigmId(), IsPerfective);
            }


    }
    catch (...) {
        return Empty;
    };

    return Empty;
};


void CTranslatorHolder::ErrorMessage(std::string Mess) {
    if (m_bSilentMode)
        m_LastError = Mess;
    else
        ::ErrorMessage("Seman Error", Mess);
};


uint32_t CTranslatorHolder::GetFirstParadigmIdAndLemma(const MorphLanguageEnum langua, std::string WordForm,
                                                       part_of_speech_mask_t Poses,
                                                       std::string &ResultLemma) const {
    const CLemmatizer *L = GetLemmatizer(langua);
    const CAgramtab *G = GetGramTab(langua);
    try {

        std::vector<CFormInfo> ParadigmCollection;
        L->CreateParadigmCollection(false, WordForm, false, false, ParadigmCollection);

        if (Poses == 0) {
            if (!ParadigmCollection.empty()) {
                ResultLemma = ParadigmCollection[0].GetWordForm(0);
                return ParadigmCollection[0].GetParadigmId();
            }
        };

        for (long k = 0; k < ParadigmCollection.size(); k++) {
            std::string AnCode = ParadigmCollection[k].GetAncode(0);
            long POS = G->GetPartOfSpeech(AnCode.c_str());
            if ((1 << POS) & Poses) {
                ResultLemma = ParadigmCollection[k].GetWordForm(0);
                return ParadigmCollection[k].GetParadigmId();
            };
        };
        return UnknownParadigmId;
    }
    catch (...) {
        return UnknownParadigmId;
    };

};


uint32_t CTranslatorHolder::GetFirstParadigmId(const MorphLanguageEnum langua, std::string Lemma,
                                               part_of_speech_mask_t Poses) const {
    const CLemmatizer *L = GetLemmatizer(langua);
    const CAgramtab *G = GetGramTab(langua);

    try {

        std::vector<CFormInfo> ParadigmCollection;
        L->CreateParadigmCollection(true, Lemma, false, false, ParadigmCollection);


        if (Poses == 0) {
            if (!ParadigmCollection.empty())
                return ParadigmCollection[0].GetParadigmId();
        };
        for (long k = 0; k < ParadigmCollection.size(); k++) {
            std::string AnCode = ParadigmCollection[k].GetAncode(0);

            long POS = G->GetPartOfSpeech(AnCode.c_str());

            if ((1 << POS) & Poses)
                return ParadigmCollection[k].GetParadigmId();
        };
        return UnknownParadigmId;
    }
    catch (...) {
        return UnknownParadigmId;
    };

};

bool CTranslatorHolder::MakeSyntaxStr(const char *utf8text, CMyTimeSpanHolder &GlobalSpan) {
    return m_RusHolder.GetSentencesFromSynAn(utf8text, false);
}
