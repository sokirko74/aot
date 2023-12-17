#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CGraphmatFile graphan;

CLemmatizedText lemmatize_text(std::string s) {
	graphan.LoadStringToGraphan(s);
	CLemmatizedText t(morphGerman);
	t.CreateFromTokemized(&graphan);
	t.SaveToFile("test_text.lem");
	return t;
}

void check_first_entity(CWorkGrammar& g, std::string s, CFoundEntity canon) {
	auto t = lemmatize_text(s);
	auto res = g.GetFoundOccurrences(t);
	CHECK(!res.empty());
	CHECK(canon == res[0]);
}

void check_not_parsed(CWorkGrammar& g, std::string s) {
	auto t = lemmatize_text(s);
	auto res = g.GetFoundOccurrences(t);
	CHECK(res.empty());

}

fs::path get_grm_path(std::string folder) {
	return fs::path(__FILE__).parent_path() / "data" / folder / "test.grm";
}

void remove_precompiled_files(std::string folder) {
	auto p = get_grm_path(folder).string();
	fs::remove(MakeFName(p, "grammar_precompiled"));
	fs::remove(MakeFName(p, "table"));
	fs::remove(MakeFName(p, "automat"));
}

void compile_grammar(std::string folder, CWorkGrammar& g) {
	g.InitalizeGrammar(morphGerman, get_grm_path(folder).string());
	g.CreatePrecompiledGrammar();
	g.LoadGrammarForGLR(true);
	remove_precompiled_files(folder);
}


TEST_CASE("filtering_for_postmorph") {
	CWorkGrammar g;
	compile_grammar("01.basic", g);
	CLemmatizedText t = lemmatize_text("Rot Bild");
	CHECK(2 == t.m_LemWords.size());
	CHECK(2 == t.m_LemWords[0].GetHomonymsCount());
	auto res = g.FilterHomonymsByGrammar(t);
	CHECK(1 == res.m_LemWords[0].GetHomonymsCount());
}

TEST_CASE("grammar_precompiled") {
	CWorkGrammar g1;
	auto path = get_grm_path("01.basic");
	g1.InitalizeGrammar(morphGerman, path.string());
	g1.CreatePrecompiledGrammar();

	auto save_path = fs::current_path();
	fs::current_path(save_path.root_directory()); // cd to root

	CWorkGrammar g2;
	g2.InitalizeGrammar(morphGerman, fs::absolute(path).string());
	g2.LoadGrammarForGLR(true);

	fs::current_path(save_path);
	remove_precompiled_files("01.basic");
}

TEST_CASE("simple_np") {
	CWorkGrammar g;
	compile_grammar("02.np", g);
	check_first_entity(g, "die schwache Resolution", { "MODIF_NP", 0, 3 });
	check_first_entity(g, "die schwache Fürstin", { "MODIF_NP", 0, 3 }); // check umlauts (encoding)
}


TEST_CASE("check_register") {
	CWorkGrammar g;
	compile_grammar("03.register", g);
	check_first_entity(g, "Rot", { "main", 0, 1 });
	check_first_entity(g, "ähnlich", { "main", 0, 1 });
	
	auto t = lemmatize_text("afdfd");
	auto res = g.GetFoundOccurrences(t);
	CHECK(res.empty());
}

TEST_CASE("check_inclusion") {
	CWorkGrammar g;
	compile_grammar("04.coordination", g);
	check_first_entity(g, "Rot", { "main", 0, 1 });
	check_first_entity(g, "Bild Bilds", { "main", 0, 2 });
}

TEST_CASE("check_predicted_word") {
	CWorkGrammar g;
	compile_grammar("05.predicted", g);
	check_first_entity(g, "RRRRRRRRRRR", { "main", 0, 1 });
	check_not_parsed(g, "Mann");
	// left truncation
	check_first_entity(g, "Mutter", { "main", 0, 1 });

}

TEST_CASE("check_numbers") {
	CWorkGrammar g;
	compile_grammar("06.numbers", g);
	check_first_entity(g, "2.650", { "main", 0, 1 });
	check_first_entity(g, "2,650", { "main", 0, 1 });
	check_first_entity(g, "II", { "main", 0, 1 });
}



int main(int argc, char** argv) {
	init_plog(plog::Severity::verbose, "test_simple_grammar.log");
	GlobalLoadMorphHolder(morphGerman);
	graphan.LoadDicts(morphGerman);

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

