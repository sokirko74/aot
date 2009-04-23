// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef rus_format_caller
 #define rus_format_caller


#include "RusGroup.h"
#include "RusSyntaxOpt.h"
#include "../SynCommonLib/FormatCaller.h"



enum ESynRules {
	rule_for_numbers = 0,
	rule_for_num_complex,
	rule_for_noun_num,
	rule_for_names,
	rule_for_adv_adj,
	rule_for_similar_adjs,
	rule_for_modif_adj,
	rule_for_similar_advs,
	rule_for_similar_infs,
	rule_for_similar_comp_adjs,	
	rule_for_comp_adv,
	rule_for_adv_verb,
	rule_for_noun_groups,
	rule_for_number_adverb,
	rule_for_number_noun,
	rule_for_approx_noun_number,
	rule_for_rank_surname,
	rule_for_gen_chains,
	rule_for_from_compar,
	rule_for_preps,
	rule_for_prep_IZ,
	rule_for_selective_groups,	
	rule_for_similar_noun_groups,
	rule_for_neg_verb,
	rule_for_dir_obj,
	rule_for_instr_obj,
	rule_for_dir_obj_rev,
	rule_for_verb_inf,
	rule_for_adv_adv,
	rule_for_web_addr,
	rule_for_similar_numerals,
	rule_for_noun_adjuncts,
	rule_adv_number_adverb,
	rule_for_approx_prep_noun,
	rule_for_partic_clause,
	rule_for_adj_in_commas_preposition,
	rule_for_plural_noun_adj,
	rule_for_noun_adj_postposition,
	rule_for_adjunct_group,
	rule_for_noun_detached_adj_postposition, 
	rule_for_keyb,
	rule_for_disrupt_conj,
	rule_for_adv_predik,
	rule_for_whose_clause,
	rule_for_anat_compar,
	rule_for_similar_advs_comp_conj,
	rule_for_foreign_term,
	rule_for_preps_adj,
	rule_for_oborots

};



class CRusFormatCaller : public CFormatCaller
{
public:
	CRusFormatCaller(const CSyntaxOpt* Opt);
	~CRusFormatCaller(){};

	const CRusSyntaxOpt* GetOpt() const {  return (const CRusSyntaxOpt*)CFormatCaller::GetOpt(); };
	


	int		GetRuleByGroupTypeForThesaurus(int GroupType) const;
	void	AddAllRules();
	void	AddGroupRulesForClause( );
	void	AddRulesBeforeSimClauses( );
	void	AddRulesAfterSimClauses( );
	void	AddSimpleSimilarRules( );
	void	AddSimpleRulesForAnalyticalVerbFormChecking( );
	void	AddSimilarNounGroupRules(  );
    void    BuildOborotGroups();

	bool StandardGleiche (size_t i, size_t k);

	bool format_for_small_number_noun(CGroup& G);
	bool format_for_both(CGroup& G);
	

	bool format_for_similar_adjs(CGroup& G);
	bool format_for_modif_adj_groups (CGroup& G);
	bool format_for_similar_advs(CGroup& G);
	bool format_for_similar_advs_comp_conj(CGroup& G);
	bool format_for_similar_infs(CGroup& G);
	bool format_for_similar_comp_adjs(CGroup& G);
	bool format_for_similar_numerals(CGroup& G);

	bool format_for_noun_adjunct_group(CGroup& G);

	size_t search_for_similar_noun_groups (const CGroup& FirstGroup, CGroup& G);
	bool format_for_similar_nouns(CGroup& G);

	bool format_for_comp_adv (CGroup& G);
	bool format_for_adv_adj(CGroup& G);
	bool format_for_adv_number_adverb (CGroup& G);
	bool format_for_adv_adv(CGroup& G);
	bool format_for_adv_predik(CGroup& G);
	bool format_for_noun_groups(CGroup& G); 

	bool format_for_numbers(CGroup& G); 
	bool format_for_approx_noun_number(CGroup& G);
	bool format_for_odin_group(CGroup& G);
	bool format_for_num_complex (CGroup& G); 
	bool format_for_noun_num (CGroup& G); 
	bool format_for_approx_noun_prep(CGroup& G); 
	bool is_small_number_group (size_t WordNo);

	bool format_for_DT1_DT2 (CGroup& G);
	bool format_for_month_and_year (CGroup& G);
	bool format_for_standart_date (CGroup& G);
	bool format_for_year(CGroup& G);
	bool format_for_dates	 (CGroup& G); 

	bool format_for_number_adverb (CGroup& G); 
	int can_start_number_noun_group (int StartWordNo) const;
	bool format_standard_param_abbr(CGroup& G);
	bool format_for_number_noun (CGroup& G);
	bool format_for_selective_groups(CGroup& G);

	bool  format_for_rank_surname (CGroup& G);
	bool  find_db_main_group (CGroup& G);	 
	bool  format_for_preps_adj (CGroup& G);
	bool  format_for_preps (CGroup& G);
	bool  format_for_prep_IZ (CGroup& G);
	bool  format_for_gen_chains (CGroup& G);
	bool  format_for_from_compar(CGroup& G);
	bool  format_for_similar_noun_groups(CGroup& G);	
	bool  format_for_neg_verb(CGroup& G);
	bool  format_for_dir_obj(CGroup& G);
	bool  format_for_instr_obj(CGroup& G);
	
	bool  format_for_dir_obj_rev(CGroup& G);
	bool  format_for_verb_inf(CGroup& G);

	bool  format_for_partic_clause(CGroup& G); //правило построения группы с причастным оборотом в постпозиции;
	bool  format_for_whose_clause(CGroup& G); //правило построения группы с придаточным определительным;	
	bool  format_for_adj_in_commas_preposition(CGroup& G);
	bool  format_for_plural_noun_adj(CGroup& G);

	
	bool	format_for_adv_verb(CGroup& G);

	bool CanConnectSimilar(const CSynPlmLine& L) const;
	
	bool gleiche_for_small_numbers(int i_noun, int i_number, bool change_grammems, QWORD& new_group_grammems, bool& unusual_case);
	bool gleiche_noun_numeral_for_approx(int i_noun, int i_num);
	bool is_Pronoun_P_in_similar_groups(const CGroup& G);
	bool format_for_noun_adj_postposition(CGroup& G);
	bool format_for_noun_detached_adj_postposition(CGroup& G);
	
	bool format_for_anat_compar(CGroup& G);

	
	bool format_for_foreign_term(CGroup& G);
	

	bool	IsSimilarConnector(const CSynPlmLine& L) const;
	
	
protected:
	bool Is_Repeating_Disrupt_Conj(SDoubleConj& pConj);
	
};


inline bool CanNumeralBeNoun(const char* s)
{
	return !strcmp(s, "ДВОЕ") 
		|| !strcmp(s, "ТРОЕ") 
		|| !strcmp(s, "ЧЕТВЕРО") 
		|| !strcmp(s, "ПЯТЕРО") 
		|| !strcmp(s, "ШЕСТЕРО")
		|| !strcmp(s, "СЕМЕРО") 
		|| !strcmp(s, "ВОСЬМЕРО") 
		|| !strcmp(s, "ДЕВЯТЕРО")
		|| !strcmp(s, "ДЕСЯТЕРО")
		|| !strcmp(s, "ОБА");
};







#endif

