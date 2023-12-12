// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma once

#include "../StdSynan.h"
#include "../RusGroup.h"
#include "../RusSyntaxOpt.h"
#include "synan/SynCommonLib/FormatCaller.h"




class CRusFormatCaller : public CFormatCaller
{
	bool gleiche_for_big_numbers(int i_noun, int i_number, uint64_t& new_group_grammems);
	int can_start_number_noun_group(int StartWordNo) const;
	bool format_for_number_noun_private(CGroup& G);
	bool format_for_noun_number_private(CGroup& G);
public:
	CRusFormatCaller(const CSyntaxOpt* Opt);
	~CRusFormatCaller(){};

	const CRusSyntaxOpt* GetOpt() const {  return (const CRusSyntaxOpt*)CFormatCaller::GetOpt(); };
    const CRusGramTab* GetRusGramTab() const {  return (const CRusGramTab*)GetOpt()->GetGramTab(); };


	int		GetRuleByGroupTypeForThesaurus(int GroupType) const;
	void	AddAllRules();
	void	AddGroupRulesForClause( );
	void	AddRulesBeforeSimClauses( );
	void	AddRulesAfterSimClauses( );
	void	AddSimpleSimilarRules( );
	void	AddSimpleRulesForAnalyticalVerbFormChecking( );
	void	AddSimilarNounGroupRules(  );
    void    BuildOborotGroups();
	
	
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

	bool format_for_comp_adv (CGroup& G);
	bool format_for_adv_adj(CGroup& G);
	bool format_for_adv_number_adverb (CGroup& G);
	bool format_for_adv_adv(CGroup& G);
	bool format_for_adv_predik(CGroup& G);
	bool format_for_noun_groups(CGroup& G); 

	bool format_for_numbers(CGroup& G); 
	bool format_for_odin_group(CGroup& G);
	bool format_for_num_complex (CGroup& G); 
	bool format_for_noun_num (CGroup& G); 
	bool format_for_approx_noun_prep(CGroup& G); 
	bool is_small_number_group (size_t WordNo);

	bool format_for_number_adverb (CGroup& G);
	bool format_for_number_noun (CGroup& G);
	bool format_for_selective_groups(CGroup& G);

	bool  format_for_rank_surname (CGroup& G);
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
	
	bool gleiche_for_plural_numbers(int i_noun, int i_number, uint64_t& new_group_grammems, bool small_number);
	bool gleiche_noun_numeral_for_approx(int i_noun, int i_num);
	bool is_Pronoun_P_in_similar_groups(const CGroup& G);
	bool format_for_noun_adj_postposition(CGroup& G);
	bool format_for_noun_detached_adj_postposition(CGroup& G);
	
	bool format_for_anat_compar(CGroup& G);

	
	bool format_for_foreign_term(CGroup& G);
	

	bool	IsSimilarConnector(const CSynPlmLine& L) const;
	
	
};


