#pragma once

#include "morph_dict/common/utilit.h"
#include "morph_dict/AgramtabLib/RusGramTab.h"
#include "morph_dict/AgramtabLib/GerGramTab.h"
#include "morph_dict/AgramtabLib/EngGramTab.h"
#include "morph_dict/AgramtabLib/rus_consts.h"
#include "morph_dict/AgramtabLib/eng_consts.h"
#include "morph_dict/AgramtabLib/ger_consts.h"
#include "Paradigm.h"
#include "Lemmatizers.h"

class CMorphanHolder
{
protected:
	std::string GetGrammems(const char* tab_str) const;
	bool GetParadigmCollection(std::string WordForm, std::vector<CFormInfo>& Paradigms) const;
	void CreateMorphDicts(MorphLanguageEnum langua);
public:
	MorphLanguageEnum				m_CurrentLanguage;
	CAgramtab*						m_pGramTab;
    CLemmatizer* 			        m_pLemmatizer;
	bool                            m_bUsePrediction;

	CMorphanHolder();
	~CMorphanHolder();

    void LoadLemmatizer(MorphLanguageEnum langua, std::string custom_folder="");
	void DeleteProcessors();
	DwordVector GetParadigmIdsByNormAndAncode(std::string &str, const std::string &code) const;
	void		string_to_ids(const char *str, DwordVector &ids, bool bNorm) const;
	std::string		id_to_string(long id) const;
	CFormInfo		id_to_paradigm(long id) const;
	std::string PrintMorphInfoUtf8(std::string Form, bool printIds, bool printForms, bool sortParadigms) const;
	std::string LemmatizeJson(std::string WordForm, bool withParadigms, bool prettyJson=false, bool sortForms=false) const;
	bool IsInDictionary(std::string WordForm) const;
};

