#include "morph_dict/common/utilit.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include  "dicts/BinaryDictsLib/AspPairDict.h"

#include <iostream>
#include <ctime>
#include <set>


int main()
{
	CAspPairDict d;
	d.Load();
	CMorphanHolder RussianHolder;
	RussianHolder.LoadMorphology(morphRussian);
	auto ids1 = RussianHolder.GetLemmaIds("застигать", false);
	auto ids2 = RussianHolder.GetLemmaIds("застигнуть", false);
	std::set<uint32_t> perf_ids;
	for (auto id1 : ids1) {
		DwordVector res;
		d.nonperf2perf(id1, res);
		perf_ids.insert(res.begin(), res.end());
	}
	for (auto id2 : ids2) {
		if (perf_ids.find(id2) != perf_ids.end()) {
			return 0;
		}
	}
	return 1;

}

