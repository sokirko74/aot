#ifndef EngBinaryTranslate_h
#define EngBinaryTranslate_h

#pragma warning(disable:4786)
#include "../SemanLib/RossHolder.h"
class CEngSemWord;
class CEngSemNode;
//===========================================================================

struct translate_option_t{
private:
	string	force_lemm;
	int	force_pos;
	UINT	force_grammems;
	bool	has_grammems;
public:
	void	set_force_lemm(const char *str) {force_lemm = str;}
	bool	has_force_lemm()		{return !force_lemm.empty();}
	string&	get_force_lemm()		{return force_lemm;}

	void	set_force_pos(int pos)		{force_pos = pos;}
	bool	has_force_pos()			{return force_pos != -1;}
	int	get_force_pos()			{return force_pos;}

	void	set_force_grammems(UINT gr)	{force_grammems = gr; has_grammems = true;}
	bool	has_force_grammems()		{return has_grammems;}
	UINT	get_force_grammems()		{return force_grammems;}


	// options for dual freq
	const CFreqDict* dual_freq_dict;
	long dual_id1, dual_id2;

	translate_option_t()
		:has_grammems(false),
		dual_freq_dict(0)
	{}

};

//===========================================================================

struct  translate_result_t{
	string str;
	string prefix, postfix;
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

    // �������� ������ ������  m_bad_flags_arr � except_vec
	void init_private_lists();

	// �����������
	translate_helper();

	// �������� ����������
	void flush();
	
	void SetData(const CSemanticsHolder* pData);
	
	string create_form_by_id(long EngId, QWORD eng_grammems) const;
	string create_norm_by_id(long RusId) const;
	

	// ��������� ������� a ��� an � ������. ������ ������ s �� ������ ���� ��������
	string an_article_before(const string &s) const;

	void synthesize(CEngSemWord& EngWord) const;
	void synthesize_by_node(CEngSemNode& Node) const;

    void translate_id(long id, vector<long> &res, poses_mask_t Poses) const;

	void transliterate(CEngSemWord& EngWord);
	
	long GetParadigmIdByLemma(MorphLanguageEnum langua,  string str, UINT pos = -1, bool bProper=false) const;
	QWORD GetFixedGrammemsByLemma(MorphLanguageEnum langua, string str, UINT pos, bool bProper) const;

	const CRossHolder* GetRossHolder (DictTypeEnum type) const;
	const CLemmatizer* GetRusLemmatizer () const;
	const CLemmatizer* GetEngLemmatizer () const;
	const CLemmatizer* GetLemmatizer (MorphLanguageEnum langua) const;
	const CRusGramTab* GetRusGramTab () const;
	const CEngGramTab* GetEngGramTab () const;
	const CAgramtab* GetGramTab (MorphLanguageEnum langua) const;
	const CThesaurus*   GetThes(int ThesId)  const;

	

private:
	// ����� �� ������� ����, ������� �������� �����, �������� ����� �� ����������
	vector<int>    m_bad_flags_arr;
	// ������ ����������, ����� �������� ����� ������� ������� "an",  � �� "a"
	StringVector except_vec;


	bool has_not_english_comparative(long Id) const;


	bool starts_with(const string &s, const StringVector &starts) const;

	void init_list_from_ross(const CRossHolder* RossHolder, const string &list_name, StringVector &res);

private:
	vector<translate_result_t> result_vec;
	translate_option_t  m_option;
};


#endif //INCL_GROUP_TR
