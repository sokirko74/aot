#pragma once

#include "morph_dict/common/utilit.h"
#include "morphan/LemmatizerLib/LemmatizedText.h"
#include "AhoKorasick.h"
#include "GLR.h"

#include "GrammarItem.h"

#include  <iostream>


enum ParseMethodEnum {
    GLRRestartParsing, GLRFullParse
};

struct CWorkRightRulePart {
    std::vector<TerminalSymbolType> m_RuleItems;

    //  calculated by CGrammarItem::m_SynMain in  CParseGrammar::EncodeGrammar
    size_t m_SynMainItemNo;


    bool operator==(const CWorkRightRulePart &X) const {
        return m_RuleItems == X.m_RuleItems
               && m_SynMainItemNo == X.m_SynMainItemNo;
    };

    bool operator<(const CWorkRightRulePart &X) const {
        if (m_RuleItems != X.m_RuleItems)
            return m_RuleItems < X.m_RuleItems;

        return m_SynMainItemNo < X.m_SynMainItemNo;
    };

};

enum FeatureOperatorEnum {
    foeCheck, foeAssignement
};

class CAgramtab;

enum FeatureFuncEnum {
    ffeAssign0, ffeAssign1, ffeAssign2, ffeAssign3, ffeCheck1, ffeCheck3
};

typedef bool AssignType0();

typedef std::string AssignType1(const CAgramtab *, const std::string &);

typedef std::string AssignType2(const CAgramtab *, const std::string &, const std::string &);

typedef std::string AssignType3(const CAgramtab *, const std::string &, const std::string &, const std::string &);

typedef bool CheckType1(const bool);

typedef bool CheckType3(const CAgramtab *, const std::string &, const size_t &, const uint64_t &);

struct CAttribAndId {
    CAncodePattern m_MorphPattern;
    size_t m_Id;
    std::string m_AttribName;

    std::string GetStr() const {
        if (!m_MorphPattern.HasNoInfo())
            return "\"" + m_MorphPattern.ToGrammarFormat() + "\"";
        else
            return Format("$%i.%s", m_Id, m_AttribName.c_str());
    };


};

struct CRuleFeature {
    CAttribAndId m_LeftItem;
    std::vector<CAttribAndId> m_RightItems;

    FeatureOperatorEnum m_Type;
    FeatureFuncEnum m_FuncType;
    std::string m_FuncNameStr;

    CheckType1 *m_pCheck1;
    CheckType3 *m_pCheck3;
    AssignType0 *m_pAssign0;
    AssignType1 *m_pAssign1;
    AssignType2 *m_pAssign2;
    AssignType3 *m_pAssign3;


    bool AddFeatureArgument(std::string s);

    bool AddFeatureValue(MorphLanguageEnum l, std::string s);

    std::string InitAssignement(std::string s, std::string func_name);

    std::string InitCheck(std::string func_name);

    bool InitFuncName(std::string s);

};

struct CWorkRule {
    size_t m_RuleId;
    size_t m_LeftPart;
    CWorkRightRulePart m_RightPart;
    size_t m_RuleFeaturesNo;

    bool operator==(const CWorkRule &X) const {
        return m_LeftPart == X.m_LeftPart
               && m_RightPart == X.m_RightPart
               && m_RuleFeaturesNo == X.m_RuleFeaturesNo;
    };

    bool operator<(const CWorkRule &X) const {
        //  First we should sort by the right part in order to make Aho-Korasick work;
        // for Aho-Korasick all patterns should be sorted lexicographically
        if (!(m_RightPart == X.m_RightPart))
            return m_RightPart < X.m_RightPart;

        if (m_RuleFeaturesNo != X.m_RuleFeaturesNo)
            return m_RuleFeaturesNo < X.m_RuleFeaturesNo;

        return m_LeftPart < X.m_LeftPart;
    };

};

struct CPrecompiledWorkRule {
    uint32_t m_LeftPart;
    uint32_t m_MetaGroupIndexNo;
    uint32_t m_SynMainItemNo;
};


struct CTokenItem {
    std::string m_TokenStr;
    bool m_bPunctuation;
    size_t m_UniqueRuleItemId;

    bool operator==(const CTokenItem &X) const {

        return m_TokenStr == X.m_TokenStr;
    };

    bool operator<(const CTokenItem &X) const {
        return m_TokenStr < X.m_TokenStr;
    };

};

typedef std::set<CWorkRule>::const_iterator CWRI;
typedef std::set<CWorkRule>::iterator WRI;

typedef std::vector<TerminalSymbolType> CPrefix;
typedef std::set<CPrefix> CPrefixSet;

struct CFoundEntity {
    std::string name;
    size_t start;
    size_t end;
    bool operator == (const CFoundEntity& _X) const
    { return name == _X.name && start == _X.start && end == _X.end; };
};


class CWorkGrammar {
public:
    MorphLanguageEnum m_Language;
    std::vector<CGrammarItem> m_UniqueGrammarItems;

    std::set<CWorkRule> m_EncodedRules;
    std::vector<std::vector<CRuleFeature> > m_RuleFeatures;
    std::vector<CPrecompiledWorkRule> m_PrecompiledEncodedRules;

    std::vector<CTokenItem> m_TokenList;

    std::vector<std::set<size_t> > m_FirstSets;
    std::vector<std::set<size_t> > m_FollowSets;

    //  we use here CWorkRule in order to be compatible with the functions
    //  which build Trie-Automat, though we use only CWorkRule::m_RightPart::m_Items
    //  all other slots are unused
    std::set<CWorkRule> m_RootPrefixes;

    //  a std::set of all second pass symbols (to detect abridged named entities)
    /*
     SecondPass is a list for finding all abridged named entities, which are 
     normally referenced to a full version of a named entity. For example:
     "Dr. Alexey Sokirko ....  Sokirko". Here "Dr. Alexey Sokirko" is a 
     full version of a named entity, which should be recognized by the main grammar,
     and  "Sokirko" is an abriged reference.
     The list below includes all grammar symbols, which can uniquely refer to 
     a some full named entity.
    */
    StringSet m_SecondPassSymbols;


    std::list<CTokenListFile> m_TokenListFiles;

    SymbolInformationType m_AutomatSymbolInformation;

    CTrieHolder m_TrieHolder;

    CGLRTable m_GLRTable;

    bool m_bEnableRootPrefix;
    std::string m_RootGrammarPath;


    CWorkGrammar();

    ~CWorkGrammar();

    void CreateTokenList();

    size_t GetItemId(const CGrammarItem &I);

    void AddToken(CTokenItem I);

    size_t GetRulesCount() const;

    bool DeleteMetaSymbols();

    bool HasOnlyTerminalOnTheRight(CWRI it) const;

    void Print() const;

    size_t GetCountOfSymbolOnTheRight(CWRI it, size_t ItemNo) const;

    std::string GetLeftPartStr(CWRI it) const;

    std::string GetRuleStr(CWRI it) const;

    std::string GetRuleStr(const CWorkRule &R) const;

    std::string GetRuleStr(const CWorkRule &R, int AsteriskNo, bool bPrintFeatures = true) const;

    bool GetPossibleTerminalStrings(size_t NonTerminalSymbolNo,
                                    std::map<size_t, std::vector<CWorkRightRulePart> > &MetaSymbolToTerminalRules) const;

    size_t GetCountOfRoots() const;

    CLemmatizedText FilterHomonymsByGrammar(const CLemmatizedText &text) const;

    std::vector<CFoundEntity> GetFoundOccurrences(const CLemmatizedText& PlmLines) const;


    void BuildAutomat(std::set<CWorkRule> &EncodedRules);

    void IsValid() const;

    void SavePrecompiled() const;

    void LoadFromPrecompiled();

    void ConvertToPrecompiled(const std::set<CWorkRule> &EncodedRules);

    void PrintPrecompiled() const;

    void DeleteUnusedSymbols();

    void Build_FIRST_Set();

    void Build_FOLLOW_Set();

    //  adding a new root is necessary  for building LR(0) item sets
    void AugmentGrammar();

    size_t GetNewRoot() const;

    size_t GetEndOfStreamSymbol() const;

    void BuildRootPrefixes(size_t PrefixLength);

    void LoadOptions();

    int GetFirstRoot() const;

    void CreateNodesForNodesWithWorkAttributes();

    void CheckCoherence() const;

    void LoadGrammarForGLR(bool bUsePrecompiledAutomat);

    void CreatePrecompiledGrammar();

    void InitalizeGrammar(MorphLanguageEnum l, std::string path);

protected:


    void CreateAutomatSymbolInformation();

    void Build_MAP_Node_To_FIRST_Set_k(size_t PrefixLength, std::map<size_t, CPrefixSet> &First_k) const;


};

extern void BuildWorkGrammar(CWorkGrammar &WorkGrammar);

