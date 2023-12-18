#include "seman/SemanLib/SemStructureBuilder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CSemStructureBuilder SemBuilder;

void build_sem_structure(std::string s, long clause_var_no = -1) {
	CSemOptions opts;
	opts.m_ClauseVariantsCombinationIndex = clause_var_no;
	SemBuilder.FindSituations(s, opts);
}

bool has_relation(std::string relname, std::string src, std::string trg) {
	for (auto& r : SemBuilder.m_RusStr.m_Relations) {
		auto o = SemBuilder.m_RusStr.GetOutputRelation(r);
		if (o.sem_rel == relname && src == o.src && trg == o.trg) {
			return true;
		}
	}
	return false;
}


TEST_CASE("test_adj") {
	build_sem_structure("новый двор");
	CHECK(has_relation("PROPERT", "ДВОР", "НОВЫЙ"));
}

TEST_CASE("test_trg") {
	build_sem_structure("я пойду во двор");
	CHECK(has_relation("SUB", "ПОЙДУ", "Я"));
	CHECK(has_relation("TRG-PNT", "ПОЙДУ", "ДВОР"));
}

TEST_CASE("test_colloc_interp") {
	build_sem_structure("застал врасплох");
	CHECK(SemBuilder.m_RusStr.m_Nodes.size() == 1);
	
}

TEST_CASE("test_fin_thes_interp") {
	build_sem_structure("бюджетные ассигнования");
	CHECK(SemBuilder.m_RusStr.m_Nodes.size() == 2);
	CRusSemNode& n = SemBuilder.m_RusStr.m_Nodes[0];
	CHECK(n.m_Colloc.GetThesInterp().m_ThesaurusId == FinThes);
}

TEST_CASE("test_not_dict_valency") {
	build_sem_structure("вы соединялись");
	CHECK(has_relation("ACT", "СОЕДИНЯЛИ", "ВЫ"));
}

TEST_CASE("test_clause_vars") {
	std::string s = "голубые ели, голубые дали";
	build_sem_structure(s);
	auto& r = SemBuilder.m_RusStr;
	CHECK(4 == r.m_ClauseCombinationVariantsCount);

	build_sem_structure(s, 0);
	CHECK(r.m_ClauseVariantsCombinationNo == 0);
	CHECK("ДАТЬ" == r.m_Nodes[3].m_Words[0].m_Lemma);

	build_sem_structure(s, 3);
	CHECK(r.m_ClauseVariantsCombinationNo == 3);
	CHECK("ДАЛЬ" == r.m_Nodes[3].m_Words[0].m_Lemma);
}

TEST_CASE("test_time_ross") {
	std::string s = "24 часа в сутки";
	build_sem_structure(s);
	for (auto& n : SemBuilder.m_RusStr.m_Nodes) {
		CHECK(TimeRoss == n.m_Colloc.GetRossInterp().m_DictType);
	}

}

TEST_CASE("test_subj_from_colloc") {
	std::string s = "Я готов на все";
	build_sem_structure(s);
	CHECK(SemBuilder.m_RusStr.m_Nodes.size() == 2);
	CHECK(has_relation("PROPERT", "Я", "ГОТОВ НА ВСЕ"));
	CHECK(SemBuilder.m_RusStr.m_Relations.size() == 1);
}

std::string get_interp(size_t n) {
	auto& node = SemBuilder.m_RusStr.m_Nodes[n];
	auto interp = *node.GetInterp();
	return SemBuilder.m_RusStr.GetRoss(interp.m_DictType)->GetEntries()[interp.m_UnitNo].GetEntryAndMeanNum();
}

TEST_CASE("test_bad_interp") {
	CSemOptions opts;
	opts.m_ProhibitedLexVars.push_back("быть1"); // prohibit быть1
	SemBuilder.FindSituations("быть", opts);
	CHECK("быть2" == get_interp(0));

}

TEST_CASE("test_free_prep") {
	std::string s = "Между Ваней и девушкой ничего не было";
	build_sem_structure(s);
	CHECK("быть2" == get_interp(SemBuilder.m_RusStr.m_Nodes.size() - 1));
}



int main(int argc, char** argv) {
	init_plog(plog::Severity::verbose, "test_seman.log");
	SemBuilder.InitDicts();

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

