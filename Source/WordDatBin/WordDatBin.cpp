// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ <DataFile> (пїЅпїЅ. StatTxtDat)
// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ <pid,weight> пїЅ пїЅпїЅпїЅпїЅ "r*wordweight.bin"


#include "../common/util_classes.h"
#include "../common/bserialize.h"
#include "../common/argparse.h"


#include <algorithm>
#include <iostream>
#include <fstream>

#include <string>
#include <map>
#include <vector>
#include "../LemmatizerLib/MorphologyHolder.h"


CMorphologyHolder MorphHolderRus;

static std::vector<std::pair<DWORD, DWORD> > ww;

struct CStatInfo {
    string m_Lemma;
    BYTE m_Pos;
    QWORD m_Grammems;
    DWORD m_Freq;

    bool operator<(const CStatInfo &s) const {
        if (m_Lemma != s.m_Lemma)
            return m_Lemma < s.m_Lemma;
        else if (m_Pos != s.m_Pos)
            return m_Pos < s.m_Pos;
        else
            return m_Grammems < s.m_Grammems;
    }

    string DumpToString() const {
        string Result = m_Lemma + ";";
        Result += MorphHolderRus.m_pGramTab->GetPartOfSpeechStr(m_Pos);
        Result += " ";
        if (m_Grammems != 0)
            Result += MorphHolderRus.m_pGramTab->GrammemsToStr(m_Grammems);
        Result += Format(";%i", m_Freq);
        return Result;
    }
};

bool readDat(istream &ifs, set<CStatInfo> &Infos) {
    Infos.clear();

    char buf[256];
    int lin = 0;

    while (ifs.getline(buf, 2048)) {
        lin++;
        StringTokenizer tok(buf, ";");
        CStatInfo I;
        I.m_Lemma = tok();
        std::string sgrm = tok();
        I.m_Freq = atoi(tok());

        if (I.m_Lemma.empty() || sgrm.empty()) {
            std::cout << "Error in line: " << buf << " skipped" << std::endl;
            continue;
        };

        I.m_Pos = 1;
        I.m_Grammems = 1;
        if (!MorphHolderRus.m_pGramTab->ProcessPOSAndGrammemsIfCan(sgrm.c_str(), &I.m_Pos, &I.m_Grammems)) {
            std::cout << "Error in morph. pattern line: " << buf << " skipped" << std::endl;
            continue;
        };
        pair<set<CStatInfo>::iterator, bool> r = Infos.insert(I);
        if (!r.second) {
            CStatInfo A = *r.first;;
            A.m_Freq += I.m_Freq;
            Infos.erase(r.first);
            Infos.insert(A);
        }


    }
    return true;
}

void DealWithUniqueLemmas(set<CStatInfo> &Infos) {
    set<CStatInfo> NewInfos;
    for (set<CStatInfo>::iterator it = Infos.begin(); it != Infos.end();) {
        CStatInfo I = *it;
        vector<CFormInfo> ParadigmCollection;

        string s = I.m_Lemma;
        if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(true, s, true, false, ParadigmCollection)) {
            it++;
            continue;
        }

        string CommonAncode;
        int CountWithTheSamePOS = 0;
        for (int k = 0; k < ParadigmCollection.size(); k++) {
            const CFormInfo &Pagadigm = ParadigmCollection[k];
            std::string anc = Pagadigm.GetAncode(0);
            if (I.m_Pos != MorphHolderRus.m_pGramTab->GetPartOfSpeech(anc.c_str())) continue;
            CommonAncode = Pagadigm.GetCommonAncode();
            CountWithTheSamePOS++;
        }

        if (CountWithTheSamePOS == 1) {
            set<CStatInfo>::iterator it1 = it;
            it1++;
            for (; it1 != Infos.end(); it1++)
                if ((it1->m_Lemma == I.m_Lemma)
                    && (it1->m_Pos == I.m_Pos)
                        ) {
                    I.m_Freq += it1->m_Freq;
                } else
                    break;

            I.m_Grammems = MorphHolderRus.m_pGramTab->GetAllGrammems(CommonAncode.c_str());
            it = it1;
            NewInfos.insert(I);
        } else {
            NewInfos.insert(I);
            it++;
        }

    }
    Infos.swap(NewInfos);

}

static bool loadDat(istream &ifs) {
    set<CStatInfo> Infos;
    if (!readDat(ifs, Infos))
        return false;

    DealWithUniqueLemmas(Infos);

    for (set<CStatInfo>::const_iterator it = Infos.begin(); it != Infos.end(); it++) {
        const CStatInfo &I = *it;
        bool bFound = false;
        vector<CFormInfo> ParadigmCollection;
        string s = I.m_Lemma;
        if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(true, s, true, false, ParadigmCollection)) {
            std::cout << "Exception in Lemmatizer (probably wrong ABC: " << I.m_Lemma << " skipped" << std::endl;
            continue;
        }

        for (int k = 0; k < ParadigmCollection.size(); k++) {
            const CFormInfo &Pagadigm = ParadigmCollection[k];
            std::string common_anc = Pagadigm.GetCommonAncode();
            std::string anc = Pagadigm.GetAncode(0);
            if (I.m_Pos != MorphHolderRus.m_pGramTab->GetPartOfSpeech(anc.c_str())) continue;
            if (I.m_Grammems != 0) {
                QWORD g = MorphHolderRus.m_pGramTab->GetAllGrammems(common_anc.c_str()) |
                          MorphHolderRus.m_pGramTab->GetAllGrammems(anc.c_str());
                if ((g & I.m_Grammems) != I.m_Grammems)
                    continue;
            }

            ww.push_back(std::make_pair(Pagadigm.GetParadigmId(), I.m_Freq));
            bFound = true;
        }
        if (!bFound) {
            //std::cout << "Can't produce PID: " << I.DumpToString() << " skipped" << std::endl;
            continue;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////

static bool saveBin(std::string name) {
    std::sort(ww.begin(), ww.end());
    return WriteVector(name, ww);
}

void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.Parse(argc, argv);
}

int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    if (!MorphHolderRus.LoadGraphanAndLemmatizer(args.GetLanguage())) {
        fprintf(stderr, "cannot load morphology holder\n");
        return 1;
    }
    try {
        if (loadDat(args.GetInputStream()))
            if (saveBin(args.CloseOutputStreamAndGetName()))
                return 0;

    }
    catch (CExpc e) {
        fprintf(stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
    }
    catch (...) {
        std::cout << std::endl << "An error in try{} has occurred!" << std::endl;

    }
    return 1;
}
