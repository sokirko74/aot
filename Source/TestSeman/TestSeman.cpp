#include "../SemanLib/SemStructureBuilder.h"
#include "../common/argparse.h"
#include "../SemanLib/VisualGraph.h"
#include "../common/json.h"

extern void (*GlobalErrorMessage)(const std::string &);

void MyGlobalErrorMessage(const std::string &s) {
    throw CExpc(s);

}

std::string GetGramInfo(const CRusSemStructure& SS, poses_mask_t Poses, QWORD Grammems) {
    std::string Result;
    for (size_t i = 0; i < sizeof(Poses) * 8; i++)
        if ((1 << i) & Poses) {
            Result += (const char *) SS.m_pData->GetRusGramTab()->GetPartOfSpeechStr(i);
            Result += " ";
        }
    Result += SS.m_pData->GetRusGramTab()->GrammemsToStr(Grammems);
    return Result;
}


int InitDicts(CSemStructureBuilder& SemBuilder)
{
    try {
        std::cerr << "initialize presemantic dictionaries...\n";
        if (!SemBuilder.m_RusStr.m_pData->Init()) {
            std::cerr << "cannot initialize presemantic dictionaries.\n";
            return 1;
        }
        SemBuilder.m_RusStr.m_pData->Initialize();
        SemBuilder.m_RusStr.m_pData->InitializeIndices();
        SemBuilder.m_bShouldBuildTclGraph = false;
    }
    catch (CExpc c) {
        std::cerr << c.m_strCause << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "general initialize exception";
        return 1;
    }
    return 0;
}


nlohmann::json getNodeInfo(const CRusSemStructure& SS, int nodeIndex) {
    const CRusSemNode& Node = SS.m_Nodes[nodeIndex];
    nlohmann::json words = nlohmann::json::array();

    for (auto& w : Node.m_Words) {
        words.push_back({
            {"word", w.m_Word},
            {"lemma", w.m_Lemma},
            {"inner morph", GetGramInfo(SS, w.m_Poses, w.GetAllGrammems())}
            });
    }
    nlohmann::json res = {
        {"words", words},
        {"index", nodeIndex},
        {"morph", GetGramInfo(SS, Node.GetNodePoses(), Node.GetGrammems())},
        {"interps", SS.GetNodeDictInterps(nodeIndex)}
    };
    if (Node.m_PrepWordNo >= 0) {
        res["preposition"] = SS.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord;
    }
    if (!Node.m_AntecedentStr.empty()) {
        res["anteceden"] = Node.m_AntecedentStr;
    }
    if (Node.m_Words.empty() || Node.m_Words[0].m_Word != SS.GetNodeStr1(nodeIndex)) {
        res["node_str"] = SS.GetNodeStr1(nodeIndex);
    }
    return res;
};


nlohmann::json getRelations(const CRusSemStructure& SS, const std::vector<CRusSemRelation>& rels) {
    nlohmann::json result = nlohmann::json::array();
    for (auto& r : rels) {
        long targetNodeNo = r.m_Valency.m_Direction == C_A && !r.m_bReverseRel ? r.m_SourceNodeNo : r.m_TargetNodeNo;
        long sourceNodeNo = !(r.m_Valency.m_Direction == C_A && !r.m_bReverseRel) ? r.m_SourceNodeNo
            : r.m_TargetNodeNo;
        nlohmann::json rel = {
            {   "name",  r.m_Valency.m_RelationStr },
            {   "target", {
                    {"words", SS.GetNodeStr1(targetNodeNo)},
                    {"node_id", targetNodeNo}
                }
            },
            {   "source", {
                    {"words", SS.GetNodeStr1(sourceNodeNo)},
                    {"node_id", sourceNodeNo}
                }
            },
            {    "syntax_rel", r.m_SyntacticRelation }
        };
        result.push_back(rel);
    }
    return result;
}

nlohmann::json getNodes(const CRusSemStructure& SS) {
    auto r = nlohmann::json::array();
    for (int i = 0; i < SS.m_Nodes.size(); i++) {
        r.push_back(getNodeInfo(SS, i));
    }
    return r;
}

nlohmann::json PrintRelationsToText(const CRusSemStructure& SS) {
    nlohmann::json result = {
        {"nodes", getNodes(SS)},
        {"relations", getRelations(SS, SS.m_Relations)},
        {"aux relations", getRelations(SS, SS.m_DopRelations)}
    };
    //std::string debug = result.dump();
    ConvertToUtfRecursive(result, morphRussian);
    return result;
}


nlohmann::json PrintRelationsAsToJavascript(const CSemStructureBuilder& SemBuilder) {
    std::string r;
    CVisualSemGraph graph;
    graph.InitFromSemantics(SemBuilder);
    graph.SetGraphLayout();
    nlohmann::json result = {
        {"java_script_data", graph.GetResultStr()}
    };
    ConvertToUtfRecursive(result, morphRussian);
    return result;
}


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input-file", "input file", true);
    parser.AddArgument("--output-file", "output file", true);
    parser.AddArgument("--input-file-mask", "c:/*.txt", true);
    parser.AddOption("--visual", "print visual graph (for javascript)");
    parser.AddOption("--translate", "print English translation");
    parser.Parse(argc, argv);
}


nlohmann::json processText(CSemStructureBuilder& SemBuilder, bool printVisual, bool printTranslation, std::string inputText) {
    try {
        if (printTranslation) {
            auto s = SemBuilder.TranslateRussianText(inputText, _R("общ"), nullptr);
            return convert_to_utf8(s, morphRussian); //остаются недопереведенные слова
        }
        else {
            std::string dummy;
            SemBuilder.FindSituations(inputText, 0, _R("общ"), 20000, -1, "", dummy);
            if (printVisual) {
                return PrintRelationsAsToJavascript(SemBuilder);
            }
            else {
                return PrintRelationsToText(SemBuilder.m_RusStr);
            }
        }

    }
    catch (CExpc e) {
        std::cerr << "an exception occurred: " << e.m_strCause << "\n";
        throw;
    }
    catch (...) {
        std::cerr << "an exception occurred, sentence " << inputText << "\n";
        throw;
    };

}

void processOneFile(CSemStructureBuilder& SemBuilder, bool printVisual, bool printTranslation, string inputFile, string outputFile) {
    std::cerr << inputFile << "\n";
    CTestCaseBase base;
    std::ifstream inp(inputFile);
    base.read_test_cases(inp);
    for (auto& t : base.TestCases) {
        if (t.Text.length() > 250) {
            std::cerr << "skip the sentence of " << t.Text.length() << " chars (too long)\n";
            continue;
        }
        if (t.Text.empty()) {
            continue;
        }
        t.Result = processText(SemBuilder, printVisual, printTranslation, t.Text);
    }
    std::ofstream outp(outputFile, std::ios::binary);
    base.write_test_cases(outp);

}


int main(int argc, const char *argv[]) {
    static CSemStructureBuilder SemBuilder;
    ArgumentParser args;
    initArgParser(argc, argv, args);

    GlobalErrorMessage = MyGlobalErrorMessage;
    std::cerr << "init dicts ... (wait one minute)\n";
    if (InitDicts(SemBuilder) != 0) {
        return 1;
    }
    SemBuilder.m_RusStr.m_pData->GetSynan()->SetKillHomonymsMode(CoverageKillHomonyms);

    std::vector <std::pair<std::string, std::string> > file_pairs;
    if (args.Exists("input-file-mask")) {
        auto file_names = list_path_by_file_mask(args.Retrieve("input-file-mask"));
        for (auto filename : file_names) {
            file_pairs.push_back({ filename, filename + ".seman" });
        }
    }
    else {
        file_pairs.push_back({ args.Retrieve("input-file"), args.Retrieve("output-file") });
    }

    for (auto& p : file_pairs) {
        processOneFile(SemBuilder, args.Exists("visual"), args.Exists("translate"), p.first, p.second);
    }
    std::cerr << "normal exit\n";
    return 0;
}

