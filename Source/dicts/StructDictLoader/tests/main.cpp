#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"
#include "morph_dict/common/argparse.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

ArgumentParser Args;
std::string TestName = "";

std::string file_to_string(std::string path) {
	std::ifstream t("file.txt");
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

CDictionary LoadScheme(std::string local_folder) {
	auto folder = MakePath(Args.Retrieve("test-folder"), local_folder);
	REQUIRE(FileExists(folder.c_str()));
	CDictionary D;
	
	D.LoadDictScheme(folder);
	return D;
}

TEST_CASE("import_export_test") {
	auto D = LoadScheme("test1");
	auto ross_txt = MakePath(D.GetDictFolder(), "ross.txt");
	D.ImportFromTextFile(ross_txt);
	D.Save();

	auto ross_txt_test = "ross.txt.new";
	D.LoadAndExportDict(D.GetDictFolder(), ross_txt_test);
	auto canon = file_to_string(ross_txt);
	auto test = file_to_string(ross_txt_test);
	CHECK(canon == test);
}

TEST_CASE("search_by_cortege") {
	auto D = LoadScheme("test2");
	auto ross_txt = MakePath(D.GetDictFolder(), "ross.txt");
	D.ImportFromTextFile(ross_txt);

	auto item_id1 = D.GetItemIdByItemStr("НАР", "D_PART_OF_SPEECH");
	CHECK(!is_null(item_id1));

	auto item_id2 = D.GetItemIdByItemStr("УСИЛ", "D_SYN_FUNCT");
	CHECK(!is_null(item_id2));

	size_t count = 0;
	for (auto& unit: D.GetUnits()) {
		for (size_t i = unit.m_StartCortegeNo; i <= unit.m_LastCortegeNo; ++i) {
			auto& c = D.GetCortege(i);
			if (c.GetItem(0) == item_id1 && c.GetItem(1) == item_id2) {
				CHECK(c.is_null(2));
				count += 1;
			}
		}
	}
	CHECK(count == 1);
}

TEST_CASE("search_by_article") {
	auto D = LoadScheme("test2");
	auto ross_txt = MakePath(D.GetDictFolder(), "ross.txt");
	D.ImportFromTextFile(ross_txt);
	auto unit_no = D.LocateUnit("мама", 1);
	REQUIRE(unit_no != ErrUnitNo);

	CTempArticle A(&D);
	A.ReadFromUtf8String("GF      = *  НАР : УСИЛ");

	size_t count = 0;
	for (size_t i = 0; i < D.GetUnitsSize(); ++i) {
		if (D.IncludesArticle(i, &A)) {
			count++;
		}
	}
	CHECK(count == 1);

}

TEST_CASE ("test_D_ENGL") {
	CDictionary D = LoadScheme("test2");
	try {
		CTempArticle A(&D);
		A.ReadFromUtf8String("TESTFIELD      = * мама");
		CHECK(false);  //it is prohibited to insert Russian token to D_ENGL"
	}
	catch (article_parse_error a) {
	}
	CTempArticle A1(&D);
	std::string art_str = "TESTFIELD= 1  testnew";
	A1.ReadFromUtf8String(art_str.c_str());
	A1.SetUnitNo(D.InsertUnit("new_entry", 1));
	A1.WriteToDictionary();
	
	CTempArticle A2(&D);
	A2.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.IsPartOf(&A2, true));
	CHECK(A2.IsPartOf(&A1, true));
	size_t count = 0;
	for (size_t i = 0; i < D.GetUnitsSize(); ++i) {
		if (D.IncludesArticle(i, &A1)) {
			count++;

			CTempArticle A3(&D);
			A3.ReadFromDictionary(i, false, true);
			auto s = Trim(A1.GetArticleStrUtf8());
			CHECK(s == art_str);
		}
	}
	CHECK(count == 1);
}


TEST_CASE("test_read_example") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "EXM     =  появилась женщина , такая красивая , что они остановились";
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 1);
}

TEST_CASE("test_Russian_free_domain") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "DERIV   =  ПРИЛ ( абсолютный )";
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 1);
	const TCortege& c = A1.GetCortege(0);
	auto dom_no = get_dom_no(c.GetItem(1));
	auto dom_str = D.m_Domens[dom_no].GetDomStr();
	CHECK(dom_str == "D_RLE");
}

TEST_CASE("test_field_aux") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "AUX     =  LOK ( A2 , A3 )";
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 1);
}

int main(int argc, char** argv) {
	init_plog(plog::Severity::debug, "struct_dict_test.log");
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int res;
	try {
		res = context.run(); // run doctest
	}
	catch (article_parse_error& a)
	{
		PLOGE << "test = " << TestName << " : " << a.what();
		return 1;
	}
	catch (CExpc& e)
	{
		PLOGE << "test = " << TestName << " : " << e.what();
		return 1;
	}
	catch (std::exception& e)
	{
		PLOGE << "test = " << TestName << " : " << e.what();
		return 1;
	}
	// important - query flags (and --exit) rely on the user doing this
	if (context.shouldExit()) {
		// propagate the result of the tests
		return res;
	}
}

