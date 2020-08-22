//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//================================

#include "../common/utilit.h"
#include "../common/argparse.h"
#include "../SynanLib/SyntaxHolder.h"


#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>



void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--language", "language");
    parser.AddArgument("--input-file", "specify file for input text in utf-8");
    parser.AddArgument("--output-file", "specify file for tokens table");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    MorphLanguageEnum  language = args.GetLanguage();
    CSyntaxHolder H;
    if (!H.LoadSyntax(args.GetLanguage())) {
        std::cerr << "initialization error\n";
        return 1;
    };
    H.m_pPostMorph->m_bHumanFriendlyOutput = true;
    int dummy;
    if (!H.GetMorphology(args.Retrieve("input-file"), true, dummy)) {
        return 1;
    }
    CPlmLineCollection MapostPlmLines;
    if (!H.RunMapost(MapostPlmLines)) {
        return 1;
    }
    MapostPlmLines.SaveToFile(args.Retrieve("output-file"));
    return 0;
}
