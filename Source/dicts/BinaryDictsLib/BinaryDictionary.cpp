#include "BinaryDictionary.h"
#include "morph_dict/common/utilit.h"
#include "SetOfWordPairs.h"
#include "morph_dict/common/bserialize.h"
#include "fstream"

#include "morph_dict/common/json.h"
#include "morph_dict/agramtab/RusGramTab.h"
#include "morph_dict/agramtab/EngGramTab.h"


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

std::unordered_set<BYTE> get_bad_labels() {
    auto labels_path = GetRegistryString("DictLabels");
    LOGD << "Reading binary dict from %s " << labels_path;
    std::ifstream inp;
    inp.open(labels_path);
    if (!inp.good()) {
        throw CExpc("cannot open %s", labels_path.c_str());
    }
    nlohmann::json js = nlohmann::json::parse(inp);
    std::unordered_set<BYTE> bad_labels;
    for (auto& [key, val] : js.items()) {
        if (!val.value("norm", true)) {
            BYTE id = val["rml_id"];
            bad_labels.insert(id);
        }
    }
    return bad_labels;
}

void CBinaryDictionary::Load() {
    bad_labels = get_bad_labels();
    LOGD << "Reading binary dict from %s " << get_dict_path();
    ReadVector(get_dict_path(), eng_vec);
    if (eng_vec.empty())
        throw CExpc(Format("binary dict %s is empty", get_dict_path().c_str()));

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
    // TODO: Add your implementation code here
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
