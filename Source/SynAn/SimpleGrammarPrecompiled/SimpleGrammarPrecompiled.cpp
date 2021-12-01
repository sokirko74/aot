#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/LemmatizerBaseLib/MorphanHolder.h"

#ifdef WIN32
#include <direct.h> // _getcwd
#endif

#include "morph_dict/common/argparse.h"

void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.AddOption("--print-rules");
    parser.Parse(argc, argv);
}


int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    bool bPrintRules = args.Exists("print-rules");

    CWorkGrammar WorkGrammar(args.GetOutputStream());
    WorkGrammar.m_Language = args.GetLanguage();

    CMorphanHolder Holder;
    if (!Holder.LoadLemmatizer(WorkGrammar.m_Language))
        return 1;

    std::string GrammarFileName = args.Retrieve("input");

    char currdir[256];
#ifdef WIN32
    _getcwd(currdir, 256);
    _chdir(GetPathByFile(GrammarFileName).c_str());
#else
    getcwd(currdir, 256);
    chdir(GetPathByFile(GrammarFileName).c_str());
#endif

    WorkGrammar.m_pGramTab = Holder.m_pGramTab;
    WorkGrammar.m_SourceGrammarFile = GrammarFileName.substr(GetPathByFile(GrammarFileName).length());

    bool bResult = LoadGrammarForGLR(WorkGrammar, false, bPrintRules);

    if (!bResult) {
        std::cerr << "Cannot load " << GrammarFileName << "\n";
        return 1;
    };

#ifdef WIN32
    _chdir(currdir);
#else
    chdir(currdir);
#endif

    if (!WorkGrammar.SavePrecompiled(GrammarFileName)) {
        std::cerr << "Cannot save precompiled version of " << GrammarFileName << "\n";
        return 1;
    };

    if (!WorkGrammar.m_GLRTable.ConvertAndSaveGLRTable(MakeFName(GrammarFileName, "table"))) {
        std::cerr << "Cannot save " << GrammarFileName << "\n";
        return 1;
    };

    return 0;
}



