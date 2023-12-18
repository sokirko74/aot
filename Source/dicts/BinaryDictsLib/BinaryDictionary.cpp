#include "BinaryDictionary.h"
#include "morph_dict/common/utilit.h"
#include "SetOfWordPairs.h"
#include "morph_dict/common/bserialize.h"
#include "fstream"

#include "morph_dict/common/rapidjson.h"
#include "morph_dict/agramtab/RusGramTab.h"
#include "morph_dict/agramtab/EngGramTab.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include "morph_dict/lemmatizer_base_lib/Paradigm.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"


inline size_t get_size_in_bytes(const WordPair &t) {
    return get_size_in_bytes(t.rus) + get_size_in_bytes(t.eng) + data_length;
};


inline size_t save_to_bytes(const WordPair &i, BYTE *buf) {
    buf += save_to_bytes(i.rus, buf);
    buf += save_to_bytes(i.eng, buf);
    memcpy(buf, i.data, data_length);
    return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(WordPair &i, const BYTE *buf) {
    buf += restore_from_bytes(i.rus, buf);
    buf += restore_from_bytes(i.eng, buf);
    memcpy(i.data, buf, data_length);
    return get_size_in_bytes(i);
}


BYTE get_simplified_part_of_speech(BYTE flag, MorphLanguageEnum langua) {
    if (flag & (BYTE) 32)  {
        return langua == morphRussian ? (BYTE)PREDK : (BYTE)eVERB;
    }
    flag = flag & (BYTE) 7;
    switch (flag) {
        case 0:
            return langua == morphRussian ? (BYTE)NOUN : (BYTE)eNOUN;
        case 1:
            return langua == morphRussian ? (BYTE)INFINITIVE : (BYTE)eVERB;
        case 2:
            return langua == morphRussian ? (BYTE)ADJ_FULL : (BYTE)eADJ;
        case 3:
            return (langua == morphRussian) ? (BYTE)ADV : (BYTE)eADV;
    }
    assert (false);
    return -1;
}

bool LessRus1(const WordPair &w1, const WordPair &w2) {
    return (w1.rus < w2.rus) || ((w1.rus == w2.rus) && (w1.eng < w2.eng));
}

bool LessRus(const WordPair &w1, const WordPair &w2) {
    return (w1.rus < w2.rus);
}

bool LessEng(const WordPair &w1, const WordPair &w2) {
    return (w1.eng < w2.eng);
}


bool LessEng1(const WordPair &w1, const WordPair &w2) {
    return (w1.eng < w2.eng) || ((w1.eng == w2.eng) && (w1.rus < w2.rus));
}


CBinaryDictionary::CBinaryDictionary() {
    eng_vec.reserve(260000);
}

std::string CBinaryDictionary::get_dict_path() const {
    return std::string(GetRegistryString("Software\\Dialing\\ENGLISH_RUSSIAN_DICT\\DictPath"));
}

std::unordered_set<BYTE> get_bad_labels(std::string labels_path) {
    LOGD << "Reading binary dict from %s " << labels_path;
    std::ifstream inp;
    inp.open(labels_path);
    if (!inp.good()) {
        throw CExpc("cannot open %s", labels_path.c_str());
    }
    rapidjson::Document doc;
    rapidjson::IStreamWrapper isw(inp);
    doc.ParseStream(isw);

    std::unordered_set<BYTE> bad_labels;
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        auto& val = it->value;
        if (val.HasMember("norm") && !val["norm"].GetBool()) {
            BYTE id = val["rml_id"].GetInt();
            bad_labels.insert(id);
        }
    }
    return bad_labels;
}

void CBinaryDictionary::Load(std::string path, std::string labels_path) {
    if (path.empty()) {
        path = get_dict_path();
    }
    if (labels_path.empty()) {
        labels_path = GetRegistryString("DictLabels");
    }
    bad_labels = get_bad_labels(labels_path);

    LOGD << "Reading binary dict from %s " << path;
    ReadVector(path, eng_vec);
    if (eng_vec.empty())
        throw CExpc(Format("binary dict %s is empty", path.c_str()));

    std::sort(eng_vec.begin(), eng_vec.end(), LessEng1);


    rus_vec = eng_vec;;
    std::sort(rus_vec.begin(), rus_vec.end(), LessRus1);
}

long CBinaryDictionary::GetIndirectId(long index) const {
    return rus_vec[index].eng;
}

long CBinaryDictionary::GetDirectId(long index) const {
    return eng_vec[index].rus;
}


part_of_speech_t CBinaryDictionary::GetSimplifiedPartOfSpeech(long pair_index, bool is_direct) const {
    const WordPair &wp = (is_direct) ? eng_vec[pair_index] : rus_vec[pair_index];
    MorphLanguageEnum  langua = is_direct ? morphEnglish : morphRussian;
    return get_simplified_part_of_speech(wp.data[0], langua);
}

bool CBinaryDictionary::IsNormalLanguage(long pair_index, bool is_direct) const {
    const WordPair &wp = (is_direct) ? eng_vec[pair_index] : rus_vec[pair_index];
    for (size_t  i  = 1; i < data_length; ++i) {
        if (bad_labels.find(wp.data[i]) != bad_labels.end()) {
            return false;
        }
    }
    return true;
}

CSetOfWordPairs CBinaryDictionary::TranslateDirect(long id) const {
    typedef std::vector<WordPair>::const_iterator iter;
    WordPair wp;
    wp.eng = id;

    std::pair<iter, iter> range = std::equal_range(eng_vec.begin(), eng_vec.end(), wp, LessEng);
    int begin = range.first - eng_vec.begin();
    int end = range.second - eng_vec.begin();

    CSetOfWordPairs p;
    p.SetDictionary(this, true);
    p.SetRange(begin, end);
    return p;
}

CSetOfWordPairs CBinaryDictionary::TranslateIndirect(long id) const {
    // TODO: Add your implementation code here
    typedef std::vector<WordPair>::const_iterator iter;
    WordPair wp;
    wp.rus = id;

    std::pair<iter, iter> range = std::equal_range(rus_vec.begin(), rus_vec.end(), wp, LessRus);
    int begin = range.first - rus_vec.begin();
    int end = range.second - rus_vec.begin();

    CSetOfWordPairs p;
    p.SetDictionary(this, false);
    p.SetRange(begin, end);
    return p;
}

bool CBinaryDictionary::HavePair(long id1, long id2) const {
    typedef std::vector<WordPair>::const_iterator iter;
    WordPair wp;
    wp.eng = id1;

    std::pair<iter, iter> range = std::equal_range(eng_vec.begin(), eng_vec.end(), wp, LessEng);
    for (iter i = range.first; i != range.second; ++i) {
        if (i->rus == id2) {
            return true;
        }
    }
    return false;
}


DwordVector GetParadigmIdsByNormAndPos(const CMorphanHolder& holder, std::string& str, int part_of_speech)
{
    std::vector<CFormInfo> ParadigmCollection;
    DwordVector res;
    std::string word_s8 = convert_from_utf8(str.c_str(), holder.m_CurrentLanguage);
        if (!holder.m_pLemmatizer->CreateParadigmCollection(true, word_s8, true, false, ParadigmCollection))
            throw CExpc("Cannot lemmatize %s", str.c_str());

    for (auto& p : ParadigmCollection)
    {
        if (!p.m_bFound) return res;
        auto code = p.GetAncode(0).substr(0, 2);
        if (holder.m_pGramTab->GetPartOfSpeech(code.c_str()) == part_of_speech) {
            res.push_back(p.GetParadigmId());
        }
    }

    return res;
}

void make_bin(const CMorphanHolder* rus, const CMorphanHolder* eng, std::string& r, std::string& e, BYTE d[5], std::ostream& out) {
    auto pos_r = get_simplified_part_of_speech(d[0], morphRussian);
    auto pos_e = get_simplified_part_of_speech(d[0], morphEnglish);
    DwordVector r_id = GetParadigmIdsByNormAndPos(*rus, r, pos_r);
    DwordVector e_id = GetParadigmIdsByNormAndPos(*eng, e, pos_e);
    if (pos_r == ADJ_FULL && e_id.empty()) {
        // московский -> Moscow
        e_id = GetParadigmIdsByNormAndPos(*eng, e, eNOUN);
    }
    if (pos_r == NOUN && r_id.empty()) {
        // клиентский -> client
        r_id = GetParadigmIdsByNormAndPos(*rus, r, ADJ_FULL);
    }

    for (auto id1 : r_id) {
        for (auto id2 : e_id) {
            //std::cout << convert_to_utf8(r, morphRussian) << " " << e << " " << id1 << " " << id2 << "\n";
            out.write((char*)&(id1), sizeof(uint32_t));
            out.write((char*)&(id2), sizeof(uint32_t));
            out.write((char*)d, 5);
        }
    }
}

void BuildBinaryDict(const CMorphanHolder* rus, const CMorphanHolder* eng, std::string input_path, std::string output_path) {
    std::ifstream inp;
    inp.open(input_path);

    std::ofstream out;
    out.open(output_path, std::ios::binary);

    int line_no = 0;
    std::string line;
    while (std::getline(inp, line)) {
        line_no++;
        Trim(line);
        auto items = split_string(line, ' ');
        if (items.size() != 7) {
            throw CExpc(" Bad format at line %s (line no = %i)", line.c_str(), line_no);
        }
        auto r = items[0];
        auto e = items[1];
        BYTE d[data_length];
        for (size_t i = 2; i < items.size(); ++i) {
            int u = atoi(items[i].c_str());
            assert(0 < u && u < 256);
            d[i - 2] = u;
        }
        make_bin(rus, eng, r, e, d, out);
        //if (line_no % 1000 == 0) std::cerr << line_no << '\r';
    }
    out.close();
    inp.close();
}

