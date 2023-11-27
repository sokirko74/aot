#include "stdafx.h"
#include "SemanticRusStructure.h"

/*
функция делает следующее преобразование:
  учительница -> учитель
  читательница -> читатель
  наездница -> наездник
  велосипедистка -> велосипедист
*/
struct CFemDerivSuffix {
    std::string fem;
    size_t cut_len;
    std::string masc;
};

const static std::vector<CFemDerivSuffix> rus_fem_suffixes = {
    {"тельница", std::string("ница").length(), ""},
    {"ница", std::string("ца").length(), "к"},
    {"ица",  std::string("ца").length(), "к"},
    {"стка", std::string("ка").length(), ""}
};

bool CRusSemStructure::BuildWordWithoutFemineSuffix(std::string& lemma) const {

    for (auto& s : rus_fem_suffixes) {
        if (endswith(lemma, s.fem)) {
            auto new_lemma = lemma.substr(0, lemma.length() - s.cut_len) + s.masc;
            if (m_pData->GetFirstParadigmId(morphRussian, new_lemma, 1 << NOUN) == UnknownParadigmId)
                return false;
            lemma = new_lemma;
        }
    }
    return false;
};
