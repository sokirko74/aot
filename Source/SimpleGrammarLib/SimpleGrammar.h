#pragma once

#include "../common/utilit.h"
#include "../common/PlmLine.h"
#include "../LemmatizerLib/PLMLineCollection.h"
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

typedef bool CheckType3(const CAgramtab *, const std::string &, const size_t &, const QWORD &);

struct CAttribAndId {
    CMorphPattern m_MorphPattern;
    size_t m_Id;
    std::string m_AttribName;

    std::string GetStr() const {
        if (!m_MorphPattern.m_GrmAttribute.empty())
            return "\"" + m_MorphPattern.m_GrmAttribute + "\"";
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

    bool AddFeatureValue(const CAgramtab *pGramTab, std::string s);

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


class CWorkGrammar {
public:
    MorphLanguageEnum m_Language;
    const CAgramtab *m_pGramTab;
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
    StringSet m_SecondPassSymbols;


    std::list<CTokenListFile> m_TokenListFiles;

    SymbolInformationType m_AutomatSymbolInformation;

    CTrieHolder m_TrieHolder;

    CGLRTable m_GLRTable;

    bool m_bEnableRootPrefix;
    std::string m_SourceGrammarFile;
    std::ostream &LogStream;


    CWorkGrammar(std::ostream& logStream = std::cerr);

    ~CWorkGrammar();

    bool CreateTokenList(std::string &ErrorStr);

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

    bool ParseFile(ParseMethodEnum ParseMethod, const CPlmLineCollection &PlmLines, const CAgramtab *pGramTab,
                   std::vector<std::string> &Result, bool bDumpOccurrences) const;

    void BuildAutomat(std::set<CWorkRule> &EncodedRules);

    bool IsValid() const;

    bool SavePrecompiled(std::string GrammarFileName) const;

    bool LoadFromPrecompiled();

    void ConvertToPrecompiled(const std::set<CWorkRule> &EncodedRules);

    void PrintPrecompiled() const;

    void DeleteUnusedSymbols();

    void Build_FIRST_Set();

    void Build_FOLLOW_Set();

    //  adding a new root is necessary  for building LR(0) item sets
    bool AugmentGrammar();

    size_t GetNewRoot() const;

    size_t GetEndOfStreamSymbol() const;

    void BuildRootPrefixes(size_t PrefixLength);

    bool LoadOptions();

    int GetFirstRoot() const;

    bool CreateNodesForNodesWithWorkAttributes(std::string &ErrorStr);

    bool CheckCoherence() const;

protected:


    void CreateAutomatSymbolInformation();

    void Build_MAP_Node_To_FIRST_Set_k(size_t PrefixLength, std::map<size_t, CPrefixSet> &First_k) const;

    int FindTokenListByFileName(const std::string &FileName, size_t EndItemNo);


};

extern bool ProcessFile(std::string GrammarFileName, std::string TextFileName);

extern bool BuildWorkGrammar(CWorkGrammar &WorkGrammar, bool bPrintRules);

extern bool LoadGrammarForGLR(CWorkGrammar &WorkGrammar, bool bPrecompiledAutomat, bool bPrintRules);