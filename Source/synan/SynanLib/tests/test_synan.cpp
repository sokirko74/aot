#include "synan/SynCommonLib/RelationsIterator.h"
#include "synan/SynanLib/SyntaxHolder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CSyntaxHolder H(morphRussian);

void build_syntax(std::string s) {
	H.GetSentencesFromSynAn(s, false);
}

bool has_relation(std::string relname, std::string src, std::string trg) {
	auto& sent = H.m_Synan.m_vectorSents[0];
	std::vector<size_t> clause_var;
	while (sent->GetNextClauseVariant(clause_var)) {
		CRelationsIterator RelIt;
		RelIt.SetSentence(sent);
		for (auto& i : sent->m_vectorPrClauseNo)
			RelIt.AddClauseNoAndVariantNo(i, clause_var[i]);
		RelIt.BuildRelations();
		for (auto& r : RelIt.GetRelations()) {
			auto n = RelIt.GetRelationName(r);
			auto s = RelIt.GetSourceNodeStr(r);
			auto t = RelIt.GetTargetNodeStr(r);
			if (n == relname && s == src && t == trg) {
				return true;
			}
		}
	}
	return false;
}


TEST_CASE("test_adj") {
	build_syntax("новый двор");
	CHECK(has_relation("ПРИЛ_СУЩ", "двор", "новый"));
}

TEST_CASE("test_homogenius_adjs") {
	build_syntax("красивый, злой");
	CHECK(has_relation("ОДНОР_ПРИЛ", ",", "злой"));
	CHECK(has_relation("ОДНОР_ПРИЛ", ",", "красивый"));
}

TEST_CASE("test_number_noun") {
	build_syntax("4 часа");
	CHECK(has_relation("ЧИСЛ_СУЩ", "часа", "4")); // cardinal
	CHECK(has_relation("ПРИЛ_СУЩ", "часа", "4")); // ordinal

	build_syntax("24 часа");
	CHECK(has_relation("ЧИСЛ_СУЩ", "часа", "24")); // cardinal
	CHECK(has_relation("ПРИЛ_СУЩ", "часа", "24")); // ordinal

}


int main(int argc, char** argv) {
	init_plog(plog::Severity::verbose, "test_synan.log");
	GlobalLoadMorphHolder(morphRussian);
	H.LoadSyntax();

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

