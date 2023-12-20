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


CTranslatorHolder::CTranslatorHolder(): m_RusHolder(morphRussian) {

    m_LastUpdateTime = 0;
    m_bSilentMode = false;
}

CTranslatorHolder::~CTranslatorHolder() {
}


CSentencesCollection *CTranslatorHolder::GetSynan() {
    return &m_RusHolder.m_Synan;
};

const CLemmatizer *CTranslatorHolder::GetRusLemmatizer() const {
    return GetMHolder(morphRussian).m_pLemmatizer;
};

const CLemmatizer *CTranslatorHolder::GetEngLemmatizer() const {
    return GetMHolder(morphEnglish).m_pLemmatizer;
};

const CRusGramTab *CTranslatorHolder::GetRusGramTab() const {
    return (const CRusGramTab *)GetMHolder(morphRussian).m_pGramTab;
};

const CEngGramTab *CTranslatorHolder::GetEngGramTab() const {
    return (const CEngGramTab *)GetMHolder(morphEnglish).m_pGramTab;
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

void CTranslatorHolder::Init() {
    GlobalLoadMorphHolder(morphRussian);
    GlobalLoadMorphHolder(morphEnglish);
    m_RusHolder.LoadSyntax();
    m_AspDict.Load();
    m_CompFreq.Load(GetRegistryString(g_strFreqCompPath));
    m_FinFreq.Load(GetRegistryString(g_strFreqFinPath));
    m_HudFreq.Load(GetRegistryString(g_strFreqHudPath));
    m_BinaryDict.Load();
    // data is lost (probably I can find it on some old CDs
    // m_AdjNounDualFreq.Load(GetRegistryString(g_strAdjNounDualFreqPath));
}


StringVector CTranslatorHolder::GetAspVerb(uint32_t ParadigmId, bool IsPerfective) {

    std::vector<uint32_t> ids;

    if (!IsPerfective)
        m_AspDict.nonperf2perf(ParadigmId, ids);
    else
        m_AspDict.perf2nonperf(ParadigmId, ids);


    StringVector res;
    for (auto& paradigm_id: ids) {
        CFormInfo p;
        bool b = GetRusLemmatizer()->CreateParadigmFromID(paradigm_id, p);
        assert (b);
        res.push_back(convert_to_utf8(p.GetWordForm(0), morphRussian));
    };

    return res;
};

StringVector CTranslatorHolder::GetAspVerb(std::string verb, bool IsPerfective) {
    StringVector Empty;
    try {

        std::vector<CFormInfo> ParadigmCollection;
        std::string s8 = convert_from_utf8(verb.c_str(), morphRussian);
        GetRusLemmatizer()->CreateParadigmCollection(true, s8, false, false, ParadigmCollection);

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

        std::vector<CFormInfo> paradigms;
        auto s8 = convert_from_utf8(WordForm.c_str(), langua);
        L->CreateParadigmCollection(false, s8, false, false, paradigms);
        for (auto& p: paradigms) {
            std::string AnCode = p.GetAncode(0);
            part_of_speech_t POS = G->GetPartOfSpeech(AnCode.c_str());
            if (((1 << POS) & Poses) == Poses) {
                ResultLemma = convert_to_utf8(p.GetWordForm(0), morphRussian);
                return p.GetParadigmId();
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
        Lemma = convert_from_utf8(Lemma.c_str(), langua);
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
