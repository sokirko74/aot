#include "../common/util_classes.h"
#include "../common/MorphXmlToken.h"
#include "RusCorpXml.h"
#include "../AgramtabLib/agramtab_.h"
#include "../TrigramLib/TrigramModel.h"
#include "../common/argparse.h"


void PrintCookedSentence(const CAgramtab* gramTab, const CTagSet& tagSet, const std::vector<CXmlToken> &Tokens, std::ostream& outputStream) {
    for (size_t i = 0; i < Tokens.size(); i++) {
        const CXmlToken &W = Tokens[i];
        if (!W.m_Annots.empty() && !W.m_Annots[0].m_Lemma.empty()) {
            CTag T;
            std::string gr = W.m_Annots[0].m_GrammemsStr;
            for (size_t i = 0; i < gr.size(); i++)
                if (gr[i] == '|')
                    gr[i] = ' ';

            gramTab->ProcessPOSAndGrammems(gr.c_str(), T.m_Pos, T.m_Grammems);
            T.m_Lemmas.push_back(W.m_Annots[0].m_Lemma);
            T = tagSet.GetBestSuitedTag(T);
            std::string TagStr;
            if (T.IsEmptyTag()) {

                if (T.m_Pos == gramTab->GetPartOfSpeechesCount())
                    TagStr = "UNK";
                else
                    TagStr = gramTab->GetPartOfSpeechStr(T.m_Pos);

            } else {
                TagStr = T.m_Name;
                if (TagStr.empty())
                    throw CExpc("Error! Cannot find a suited tag  for %s (grams = %s)\n",
                                W.m_WordStr.c_str(),
                                W.m_Annots[0].m_GrammemsStr.c_str());
            };

            outputStream << W.m_WordStr << " " << TagStr << " ";
        } else if (W.m_Annots.empty() || W.m_Annots[0].m_Lemma.empty()) {
            std::string pos = "UNK";

            if (isdigit((BYTE) pos[0]))
                pos = _R("пїЅпїЅ");
            else if (W.IsImportantPunct())
                pos = W.m_WordStr.c_str();

            outputStream << W.m_WordStr << " " << pos << " ";
        } else
            throw CExpc("Number of annotations is more than 1 for %s\n", W.m_WordStr.c_str());

        if (W.m_bLastInSentence)
            outputStream <<  "\n";
    };
}


bool CookSentence(const CTrigramModel& M, std::istream& inputStream, std::ostream& outputStream) {
    size_t TokensCount = 0;

    CRusCorpXmlFile XmlFile;
    try {
        while (XmlFile.ReadNextSentence(inputStream)) {
            XmlFile.CreateGraTable();
            PrintCookedSentence(M.m_pAgramtab, M.m_TagSet, XmlFile.m_CurrSentTokens, outputStream);
            TokensCount += XmlFile.m_CurrSentTokens.size();
        }
    }
    catch (CExpc c) {
        fprintf(stderr, "Error! %s, processed tokens count = %zi\n", c.m_strCause.c_str(), TokensCount);
        return false;
    }
    return true;
}

bool Disambiguate(const CTrigramModel &M, std::istream& inputStream, std::ostream& outputStream) {
    size_t TokensCount = 0;
    std::string Sent;
    try {
        CRusCorpXmlFile XmlFile;
        size_t SentNo = 0;
        while (XmlFile.ReadNextSentence(inputStream)) {
            XmlFile.CreateGraTable();
            M.DisambiguateRusCorpXml(XmlFile.m_CurrSentTokens);
            XmlFile.PrintDisambiguatedXmlNodes(outputStream);
            TokensCount += XmlFile.m_CurrSentTokens.size();
            SentNo++;
            if (SentNo % 100)
                fprintf(stderr, ".");
        }
        fprintf(stderr, "\n");
    }
    catch (CExpc c) {
        fprintf(stderr, "Error! %s, processed tokens count = %zi\n", c.m_strCause.c_str(), TokensCount);
        return false;
    }

    return true;
}


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "is a ruscorpus .xhtml3  file");
    parser.AddArgument("--output", "output file");
    parser.AddOption("--action");
    parser.AddOption("--input-is-list-file");
    parser.AddArgument("--trigram-config", "<trigram.cfg>  is a trigram model config", false);
    parser.Parse(argc, argv);
}

int main(int argc, const char *argv[]) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    CTrigramModel M;
    std::string Action = args.Retrieve("action");
    if (Action != "cook" && Action != "disamb") {
        std::cerr << "action  can be cook disamb\n";
        return 1;
    }
    if (Action == "disamb") {
        if (args.Retrieve("trigram-config").empty()) {
            std::cerr << "no trigram config specified\n";
            return 1;
        }
        try {
            M.InitModelFromConfigAndBuildTagset(args.Retrieve("trigram-config"), 0, 0, true);
            M.ReadBinary();
            M.m_bQuiet = true;
        }
        catch (CExpc c) {
            std::cerr << c.m_strCause << "\n";
            return 1;
        }
    }
    bool bRes = true;
    std::vector<std::string> inputFiles = args.GetInputFiles();

    for (size_t i = 0; bRes && i < inputFiles.size(); i++) {
        if (inputFiles.size() > 1)
            std::cerr << i << "/" <<  inputFiles.size() << ": " << inputFiles[i] << "\n";
        std::istream* inputStream = &args.GetInputStream();
        std::ostream* outputStream = &args.GetOutputStream();
        std::ofstream outputFile;
        std::ifstream inputFile;
        if (inputFiles.size() > 1) {
            outputFile.open(MakeFName(inputFiles[i], "trigram"));
            outputStream = &outputFile;
            inputFile.open(inputFiles[i]);
            inputStream = &inputFile;
        }

        if (Action == "cook")
            bRes = CookSentence(M, *inputStream, *outputStream);
        else
            bRes = Disambiguate(M, *inputStream, *outputStream);
    }

    return bRes ? 0 : 1;
}


