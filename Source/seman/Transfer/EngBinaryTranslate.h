#pragma once 

#pragma warning(disable:4786)
#include "seman/SemanLib/struct_dict_holder.h"
class CEngSemWord;
class CEngSemNode;
//===========================================================================

struct translate_option_t{
private:
	std::string	force_lemm;
public:
	void	set_force_lemm(const char *str) {force_lemm = str;}
	bool	has_force_lemm()		{return !force_lemm.empty();}
	std::string&	get_force_lemm()		{return force_lemm;}

	// options for dual freq
	const CFreqDict* dual_freq_dict;
	long dual_id1, dual_id2;

	translate_option_t():
		dual_freq_dict(0)
	{}

};

//===========================================================================

struct  translate_result_t{
	std::string str;
	std::string prefix, postfix;
};


//===========================================================================

class CEngSynthes;
class CEngVerbTense;

class translate_helper 
{
	friend class CEngSynthes;
	friend class CEngVerbTense;
	const CSemanticsHolder* m_pData;		

public:
	
	


	enum TextKind{
		TEXT_COMP,
		TEXT_FIN,
		TEXT_HUD
	}text_kind;

    // прочесть заново списки   except_vec
	void init_private_lists();

	// конструктор
	translate_helper();

	// обновить переводчик
	void flush();
	
	void SetData(const CSemanticsHolder* pData);
	
	std::string create_form_by_id(long EngId, grammems_mask_t eng_grammems) const;
	std::string create_norm_by_id(long RusId) const;
	

	// поставить артикль a или an и пробел. Первый символ s не должен быть пробелом
	std::string an_article_before(const std::string &s) const;

	void synthesize(CEngSemWord& EngWord) const;
	void synthesize_by_node(CEngSemNode& Node) const;
	std::string SetIndefiniteArticle(const std::string& str) const;

    void translate_id(long id, std::vector<long> &res, part_of_speech_mask_t Poses) const;

	void transliterate(CEngSemWord& EngWord);
	
	uint32_t GetParadigmIdByLemma(MorphLanguageEnum langua,  std::string str, part_of_speech_t pos, bool bProper=false) const;
	grammems_mask_t GetFixedGrammemsByLemma(MorphLanguageEnum langua, std::string str, part_of_speech_t pos, bool bProper) const;

	const CStructDictHolder* GetRossHolder (DictTypeEnum type) const;

    const CLemmatizer* GetEngLemmatizer () const;
	const CLemmatizer* GetLemmatizer (MorphLanguageEnum langua) const;
	const CRusGramTab* GetRusGramTab () const;
	const CEngGramTab* GetEngGramTab () const;
	const CAgramtab* GetGramTab (MorphLanguageEnum langua) const;
	const CThesaurus*   GetThes(int ThesId)  const;

	

private:
	// список исключений, перед которыми нужно ставить ратикль "an",  а не "a"
	StringVector except_vec;


	bool has_not_english_comparative(long Id) const;


	bool starts_with(const std::string &s, const StringVector &starts) const;

	void init_list_from_ross(const CStructDictHolder* RossHolder, const std::string &list_name, StringVector &res);

private:
	std::vector<translate_result_t> result_vec;
	translate_option_t  m_option;
};

