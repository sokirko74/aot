#include "common/test_corpus.h"

#include "seman/SemanLib/SemStructureBuilder.h"
#include "morph_dict/common/argparse.h"
#include "seman/SemanLib/VisualGraph.h"



extern void (*GlobalErrorMessage)(const std::string &);

void MyGlobalErrorMessage(const std::string &s) {
    throw CExpc(s);

}


std::string GetGramInfo(const CRusSemStructure& semStr, part_of_speech_mask_t Poses, uint64_t Grammems) {
    std::string Result;
    for (size_t i = 0; i < sizeof(Poses) * 8; i++)
        if ((1 << i) & Poses) {
            Result += (const char *) semStr.m_pData->GetRusGramTab()->GetPartOfSpeechStr(i);
            Result += " ";
        }
    Result += semStr.m_pData->GetRusGramTab()->GrammemsToStr(Grammems);
    return Result;
}


void getNodeInfo(const CRusSemStructure& semStr, int nodeIndex, CJsonObject& o) {
    const CRusSemNode& Node = semStr.m_Nodes[nodeIndex];
    CJsonObject words(o.get_doc(), rapidjson::kArrayType);
    for (auto& w : Node.m_Words) {
        CJsonObject j(o.get_doc());
        j.add_string_copy("word", w.GetWord());
        j.add_string_copy("lemma", w.m_Lemma);
        j.add_string_copy("inner_morph", GetGramInfo(semStr, w.m_Poses, w.GetAllGrammems()));
        words.push_back(j);
    }
    o.move_to_member("words", words.get_value());
    o.add_int("index", nodeIndex);
    o.add_string_copy("morph", GetGramInfo(semStr, Node.GetNodePoses(), Node.GetGrammems()));
    CJsonObject interps(o.get_doc(), rapidjson::kArrayType);
    for (auto& w : semStr.GetNodeDictInterps(nodeIndex)) {
        rapidjson::Value j;
        j.SetString(w.c_str(), w.length(), o.get_allocator());
        interps.push_back(j);
    }
    o.move_to_member("interps", interps.get_value());
    if (Node.m_PrepWordNo >= 0) {
        o.add_string_copy("preposition", semStr.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord);
    }
    if (!Node.m_AntecedentStr.empty()) {
        o.add_string_copy("antecedent", Node.m_AntecedentStr);
    }
    if (Node.m_Words.empty() || Node.m_Words[0].GetWord() != semStr.GetNodeStr1(nodeIndex)) {
        o.add_string_copy("node_str", semStr.GetNodeStr1(nodeIndex));
    }
};

struct LessRelation {
    const CRusSemStructure& SemStr;
    LessRelation(const CRusSemStructure& semStr) : SemStr(semStr) {
    }
    bool operator () (const CRusSemRelation& x1, const CRusSemRelation& x2) const {
        auto a1 = x1.m_Valency.m_RelationStr + " " + SemStr.GetNodeStr1(x1.m_SourceNodeNo) + " " + SemStr.GetNodeStr1(x1.m_SourceNodeNo);
        auto a2 = x1.m_Valency.m_RelationStr + " " + SemStr.GetNodeStr1(x1.m_SourceNodeNo) + " " + SemStr.GetNodeStr1(x1.m_SourceNodeNo);
        return a1 < a2;
    }
};

void getRelations(const CRusSemStructure& semStr, const std::vector<CRusSemRelation>& rels, CJsonObject& o) {
    auto sortedRels = rels;
    sort(sortedRels.begin(), sortedRels.end(), LessRelation(semStr));
    for (auto& i : sortedRels) {
        auto r = semStr.GetOutputRelation(i);
        CJsonObject j(o.get_doc());
        j.add_string_copy("name", r.sem_rel);
        j.add_string_copy("source", r.src);
        j.add_string_copy("target", r.trg);
        j.add_string_copy("syntax_rel", r.syn_rel);
        o.push_back(j.get_value());
    }
}

void getNodes(const CRusSemStructure& semStr, CJsonObject& o) {
    for (int i = 0; i < semStr.m_Nodes.size(); i++) {
        CJsonObject j(o.get_doc());
        getNodeInfo(semStr, i, j);
        o.push_back(j);
    }
}

void SemStructureForText(const CRusSemStructure& semStr, CJsonObject& o) {
    CJsonObject nodes(o.get_doc(), rapidjson::kArrayType);
    getNodes(semStr, nodes);
    o.move_to_member("nodes", nodes.get_value());

    CJsonObject rels(o.get_doc(), rapidjson::kArrayType);
    getRelations(semStr, semStr.m_Relations, rels);
    o.move_to_member("relations", rels.get_value());


    CJsonObject aux_rels(o.get_doc(), rapidjson::kArrayType);
    getRelations(semStr, semStr.m_DopRelations, aux_rels);
    o.move_to_member("aux_relations", aux_rels.get_value());
}


std::string PrintRelationsAsToJavascript(const CSemStructureBuilder& SemBuilder) {
    std::string r;
    CVisualSemGraph graph;
    graph.InitFromSemantics(SemBuilder);
    graph.SetGraphLayout();
    return graph.GetResultStr();
}


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input-file", "input file", true);
    parser.AddArgument("--log-level", "log level", true);
    parser.AddArgument("--output-file", "output file", true);
    parser.AddArgument("--input-file-mask", "c:/*.txt", true);
    parser.AddOption("--visual", "print visual graph (for javascript)");
    parser.AddOption("--translate", "print English translation");
    parser.Parse(argc, argv);
}


void processText(CSemStructureBuilder& SemBuilder, bool printVisual, bool printTranslation, std::string inputText, CJsonObject& o) {
    try {
        if (printTranslation) {
            o.add_string_copy("result", SemBuilder.TranslateRussianText(inputText, CommonDomain));
        }
        else {
            SemBuilder.FindSituations(inputText);
            if (printVisual) {
                auto res = PrintRelationsAsToJavascript(SemBuilder);
                o.add_string_copy("result", res);
            }
            else {
                CJsonObject res(o.get_doc());
                SemStructureForText(SemBuilder.m_RusStr, res);
                o.move_to_member("result", res.get_value());
            }
        }

    }
    catch (std::exception& e) {
        PLOGE << "an exception occurred: " << e.what();
        throw;
    }
    catch (...) {
        PLOGE << "an exception occurred, sentence " << inputText;
        throw;
    };

}

void processOneFile(CSemStructureBuilder& SemBuilder, bool printVisual, bool printTranslation, string inputFile, string outputFile) {
    PLOGI << "process " << inputFile;
    std::ifstream inp(inputFile);
    if (!inp.good()) {
        throw CExpc("cannot read %s", inputFile.c_str());
    }
    rapidjson::Document d;
    CTestCaseBase base(d);
    base.read_test_cases(inp);
    size_t cnt = 0;
    CJsonObject new_array(d, rapidjson::kArrayType);
    for (auto& t : base.TestCases.get_value().GetArray()) {
        std::string text = t["input"].GetString();
        CJsonObject j (t, d);
        if (text.length() > 1050) {
            PLOGD << "skip the sentence of " << text.length() << " chars (too long)\n";
            continue;
        }
        if (text.empty()) {
            continue;
        }
        cnt += 1;
        processText(SemBuilder, printVisual, printTranslation, text, j);
        new_array.push_back(j);
    }
    PLOGI << "processed " << cnt << " sentences";
    new_array.dump_rapidjson_pretty(outputFile, 4);

}


int main(int argc, const char* argv[]) {
    CSemStructureBuilder SemBuilder;

    ArgumentParser args;
    initArgParser(argc, argv, args);
    init_plog_seman(args.GetLogLevel(), "test_seman.log");

    try {
        GlobalErrorMessage = MyGlobalErrorMessage;
        PLOGI << "init dicts ... (wait one minute)";
        SemBuilder.InitDicts();

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
        PLOGD << "normal exit\n";
    }
    catch (std::exception& e) {
        PLOGE << e.what() << "\n";
        return 1;
    }
    catch (...) {
        PLOGE << "general exception\n";
        return 1;
    };

    return 0;
}

