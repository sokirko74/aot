// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
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
    delete m_piGramTab;
    m_pOborDic.reset(nullptr);
    m_pThesaurus.reset(nullptr);
    m_pProfessions.reset(nullptr);
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
        s = convert_from_utf8(s.c_str(), m_Language);
        if (!s.empty())
            C.insert(s);
    };
};

void CSyntaxOpt::OutputErrorString(std::string strMsg) const {
    if (!m_bSilentMode)
        ErrorMessage("Synan", strMsg);
}


bool GetRegString(std::string key, std::string &value) {
    try {
        value = GetRegistryString(key);
        if (value.empty())
            throw CExpc("");
    }
    catch (CExpc &) {
        char strMsg[400];
        strcpy(strMsg, "Failed to read registry entry ");
        strcat(strMsg, key.c_str());
        ErrorMessage("SynAn", strMsg);
        return false;
    }

    return true;
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
        RmlMakeLower(terminStr, lang);
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
        m_pProfessions->m_vectorDatItems = CollectLowerTerms(thes.get(), "PROF", m_Language);
    }

    m_pThesaurus->SortIndexes();
}


const char g_strRusRegOborDicPath[] = "Software\\Dialing\\Obor\\DictPath";
const char g_strGerRegOborDicPath[] = "Software\\Dialing\\GerObor\\DictPath";

void CSyntaxOpt::InitializeOptions() {
    m_pProfessions.reset(new SDatItems(0xffff));     //NOUN

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
    LoadTermins(GetOborDictionary());
    InitOptionsLanguageSpecific();
};

const char *CSyntaxOpt::GetGroupNameByIndex(long lType) const {
    if (lType < 0) return 0;
    if (lType >= m_SyntaxGroupTypes.size()) return 0;
    return m_SyntaxGroupTypes[lType].c_str();
}


int CSyntaxOpt::GetGroupTypebyName(const char *TypeName) const {

    if (!TypeName) return false;

    for (int i = 0; i < m_SyntaxGroupTypes.size(); i++) {
        if (m_SyntaxGroupTypes[i] == TypeName) {
            return i;
        }
    }
    return -1;

}

int CSyntaxOpt::GetSyntaxGroupOrRegister(const char *TypeName) const {
    int i = GetGroupTypebyName(TypeName);
    if (i != -1)
        return i;

    m_SyntaxGroupTypes.push_back(TypeName);

    return m_SyntaxGroupTypes.size() - 1;

}
