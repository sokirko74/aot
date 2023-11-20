#include "synan/SimpleGrammarLib/SimpleGrammar.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

TEST_CASE("load_grammar") {
	CWorkGrammar g1;
	g1.CreatePrecompiledGrammar(morphGerman, "grammar/test.grm");
	
	CWorkGrammar g2;
	g2.LoadGrammarForGLR(true);
}


int main(int argc, char** argv) {
	init_plog(plog::Severity::debug, "test_simple_grammar.log");
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

