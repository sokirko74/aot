#include "graphan/GraphanLib/GraphmatFile.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "morph_dict/contrib/doctest/doctest.h"

CGraphmatFile rus;
CGraphmatFile ger;

void print_results(CGraphmatFile& g) {
	for (auto& u : g.GetUnits()) {
		std::cerr << u.GetGraphematicalLine() << "\n";
	}
}

TEST_CASE("test_tokens") {
	rus.LoadStringToGraphan("Яя ЯЯ яя z");
	print_results(rus);
	CHECK(4 == rus.GetUnits().size());

	auto u0 = rus.GetUnits()[0];
	CHECK(u0.HasDes(ORLE));
	CHECK(u0.HasDes(OUpLw));

	auto u1 = rus.GetUnits()[1];
	CHECK(u1.HasDes(ORLE));
	CHECK(u1.HasDes(OUp));

	auto u2 = rus.GetUnits()[2];
	CHECK(u2.HasDes(ORLE));
	CHECK(u2.HasDes(OLw));

	auto u3 = rus.GetUnits()[3];
	CHECK(u3.HasDes(OLLE));
	CHECK(u3.HasDes(OLw));
}

TEST_CASE("test_numbers") {
	rus.LoadStringToGraphan("1 1b I");
	CHECK(3 == rus.GetUnits().size());

	auto u0 = rus.GetUnits()[0];
	CHECK(u0.HasDes(ODigits));

	auto u1 = rus.GetUnits()[1];
	CHECK(u1.HasDes(ONumChar));

}

TEST_CASE("test_spaced") {
	auto s = "з а к о н";
	rus.LoadStringToGraphan(s);
	print_results(rus);
	CHECK(2 == rus.GetUnits().size());
	CHECK("закон" == rus.GetTokenUtf8(0));
	CHECK("    " == rus.GetToken(1));

	s = "У  К  А  З";
	rus.LoadStringToGraphan(s);
	print_results(rus);
	CHECK(2 == rus.GetUnits().size());
	CHECK("УКАЗ" == rus.GetTokenUtf8(0));
	CHECK("      " == rus.GetToken(1));

}

TEST_CASE("test_puncts") {
	rus.LoadStringToGraphan("..\\");
	CHECK(2 == rus.GetUnits().size());

	rus.LoadStringToGraphan("N%");
	CHECK(1 == rus.GetUnits().size());

}

TEST_CASE("test_idents") {
	rus.LoadStringToGraphan("c++");
	CHECK(1 == rus.GetUnits().size());
	auto gra = rus.GetUnits()[0].GetGraphematicalLine();
	auto canon = "c++\t0 3\t LLE aa";
	CHECK(canon == gra);
	rus.LoadStringToGraphan("бит/с OS/2 peer-to-peer");
	CHECK(3 == rus.GetUnits().size());
}

TEST_CASE("test_empty") {
	rus.LoadStringToGraphan("");
	CHECK(0 == rus.GetUnits().size());

}

TEST_CASE("test_one_char") {
	rus.LoadStringToGraphan("d");
	CHECK(1 == rus.GetUnits().size());

}

TEST_CASE("test_zero_char") {
	std::string s = "\x0\n";
	std::string path = std::tmpnam(nullptr);
	std::ofstream outp(path);
	outp.write("\x0\n", 2);
	outp.close();
	rus.LoadFileToGraphan(path);

	CHECK(2 == rus.GetUnits().size());
	auto s0 = rus.GetTokenUtf8(0);
	CHECK("\x1" == s0);
	auto s1 = rus.GetTokenUtf8(1);
	CHECK(s1 == std::string(1, '\n'));
	fs::remove(path);
}

TEST_CASE("test_basic") {
	rus.LoadStringToGraphan("мама мыла раму");
	CHECK(3 == rus.GetUnits().size());
	CHECK("мама" == rus.GetTokenUtf8(0));
	CHECK("мыла" == rus.GetTokenUtf8(1));
	CHECK("раму" == rus.GetTokenUtf8(2));
	auto s = rus.GetUnits()[0].GetGraphematicalLine();
	std::string canon = "мама\t0 4\t RLE aa";
	CHECK(canon == s);
}

TEST_CASE("test_force_rus") {
	rus.LoadStringToGraphan("cлишком"); // c is latin
	CHECK(1 == rus.GetUnits().size());
	CHECK("слишком" == rus.GetTokenUtf8(0)); // c is Russian

	rus.LoadStringToGraphan("IP-адрес"); // P must stay latin
	CHECK(1 == rus.GetUnits().size());
	auto s = rus.GetTokenUtf8(0);
	CHECK("IP-адрес" == s); 

	rus.LoadStringToGraphan("яAaEeKkMHOoPpCcyXxuT"); //я+english
	CHECK(1 == rus.GetUnits().size());
	auto t = rus.GetTokenUtf8(0);
	CHECK("яАаЕеКкМНОоРрСсуХхиТ" == t); //я+russian


}

size_t check_sent_breaks(CGraphmatFile& g, std::string s, size_t canon_count) {
	g.LoadStringToGraphan(s);
	size_t count = 0;
	for (auto& u : g.GetUnits()) {
		if (u.HasDes(OSentEnd)) {
			++count;
		}
	}
	auto m = Format("\"%s\": number of sentence must be %i but we have got %i", s.c_str(), canon_count, count);
	if (count != canon_count) {
		print_results(ger);
	}
	CHECK_MESSAGE(count == canon_count, m);
	return count;
}


TEST_CASE("test_sent_breaker") {
	check_sent_breaks(rus, "Мама мыла раму.", 1);
	check_sent_breaks(rus, "Мама (!) мыла раму.", 1);
	check_sent_breaks(rus, "ст. 129 УК РФ", 1);
	
}

void check_file_name(std::string s) {
	rus.LoadStringToGraphan(s);
	auto m = Format("file name %s is not recognized", s.c_str());
	//print_results(rus);
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(OFile1), m);
	CHECK_MESSAGE(rus.GetUnits().back().HasDes(OFile2), m);

}

TEST_CASE("test_file_names") {
	check_file_name("c:\\test.txt");
	check_file_name("1.txt");
	check_file_name("test2.txt");
	check_file_name("test.txt");
	check_file_name("/home/a.txt");
	check_file_name("c:\\drivers\\text.txt");
	check_file_name("\\drivers\\text.txt");
	check_file_name("..\\drivers\\text.txt");
	check_file_name("..\\drivers\\*.txt");
	check_file_name("drivers\\*.txt");
	check_file_name("C:\\test.txt\\tst.txt");
	check_file_name("THISIS~1.TXT");
	
}

void check_person(std::string s, bool check_value=true) {
	rus.LoadStringToGraphan(s);
	auto m = Format("person name \"%s\" is not recognized", s.c_str());
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(OFAM1) == check_value, m);
	CHECK_MESSAGE(rus.GetUnits().back().HasDes(OFAM2) == check_value, m);
}

TEST_CASE("test_names") {
	check_person("Bill J. Bush");
	check_person("Bill Bush");
	check_person("Т.Т.Иванов");
	check_person("Иванов Т.Т.");
	check_person("Т.Е. ОТКАЗАТЬСЯ", false);
	check_person("Пашков А.В.");
	
}

void check_oborot(std::string s) {
	rus.LoadStringToGraphan(s);
	auto m = Format("oborot \"%s\" is not recognized", s.c_str());
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(OEXPR1), m);
	CHECK_MESSAGE(rus.GetUnits().back().HasDes(OEXPR2), m);
	std::string gra = rus.GetUnits()[0].GetGraphematicalLine();
	CHECK(gra.find("EXPR_NO") != std::string::npos);
}

TEST_CASE("test_oborot") {
	check_oborot("потому что");
	check_oborot("Т.Е.");
}

void check_reference(std::string s) {
	rus.LoadStringToGraphan(s);
	auto m = Format("reference \"%s\" is not recognized", s.c_str());
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(ORef1), m);
	CHECK_MESSAGE(rus.GetUnits().back().HasDes(ORef2), m);
}

TEST_CASE("test_reference") {
	check_reference("ч. 1 ст. 2");
}

void check_key(std::string s) {
	rus.LoadStringToGraphan(s);
	auto m = Format("key sequence \"%s\" is not recognized", s.c_str());
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(OKey1), m);
	CHECK_MESSAGE(rus.GetUnits().back().HasDes(OKey2), m);
}

TEST_CASE("test_key_seq") {
	check_key("ctrl-alt-del");
	check_key("control-PgUp");
	check_key("control-Num Lock");
	check_key("control+Num Lock");
}


void check_web(std::string s) {
	rus.LoadStringToGraphan(s);
	auto m = Format("web address  \"%s\" is not recognized", s.c_str());
	CHECK_MESSAGE(rus.GetUnits()[0].HasDes(OElectAddr), m);
}

TEST_CASE("test_web_addr") {
	check_web("http://www.yandex.ru");
	check_web("www.yandex.ru");
	check_web("sokirko@yandex.ru");
}

TEST_CASE("test_long") {
	std::string s(300, 'a');
	rus.LoadStringToGraphan(s);
	CHECK(2 == rus.GetUnits().size());
	CHECK(CriticalTokenLength == rus.GetUnits()[0].GetTokenLength());

}

TEST_CASE("test_english") {
	std::string s = "The mother cleaned the window.";
	CGraphmatFile eng;
	eng.LoadDicts(morphEnglish);
	eng.LoadStringToGraphan(s);
	CHECK(6 == eng.GetUnits().size());
	auto res = eng.GetUnits().back().GetGraphematicalLine();
	CHECK(".\t29 1\t PUN CS? SENT_END" == res);
}

TEST_CASE("test_german") {
	std::string s = "Die Mütter putzten das Fenster.";
	ger.LoadStringToGraphan(s);
	CHECK(6 == ger.GetUnits().size());
	CHECK("Mütter" == ger.GetTokenUtf8(1));
}


TEST_CASE("test_sent_breaker_ger") {
	check_sent_breaks(ger, "Ich gehe\n\nIch gehe", 2);
	check_sent_breaks(ger, "Mother comes\n\n[Reuter] We were informed...", 2);
	check_sent_breaks(ger, "Du gehst! Gehe ich? Wir gehen...  Ich gehe.", 4);
	check_sent_breaks(ger, "Du (gehst!) Gehe ich.", 2);
	check_sent_breaks(ger, "Du (gehst!) ins Haus.", 1);
	check_sent_breaks(ger, "Ich gehe. \"Du gehst.\"", 2);
	check_sent_breaks(ger, "Ich gehe. \"Du gehst.\"  Ich gehe.", 3);
	check_sent_breaks(ger, "Du \"gehst.\"  Ich gehe.", 2);
	CHECK(ger.GetUnits()[4].HasDes(OSentEnd));
	check_sent_breaks(ger, "Ich gng., Peter macht den Korb.", 1);
	check_sent_breaks(ger, "auf den 8. Mai. Das 5.  Corps", 2);
	CHECK(ger.GetUnits()[5].HasDes(OSentEnd));
	check_sent_breaks(ger, "Dr. Geyken gruesst Mr. Bush.", 1);
	check_sent_breaks(ger, "sf .html", 1);
	check_sent_breaks(ger, "Am 21 . Juni 1931", 1);
	check_sent_breaks(ger, "Von Dr . L . Fleck und Dr . J . Hescheles", 1);
	check_sent_breaks(ger, "\"Kennen  jungen!\" Das.", 2);
	CHECK(ger.GetUnits()[5].HasDes(OSentEnd));

}



int main(int argc, char** argv) {
	rus.LoadDicts(morphRussian);
	ger.LoadDicts(morphGerman);
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int res;
	try {
		res = context.run(); // run doctest
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
		return 1;
	}
	// important - query flags (and --exit) rely on the user doing this
	if (context.shouldExit()) {
		// propagate the result of the tests
		return res;
	}
}




