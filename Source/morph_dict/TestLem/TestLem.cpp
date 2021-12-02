#include "morph_dict/common/utilit.h"
#include "morph_dict/common/argparse.h"
#include "morph_dict/AgramtabLib/RusGramTab.h"
#include "morph_dict/AgramtabLib/EngGramTab.h"
#include "morph_dict/AgramtabLib/GerGramTab.h"
#include "morph_dict/LemmatizerBaseLib/Lemmatizers.h"
#include "morph_dict/LemmatizerBaseLib/MorphanHolder.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>
#include <cstdlib>

CMorphanHolder Holder;



void CheckSpeed(std::istream& inputStream, std::ostream& output) {
    StringVector Forms;
    {
        std::string line;
        while(getline(inputStream, line)) {
            Trim(line);
            if (line.empty()) continue;
            RmlMakeUpper(line, Holder.m_pLemmatizer->GetLanguage());
            Forms.push_back(line);
        };
    };
    size_t Count = Forms.size();

    StringVector Results;
    Results.resize(Count, std::string(255, ' '));
    clock_t start = clock();
    const size_t OutBufferSize = 2000;
    char OutBuffer[OutBufferSize];
    for (size_t i = 0; i < Count; i++) {
        if (!Holder.m_pLemmatizer->GetAllAncodesAndLemmasQuick(Forms[i], true, OutBuffer, OutBufferSize, true)) {
            std::cerr << "Too many variants for " <<  Forms[i] << "\n";
        };
        Results[i] = (char *) OutBuffer;
    };
    long ticks = clock() - start;
    std::cerr << "Count of words = " <<  Count << "\n";
    double seconds = (double) ticks / (double) CLOCKS_PER_SEC;
    std::cerr << "Time = " << seconds <<  "; " << ticks << " ticks" << "\n";
    if (seconds > 0)
        std::cerr << "Speed = " << ((double) Count) / seconds << " words per seconds\n";
    else
        std::cerr << "too few words to measure the speed\n";

    for (size_t i = 0; i < Count; i++)
        output << Forms[i] << " -> " << Results[i] << "\n";
};



void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file in utf8");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.AddOption("--no-ids");
    parser.AddOption("--sort");
    parser.AddOption("--forms");
    parser.AddOption("--morphan");
    parser.AddArgument("--RML", "set env variable RML before running", true);
    parser.AddOption("--speed-test", "attention, input file must be in a single-byte encoding");
    parser.AddOption("--echo");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    MorphLanguageEnum  language = args.GetLanguage();
    auto printIds = !args.Exists("no-ids");
    auto printForms = args.Exists("forms");
    auto sortParadigms = args.Exists("sort");
    bool bEchoInput = args.Exists("echo");
    
	std::cerr << "Loading..\n";
    if (args.Exists("RML")) {
        SetEnvVariable("RML", args.Retrieve("RML"));
    }
    

    try {
        Holder.LoadLemmatizer(language);

        if (args.Exists("speed-test")) {
            CheckSpeed(args.GetInputStream(), args.GetOutputStream());
            return 0;
        };

        std::cerr << "Input a word..\n";
        std::string s;
        while (getline(args.GetInputStream(), s)) {
            Trim(s);
            if (s.empty()) break;
		    if (bEchoInput) {
			    args.GetOutputStream() << s << "\t";
		    }
		    s = convert_from_utf8(s.c_str(), language);
		    std::string result;

                if (args.Exists("morphan")) {
                    result = Holder.LemmatizeJson(s.c_str(), printForms, true, true);
                }
                else {
                    result = Holder.PrintMorphInfoUtf8(s, printIds, printForms, sortParadigms);
                
                }
                args.GetOutputStream() << result << "\n";
    	};
    }
    catch (CExpc c) {
        std::cerr << c.m_strCause << "\n";
        exit(1);
    }

    return 0;
}
