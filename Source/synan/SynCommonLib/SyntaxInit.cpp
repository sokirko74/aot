// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include  "stdafx.h"
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <assert.h>

#include  "SyntaxInit.h"


CSyntaxOpt::CSyntaxOpt(MorphLanguageEnum Langua) {
    m_bSilentMode = false;
    m_KillHomonymsMode = CoverageKillHomonyms;
    m_Language = Langua;
    m_bEnableFinThesaurus = true;
    m_bEnableCompThesaurus = true;
    m_bEnableOmniThesaurus = true;
    m_bEnableLocThesaurus = true;
}

bool CSyntaxOpt::IsValid() const {
    return (GetOborDictionary() != 0) && (GetGramTab() != 0);
};


void CSyntaxOpt::DestroyOptions() {
    m_pOborDic.reset(nullptr);
    m_pThesaurus.reset(nullptr);
    
};

void CSyntaxOpt::ReadListFile(const std::string &FileName, StringHashSet& C) {

    std::ifstream inp(FileName.c_str());
    if (!inp.is_open() ) {
        throw CExpc(Format("Can't open file \'%s\'.", FileName.c_str()));
    }

    std::string s;
    while (std::getline(inp, s)) {
        int i = s.find("//");
        if (i != std::string::npos)
            s.erase(i);
        Trim(s);
        if (!s.empty())
            C.insert(s);
    };
};

void CSyntaxOpt::OutputErrorString(std::string strMsg) const {
    if (!m_bSilentMode)
        ErrorMessage("Synan", strMsg);
}


std::unique_ptr<CThesaurus>& CSyntaxOpt::GetThesPointerByThesId(uint32_t ThesId) {
    switch (ThesId) {
        case LocThes :
            return m_LocThes;
        case FinThes :
            return m_FinThes;
        case CompThes :
            return m_CompThes;
        case OmniThes :
            return m_OmniThes;
    };
    return m_LocThes;

};

const CThesaurus *CSyntaxOpt::GetThesByThesId(uint32_t ThesId) const {
    switch (ThesId) {
        case LocThes :
            return m_LocThes.get();
        case FinThes :
            return m_FinThes.get();
        case CompThes :
            return m_CompThes.get();
        case OmniThes :
            return m_OmniThes.get();
    };
    return m_LocThes.get();
};

void CSyntaxOpt::LoadTerminsForOneThesaurus(std::string ThesName) {
    auto& P = GetThesPointerByThesId(GetThesTypeByStr(ThesName));

    if (P.get() == nullptr) {
        P.reset(m_pThesaurus->LoadThesaurus(ThesName));
    }

    if (!P.get()) {
        throw CExpc ("cannot initialize thesaurus pointer");
    }

    m_pThesaurus->ReadThesaurusForSyntax(ThesName, P.get());
};

StringHashSet CollectLowerTerms(const CThesaurus* Thes, std::string conceptStr, MorphLanguageEnum lang)
{
    StringHashSet lowerTerms;
    for (auto i : Thes->QueryLowerTermins("PROF", lang))
    {
        std::string terminStr = Thes->m_Termins[i].m_TerminStr;
        MakeLowerUtf8(terminStr);
        lowerTerms.insert(terminStr);
    };
    return lowerTerms;
}

void CSyntaxOpt::LoadTermins(const CDictionary *piOborDic) {
    if (m_bEnableLocThesaurus)
        LoadTerminsForOneThesaurus("RML_THES_LOC");

    if (m_bEnableFinThesaurus)
        LoadTerminsForOneThesaurus("RML_THES_FIN");

    if (m_bEnableCompThesaurus)
        LoadTerminsForOneThesaurus("RML_THES_COMP");

    if (m_bEnableOmniThesaurus) {
        LoadTerminsForOneThesaurus("RML_THES_OMNI");
        auto& thes = GetThesPointerByThesId(GetThesTypeByStr("RML_THES_OMNI"));
        for (auto a : CollectLowerTerms(thes.get(), "PROF", m_Language)) {
            m_Professions.add_lemma(a);
        }
    }

    m_pThesaurus->SortIndexes();
}


const char g_strRusRegOborDicPath[] = "Software\\Dialing\\Obor\\DictPath";
const char g_strGerRegOborDicPath[] = "Software\\Dialing\\GerObor\\DictPath";

void CSyntaxOpt::InitializeOptions() {
    
    if (GetLemmatizer() == nullptr) {
        auto pLem = NewLemmatizer();
        pLem->LoadDictionariesRegistry();
        m_piLemmatizer.reset(pLem);
        SetLemmatizerWeak(pLem);
    };


    //loading obor.dic
    m_pOborDic.reset(NewOborDic(this));

    if (GetOborDictionary() == nullptr) {
        std::string strPath;
        auto pOborDictionary = new CDictionary;
        if (m_Language == morphRussian)
            strPath = GetRegistryString(g_strRusRegOborDicPath);
        else
            strPath = GetRegistryString(g_strGerRegOborDicPath);

        pOborDictionary->Load(strPath.c_str());

        m_piOborDictionary.reset(pOborDictionary);
        m_piOborDictionaryWeak = pOborDictionary;
    }

    if (!m_pOborDic->ReadOborots(m_piOborDictionaryWeak)) {
        throw CExpc("Failed read oborots");
    }
    m_pThesaurus.reset(NewThesaurus(this));
    m_piGramTab = GetMHolder(m_Language).m_pGramTab;
    InitOptionsLanguageSpecific();
    LoadTermins(GetOborDictionary());
};

const char *CSyntaxOpt::GetGroupNameByIndex(long lType) const {
    return m_SyntaxGroupTypes[lType].c_str();
}


int CSyntaxOpt::GetGroupTypebyName(const char *TypeName) const {

    if (!TypeName) return false;

    for (auto& [key, value] : m_SyntaxGroupTypes) {
        if (value == TypeName) {
            return key;
        }
    }
    return -1;

}

int CSyntaxOpt::GetSyntaxGroupOrRegister(const std::string& group_str) const {
    int i = GetGroupTypebyName(group_str.c_str());
    if (i != -1)
        return i;

    int group_id = m_SyntaxGroupTypes.size();
    m_SyntaxGroupTypes[group_id] = group_str;
    return group_id;

}
