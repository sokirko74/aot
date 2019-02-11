//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>

#include "../common/utilit.h"
#include "../common/argparse.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/Paradigm.h"
#include "../LemmatizerLib/Morphan.h"
#include "../LemmatizerLib/MorphologyHolder.h"
#include <iostream>
#include <fstream>

bool bPrintIds = true;
bool bPrintForms = false;
bool bSortParadigms = false;
CMorphologyHolder Holder;


string GetGrammems(const char *tab_str) {
    QWORD G;
    Holder.m_pGramTab->GetGrammems(tab_str, G);
    string s = Holder.m_pGramTab->GrammemsToStr(G);
    if (!s.empty() && (s[s.length() - 1] == ','))
        s.erase(s.length() - 1);
    return s;
}

std::string GetMorphInfo(std::string Form) {


    bool bCapital = is_upper_alpha((BYTE) Form[0], Holder.m_CurrentLanguage);

    std::vector<CFormInfo> Paradigms;
    Holder.m_pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);

    std::vector<std::string> Results;
    for (int i = 0; i < Paradigms.size(); i++) {
        std::string Result;
        const CFormInfo &F = Paradigms[i];
        Result += F.m_bFound ? "+ " : "- ";

        Result += F.GetWordForm(0) + " ";

        {
            string GramCodes = F.GetSrcAncode();
            BYTE PartOfSpeech = Holder.m_pGramTab->GetPartOfSpeech(GramCodes.c_str());
            Result += Holder.m_pGramTab->GetPartOfSpeechStr(PartOfSpeech) + std::string(" ");

            string CommonAncode = F.GetCommonAncode();
            Result += Format("%s ", (CommonAncode.empty()) ? "" : GetGrammems(CommonAncode.c_str()).c_str());

            for (long i = 0; i < GramCodes.length(); i += 2) {
                if (i > 0)
                    Result += ";";
                Result += Format("%s", GetGrammems(GramCodes.c_str() + i).c_str());
            }

        }

        if (bPrintIds)
            Result += Format(" %i", F.GetParadigmId());

        BYTE Accent = F.GetSrcAccentedVowel();
        if (Accent != 0xff)
            Result += Format(" %s'%s", Form.substr(0, Accent + 1).c_str(), Form.substr(Accent + 1).c_str());

        if (bPrintForms) {
            Result += " ";
            for (int k = 0; k < F.GetCount(); k++) {
                if (k > 0)
                    Result += ",";
                Result += Paradigms[i].GetWordForm(k);
            };
        };
        Results.push_back(Result);
    };

    if (bSortParadigms) {
        std::sort(Results.begin(), Results.end());
    };
    std::string Result;
    for (int i = 0; i < Results.size(); i++) {
        if (i > 0)
            Result += "\t";
        Result += Results[i] + "\n";
    }
    return Result;
};


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
    Results.resize(Count, string(255, ' '));
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
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.AddOption("--no-ids");
    parser.AddOption("--sort");
    parser.AddOption("--forms");
    parser.AddOption("--morphan");
    parser.AddOption("--speed-test");
    parser.AddOption("--echo");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    MorphLanguageEnum  Language = args.GetLanguage();

    bPrintIds = !args.Exists("no-ids");
    bPrintForms = args.Exists("forms");
    bSortParadigms = args.Exists("sort");
    bool bEchoInput = args.Exists("echo");

    std::cerr << "Loading..\n";
    if (!Holder.LoadLemmatizer(Language)) {
        std::cerr << "Cannot load %s morphology\n";
        return 1;
    }

    if (args.Exists("speed-test")) {
        CheckSpeed(args.GetInputStream(), args.GetOutputStream());
        return 0;
    };

    std::cerr << "Input a word..\n";
    std::string s;
    while (getline(args.GetInputStream(), s)) {
        Trim(s);
        if (s.empty()) break;
        string Result;

        if (args.Exists("morphan")) 
            Result = LemmatizeJson(s.c_str(), &Holder, bPrintForms);
        else
            Result = GetMorphInfo(s);

        if (bEchoInput)  {
            args.GetOutputStream() << s << "\t";
        }

        args.GetOutputStream() << Result << "\n";

    };
    return 0;
}
