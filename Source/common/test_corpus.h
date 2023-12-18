#pragma once 
#include "morph_dict/common/utilit.h"
#include "morph_dict/common/rapidjson.h"

struct CTestCaseBase {
	rapidjson::Document& RapidDoc;
	CJsonObject TestCases;

	void read_test_cases(std::istream& inp);
	void write_test_cases(std::ostream& outp) const;
	CTestCaseBase(rapidjson::Document& d);
};
