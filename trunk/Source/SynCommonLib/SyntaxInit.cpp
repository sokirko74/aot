// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma warning(disable:4786)

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
    m_pProfessions.reset(nullptr);
};

bool CSyntaxOpt::ReadListFile(const string &FileName, StringVector &C) {

    FILE *fp = fopen(FileName.c_str(), "r");
    if (!fp) {
        char strMsg[200];
        sprintf(strMsg, "Can't open file \'%s\'.", FileName.c_str());
        OutputErrorString(strMsg);
        return false;
    }

    char buffer[500];

    while (fgets(buffer, 500, fp)) {
        string s = buffer;
        int i = s.find("//");
        if (i != string::npos)
            s.erase(i);
        Trim(s);

        if (!s.empty())
            C.push_back(s);
    };

    fclose(fp);

    sort(C.begin(), C.end());

    return true;

};

void CSyntaxOpt::OutputErrorString(string strMsg) const {
    if (!m_bSilentMode)
        ErrorMessage("Synan", strMsg);
}


bool GetRegString(string key, string &value) {
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

std::unique_ptr<CThesaurus>& CSyntaxOpt::GetThesPointerByThesId(UINT ThesId) {
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

const CThesaurus *CSyntaxOpt::GetThesByThesId(UINT ThesId) const {
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

bool CSyntaxOpt::LoadTerminsForOneThesaurus(const char *ThesName) {
    auto& P = GetThesPointerByThesId(GetThesTypeByStr(ThesName));

    if (P.get() == nullptr) {
        P.reset(m_pThesaurus->LoadThesaurus(ThesName));
    }

    if (!P.get()) return false;

    return m_pThesaurus->ReadThesaurusForSyntax(ThesName, P.get(), m_pProfessions->m_vectorDatItems);
};

bool CSyntaxOpt::LoadTermins(const CDictionary *piOborDic) {
    try {
        if (m_bEnableLocThesaurus)
            LoadTerminsForOneThesaurus("RML_THES_LOC");

        if (m_bEnableFinThesaurus)
            LoadTerminsForOneThesaurus("RML_THES_FIN");

        if (m_bEnableCompThesaurus)
            LoadTerminsForOneThesaurus("RML_THES_COMP");

        if (m_bEnableOmniThesaurus)
            LoadTerminsForOneThesaurus("RML_THES_OMNI");

        m_pThesaurus->SortIndexes();
    }
    catch (...) {
        return false;
    }
    return true;
}


const char g_strRusRegOborDicPath[] = "Software\\Dialing\\Obor\\DictPath";
const char g_strGerRegOborDicPath[] = "Software\\Dialing\\GerObor\\DictPath";

bool CSyntaxOpt::InitializeOptions() {
    m_pProfessions.reset(new SDatItems(0xffff));     //NOUN

    try {
        if (!GetGramTab()) {
            auto G = NewGramTab();
            if (!G) return false;
            if (!G->LoadFromRegistry()) return false;
            m_piGramTab.reset(G);
            SetGramTabWeak(G);
        }
    }
    catch (...) {
        OutputErrorString("Failed to load \"agramtab.dll\"");
        return false;
    }

    if (GetLemmatizer() == nullptr) {
        auto pLem = NewLemmatizer();
        string strError;
        if (!pLem->LoadDictionariesRegistry(strError)) return false;
        m_piLemmatizer.reset(pLem);
        SetLemmatizerWeak(pLem);
    };


    //loading obor.dic
    try {
        m_pOborDic.reset(NewOborDic(this));

        if (GetOborDictionary() == nullptr) {
            string strPath;
            auto pOborDictionary = new CDictionary;
            if (!pOborDictionary) {
                OutputErrorString("Failed to load \"StructDict.dll\"");
                return false;
            }
            if (m_Language == morphRussian)
                strPath = GetRegistryString(g_strRusRegOborDicPath);
            else
                strPath = GetRegistryString(g_strGerRegOborDicPath);

            pOborDictionary->Load(strPath.c_str());

            m_piOborDictionary.reset(pOborDictionary);
            m_piOborDictionaryWeak = pOborDictionary;
        }

        if (!m_pOborDic->ReadOborots(m_piOborDictionaryWeak)) {
            OutputErrorString("Failed to load \"StructDict.dll\"");
            return false;
        }

    }
    catch (CExpc &) {
        OutputErrorString("Failed to find registry entry for oborot dictionary");
        return false;
    }
    catch (...) {
        OutputErrorString("Failed to load \"StructDict.dll\" (Obor.dic)");
        return false;
    }

    m_pThesaurus.reset(NewThesaurus(this));

    if (!LoadTermins(GetOborDictionary()))
        OutputErrorString("Failed to load Thesaurus");

    if (!InitOptionsLanguageSpecific())
        return false;

    return true;
};

bool has_item(const StringVector *C, const char *item) {
    if (!item) return false;
    return binary_search(C->begin(), C->end(), string(item));
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
