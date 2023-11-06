#include "dicts/BinaryDictsLib/BinaryDictionary.h"

#include "morph_dict/common/util_classes.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


CMorphanHolder MorphHolderRus;
CMorphanHolder MorphHolderEng;


DwordVector GetParadigmIdsByNormAndPos(CMorphanHolder& holder, std::string &str, int part_of_speech)
{
    std::vector<CFormInfo> ParadigmCollection;
    DwordVector res;
    if (!holder.m_pLemmatizer->CreateParadigmCollection(true, str, true, false, ParadigmCollection ))
        throw CExpc(Format("Cannot lemmatize %s", str.c_str()));

    for(auto& p: ParadigmCollection)
    {
        if (!p.m_bFound) return res;
        auto code = p.GetAncode(0).substr(0, 2);
        if (holder.m_pGramTab->GetFirstPartOfSpeech(code.c_str()) == part_of_speech)  {
            res.push_back(p.GetParadigmId());
        }
    }

    return res;
}

void make_bin(std::string &r, std::string &e, BYTE d[5], std::ostream &out) {
    auto pos_r = get_simplified_part_of_speech(d[0], morphRussian);
    auto pos_e = get_simplified_part_of_speech(d[0], morphEnglish);
    DwordVector r_id = GetParadigmIdsByNormAndPos(MorphHolderRus, r, pos_r);
    DwordVector e_id = GetParadigmIdsByNormAndPos(MorphHolderEng, e, pos_e);
    if (pos_r == ADJ_FULL  && e_id.empty()) {
        // московский -> Moscow
        e_id = GetParadigmIdsByNormAndPos(MorphHolderEng, e, eNOUN);
    }
    if (pos_r == NOUN  && r_id.empty()) {
        // клиентский -> client
        r_id = GetParadigmIdsByNormAndPos(MorphHolderRus, r, ADJ_FULL);
    }

    for (auto id1:  r_id) {
        for (auto id2: e_id) {
            //std::cout << convert_to_utf8(r, morphRussian) << " " << e << " " << id1 << " " << id2 << "\n";
            out.write((char *) &(id1), sizeof(uint32_t));
            out.write((char *) &(id2), sizeof(uint32_t));
            out.write((char *) d, 5);
        }
    }
}


int main(int argc, char **argv) {
    try {
        assert(argc == 3);
        std::ifstream inp;
        inp.open(argv[1]);

        std::ofstream out;
        out.open(argv[2], std::ios::binary);

        MorphHolderRus.LoadLemmatizer(morphRussian);
        MorphHolderEng.LoadLemmatizer(morphEnglish);

        int line_no = 0;
        std::string line;
        while (std::getline(inp, line)) {
            line_no++;
            Trim(line);
            auto items = split_string(convert_from_utf8(line.c_str(), morphRussian), ' ');
            if (items.size() != 7) {
                fprintf(stderr, " Bad format at line %s (line no = %i)\n", line.c_str(), line_no);
                return 0;
            }
            auto r = items[0];
            auto e = items[1];
            BYTE d[data_length];
            for (size_t i = 2; i < items.size(); ++i) {
                int u = atoi(items[i].c_str());
                assert(0 < u < 256);
                d[i - 2] = u;
            }
            make_bin(r, e,  d, out);
            if (line_no % 1000 == 0) std::cerr << line_no << '\r';
        }
        out.close();
        inp.close();
        return 0;
    }
    catch (CExpc e) {
        fprintf(stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
        return 1;
    }
    catch (...) {
        fprintf(stderr, "some exception occurred!\n");
        return 1;
    }
}
