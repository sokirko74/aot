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

bool import_export_test() {
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

bool search_by_cortege() {
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

bool search_by_article() {
	TestName = "search_by_article";
	auto folder = MakePath(Args.Retrieve("test-folder"), "test2");
	CDictionary D;
	auto ross_txt = MakePath(folder, "ross.txt");
	D.ImportFromTextFile(ross_txt, folder);

	CTempArticle A(&D);
	A.ReadFromUtf8String("GF      = *  НАР : УСИЛ");
	size_t count = 0;
	for (auto& unit : D.GetUnits()) {
		CTempArticle B(&D);
		B.ReadFromDictionary(unit.m_EntryId, false, true);
		if (A.IsPartOf(&B, true)) {
			count++;
		}
	}
	if (count != 1) {
		throw CExpc("search_article failed, count != 1");
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
		//import_export_test();
		//search_article();
		search_by_article();
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

