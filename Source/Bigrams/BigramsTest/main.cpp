#include "../../common/util_classes.h"
#include "../../common/argparse.h"
#include "../../common/BigramsReader.h"


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--bigrams", "bigrams file with indices");
    parser.AddArgument("--input-word", "input file in utf8");
    parser.AddArgument("--language", "language");
    parser.AddArgument("--output", "output");
    parser.AddOption("--indirect");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    std::string fileName = args.Retrieve("bigrams");
    if (!FileExists(fileName.c_str()))
        throw CExpc(Format("cannot find bigrams file: %s", fileName));
    if (!InitializeBigrams(fileName))
        throw CExpc(Format("cannot init bigrams"));

    MorphLanguageEnum  language = args.GetLanguage();
    bool direct  = ! args.Exists("indirect");
    std::string word = convert_from_utf8(args.Retrieve("input-word").c_str(), language);
    EngRusMakeUpper(word);
    std::string result = GetConnectedWords(word, 0, direct, "SortByBigramsFreq", language );
    args.GetOutputStream() << result << "\n";
    return 0;
}





