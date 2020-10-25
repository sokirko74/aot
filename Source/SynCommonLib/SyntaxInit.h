// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once

#include "stdafx.h"
#include  "ThesaurusForSyntax.h"
#include "../SimpleGrammarLib/SimpleGrammar.h"
#include "oborot.h"
#include <memory>


class CFormatCaller;

class COborDic;

class CAgramtab;

class CSentence;


struct SDatItems {
    uint32_t m_Poses;
    StringHashSet m_vectorDatItems;
    SDatItems(uint32_t Poses) { m_Poses = Poses; };

};


class CSyntaxOpt {
    std::unique_ptr<CThesaurus>& GetThesPointerByThesId(UINT ThesId);

protected:
    CAgramtab* m_piGramTab;
    std::unique_ptr<const CLemmatizer> m_piLemmatizer; // own pointer
    std::unique_ptr<const CDictionary> m_piOborDictionary;
    std::unique_ptr<CThesaurusForSyntax> m_pThesaurus;
    std::unique_ptr<COborDic> m_pOborDic;

    const CLemmatizer *m_piLemmatizerWeak = nullptr; // can be std::set from COM (external)
    const CDictionary *m_piOborDictionaryWeak = nullptr;
    std::unique_ptr<CThesaurus> m_FinThes;
    std::unique_ptr<CThesaurus> m_CompThes;
    std::unique_ptr<CThesaurus> m_LocThes;
    std::unique_ptr<CThesaurus> m_OmniThes;

public:
    std::unique_ptr<SDatItems> m_pProfessions;
    // special constants
    uint64_t m_PluralMask;
    uint64_t m_SingularMask;
    int m_SimilarNPGroupType;
    int m_GenNounGroupType;
    int m_PrepNounGroupType;
    int m_DirObjGroupType;
    int m_NPGroupType;
    int m_NounAdjGroupType;
    int m_NameGroupType;
    int m_DisruptConjRelation;
    int m_DisruptConjGroupType;
    int m_OborotGroupType;
    int m_WWWGroupType;
    int m_KEYBGroupType;
    int m_SubjRelation;
    int m_RusParenthesis;
    int m_Preposition;
    int m_Conjunction;
    uint64_t m_IndeclinableMask;
    int m_InfintiveClauseType;


    MorphLanguageEnum m_Language;
    bool m_bEnableCompThesaurus;
    bool m_bEnableFinThesaurus;
    bool m_bEnableOmniThesaurus;
    bool m_bEnableLocThesaurus;
    KillHomonymsEnum m_KillHomonymsMode;
    bool m_bSilentMode;

    size_t m_EmptyGroupType;
    size_t m_EmptyClauseType;


    mutable std::vector <std::string> m_SyntaxGroupTypes;

    CWorkGrammar m_FormatsGrammar;

    CSyntaxOpt(MorphLanguageEnum langua);

    virtual ~CSyntaxOpt() {};

    virtual void DestroyOptions();

    bool IsValid() const;

    bool ReadListFile(const std::string& FileName, StringHashSet& C);

    const CThesaurus *GetThesByThesId(UINT ThesId) const;

    void OutputErrorString(std::string strMsg) const;

    void LoadTermins(const CDictionary *piOborDic);

    void LoadTerminsForOneThesaurus(std::string ThesName);

    const CThesaurusForSyntax* GetThesaurus() const { return m_pThesaurus.get(); }
    virtual CThesaurusForSyntax* NewThesaurus(const CSyntaxOpt* opt) = 0;

    const CAgramtab *GetGramTab() const { return m_piGramTab; };

    void SetLemmatizerWeak(const CLemmatizer *L) { m_piLemmatizerWeak = L; };
    const CLemmatizer *GetLemmatizer() const { return m_piLemmatizerWeak; };
    virtual CLemmatizer *NewLemmatizer() const = 0;

    void SetOborDictionary(const CDictionary *L) { m_piOborDictionaryWeak = L; };
    const CDictionary *GetOborDictionary() const { return m_piOborDictionaryWeak; };

    const COborDic *GetOborDic() const { return m_pOborDic.get();};
    virtual COborDic * NewOborDic(const CSyntaxOpt* opt) = 0;

    bool InitializeOptions();

    virtual bool InitOptionsLanguageSpecific() = 0;

    const char *GetGroupNameByIndex(long lType) const;

    const size_t GetGroupTypesCount() const {
        return m_SyntaxGroupTypes.size();
    };

    int GetGroupTypebyName(const char *TypeName) const;

    virtual bool is_firm_group(int GroupType) const = 0;

    virtual bool IsGroupWithoutWeight(int GroupType, const char *cause) const = 0;

    virtual bool IsSimilarGroup(int type) const = 0;

    int GetSyntaxGroupOrRegister(const char *TypeName) const;
    virtual CSentence* NewSentence() const = 0;
};


bool GetRegString(std::string key, std::string &value);

inline bool has_item(const StringHashSet& C, const char* item) {
    if (!item) return false;
    return C.find(item) != C.end();
};

inline bool has_item(const StringHashSet& C, const std::string& item) {
    return C.find(item) != C.end();
};
