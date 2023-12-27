// BigramsIndex.cpp : Defines the entry point for the console application.
//

#include "common/BigramsReader.h"
#include <zlib.h>

#include <climits>
#include "morph_dict/common/utilit.h"

void PrintUsageAndExit() {
    fprintf(stderr, "BigramsIndex,  creates word-index for bigrams file\n");
    fprintf(stderr, "Usage: Bigrams <input_folder> \n");
    exit(1);

};

struct CWordInfo {
    std::string m_WordStr;
    size_t m_Freq;

    size_t m_FileOffset1 = UINT_MAX;
    std::vector<CBigramInfo> m_Bigrams1;

    size_t m_FileOffset2 = UINT_MAX;
    std::vector<CBigramInfo> m_Bigrams2;

    bool operator<(const CWordInfo &W) const {
        return m_WordStr < W.m_WordStr;
    }

    bool operator==(const CWordInfo &W) const {
        return m_WordStr == W.m_WordStr;
    }

    bool operator<(const std::string &s) const {
        return m_WordStr < s;
    }

    bool operator==(const std::string &s) const {
        return m_WordStr == s;
    }

};

std::vector<CWordInfo> ReadWordFreqs(std::string WordFreqFileName) {
    fprintf(stderr, "open file %s\n", WordFreqFileName.c_str());
    FILE *freq_fp = fopen(WordFreqFileName.c_str(), "rb");
    if (!freq_fp) {
        throw CExpc("cannot open file %s\n", WordFreqFileName.c_str());
    }
    std::vector<CWordInfo> wordInfos;
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
        MakeUpperUtf8(I.m_WordStr);
        if (!wordInfos.empty())
            if (!(wordInfos.back() < w)) {
                throw CExpc("Wrong  position  for word \"%s\" is found, the input file is not sorted\n", w);
            }
        wordInfos.push_back(I);
    }
    fprintf(stderr, "Number of read words: %zu\n", wordInfos.size());
    return wordInfos;
}

void ReadBigrams(std::string path, std::vector<CWordInfo> &wordInfos) {
    fprintf(stderr, "open file %s\n", path.c_str());
    gzFile file = gzopen(path.c_str(), "rb");
    if(!file) {
        throw CExpc("cannot open file %s\n", path.c_str());
    }
    std::string line;
    size_t linesCount = 0;
    char buffer[1024];
    while (gzgets(file, buffer, sizeof(buffer)) != nullptr) {
        line = buffer;
        ++linesCount;
        if ((linesCount % 100000) == 0)
            fprintf(stderr, "%zu               \r", linesCount);
        Trim(line);
        auto items = split_string(line, '\t');
        if (items.size() != 3) {
            fprintf(stderr, "%s: skip line %zu \"%s\"", path.c_str(), linesCount, line.c_str());
            continue;
        }
        std::string w1 = items[0];
        std::string w2 = items[1];
        uint32_t bigramFreq = atoi(items[2].c_str());
        MakeUpperUtf8(w1);
        MakeUpperUtf8(w2);
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
        wordInfos[word1].m_Bigrams1.push_back({(uint32_t)word2, (uint32_t)bigramFreq});
        wordInfos[word2].m_Bigrams2.push_back({(uint32_t)word1, (uint32_t)bigramFreq});
    }
    gzclose(file);

    fprintf(stderr, "read %zu bigrams\n", linesCount);
}

void WriteBigramsBin(bool bFirstOffset1, std::vector<CWordInfo>& WordInfos, std::string BinName ) {
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
    auto folder = fs::path(argv[1]);
    if (!fs::exists(folder)) {
        std::cerr << "folder " << folder << " does not exist!";
        return 1;
    }
    std::string BigramsFileName = (folder / "bigrams.txt.gz").string();
    std::string WordFreqFileName = (folder / "unigrams.txt").string();

    try {
        std::vector<CWordInfo> wordInfos = ReadWordFreqs(WordFreqFileName);
        ReadBigrams(BigramsFileName, wordInfos);
        WriteBigramsBin(true, wordInfos, MakeFName(BigramsFileName, "bin1"));
        WriteBigramsBin( false, wordInfos, MakeFName(BigramsFileName, "bin2"));

        std::string OutIndexFile = MakeFName(BigramsFileName, "wrd_idx");
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
        fprintf (stderr, "Exception: %s", e.what());
        return 1;
    }

    return 0;
}





