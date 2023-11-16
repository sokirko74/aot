#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"
#include "morph_dict/common/argparse.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

ArgumentParser Args;
std::string TestName = "";

std::string file_to_string(std::string path) {
	std::ifstream t(path);
	assert(t.good());
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
	REQUIRE(canon.length() > 0);
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

TEST_CASE("test_good_vals") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "VAL = SUB, A1, C\n"
		"RESLT, A2, C\n"
		"SRC-PNT, A3, C\n"
		"TRG-PNT, A4, C\n"
		"LOK, A5, C\n"
		"TIME, A6, C\n"
		"AIM, A7, C\n"
		;
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 7);
	A1.SetUnitNo(D.InsertUnit("test1", 1));
	A1.WriteToDictionary();

}

TEST_CASE("test_long_example") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "EXM     =  выявить потенциальных лидеров , способных не на словах , а на деле двигать перестройку вперед да";
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 1);
	A1.SetUnitNo(D.InsertUnit("test1", 1));
	A1.WriteToDictionary();

}

TEST_CASE("test_broken_vals") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "VAL = SUB, A2, C";
	A1.ReadFromUtf8String(art_str.c_str());
	A1.SetUnitNo(D.InsertUnit("test1", 1));
	try {
		A1.WriteToDictionary();
		CHECK(false);
	}
	catch (CExpc c) {
	}
}

TEST_CASE("test_masked_vals") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "VAL     =  SUB , * , C";
	A1.ReadFromUtf8String(art_str.c_str());
	A1.SetUnitNo(D.InsertUnit("test1", 1));
	A1.WriteToDictionary();
}

TEST_CASE("test_field_aux") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string art_str = "AUX     =  LOK ( A2 , A3 )";
	A1.ReadFromUtf8String(art_str.c_str());
	CHECK(A1.GetCortegesSize() == 1);
}

TEST_CASE("test_add_new") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string new_value = "——new example 1000";
	std::string art_str = "EXM     =  " + new_value;
	auto dom_no = D.GetDomenNoByDomStr("D_EXM");
	CHECK(dom_no != ErrUChar);
	const CDomen& d = D.m_Domens[dom_no];
	auto item_id1 = d.GetDomItemIdByStr(new_value);
	CHECK(is_null(item_id1));
	A1.ReadFromUtf8String(art_str.c_str());
	auto item_id2 = d.GetDomItemIdByStr(new_value);
	CHECK(!is_null(item_id2));
	auto saved_item_str = d.GetDomItemStrById(item_id2);
	CHECK(saved_item_str == new_value);
	std::set<uint32_t> ids;
	for (auto s : d.GetAllStrings()) {
		if (s != new_value) {
			auto id = d.GetDomItemIdByStr(s);
			CHECK(id != item_id2);
			CHECK(ids.find(id) == ids.end());
			ids.insert(id);
			auto s1 = d.GetDomItemStrById(id);
			CHECK(s1 == s);
		}
	}
}

TEST_CASE("test_add_new") {
	CDictionary D = LoadScheme("test2");
	CTempArticle A1(&D);
	std::string new_value = "——new example 1000";
	std::string art_str = "EXM     =  " + new_value;
	auto dom_no = D.GetDomenNoByDomStr("D_EXM");
	CHECK(dom_no != ErrUChar);
	const CDomen& d = D.m_Domens[dom_no];
	auto item_id1 = d.GetDomItemIdByStr(new_value);
	CHECK(is_null(item_id1));
	A1.ReadFromUtf8String(art_str.c_str());
	auto item_id2 = d.GetDomItemIdByStr(new_value);
	CHECK(!is_null(item_id2));
	auto saved_item_str = d.GetDomItemStrById(item_id2);
	CHECK(saved_item_str == new_value);
	for (auto s : d.GetAllStrings()) {
		if (s != new_value) {
			CHECK(d.GetDomItemIdByStr(s) != item_id2);
		}
	}
}


int main(int argc, char** argv) {
	init_plog(plog::Severity::debug, "struct_dict_test.log");
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int res;
	try {
		res = context.run(); // run doctest
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

