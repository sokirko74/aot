#include "test_corpus.h"

CTestCaseBase::CTestCaseBase(rapidjson::Document& d): 
	RapidDoc(d), 
	TestCases(d, rapidjson::kArrayType)
{

}

void CTestCaseBase::read_test_cases(std::istream& inp) {
	std::string s;
	while (getline(inp, s)) {
		Trim(s);
		std::string comments;
		std::string text;
		size_t commentsIndex = s.find("//");
		if (commentsIndex != std::string::npos) {
			comments = s.substr(commentsIndex);
			text = s.substr(0, commentsIndex);
			Trim(text);
			Trim(comments);
		}
		else {
			text = s;
		}
		if (text.empty()) {
			continue;
		}
		CJsonObject j(RapidDoc);
		j.add_string_copy("input", text);
		j.add_string_copy("comments", comments);

		TestCases.push_back(j.get_value());
	}
}

void CTestCaseBase::write_test_cases(std::ostream& outp) const {
	outp << TestCases.dump_rapidjson_pretty(4);
}
