#include  "SimpleGrammarLib.h"
#include "SimpleGrammar.h"
#include "morph_dict/common/bserialize.h"
#include "morph_dict/common/rapidjson.h"
#include "morph_dict/agramtab/agramtab.h"

#include <time.h>
#include <sys/stat.h>


CWorkGrammar::CWorkGrammar() {
    m_GLRTable.m_pWorkGrammar = this;
    m_bEnableRootPrefix = true;
};


bool CWorkGrammar::IsLoaded() const {
    return m_UniqueGrammarItems.size() > 0;
}

size_t CWorkGrammar::GetItemId(const CGrammarItem &I) {
    auto it = find(m_UniqueGrammarItems.begin(), m_UniqueGrammarItems.end(), I);

    if (it != m_UniqueGrammarItems.end())
        return it - m_UniqueGrammarItems.begin();
    else {
        m_UniqueGrammarItems.push_back(I);
        // slot m_bSynMain is no used in  WorkGrammar it is used only in class CParseGrammar
        m_UniqueGrammarItems.back().m_bSynMain = false;
        return m_UniqueGrammarItems.size() - 1;

    };
};


std::string CWorkGrammar::GetLeftPartStr(std::set<CWorkRule>::const_iterator it) const {
    return m_UniqueGrammarItems[it->m_LeftPart].GetDumpString();
};


std::string CWorkGrammar::GetRuleStr(const CWorkRule &R, int AsteriskNo, bool bPrintFeatures) const {
    std::string Result = m_UniqueGrammarItems[R.m_LeftPart].GetDumpString() + "->";
    size_t l = 0;
    for (; l < R.m_RightPart.m_RuleItems.size(); l++) {
        if (AsteriskNo == l)
            Result += "*";

        Result += m_UniqueGrammarItems[R.m_RightPart.m_RuleItems[l]].GetDumpString();

        if (l + 1 != R.m_RightPart.m_RuleItems.size())
            Result += ", ";
    };
    if (AsteriskNo == l)
        Result += "*";

    if (bPrintFeatures) {
        const std::vector<CRuleFeature> &Features = m_RuleFeatures[R.m_RuleFeaturesNo];
        for (size_t i = 0; i < Features.size(); i++) {
            const CRuleFeature &F = Features[i];
            Result += "\n\t\t : ";
            if (F.m_Type == foeAssignement) {
                Result += F.m_LeftItem.GetStr();
                Result += " := ";
            }
            Result += F.m_FuncNameStr;
            Result += "( ";
            for (size_t j = 0; j < F.m_RightItems.size(); j++) {
                Result += F.m_RightItems[j].GetStr();

                if (j + 1 != F.m_RightItems.size())
                    Result += ", ";
            };
            Result += ") ";
        };
    };


    return Result;
};


std::string CWorkGrammar::GetRuleStr(const CWorkRule &R) const {
    return GetRuleStr(R, R.m_RightPart.m_SynMainItemNo);
};

std::string CWorkGrammar::GetRuleStr(CWRI it) const {
    return GetRuleStr(*it);

};


void CWorkGrammar::Print() const {
    LOGV << "===========";
    for (auto& r: m_EncodedRules) {
        LOGV << r.m_RuleId << ") " << GetRuleStr(r);
    };
};


void CWorkGrammar::AddToken(CTokenItem I) {
    assert (!I.m_TokenStr.empty());
    MakeUpperUtf8(I.m_TokenStr);

    I.m_bPunctuation = (std::iswpunct(I.m_TokenStr[0]) != 0);

    m_TokenList.push_back(I);
};


#ifdef WIN32

size_t GetFileModifTime (const char* FileName)
{
   struct _stat buf;
   FILE* fp = fopen (FileName, "r");
   _fstat( fileno(fp), &buf ); 
   fclose (fp);
   return buf.st_mtime;
};
#else

size_t GetFileModifTime(const char *FileName) {
    struct stat buf;
    FILE *fp = fopen(FileName, "r");
    fstat(fileno(fp), &buf);
    fclose(fp);
    return buf.st_mtime;
};

#endif


void CWorkGrammar::LoadOptions() {
    std::string path = MakeFName(m_RootGrammarPath, "opt");
    LOGV << "loading options from " << path.c_str();
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw CExpc("CWorkGrammar::LoadOptions cannot open file %s\n", path.c_str());
    }
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document d;
    d.ParseStream(isw);

    auto a = rapidjson::Pointer("/SecondPass").Get(d);
    if (a != nullptr) {
        for (auto& v : a->GetArray()) {
            m_SecondPassSymbols.insert(v.GetString());
        }
        LOGV << "Number of second pass symbols : " << m_SecondPassSymbols.size();
    }
    auto a1 = rapidjson::Pointer("/EnableRootPrefix").Get(d);
    if (a1) {
        m_bEnableRootPrefix = a1->GetBool();
        LOGV << "EnableRootPrefix = " << m_bEnableRootPrefix;
    }
    ifs.close();
};

void CWorkGrammar::CreateTokenList() {
    std::unordered_map<std::string, const CGrammarItem*> pref_paths;

    for (auto& I: m_UniqueGrammarItems) {
        auto attr_it = I.m_Attributes.find("filename");
        if (attr_it == I.m_Attributes.end()) continue;
        auto path = fs::path(m_RootGrammarPath).parent_path() / attr_it->second;
        if (!fs::exists(path)) {
            throw CExpc("cannot access %s\n", path.c_str());
        };
        auto prev = pref_paths.find(path.string());
        if (prev != pref_paths.end()) {
            // this list was already loaded, we can use one more time withoutz loading another copy
            I.m_pListFile = prev->second->m_pListFile;
            assert (I.m_pListFile != nullptr);
            continue;
        };
        pref_paths[path.string()] = &I;

        {
            size_t ModifTime = GetFileModifTime(path.string().c_str());
            if ((I.m_pListFile != NULL) && (I.m_pListFile->m_SavedModifTime == ModifTime))
                continue;
            else if (I.m_pListFile != NULL)
                I.m_pListFile->m_SavedModifTime = ModifTime;
            else {
                CTokenListFile F;
                F.m_SavedModifTime = ModifTime;
                m_TokenListFiles.push_back(F);
                I.m_pListFile = &m_TokenListFiles.back();
            };
        }
        assert (I.m_pListFile != NULL);
        CTokenListFile &F = *I.m_pListFile;
        F.m_PossibleLemmas.clear();
        std::ifstream inp(path);
        if (!inp.is_open()) {
            throw CExpc("CWorkGrammar::CreateTokenList cannot open %s", path.c_str());
        };
        std::string line;
        LOGV << "read file " << path;
        while (std::getline(inp, line)) {
            Trim(line);
            if (!line.empty()) {
                F.m_PossibleLemmas.insert(line);
            }
        };
        inp.close();
    };
};


bool CWorkGrammar::HasOnlyTerminalOnTheRight(CWRI it) const {
    for (size_t i = 0; i < it->m_RightPart.m_RuleItems.size(); i++) {
        const CGrammarItem &I = m_UniqueGrammarItems[it->m_RightPart.m_RuleItems[i]];
        if (I.m_bMeta) return false;
    };
    return true;
};


size_t CWorkGrammar::GetCountOfSymbolOnTheRight(CWRI it, size_t ItemNo) const {
    size_t Result = 0;
    for (size_t l = 0; l < it->m_RightPart.m_RuleItems.size(); l++)
        if (it->m_RightPart.m_RuleItems[l] == ItemNo)
            Result++;

    return Result;
};

size_t CWorkGrammar::GetCountOfRoots() const {
    size_t result = 0;
    for (auto& i : m_UniqueGrammarItems)
        if (i.m_bGrammarRoot) {
            result++;
            LOGV << "root = " << i.GetDumpString();
        }
    return result;
};

int CWorkGrammar::GetFirstRoot() const {
    for (size_t i = 0; i < m_UniqueGrammarItems.size(); i++)
        if (m_UniqueGrammarItems[i].m_bGrammarRoot)
            return i;
    return -1;
};


size_t CWorkGrammar::GetRulesCount() const {
    return m_EncodedRules.size();
};

void CWorkGrammar::CreateAutomatSymbolInformation() {
    m_AutomatSymbolInformation.clear();

    for (TerminalSymbolType i = 0; i < m_UniqueGrammarItems.size(); i++)
        m_AutomatSymbolInformation[i] = m_UniqueGrammarItems[i].GetDumpString();

};


void PrintGroups(const CWorkGrammar &Grammar) {
    for (CWRI it = Grammar.m_EncodedRules.begin(); it != Grammar.m_EncodedRules.end(); it++) {
        const CWorkRule &R = *it;;
        printf("%s ->", Grammar.m_UniqueGrammarItems[R.m_LeftPart].GetDumpString().c_str());

        printf("\n");
    };

};

void CWorkGrammar::ConvertToPrecompiled(const std::set<CWorkRule> &EncodedRules) {
    LOGI << "ConvertToPrecompiled";
    m_PrecompiledEncodedRules.clear();
    m_PrecompiledEncodedRules.reserve(EncodedRules.size());
    for (CWRI it = EncodedRules.begin(); it != EncodedRules.end(); it++) {
        CPrecompiledWorkRule R;
        R.m_LeftPart = it->m_LeftPart;
        R.m_SynMainItemNo = it->m_RightPart.m_SynMainItemNo;
        m_PrecompiledEncodedRules.push_back(R);
    };

};

void CWorkGrammar::BuildAutomat(std::set<CWorkRule> &EncodedRules) {
    CreateAutomatSymbolInformation();

    m_TrieHolder.Create(EncodedRules, &m_AutomatSymbolInformation);
}

void CWorkGrammar::IsValid() const {
    LOGI << "validation...\n";
    for (auto& r: m_EncodedRules) {
        const CWorkRightRulePart &R = r.m_RightPart;
        if (R.m_SynMainItemNo >= R.m_RuleItems.size()) {

            throw CExpc("%s has no syntax root", GetRuleStr(r).c_str());
        };
    };
};

void UpdateOld2NewMap(std::map<int, int> &Old2New, size_t &Index, int &MaxNewIndex) {
    std::map<int, int>::const_iterator it = Old2New.find(Index);
    if (it == Old2New.end()) {
        Old2New[Index] = MaxNewIndex;
        Index = MaxNewIndex;
        MaxNewIndex++;
    } else {
        Index = it->second;
    };
};

void CWorkGrammar::DeleteUnusedSymbols() {
    std::set<CWorkRule> NewEncodedRules;
    std::map<int, int> Old2New;
    int CountOfUsedSymbols = 0;
    for (WRI it = m_EncodedRules.begin(); it != m_EncodedRules.end(); it++) {
        CWorkRule R = (*it);
        UpdateOld2NewMap(Old2New, R.m_LeftPart, CountOfUsedSymbols);
        for (int l = 0; l < R.m_RightPart.m_RuleItems.size(); l++)
            UpdateOld2NewMap(Old2New, (size_t &) R.m_RightPart.m_RuleItems[l], CountOfUsedSymbols);

        NewEncodedRules.insert(R);
    };

    m_EncodedRules = NewEncodedRules;


    std::vector<CGrammarItem> NewUniqueRuleItems(CountOfUsedSymbols);
    std::map<int, int>::const_iterator it1 = Old2New.begin();
    for (; it1 != Old2New.end(); it1++)
        NewUniqueRuleItems[it1->second] = m_UniqueGrammarItems[it1->first];

    m_UniqueGrammarItems = NewUniqueRuleItems;
};


/*
A work attribute is an attribute which is used to compare a node with the input text.
For example, attribute "grm" (grammems) contains morphological grammems which should 
be found int the word's properties.
A work attribute is always transferred to a special CGrammarItem's property while a simple attribute
is stored in CGrammarItem::m_Attributes.
A work attribute can be assigned only to nodes which cover one word, not a sequence of words.
For example it is not possible:
[A] -> [B],[C]
D->[A grm="gen"]

Procedure CParseGrammar::CreateNodesForNodesWithWorkAttributes finds for each node [X w],
where "w" is a std::set of  work attributes, and for each rule [X]->z, 
where X is the same node as "[X w]",  but without work attributes, and adds a rule [X w]->z.
For example,
[D] -> [FILE name="test.txt"]			[D] -> [FILE name="test.txt"]
[B] -> [D]						===>	[B] -> [D]
[A] -> [B grm="gen"]					[A] -> [B grm="gen"]
										[B grm="gen"] -> [D grm="gen"]
										[D grm="gen"] -> [FILE name="test.txt" grm="gen"]


*/


void SolveNodeWithWorkAttributes(CWorkGrammar &Grammar, const CGrammarItem &Item) {
    std::vector<CWorkRule> NewRules;

    for (const auto& Rule: Grammar.m_EncodedRules) {
        if (!Grammar.m_UniqueGrammarItems[Rule.m_LeftPart].RuleItemPartialEqual(Item)) continue;

        //  "Item"  has working attributes, that means that it is used
        //  in the right-hand part, so Grammar.m_UniqueGrammarItems[Rule.m_LeftPart] is not root
        //  since the names of "Item" and  Grammar.m_UniqueGrammarItems[Rule.m_LeftPart] are equal
        Grammar.m_UniqueGrammarItems[Rule.m_LeftPart].m_bGrammarRoot = false;

        CGrammarItem LeftPart = Grammar.m_UniqueGrammarItems[Rule.m_LeftPart];
        LeftPart.CopyNonEmptyWorkAttributesFrom(Item);

        CWorkRule NewRule = Rule;
        NewRule.m_LeftPart = Grammar.GetItemId(LeftPart);
        NewRule.m_RuleId = Grammar.m_EncodedRules.size() + NewRules.size() + 1;


        { // copy working attributes from Item to the root of Rule
            assert (NewRule.m_RightPart.m_SynMainItemNo < NewRule.m_RightPart.m_RuleItems.size());
            int &MainItemId = NewRule.m_RightPart.m_RuleItems[NewRule.m_RightPart.m_SynMainItemNo];
            CGrammarItem RightPart = Grammar.m_UniqueGrammarItems[MainItemId];
            RightPart.CopyNonEmptyWorkAttributesFrom(Item);
            MainItemId = Grammar.GetItemId(RightPart);
        };

        NewRules.push_back(NewRule);

    };

    Grammar.m_EncodedRules.insert(NewRules.begin(), NewRules.end());
};


void CWorkGrammar::CreateNodesForNodesWithWorkAttributes() {

    for (auto& i :m_UniqueGrammarItems) {
        if (!i.HasAnyOfWorkingAttributes() || !i.m_bMeta)
            continue;
        SolveNodeWithWorkAttributes(*this, i);
    }
};


void CWorkGrammar::CheckCoherence() const {
    std::set<size_t> AllLeftParts;
    size_t i = 0;

    for (CWRI it = m_EncodedRules.begin(); it != m_EncodedRules.end(); it++) {
        AllLeftParts.insert(it->m_LeftPart);
    };

    for (CWRI it = m_EncodedRules.begin(); it != m_EncodedRules.end(); it++) {
        const CWorkRule &R = (*it);
        for (size_t j = 0; j < R.m_RightPart.m_RuleItems.size(); j++) {
            size_t id = R.m_RightPart.m_RuleItems[j];
            const CGrammarItem &I = m_UniqueGrammarItems[id];
            if (I.m_bMeta) {
                if (AllLeftParts.find(id) == AllLeftParts.end()) {
                    throw CExpc("\"%s\" is used , but not defined\n", I.GetDumpString().c_str());
                };
            };
        };
    };
};


void  CPrecompiledWorkRule::ToJsonObject(CJsonObject& b) const
{
    b.add_int("left", m_LeftPart);
    b.add_int("syn_main", m_SynMainItemNo);
};

void CPrecompiledWorkRule::FromJsonObject(const rapidjson::Value& inj)
{
    m_LeftPart = inj["left"].GetInt();
    m_SynMainItemNo = inj["syn_main"].GetInt();
};


void CWorkGrammar::SavePrecompiled() const {
    rapidjson::Document d;
    CJsonObject out_js(d);
    out_js.add_int("language", m_Language);

    CJsonObject items(d, rapidjson::kArrayType);
    for (auto& i : m_UniqueGrammarItems) {
        CJsonObject o(d);
        i.ToJsonObject(o);
        items.push_back(o);
    };
    out_js.move_to_member("grammar_items", items.get_value());

    CJsonObject rules(d, rapidjson::kArrayType);
    for (auto& r : m_PrecompiledEncodedRules) {
        CJsonObject o(d);
        r.ToJsonObject(o);
        rules.push_back(o);
    }
    out_js.move_to_member("rules", rules.get_value());

    std::string path = MakeFName(m_RootGrammarPath, "grammar_precompiled");
    LOGI << "save to " << path;
    std::ofstream outp(path);
    if (!outp.good()) {
        throw CExpc("Cannot write " + path);
    };
    outp << out_js.dump_rapidjson_pretty();
    outp.close();
};


void CWorkGrammar::LoadFromPrecompiled() {
    std::string path = MakeFName(m_RootGrammarPath, "grammar_precompiled");
    std::ifstream inp(path);
    LOGI << "loading from precompiled grammar " << path;
    if (!inp.good()) {
        throw CExpc("Cannot open %s", path.c_str());
    };

    rapidjson::Document doc;
    rapidjson::IStreamWrapper isw(inp);
    doc.ParseStream(isw);
    inp.close();
    m_Language = (MorphLanguageEnum)doc["language"].GetInt();
    m_UniqueGrammarItems.clear();
    for (auto& a : doc["grammar_items"].GetArray()) {
        CGrammarItem i;
        i.FromJsonObject(a);
        m_UniqueGrammarItems.push_back(i);
    }   
    m_PrecompiledEncodedRules.clear();
    for (auto& a : doc["rules"].GetArray()) {
        CPrecompiledWorkRule r;
        r.FromJsonObject(a);
        m_PrecompiledEncodedRules.push_back(r);
    }

};

// adding a new root non-terminal "NewRoot" and a special 
// symbol for end of input ("$")
void CWorkGrammar::AugmentGrammar() {
    size_t roots = GetCountOfRoots();

    if (roots != 1) {
        throw CExpc("Number of roots: %zi, a simple grammar should have only one root!", roots);
    };
    size_t i = 0;
    for (; i < m_UniqueGrammarItems.size(); i++)
        if ((m_UniqueGrammarItems[i].m_ItemStrId == "NewRoot")
            || (m_UniqueGrammarItems[i].m_ItemStrId == "$")
                ) {
            throw CExpc("exception in CWorkGrammar::AugmentGrammar!");
        };


    for (i = 0; i < m_UniqueGrammarItems.size(); i++)
        if (m_UniqueGrammarItems[i].m_bGrammarRoot) {
            break;
        };

    CGrammarItem I;


    //  adding a special symbol (end of input)
    I.m_ItemStrId = "$";
    I.m_Token = "$";
    I.m_bMeta = false;
    // w
    m_UniqueGrammarItems.push_back(I);

    //  adding a new root
    I.m_ItemStrId = "NewRoot";
    I.m_bMeta = true;
    m_UniqueGrammarItems.push_back(I);


    CWorkRule R;
    R.m_RuleId = 0;
    R.m_RuleFeaturesNo = m_RuleFeatures.size();
    m_RuleFeatures.push_back(std::vector<CRuleFeature>());
    R.m_LeftPart = m_UniqueGrammarItems.size() - 1;
    R.m_RightPart.m_SynMainItemNo = 0;
    R.m_RightPart.m_RuleItems.push_back(i);
    R.m_RightPart.m_RuleItems.push_back(m_UniqueGrammarItems.size() - 2);
    m_EncodedRules.insert(R);
};

size_t CWorkGrammar::GetNewRoot() const {
    assert (!m_UniqueGrammarItems.empty());
    assert (m_UniqueGrammarItems.back().m_ItemStrId == "NewRoot");
    return m_UniqueGrammarItems.size() - 1;
};

size_t CWorkGrammar::GetEndOfStreamSymbol() const {
    assert (m_UniqueGrammarItems.size() > 1);
    assert (m_UniqueGrammarItems.back().m_ItemStrId == "NewRoot");
    assert (m_UniqueGrammarItems[m_UniqueGrammarItems.size() - 2].m_ItemStrId == "$");
    return m_UniqueGrammarItems.size() - 2;

};


void CWorkGrammar::LoadGrammarForGLR(bool bUsePrecompiledAutomat) {
    assert(!m_RootGrammarPath.empty());
    BuildWorkGrammar(*this);
    LoadOptions();
    if (bUsePrecompiledAutomat) {

        AugmentGrammar();

        size_t SaveSize = m_UniqueGrammarItems.size();
        LoadFromPrecompiled();
        CreateAutomatSymbolInformation();
        assert(!m_AutomatSymbolInformation.empty());
        if (!m_TrieHolder.Load(&m_AutomatSymbolInformation, m_RootGrammarPath)) {
            throw CExpc("Cannot load automat");
        };

        if (SaveSize != m_UniqueGrammarItems.size()) {
            std::ostringstream ss;
            ss << "Number of Symbols in the precompiled  version = " << m_UniqueGrammarItems.size()
                      << "\n";
            ss << "Number of Symbols in the current version = " << SaveSize << "\n";
            //  should  be the same size, the difference means that the precompiled version 
            //  contain a different grammar
            throw CExpc(ss.str());
        };

        if (!m_GLRTable.LoadGLRTable(MakeFName(m_RootGrammarPath, "table"))) {
            throw CExpc("Cannot load GLR Table");
        };

    } else {
        if (m_bEnableRootPrefix) {
            size_t PrefixSize = 3;
            LOGI << "build RootPrefixes( " << PrefixSize << ")\n";
            BuildRootPrefixes(PrefixSize);
            LOGI << "build automat for prefixes \n";
            BuildAutomat(m_RootPrefixes);
            ConvertToPrecompiled(m_RootPrefixes);
        };

        AugmentGrammar();
        Build_FIRST_Set();
        Build_FOLLOW_Set();


        if (!m_GLRTable.BuildGLRTable()) {
            throw CExpc ("BuildGLRTable failed");
        };


    }
    LOGI << "create token list for each node";
    CreateTokenList();
    LOGI << "all done\n";

};

void CWorkGrammar::InitalizeGrammar(MorphLanguageEnum l, std::string path) {
    m_Language = l;
    m_RootGrammarPath = path;
}

void CWorkGrammar::CreatePrecompiledGrammar() {
    // we have to change directory to grammar root folder in order to store all paths relative to the root grammar
    // Doing this we enable to load the grammar from the any place
    auto save_curent_dir = fs::current_path();
    auto save_root_path = m_RootGrammarPath;
    if (fs::path(m_RootGrammarPath).has_parent_path()) {
        fs::current_path(fs::path(m_RootGrammarPath).parent_path());
        m_RootGrammarPath = fs::path(m_RootGrammarPath).filename().string();
    }

    LoadGrammarForGLR(false);
    SavePrecompiled();
    m_TrieHolder.Save(m_RootGrammarPath);
    m_GLRTable.ConvertAndSaveGLRTable(MakeFName(m_RootGrammarPath, "table"));

    fs::current_path(save_curent_dir);
    m_RootGrammarPath = save_root_path;
}
