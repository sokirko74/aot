// BigramsIndex.cpp : Defines the entry point for the console application.
//

#include "common/utilit.h"
#include <climits>
#include <common/BigramsReader.h>

void PrintUsageAndExit() {
    fprintf(stderr, "BigramsIndex,  creates word-index for bigrams file\n");
    fprintf(stderr, "Usage: Bigrams <bigram-file> \n");
    exit(1);

};

struct CWordInfo {
    string m_WordStr;
    size_t m_Freq;

    size_t m_FileOffset1 = UINT_MAX;
    vector<CBigramInfo> m_Bigrams1;

    size_t m_FileOffset2 = UINT_MAX;
    vector<CBigramInfo> m_Bigrams2;

    bool operator<(const CWordInfo &W) const {
        return m_WordStr < W.m_WordStr;
    }

    bool operator==(const CWordInfo &W) const {
        return m_WordStr == W.m_WordStr;
    }

    bool operator<(const string &s) const {
        return m_WordStr < s;
    }

    bool operator==(const string &s) const {
        return m_WordStr == s;
    }

};

vector<CWordInfo> ReadWordFreqs(string WordFreqFileName) {
    fprintf(stderr, "open file %s\n", WordFreqFileName.c_str());
    FILE *freq_fp = fopen(WordFreqFileName.c_str(), "rb");
    if (!freq_fp) {
        throw CExpc("cannot open file %s\n", WordFreqFileName.c_str());
    }
    vector<CWordInfo> wordInfos;
    char buffer[10000];
    while (fgets(buffer, 10000, freq_fp)) {
        char w[500];
        int freq;
        if (sscanf(buffer, "%[^ ] %i", w, &freq) != 2) {
            fprintf(stderr, "%s: skip line %s", WordFreqFileName.c_str(), buffer);
            continue;
        }
        CWordInfo I;
        I.m_Freq = freq;
        I.m_WordStr = w;
        EngRusMakeUpper(I.m_WordStr);
        if (!wordInfos.empty())
            if (!(wordInfos.back() < w)) {
                throw CExpc("Wrong  position  for word \"%s\" is found\n", w);
            }
        wordInfos.push_back(I);
    }
    fprintf(stderr, "Number of read words: %zu\n", wordInfos.size());
    return wordInfos;
}

void ReadBigrams(string BigramsFileName, vector<CWordInfo> &wordInfos) {
    fprintf(stderr, "open file %s\n", BigramsFileName.c_str());
    FILE *in_fp = fopen(BigramsFileName.c_str(), "rb");
    if (!in_fp) {
        throw CExpc("cannot open file %s\n", BigramsFileName.c_str());
    }
    char buffer[10000];
    size_t linesCount = 0;
    while (fgets(buffer, 10000, in_fp)) {
        ++linesCount;
        if ((linesCount % 100000) == 0)
            fprintf(stderr, "%zu               \r", linesCount);
        char w1[500], w2[500];
        size_t bigramFreq;
        int res = sscanf(buffer, "%[^\t]\t%[^\t]\t%zu", w1, w2, &bigramFreq);
        if (res != 3) {
            rtrim(buffer);
            fprintf(stderr, "%s: skip line %zu \"%s\" (scanf returned %i)\n",
                    BigramsFileName.c_str(), linesCount, buffer, res);
            continue;
        }
        EngRusMakeUpper(w1);
        EngRusMakeUpper(w2);
        auto curr_it = lower_bound(wordInfos.begin(), wordInfos.end(), w1);
        if (curr_it == wordInfos.end()) {
            throw CExpc("Cannot find word \"%s\" in at line %zu\n", w1, linesCount);
        }
        size_t word1 = curr_it - wordInfos.begin();

        curr_it = lower_bound(wordInfos.begin(),wordInfos.end(), w2);
        if (curr_it == wordInfos.end()) {
            throw CExpc("Cannot find word \"%s\" in at line %zu\n", w2, linesCount);
        }
        size_t word2 = curr_it - wordInfos.begin();
        wordInfos[word1].m_Bigrams1.push_back({(DWORD)word2, (DWORD)bigramFreq});
        wordInfos[word2].m_Bigrams2.push_back({(DWORD)word1, (DWORD)bigramFreq});
    }
    fprintf(stderr, "read %zu bigrams\n", linesCount);
}

void WriteBigramsBin(bool bFirstOffset1, vector<CWordInfo>& WordInfos, string BinName ) {
    fprintf(stderr, "write  to  file %s\n", BinName.c_str());
    FILE *out_fp = fopen(BinName.c_str(), "wb");
    if (!out_fp) {
        throw CExpc("cannot open file %s\n", BinName.c_str());
    }
    size_t fileOffset = 0;
    size_t recordSize = get_size_in_bytes(CBigramInfo());
    for (auto& w : WordInfos) {
        if (bFirstOffset1) {
            w.m_FileOffset1 = fileOffset;
            WriteVectorInner(out_fp, w.m_Bigrams1);
            fileOffset += w.m_Bigrams1.size() * recordSize;
        }
        else {
            w.m_FileOffset2 = fileOffset;
            WriteVectorInner(out_fp, w.m_Bigrams2);
            fileOffset += w.m_Bigrams2.size() * recordSize;
        }
    }
    fclose(out_fp);
}

int main(int argc, char *argv[]) {

    if (argc != 2)
        PrintUsageAndExit();


    string BigramsFileName = argv[1];
    string WordFreqFileName = MakeFName(BigramsFileName, "wrd_freq");

    try {
        vector<CWordInfo> wordInfos = ReadWordFreqs(WordFreqFileName);
        ReadBigrams(BigramsFileName, wordInfos);
        WriteBigramsBin(true, wordInfos, MakeFName(BigramsFileName, "bin1"));
        WriteBigramsBin( false, wordInfos, MakeFName(BigramsFileName, "bin2"));

        string OutIndexFile = MakeFName(BigramsFileName, "wrd_idx");
        fprintf(stderr, "create file %s\n", OutIndexFile.c_str());
        FILE *fp = fopen(OutIndexFile.c_str(), "w");
        if (!fp) {
            throw CExpc( "cannot open file %s\n", OutIndexFile.c_str());
        }
        for (auto w : wordInfos) {
            fprintf(fp, "%s %zu %zu %zu %zu %zu\n", w.m_WordStr.c_str(),
                    w.m_Freq,
                    w.m_FileOffset1, w.m_Bigrams1.size(),
                    w.m_FileOffset2, w.m_Bigrams2.size()
            );
        }
        fclose(fp);
    } catch (CExpc e) {
        fprintf (stderr, "Exception: %s", e.m_strCause.c_str());
        return 1;
    }

    return 0;
}





