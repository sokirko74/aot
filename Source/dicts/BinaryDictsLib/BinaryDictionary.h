#pragma once
#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include "morph_dict/common/utilit.h"


typedef	uint32_t id_type;
const uint32_t data_length = 5;

struct WordPair {

	id_type rus;
	id_type eng;
	BYTE data[data_length];
	
};


class CSetOfWordPairs;

extern part_of_speech_t get_simplified_part_of_speech(BYTE flag, MorphLanguageEnum langua);

class CMorphanHolder;


class CBinaryDictionary
{
    std::vector<WordPair> eng_vec;
    std::vector<WordPair> rus_vec;
    std::unordered_set<BYTE> bad_labels;
public:

	std::string get_dict_path() const;
	friend class CSetOfWordPairs;

	long GetDirectId(long index) const;
	long GetIndirectId(long index) const;

    part_of_speech_t GetSimplifiedPartOfSpeech(long pair_index, bool is_direct) const;
    bool IsNormalLanguage(long pair_index, bool is_direct) const;


	CBinaryDictionary();
	bool HavePair (long id1, long id2) const;
	CSetOfWordPairs TranslateIndirect(long id) const;
	CSetOfWordPairs TranslateDirect(long id) const;
	void Load(std::string path="", std::string labels_path ="");
};

extern void BuildBinaryDict(const CMorphanHolder* rus, const CMorphanHolder* eng, std::string input_path, std::string output_path);