#include "morph_dict/common/util_classes.h"
#include "morph_dict/common/argparse.h"
#include "common/BigramsReader.h"


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--data-folder", "directory with  indexed files");
    parser.AddArgument("--input-word", "input file in utf8");
    parser.AddArgument("--language", "language");
    parser.AddArgument("--output", "output");
    parser.AddOption("--indirect");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    std::string path = args.Retrieve("data-folder");
    try {
        if (!fs::exists(path))
            throw CExpc(Format("cannot find directory: %s", path.c_str()));
        InitializeBigrams(path);

        MorphLanguageEnum  language = args.GetLanguage();
        bool direct  = ! args.Exists("indirect");
        std::string word = convert_from_utf8(args.Retrieve("input-word").c_str(), language);
        MakeUpperUtf8(word);
        std::string result = GetConnectedWords(word, 0, direct, "SortByBigramsFreq", language );
        args.GetOutputStream() << result << "\n";
    }
    catch (std::exception& a) {
        std::cerr << a.what();
        return 1;
    }
    return 0;

}





