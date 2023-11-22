#include "../BinaryDictionary.h"
#include "../SetOfWordPairs.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"



TEST_CASE("test_binary") {
	CMorphanHolder rus;
	CMorphanHolder eng;
	rus.LoadMorphology(morphRussian);
	eng.LoadMorphology(morphEnglish);

	BuildBinaryDict(&rus, &eng, "dict2809.txt", "dict.bin");
	CBinaryDictionary d;
	d.Load("dict.bin", "dict_labels.json");
	DwordVector r = rus.GetLemmaIds("БОЛЬШОЙ");
	CHECK(!r.empty() > 0);
	size_t all_count = 0;
	size_t good_count = 0;
	for (auto id : r) {
		auto st = d.TranslateIndirect(id);
		
		for (size_t i = 0; i < st.get_Count(); ++i) {
			all_count ++;
			if (st.IsNormalLanguage(i)) {
				++good_count;
			}
		}
 	}
	CHECK(all_count == 2);
	CHECK(good_count == 1);
}


int main(int argc, char** argv) {
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int res;
	try {
		res = context.run(); // run doctest
	}
	catch (std::exception& e)
	{
		PLOGE << e.what();
		return 1;
	}
	// important - query flags (and --exit) rely on the user doing this
	if (context.shouldExit()) {
		// propagate the result of the tests
		return res;
	}
}

