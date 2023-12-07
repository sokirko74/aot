#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CGraphmatFile graphan;

CLemmatizedText lemmatize_text(std::string s) {
	graphan.LoadStringToGraphan(s);
	CLemmatizedText t(morphGerman);
	t.CreateFromTokemized(&graphan);
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

void compile_grammar(std::string path, CWorkGrammar& g) {
	if (!fs::exists(path)) {
		path = (fs::path(__FILE__).parent_path() / path).string();
	}
	g.InitalizeGrammar(morphGerman, path);
	g.CreatePrecompiledGrammar();
	g.LoadGrammarForGLR(true);
}


TEST_CASE("filtering_for_postmorph") {
	CWorkGrammar g;
	compile_grammar("grammar/test.grm", g);
	CLemmatizedText t = lemmatize_text("Rot Bild");
	CHECK(2 == t.m_LemWords.size());
	CHECK(2 == t.m_LemWords[0].GetHomonymsCount());
	auto res = g.FilterHomonymsByGrammar(t);
	CHECK(1 == res.m_LemWords[0].GetHomonymsCount());
}

TEST_CASE("simple_np") {
	CWorkGrammar g;
	compile_grammar("grammar2/test.grm", g);
	check_first_entity(g, "die schwache Resolution", { "MODIF_NP", 0, 3 });
	check_first_entity(g, "die schwache Fürstin", { "MODIF_NP", 0, 3 }); // check umlauts (encoding)
}


TEST_CASE("check_register") {
	CWorkGrammar g;
	compile_grammar("grammar3/test.grm", g);
	check_first_entity(g, "Rot", { "main", 0, 1 });
	check_first_entity(g, "ähnlich", { "main", 0, 1 });
	
	auto t = lemmatize_text("afdfd");
	auto res = g.GetFoundOccurrences(t);
	CHECK(res.empty());
}

TEST_CASE("check_inclusion") {
	CWorkGrammar g;
	compile_grammar("grammar4/test.grm", g);
	check_first_entity(g, "Rot", { "main", 0, 1 });
	check_first_entity(g, "Bild Bilds", { "main", 0, 2 });
}

TEST_CASE("check_predicted_word") {
	CWorkGrammar g;
	compile_grammar("grammar5/test.grm", g);
	check_first_entity(g, "RRRRRRRRRRR", { "main", 0, 1 });
	check_not_parsed(g, "Mann");
	// numbers
	check_first_entity(g, "2.650", { "main", 0, 1 });
	check_first_entity(g, "2,650", { "main", 0, 1 });
	// left truncation
	check_first_entity(g, "Mutter", { "main", 0, 1 });

}

TEST_CASE("grammar_precompiled") {
	CWorkGrammar g1;
	fs::path path = fs::path("grammar/test.grm");
	if (!fs::exists(path)) {
		path = fs::path(__FILE__).parent_path() / path;
	}
	g1.InitalizeGrammar(morphGerman, path.string());
	g1.CreatePrecompiledGrammar();
	
	auto cur_path = fs::current_path();  // cd to other folder and load grammar
	auto tmp_path = cur_path / "tmp";
	if (!fs::exists(tmp_path)) {
		fs::create_directory(tmp_path);
	}
	fs::current_path(tmp_path);
	CWorkGrammar g2;
	g2.InitalizeGrammar(morphGerman, fs::absolute(path).string());
	g2.LoadGrammarForGLR(true);
	fs::current_path(cur_path);
	fs::remove_all(tmp_path);
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
// todo: test for [TOKEN grm="-"] and CParser::AreEqual

