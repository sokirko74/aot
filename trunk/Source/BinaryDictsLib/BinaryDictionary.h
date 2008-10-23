// BinaryDictionary.h : Declaration of the CBinaryDictionary

#ifndef __BINARYDICTIONARY_H_
#define __BINARYDICTIONARY_H_
#pragma warning(disable:4786)
#pragma warning(disable:4530)

#include "../common/utilit.h"


typedef	DWORD id_type;
const DWORD data_length = 5;

struct WordPair{

	id_type rus;
	id_type eng;
	BYTE data[data_length];
	
};


class CSetOfWordPairs;


/////////////////////////////////////////////////////////////////////////////
// CBinaryDictionary
class CBinaryDictionary 
{
public:

	//--------------------------------------
	std::vector<WordPair> eng_vec;
	std::vector<WordPair> rus_vec;
	//--------------------------------------
	std::string get_dict_path() const;
	friend class CSetOfWordPairs;

	long GetDirectId(long index) const;
	long GetIndirectId(long index) const;

	bool GetFlag(long pair_index, long flag_index, string& text, bool is_direct, DWORD& flag) const;
	

	CBinaryDictionary();
	bool HavePair (long id1, long id2) const;
	CSetOfWordPairs TranslateIndirect(long id) const;
	CSetOfWordPairs TranslateDirect(long id) const;
	bool Load();
	long GetFlagCount() const;
};

#endif //__BINARYDICTIONARY_H_
