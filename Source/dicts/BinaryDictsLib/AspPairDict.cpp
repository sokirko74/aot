// BinaryDictionary.cpp : Implementation of CBinaryDictionary
#include "stdafx.h"
#include "morph_dict/common/utilit.h"
#include "AspPairDict.h"
#include "morph_dict/common/bserialize.h"


// BinaryDictionary.cpp : Implementation of CBinaryDictionary
// BinaryDictionary.cpp : Implementation of CBinaryDictionary
//----------------------------------------------------------------------------------------
/**
This class is a storage of Aspect Pairs database.
The database is a binary file of the following structure:
uint32_t -- number of pairs
uint32_t uint32_t -- first pair
.........
uint32_t uint32_t -- last pair

First index in pair is ParadigmId of NonPerfective verb, the second index - 
is ParadigmId of Perfective verb.

*/


//--------------

inline size_t get_size_in_bytes (const asp_pair& t)
{
	return get_size_in_bytes(t.NonPerf) + get_size_in_bytes(t.Perf);

};

inline size_t save_to_bytes(const asp_pair& i, BYTE* buf)
{
	buf += save_to_bytes(i.NonPerf, buf);
	buf += save_to_bytes(i.Perf, buf);
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(asp_pair& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.NonPerf, buf);
	buf += restore_from_bytes(i.Perf, buf);
	return get_size_in_bytes(i);
}




CAspPairDict::CAspPairDict()
{
}


void CAspPairDict::Load()
{
	std::string Path = GetRegistryString("Software\\Dialing\\RUSSIAN_ASPECT_DICT\\DictPath");
	ReadVector(Path, pair_vec1);
	if (pair_vec1.size() == 0) throw CExpc(Format("cannot read %s", Path.c_str()));
	pair_vec2 = pair_vec1;
	std::sort(pair_vec1.begin(), pair_vec1.end(), asp_pair::Less1());
	std::sort(pair_vec2.begin(), pair_vec2.end(), asp_pair::Less2());
}


//--------------
//!Get Perfective verbs by NonPerfective one. Return number of such verbs.
size_t CAspPairDict::nonperf2perf(uint32_t nonperf, DwordVector &res)const 
{
	res.clear();

	std::pair<CIter, CIter> range = std::equal_range(
		pair_vec1.begin(), pair_vec1.end(),
		asp_pair(nonperf, 0), asp_pair::Less1());

	for(CIter i = range.first; i != range.second; ++i)
		res.push_back(i->Perf);
	return res.size();
}
//--------------
//!Get NonPerfective verbs by Perfective one. Return number of such verbs.
size_t CAspPairDict::perf2nonperf(uint32_t perf, DwordVector &res) const 
{
	res.clear();

	std::pair<CIter, CIter> range = std::equal_range(
		pair_vec2.begin(), pair_vec2.end(),
		asp_pair(0, perf), asp_pair::Less2());

	for(CIter i = range.first; i != range.second; ++i)
		res.push_back(i->NonPerf);
	return res.size();
}



