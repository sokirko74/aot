#include  "BigramsReader.h"

#include "bserialize.h"
#include "json.h"

#include  <climits>

struct CBigramsWordInfo {
    std::string m_Word;
    size_t m_Freq;
    size_t m_FileOffset1;
    size_t m_FileLen1;
    size_t m_FileOffset2;
    size_t m_FileLen2;

    bool operator==(const std::string &Word) const {
        return m_Word == Word;
    }
};

struct CBigramAndFreq {
    std::string m_Word;
    int m_WordFreq;
    int m_BigramsFreq;
};


class CBigrams {
    std::vector<CBigramsWordInfo> m_Word2Infos;
    FILE *m_Bigrams;
    FILE *m_BigramsRev;
    // РїРѕРєР° РІС‹С‡РёСЃР»СЏРµРј С‚РѕР»СЊРєРѕ РєРѕРЅС‚Р°РєС‚РЅС‹Рµ Р±РёРіСЂР°РјРјС‹,
    // m_CorpusSize РІС‹С‡РёСЃР»СЏСЋС‚СЃСЏ СЃСѓРјРјРёСЂРѕРІР°РЅРёРµРј РІСЃРµС… Р±РёРіСЂР°РјРј РјРёРЅСѓСЃ РєРѕР»-РІРѕ РїСЂРµРґР»РѕР¶РµРЅРёР№
    // (РєРѕР»-РІРѕ РїСЂРµРґР»РѕР¶РµРЅРёР№ РјРѕР¶РЅРѕ РѕРїСѓСЃС‚РёС‚СЊ), Р° РїРѕС‚РѕРј РґР»РёРЅСѓ РєРѕСЂРїСѓСЃР°
    // С‡Р°СЃС‚РѕС‚С‹ РѕС‚РґРµР»СЊРЅС‹С… СЃР»РѕРІ Р»СѓС‡С€Рµ РІСЃРµРіРѕ Р±СѓРґРµС‚ Р·Р°РґР°РІР°С‚СЊ  РѕС‚РґРµР»СЊРЅРѕ.


public:
    size_t m_CorpusSize;

    CBigrams();

    ~CBigrams();

    bool Initialize(std::string BigramsFileName);

    std::vector<CBigramAndFreq> GetBigrams(std::string Word, int MinBigramsFreq, bool bDirectFile, size_t &WordFreq);
};

static CBigrams GlobalBigrams;

struct IsLessBigramsWordInfo {
    bool operator()(const CBigramsWordInfo &X1, const std::string &Word) const {
        return X1.m_Word < Word;
    };

    bool operator()(const std::string &Word, const CBigramsWordInfo &X2) const {
        return Word < X2.m_Word;
    };

    bool operator()(const CBigramsWordInfo &X1, const CBigramsWordInfo &X2) const {
        return X1.m_Word < X2.m_Word;
    };
};

CBigrams::CBigrams() {
    m_Bigrams = 0;
    m_BigramsRev = 0;

}

CBigrams::~CBigrams() {
    if (m_Bigrams != 0) fclose(m_Bigrams);
    if (m_BigramsRev != 0) fclose(m_BigramsRev);
}


bool CBigrams::Initialize(std::string BigramsFileName) {
    m_Word2Infos.clear();
    m_CorpusSize = 0;

    std::string IndexFile = MakeFName(BigramsFileName, "wrd_idx");
    fprintf(stderr, "load %s into memory\n", IndexFile.c_str());
    FILE *fp = fopen(IndexFile.c_str(), "r");
    if (!fp) {
        fprintf(stderr, "Cannot open %s", IndexFile.c_str());
        return false;
    }
    char buffer[1000];
    while (fgets(buffer, 1000, fp)) {
        char word[1000];
        CBigramsWordInfo I;
        if (sscanf(buffer, "%[^ ] %zu %zu %zu %zu %zu", word,
                   &I.m_Freq,
                   &I.m_FileOffset1, &I.m_FileLen1,
                   &I.m_FileOffset2, &I.m_FileLen2) != 6) {
            fprintf(stderr, "Bad format in  %s", IndexFile.c_str());
            fclose(fp);
            return false;
        }
        I.m_Word = word;

        m_Word2Infos.push_back(I);
        m_CorpusSize += I.m_Freq;
    }
    fclose(fp);
    if (m_Bigrams) fclose(m_Bigrams);

    std::string Bin1File = MakeFName(BigramsFileName, "bin1");
    fprintf(stderr, "  open %s \n", Bin1File.c_str());
    m_Bigrams = fopen(Bin1File.c_str(), "rb");
    if (!m_Bigrams) {
        fprintf(stderr, "Cannot open file %s", Bin1File.c_str());
        return false;
    }


    if (m_BigramsRev) fclose(m_BigramsRev);
    std::string Bin2File = MakeFName(BigramsFileName, "bin2");
    fprintf(stderr, "  open %s \n", Bin2File.c_str());
    m_BigramsRev = fopen(Bin2File.c_str(), "rb");
    if (!m_BigramsRev) {
        fprintf(stderr, "Cannot open file %s", Bin2File.c_str());
        return false;
    }

    return true;
}

std::vector<CBigramAndFreq> CBigrams::GetBigrams(std::string Word, int MinBigramsFreq, bool bDirectFile, size_t &WordFreq) {
    WordFreq = 0;
    std::vector<CBigramAndFreq> Result;
    if (!m_Bigrams) return Result;
    if (!m_BigramsRev) return Result;
    EngRusMakeUpper(Word);
    // find word in the index
    std::vector<CBigramsWordInfo>::const_iterator it = lower_bound(m_Word2Infos.begin(), m_Word2Infos.end(), Word,
                                                              IsLessBigramsWordInfo());
    if (it == m_Word2Infos.end()) return Result;
    if (it->m_Word != Word) return Result;

    const CBigramsWordInfo &WordInfo = *it;

    // in the bigrams file a sequence  of integer pair is written:
    // the first element is if of the other word of the bigrams
    // the second element is the bigram frequence
    size_t fpos = (bDirectFile ? WordInfo.m_FileOffset1 : WordInfo.m_FileOffset2);
    if (fpos == UINT_MAX)
        return Result;
    size_t size = bDirectFile ? WordInfo.m_FileLen1 : WordInfo.m_FileLen2;
    FILE *big_fp = bDirectFile ? m_Bigrams : m_BigramsRev;

    if (fseek(big_fp, fpos, SEEK_SET)) {
        return Result;
    }
    std::vector<CBigramInfo > Bigrams;
    ReadVectorInner(big_fp, Bigrams, size);


    for (int i = size - 1; i >= 0; i--) {
        if (Bigrams[i].BigramsFreq < MinBigramsFreq)
            Bigrams.erase(Bigrams.begin() + i);
    }

    // triming bigrams if there are to many of them
    WordFreq = WordInfo.m_Freq;
    if (Bigrams.size() > 250) {
        sort(Bigrams.begin(), Bigrams.end(),  [](const CBigramInfo & a, const CBigramInfo & b) -> bool
        {
            return a.BigramsFreq > b.BigramsFreq;
        });
        Bigrams.erase(Bigrams.begin(), Bigrams.begin() + 250);
    }


    for (auto b : Bigrams) {
        const CBigramsWordInfo &ConvWord = m_Word2Infos[b.WordIndex];
        CBigramAndFreq B;
        B.m_Word = ConvWord.m_Word;
        B.m_BigramsFreq = b.BigramsFreq;
        B.m_WordFreq = ConvWord.m_Freq;
        Result.push_back(B);
    };
    return Result;
}

struct CBigramLine {
    std::string m_Word1;
    std::string m_Word2;
    size_t m_WordFreq1;
    size_t m_WordFreq2;
    size_t m_BigramsFreq;
    double m_MutualInformation;
};

bool GreaterByMutualInformaton(const CBigramLine &_X1, const CBigramLine &_X2) {
    return _X1.m_MutualInformation > _X2.m_MutualInformation;
}

bool GreaterByWordFreq2(const CBigramLine &_X1, const CBigramLine &_X2) {
    return _X1.m_WordFreq2 > _X2.m_WordFreq2;
}

bool GreaterByBigramsFreq(const CBigramLine &_X1, const CBigramLine &_X2) {
    return _X1.m_BigramsFreq > _X2.m_BigramsFreq;
}

std::string GetConnectedWords(std::string Word, int MinBigramsFreq, bool bDirectFile, std::string sortMode, MorphLanguageEnum langua) {
    size_t WordFreq;
    std::vector<CBigramLine> table;
    for (auto &b : GlobalBigrams.GetBigrams(Word, MinBigramsFreq, bDirectFile, WordFreq)) {
        CBigramLine l;
        l.m_Word1 = convert_to_utf8(Word, langua);
        l.m_WordFreq1 = WordFreq;
        l.m_Word2 = convert_to_utf8(b.m_Word, langua);
        l.m_WordFreq2 = b.m_WordFreq;
        if (!bDirectFile) {
            l.m_Word1.swap(l.m_Word2);
            std::swap(l.m_WordFreq1, l.m_WordFreq2);
        }
        l.m_BigramsFreq = b.m_BigramsFreq;
        double p_x_y = ((double) l.m_BigramsFreq / (double) GlobalBigrams.m_CorpusSize);
        double p_x_p_y = ((double) l.m_WordFreq1 / (double) GlobalBigrams.m_CorpusSize) *
                         ((double) l.m_WordFreq2 / (double) GlobalBigrams.m_CorpusSize);
        l.m_MutualInformation = log(p_x_y / p_x_p_y) / log((double) 2);
        table.push_back(l);
    }

    if (sortMode == "SortByMutualInformation")
        sort(table.begin(), table.end(), GreaterByMutualInformaton);
    else if (sortMode == "SortByWordFreq2")
        sort(table.begin(), table.end(), GreaterByWordFreq2);
    else if (sortMode == "SortByBigramsFreq")
        sort(table.begin(), table.end(), GreaterByBigramsFreq);

    auto result = nlohmann::json::array();
    for (auto &a : table) {
        nlohmann::json b = {
                {"word1",       a.m_Word1},
                {"word2",       a.m_Word2},
                {"wordFreq1",   a.m_WordFreq1},
                {"wordFreq2",   a.m_WordFreq2},
                {"bigramsFreq", a.m_BigramsFreq},
                {"mi",          a.m_MutualInformation}
        };
        result.push_back(b);
    }
    return result.dump();
}

bool InitializeBigrams(std::string FileName) {
    return GlobalBigrams.Initialize(FileName);
}