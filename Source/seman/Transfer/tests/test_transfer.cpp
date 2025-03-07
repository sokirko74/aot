#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"
#include "morph_dict/common/argparse.h"
#include "seman/SemanLib/SemanticRusStructure.h"
#include "seman/SemanLib/SemStructureBuilder.h"
#include "seman/Transfer/numerals.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CSemStructureBuilder SemBuilder;

TEST_CASE("transliteration") {
	translate_helper t;
	CEngSemWord w;
	w.SetWord("мама");
	t.transliterate(w);
	CHECK("mama" == w.GetWord());
	w.SetWord("пуепе");
	w.m_Lemma  = "";
	t.transliterate(w);
	CHECK("puyepe" == w.GetWord());
}

TEST_CASE("fix_case") {
	std::string s = "mother , comes .";
	SetSpacesAndRegisterInSentence(s);
	CHECK("Mother, comes" == s);

	s = "<Quote>мама пришла .</Quote>";
	SetSpacesAndRegisterInSentence(s);
	CHECK("\"Мама пришла .\"" == s);

}

std::string translate(std::string s)
{
	return SemBuilder.TranslateRussianText(s, CommonDomain);
}

TEST_CASE("translate_name") {
	auto s = translate("Джон");
	CHECK(s == "John.");
	
}

TEST_CASE("test_numeral") {
	std::string s = spellout_number("2", true);
	CHECK(s == "two");

	s = spellout_number("2", false);
	CHECK(s == "second");

	s = translate("два");
	CHECK(s == "Two.");
}

TEST_CASE("test_timeross_article") {
	auto s = translate("год назад");
	CHECK(s == "A year ago.");

}

TEST_CASE("test_colloc") {
	auto s = translate("не говоря уже о багаже");
	CHECK(s == "Not to speak of luggage.");

}

TEST_CASE("test_article_with_PP") {
	// all article rules are adhoc: check that NP with  "of" is .GetTense()definite
	auto s = translate("регулирование обращения");
	CHECK(s == "The regulation of appeal.");

	s = translate("тринадцать тел из числа погибших");
	CHECK(s == "Thirteen bodies from the lost number.");
}

TEST_CASE("test_productive_prefix") {
	auto s = translate("авиакатастрофа");
	CHECK(s == "Aerocatastrophe.");
}

TEST_CASE("test_article_an") {
	auto& helper = SemBuilder.m_EngStr.helper;
	std::string s = helper.SetIndefiniteArticle("<a> unexperienced");
	CHECK(s == "an unexperienced");

	s = helper.SetIndefiniteArticle("<a> hour");
	CHECK(s == "an hour");

	s = helper.SetIndefiniteArticle("<a> such boy");
	CHECK(s == "such a boy");

	s = helper.SetIndefiniteArticle("<Quote> <a> </Quote> <Quote> ocean </Quote>");
	CHECK(s == "<Quote> an </Quote> <Quote> ocean </Quote>");

	s = "\"Он - начинающий президент\"";
	s = translate(s);
	CHECK(s == "\"He is an unexperienced president\".");

}



int main(int argc, char** argv) {
	SemBuilder.InitDicts();
	init_plog(plog::Severity::verbose, "test_transfer.log");
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int res;
	try {
		res = context.run(); // run doctest
	}
	catch (std::exception& e)
	{
		PLOGE << "test = " << " : " << e.what();
		return 1;
	}
	// important - query flags (and --exit) rely on the user doing this
	if (context.shouldExit()) {
		// propagate the result of the tests
		return res;
	}
}

