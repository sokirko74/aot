#include "../common/utilit.h"
#include "../common/bserialize.h"

struct CBigramInfo {
    DWORD WordIndex;
    DWORD BigramsFreq;
};

inline size_t get_size_in_bytes (const CBigramInfo& t)
{
    return get_size_in_bytes(t.WordIndex) + get_size_in_bytes(t.BigramsFreq);

};

inline size_t save_to_bytes(const CBigramInfo& i, BYTE* buf)
{
    buf += save_to_bytes(i.WordIndex, buf);
    buf += save_to_bytes(i.BigramsFreq, buf);
    return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(CBigramInfo& i, const BYTE* buf)
{
    buf += restore_from_bytes(i.WordIndex, buf);
    buf += restore_from_bytes(i.BigramsFreq, buf);
    return get_size_in_bytes(i);
}


extern std::string GetConnectedWords(std::string Word, int MinBigramsFreq, bool bDirectFile, std::string sortMode, MorphLanguageEnum langua);
extern bool InitializeBigrams(std::string FileName);
