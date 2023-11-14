#include "morph_dict/common/utilit.h"
#include "dicts/StructDictLib/Ross.h"
#include "dicts/StructDictLib/TempArticle.h"
#include "morph_dict/common/argparse.h"

ArgumentParser Args;
std::string TestName = "";

std::string file_to_string(std::string path) {
	std::ifstream t("file.txt");
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void import_export_test() {
	TestName = "import_export_test";
	auto folder = MakePath(Args.Retrieve("test-folder"), "test1");
	CDictionary D;
	auto ross_txt = MakePath(folder, "ross.txt");
	auto ross_txt_test = MakePath(folder, "ross.txt.new");
	D.ImportFromTextFile(ross_txt, folder);
	D.Save();
	D.LoadAndExportDict(folder, ross_txt_test);
	auto canon = file_to_string(ross_txt);
	auto test = file_to_string(ross_txt_test);
	if (canon != test) {
		throw CExpc("import_export_test test failed %s ? %s", ross_txt.c_str(), ross_txt_test.c_str());
	}
}

void search_by_cortege() {
	TestName = "search_by_cortege";
	auto folder = MakePath(Args.Retrieve("test-folder"), "test2");
	CDictionary D;
	auto ross_txt = MakePath(folder, "ross.txt");
	D.ImportFromTextFile(ross_txt, folder);

	auto item_id1 = D.GetItemIdByItemStr("НАР", "D_PART_OF_SPEECH");
	assert(!is_null(item_id1));

	auto item_id2 = D.GetItemIdByItemStr("УСИЛ", "D_SYN_FUNCT");
	assert(!is_null(item_id2));

	size_t count = 0;
	for (auto& unit: D.GetUnits()) {
		for (size_t i = unit.m_StartCortegeNo; i <= unit.m_LastCortegeNo; ++i) {
			auto& c = D.GetCortege(i);
			if (c.GetItem(0) == item_id1 && c.GetItem(1) == item_id2) {
				assert(c.is_null(2));
				count += 1;
			}
		}
	}
	if (count != 1) {
		throw CExpc("search_article failed, count != 1");
	}
}

void search_by_article() {
	TestName = "search_by_article";
	auto folder = MakePath(Args.Retrieve("test-folder"), "test2");
	CDictionary D;
	auto ross_txt = MakePath(folder, "ross.txt");
	D.ImportFromTextFile(ross_txt, folder);
	auto unit_no = D.LocateUnit("мама", 1);
	if (unit_no == ErrUnitNo) {
		throw CExpc("cannot find мама in the dict");
	};

	CTempArticle A(&D);
	A.ReadFromUtf8String("GF      = *  НАР : УСИЛ");

	size_t count = 0;
	for (size_t i = 0; i < D.GetUnitsSize(); ++i) {
		if (D.IncludesArticle(i, &A)) {
			count++;
		}
	}
	if (count != 1) {
		throw CExpc("search_article failed, count != 1");
	}

}

void test_D_ENGL() {
	TestName = "test_D_ENGL";
	auto folder = MakePath(Args.Retrieve("test-folder"), "test2");
	CDictionary D;
	auto ross_txt = MakePath(folder, "ross.txt");
	D.ImportFromTextFile(ross_txt, folder);
	bool Russian2Engl;
	try {
		CTempArticle A(&D);
		A.ReadFromUtf8String("TESTFIELD      = * мама");
		Russian2Engl = true;
	}
	catch (article_parse_error a) {
		Russian2Engl = false;
	}
	if (Russian2Engl) {
		throw CExpc("it is prohibited to insert Russian token to D_ENGL");
	}

	CTempArticle A1(&D);
	std::string art_str = "TESTFIELD= 1  testnew";
	A1.ReadFromUtf8String(art_str.c_str());
	A1.SetUnitNo(D.InsertUnit("new_entry", 1));
	A1.WriteToDictionary();
	
	CTempArticle A2(&D);
	A2.ReadFromUtf8String(art_str.c_str());
	assert(A1.IsPartOf(&A2, true));
	assert(A2.IsPartOf(&A1, true));
	size_t count = 0;
	for (size_t i = 0; i < D.GetUnitsSize(); ++i) {
		if (D.IncludesArticle(i, &A1)) {
			count++;

			CTempArticle A3(&D);
			A3.ReadFromDictionary(i, false, true);
			auto s = Trim(A1.GetArticleStrUtf8());
			assert(s == art_str);
		}
	}
	if (count != 1) {
		throw CExpc("count != 1");
	}
}

void initArgParser(int argc, const char** argv, ArgumentParser& parser) {
	parser.AddOption("--help");
	parser.AddArgument("--test-folder", "test-folder", true);
	parser.AddArgument("--log-level", "log level", true);
	parser.Parse(argc, argv);
}

int main(int argc, const char** argv)
{
	initArgParser(argc, argv, Args);
	init_plog(Args.GetLogLevel(), "struct_dict_test.log");
	try {
		import_export_test();
		search_by_article();
		search_by_article();
		test_D_ENGL();
	}
	catch (article_parse_error& a)
	{
		PLOGE << "test = " << TestName << " : "<< a.what();
		return 1;
	}
	catch (CExpc& e)
	{
		PLOGE << "test = " << TestName << " : " << e.m_strCause;
		return 1;
	}
	catch (std::exception& e)
	{
		PLOGE << "test = " << TestName << " : " << e.what();
		return 1;
	}


	return 0;
}

