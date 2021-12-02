#include <iostream>
#include "morph_dict/common/util_classes.h"
#include "morph_dict/LemmatizerBaseLib/MorphanHolder.h"
#include "morph_dict/common/bserialize.h"
#include "morph_dict/common/argparse.h"
#include <fstream>

// StatDatBin <InDataFile> <-F | -C | -L> [Output Directory]
// Подгружает данные из файла <DataFile> (см. StatTxtDat)
// Записывает триады <pid,form-num,weight> в файл "r*homoweight.bin"

struct CGroup {
    int pid;
    std::string anc;
    int m_FormNo;

    CGroup() : pid(0), anc("") {}

    friend bool operator<(const CGroup& x, const CGroup& y);
};

bool operator<(const CGroup& x, const CGroup& y) {
    if (x.anc < y.anc)
        return (1);
    if (x.anc > y.anc)
        return (0);
    return (x.pid < y.pid);
}

typedef std::map<CGroup, int> tagLines;
typedef std::map<std::string, tagLines> tagHomon;
static tagHomon homon;

//////////////////////////////////////////////////////////////////////////////

typedef troika<int, int, int> _homonode_t;
typedef std::vector<_homonode_t> _homoresults_t;

struct less4homonode : public std::less<_homonode_t> {
    bool operator()(const _homonode_t& x, const _homonode_t& y) const {
        return (x.first == y.first
            ? x.second < y.second
            : x.first < y.first);
    }
};

static _homoresults_t homonweight;
CMorphanHolder MorphHolder;

static void addLines(std::string& str, tagLines& lines) {
    std::pair<tagHomon::iterator, bool> res;
    res = homon.insert(tagHomon::value_type(str, lines));
    if (!res.second) {
        std::cout << "Duplicate word: " << str << " skipped." << std::endl;
    }
}

static bool loadDat(std::istream& ifs) {
    std::string str;
    tagLines lines;

    char buf[2048];
    int lin = 0;

    while (ifs.getline(buf, 2048)) {
        lin++;
        StringTokenizer tok(buf, " ");
        std::string swrd = tok.next_token();
        std::string spid = tok.next_token();
        std::string spos = tok.next_token();
        std::string s1 = tok.next_token();
        std::string s2 = tok.next_token();
        if (swrd.empty() || spid.empty() || spos.empty()) {
            std::cout << "Error in line: " << lin << " skipped" << std::endl;
            continue;
        }
        if (s1.empty() && s2.empty()) {
            std::cout << "Error in line: " << lin << " skipped" << std::endl;
            continue;
        };
        std::string sgrm;
        int val = -1;

        if (s2.empty())
            val = atoi(s1.c_str());
        else {
            sgrm = s1;
            val = atoi(s2.c_str());
        };


        if (val < 0 && atoi(sgrm.c_str()) >= 0) {
            val = atoi(sgrm.c_str());
            sgrm.erase();
        }
        //
        std::string grm = spos;
        grm += " ";
        grm += sgrm;
        BYTE pos;
        uint64_t gra;
        std::string def;
        if (!MorphHolder.m_pGramTab->ProcessPOSAndGrammemsIfCan(grm.c_str(), &pos, &gra)
            || !MorphHolder.m_pGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan(pos, gra, def)) {
            std::cout << "Error in line: " << lin << " skipped" << std::endl;
            continue;
        }

        //
        if (str.empty() && swrd == "*")
            continue;
        if (swrd != "*") {
            if (!str.empty() && lines.size() > 0)
                addLines(str, lines);
            str = swrd;
            lines.clear();
        }


        std::vector<CFormInfo> ParadigmCollection;

        std::string s = str;
        if (!MorphHolder.m_pLemmatizer->CreateParadigmCollection(true, spid, true, false, ParadigmCollection))
            throw CExpc(Format("Cannot lemmatize \"%s\"", spid.c_str()));


        int k = 0;
        for (; k < ParadigmCollection.size(); k++) {
            const CFormInfo& Pagadigm = ParadigmCollection[k];
            CGroup group;
            group.pid = Pagadigm.GetParadigmId();
            group.anc = Pagadigm.GetAncode(0);
            group.anc = group.anc.substr(0, 2);
            if (group.anc == def) {
                group.m_FormNo = 0;
                { // finding the  input form in the paradigm

                    for (; group.m_FormNo < Pagadigm.GetCount(); group.m_FormNo++)
                        if (Pagadigm.GetWordForm(group.m_FormNo) == str)
                            break;

                    if (group.m_FormNo == Pagadigm.GetCount()) continue;
                };

                std::pair<tagLines::iterator, bool> res;
                res = lines.insert(tagLines::value_type(group, val));
                if (!res.second)
                    std::cout << "Duplicate pid/anc: " << lin << " skipped" << std::endl;
                break;
            }
        }
        if (k >= ParadigmCollection.size()) {
            std::cout << "Can't calculate pid/anc: Line N " << lin << std::endl << "Operation aborted" << std::endl;
            return false;
        }
    }
    //
    if (!str.empty() && lines.size() > 0)
        addLines(str, lines);
    std::cout << "done" << std::endl;
    return true;
}

//////////////////////////////////////////////////////////////////////////////

static bool saveBin(std::string name) {
    std::cout << "Saving " << name.c_str() << "... ";
    tagHomon::iterator it;
    for (it = homon.begin(); it != homon.end(); it++) {
        std::string str = it->first;
        tagLines::iterator lin;
        for (lin = it->second.begin(); lin != it->second.end(); lin++) {
            int pid = lin->first.pid;
            int pos = lin->first.m_FormNo;
            int val = lin->second;

            _homoresults_t::value_type _newitem;
            _newitem.first = pid;
            _newitem.second = pos;
            _newitem.third = val;
            homonweight.push_back(_newitem);
        }
    }

    std::sort(homonweight.begin(), homonweight.end(), less4homonode());

    WriteVector(name, homonweight);;
    std::cout << "done" << std::endl;
    return true;
}

void initArgParser(int argc, const char** argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.Parse(argc, argv);
}

int main(int argc, const char** argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    try {
        MorphHolder.LoadLemmatizer(args.GetLanguage());

        if (loadDat(args.GetInputStream())) {
            if (saveBin(args.CloseOutputStreamAndGetName()))
                return 0;
        }
    }
    catch (CExpc e) {
        fprintf(stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
        return 1;
    }
    catch (...) {
        std::cout << std::endl << "An error in try{} has occurred!" << std::endl;
    }

    return 1;
}
