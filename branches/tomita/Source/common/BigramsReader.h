#ifndef BigramsReader_h
#define BigramsReader_h

#include "../common/utilit.h"


extern string GetConnectedWords(string Word, int MinBigramsFreq, bool bDirectFile);
extern string GetBigramsAsString(string Word, int MinBigramsFreq, bool bDirectFile);
extern bool InitializeBigrams(string FileName);
#endif