#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include "morph_dict/common/argparse.h"


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--language", "language");
    parser.AddArgument("--log-level", "log level", true);
    parser.Parse(argc, argv);
}


int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    init_plog(args.GetLogLevel(), "simple_grammar_precomp.log", false);


    try {
        CWorkGrammar WorkGrammar;
        CMorphanHolder Holder;
        Holder.LoadOnlyGramtab(args.GetLanguage());
        WorkGrammar.Initialize(Holder.m_pGramTab, args.Retrieve("input"))
        WorkGrammar.CreatePrecompiledGrammar();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}



