#pragma once 
#include "nlp_base/common/utilit.h"
#include "nlp_base/common/rapidjson.h"

struct CTestCaseBase {
	rapidjson::Document& RapidDoc;
	CJsonObject TestCases;

	void read_test_cases(std::istream& inp);
	void write_test_cases(std::ostream& outp) const;
	CTestCaseBase(rapidjson::Document& d);
};
