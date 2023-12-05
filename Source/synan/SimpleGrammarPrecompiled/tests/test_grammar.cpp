#include "synan/SimpleGrammarLib/SimpleGrammar.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CMorphanHolder holder;

CLemmatizedText get_plm_lines(std::string s) {
	CGraphmatFile gra;
	gra.LoadDicts(morphGerman);
	gra.LoadStringToGraphan(s);
	CLemmatizedText t;
	t.m_pLemmatizer = holder.m_pLemmatizer;
	t.CreateFromTokemized(&gra);
	return t;
}

void check_first_entity(CWorkGrammar& g, std::string s, CFoundEntity canon) {
	auto t = get_plm_lines(s);
	auto res = g.GetFoundOccurrences(t);
	CHECK(!res.empty());
	CHECK(canon == res[0]);
}

void compile_grammar(std::string path, CWorkGrammar& g) {
	g.InitalizeGrammar(holder.m_pGramTab, path);
	g.CreatePrecompiledGrammar();
	g.LoadGrammarForGLR(true);
}


TEST_CASE("filtering_for_postmorph") {
	CWorkGrammar g;
	compile_grammar("grammar/test.grm", g);
	CLemmatizedText t = get_plm_lines("Rot Bild");
	CHECK(4 == t.m_PlmItems.size());
	auto res = g.FilterHomonymsByGrammar(t);
	CHECK(3 == res.m_LemWords.size());
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
	
	auto t = get_plm_lines("afdfd");
	auto res = g.GetFoundOccurrences(t);
	CHECK(res.empty());
}

TEST_CASE("check_inclusion") {
	CWorkGrammar g;
	compile_grammar("grammar4/test.grm", g);
	check_first_entity(g, "Rot", { "main", 0, 1 });
	check_first_entity(g, "Bild Bilds", { "main", 0, 2 });
}


TEST_CASE("grammar_precompiled") {
	CWorkGrammar g1;
	g1.InitalizeGrammar(holder.m_pGramTab, "grammar/test.grm");
	g1.CreatePrecompiledGrammar();
	
	auto path = fs::current_path();  // cd to other folder and load grammar
	auto tmp_path = path / "tmp";
	if (!fs::exists(tmp_path)) {
		fs::create_directory("tmp");
	}
	fs::current_path(tmp_path);
	CWorkGrammar g2;
	g2.InitalizeGrammar(holder.m_pGramTab, "../grammar/test.grm");
	g2.LoadGrammarForGLR(true);
	fs::current_path(path);
	fs::remove_all(tmp_path);
}


int main(int argc, char** argv) {
	//init_plog(plog::Severity::debug, "test_simple_grammar.log");
	init_plog(plog::Severity::verbose, "test_simple_grammar.log");
	holder.LoadMorphology(morphGerman);
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

