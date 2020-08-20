/*
Пример вызова интерфейса Seman. В stdin подаются предложения в  кодировке Win-1251(по одному на каждой строке). 
Определите дерективу DOS для кодировки DOS (для работы в консоли Windows)
Программа печатает семантические узлы и отношения для каждого входного предложения.
Для каждого слова  семантического узла печатается лемма, часть речи и граммемы.

Для фразы "Мама мыла раму" программа должна напечатать:
Nodes:
	Node 0 МАМА: МАМА С од,жр,им,ед,
	Node 1 МЫЛА: МЫТЬ Г дст,пе,нс,прш,жр,ед,
	Node 2 РАМУ: РАМА С но,жр,вн,ед,
Relations:
	SUB (0, 1)
	OBJ (2, 1)
*/
#include "../SemanLib/SemStructureBuilder.h"
#include "../common/argparse.h"
#include "../SemanLib/VisualGraph.h"

extern void (*GlobalErrorMessage)(const std::string &);

void MyGlobalErrorMessage(const std::string &s) {
    throw CExpc(s);

}

const char *byte_to_binary(int x) {
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
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

void show_register(std::string &s, RegisterEnum Register) {

    EngRusMakeLower(s);
    if (!s.empty())
        if (Register == UpLow)
            s[0] = rtoupper(s[0]);
        else if (Register == UpUp)
            EngRusMakeUpper(s);
}

vector<std::string> words;

std::string GetWordStrOfNode(const CRusSemStructure& SS, int ssi, bool bOnlyWords = false) {
    const CRusSemNode &Node = SS.m_Nodes[ssi];
    std::string NodeStr;
    if (bOnlyWords) {
        if (Node.m_Words.size() > 1)
            NodeStr += "(";
        for (int i = 0; i < Node.m_Words.size(); i++) {
            std::string w = Node.m_Words[i].m_Word;
            show_register(w, Node.m_Words[i].m_CharCase);
            NodeStr += w;
            if (i + 1 < Node.m_Words.size())
                NodeStr += " ";
        }
        if (Node.m_Words.size() > 1)
            NodeStr += ")";

    } else {
        if (Node.m_Words.size() == 0 || Node.m_Words[0].m_Word != SS.GetNodeStr1(ssi))
            NodeStr += SS.GetNodeStr1(ssi) + ": ";
        for (int i = 0; i < Node.m_Words.size(); i++) {
            const CRusSemWord &Word = Node.m_Words[i];
            if (!(i == 0 && Word.m_Word != SS.GetNodeStr1(ssi)))
                NodeStr += Word.m_Word + ": ";

            NodeStr += Word.m_Lemma;
            NodeStr += " ";
            NodeStr += GetGramInfo(SS, Word.m_Poses, Word.GetAllGrammems());
        }
        if (Node.m_PrepWordNo >= 0) {
            NodeStr = SS.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord + " " + NodeStr;
            //words[Node.m_PrepWordNo] = SS.m_piSent->m_Words[Node.m_PrepWordNo].m_strWord;
        }
        if (!NodeStr.empty())
            NodeStr += " -> "; // Отредакт. морф. информация:
        NodeStr += GetGramInfo(SS, Node.GetNodePoses(), Node.GetGrammems());
        if (!Node.m_AntecedentStr.empty()) {
            NodeStr += " anteced=" + Node.m_AntecedentStr;
        }
    }

    return NodeStr;
};


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

void outputRelation(const CRusSemStructure& SS, const CRusSemRelation& R, std::ostream& result) {
    long targetNodeNo = R.m_Valency.m_Direction == C_A && !R.m_bReverseRel ? R.m_SourceNodeNo : R.m_TargetNodeNo;
    long sourceNodeNo = !(R.m_Valency.m_Direction == C_A && !R.m_bReverseRel) ? R.m_SourceNodeNo
                                                                                : R.m_TargetNodeNo;
    //"  %s (%s, %s) = %s (%i, %i)\n",
    result << "  " << R.m_Valency.m_RelationStr << " (";
    result << convert_to_utf8(SS.GetNodeStr1(targetNodeNo), morphRussian) << ", ";
    result << convert_to_utf8(SS.GetNodeStr1(sourceNodeNo), morphRussian) << ") = ";
    result << convert_to_utf8(R.m_SyntacticRelation, morphRussian) << " (";
    result << targetNodeNo << ", ";
    result << sourceNodeNo << ")\n";
}

void PrintRelationsToText(const CRusSemStructure& SS, ostream& result) {
    std::string r;

    words.resize(SS.m_piSent->m_Words.size());
    for (int i = 0; i < SS.m_piSent->m_Words.size(); i++)
        words[i] = SS.m_piSent->m_Words[i].m_strWord;
    result << "Nodes:\n";
    for (int i = 0; i < SS.m_Nodes.size(); i++) {
        std::string nstr = GetWordStrOfNode(SS, i);
        if (nstr.empty()) {
            nstr = SS.GetNodeStr1(i);
        }
        result << "  Node " << i << " ";
        result << convert_to_utf8(nstr, morphRussian);
        result << "\n";
    }
    result << "Relations:\n";

    for (auto r : SS.m_Relations) {
        outputRelation (SS, r, result);

    };
    if (!SS.m_DopRelations.empty()) {
        result << "Aux Relations:\n";
        for (auto r : SS.m_DopRelations) {
            outputRelation (SS, r, result);
        }
    }
}

void PrintRelationsAsToJavascript(const CSemStructureBuilder& SemBuilder, ostream& result) {
    std::string r;
    CVisualSemGraph graph;
    graph.InitFromSemantics(SemBuilder);
    graph.SetGraphLayout();
    result << convert_to_utf8(graph.GetResultStr(), morphRussian) << "\n";
}

void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file in windows-1251");
    parser.AddOption("--visual", "print visual graph");
    parser.AddArgument("--output", "output file");
    parser.Parse(argc, argv);
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

    std::cerr << "parsing sentences:\n";
    std::string s;
    size_t lineCount = 0;
    while (getline(args.GetInputStream(), s)) {
        lineCount ++;
        std::cerr << lineCount << ") " << s << "\n";
        int comment = 0;
        if ((comment = s.find("//")) != std::string::npos) {
            s = s.substr(0, comment);
        }
        Trim(s);
        if (s.length() > 250) {
            std::cerr << "skip the sentence of " << s.length() << " chars (too long)\n";
            continue;
        }
        if (s.empty()) {
            std::cerr << "skip empty sentence on line " << lineCount << "\n";
            continue;
        }
        try {
            args.GetOutputStream() << s << "\n";
            SemBuilder.m_RusStr.m_pData->GetSynan()->SetKillHomonymsMode(CoverageKillHomonyms);
            std::string dummy;
            SemBuilder.FindSituations(s, 0, _R("общ"), 20000, -1, "", dummy);
            if (args.Exists("visual")) {
                PrintRelationsAsToJavascript(SemBuilder, args.GetOutputStream());
            } else {
                PrintRelationsToText(SemBuilder.m_RusStr, args.GetOutputStream());
            }
        }
        catch (CExpc e) {
            std::cerr << "an exception occurred: " << e.m_strCause << "\n";
        }
        catch (...) {
            std::cerr << "an exception occurred: " << lineCount << "\n";
        }

    }
    std::cerr << "exit\n";
    return 0;
}
