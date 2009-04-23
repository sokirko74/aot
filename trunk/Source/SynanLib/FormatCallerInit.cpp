// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "FormatCallerInit.h"


	/*
		������� InitForSimilarAdj �������������� ������� ������ ���  ����������  ��������������.
		�������� rule_for_similar_adjs ����������� ������. �������  ��������  ������ ���������� ��������������,
		������� �� �������� � �������� ��������������� ������  ����� "�����" � "�����" .
		��������, ��� ����� 
			"��������, ����� �������� � �������"
		����� ��������� ������ "�������� � �������",� "��������, �����" ��������� �� �����.
		����� ����������� ������� �����_����, ������� ������ ��� �������������� ������� ������ 
			"����� �������� � �������"
		����� ��� ��� ����������� ������� �����_����, ������� ����������� (������)  ��� ��� �����_����, ���������
		����� "�����" � "�����" ��� ����� � ������ � ����� �� ���������  ������. ���������� �����:
			(�������� � (����� (�������� � �������)))
		���������� ���� ������� ���������� ������ ������� � ���, ��� "�����" � "�����" � ���������� ���� ������ ���������������� �� ��� 
		������ ����� ���� �����. ������ �� ����� �� ������� ���������, ���
				"�������, �������� � ����� �������� � �������"
			�������� ��� 			( (�������, ��������) � (����� (�������� � �������)) ),

			� ����� "��������, ����� �������� � ������ �������"
			�������� ���  (��������, (����� (�������� � (������ �������))) ).

		��� ��� ������, ������� ���� ���������.	
	*/

void InitForSimilarAdj( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_adjs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_modif_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_adjs);
};

void InitAllRules( CFormatCaller& FormatCaller)
{
	// ������� ������  ������   ������ �������, ������� ������������ ������ �� �����������
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_names);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_web_addr);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_keyb);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_foreign_term);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_numbers);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_num_complex);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_numerals);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_num);
	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_anat_compar);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_adv_number_adverb);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_predik);	

	InitForSimilarAdj(FormatCaller);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_advs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_comp_adv);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_verb);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_approx_prep_noun);	

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_rank_surname);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_gen_chains);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_from_compar);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_selective_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_preps);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups); //������������ ��� �������� ���� "���� � ���� � ����"
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_neg_verb);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_dir_obj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_verb_inf);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); 
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_preps_adj); 

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_adjuncts);

	//  ��� ��� �������� ������� rule_for_gen_chains ��� ����� "������������� ������ � ����� �������"
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_gen_chains);

	//  ��� ��� �������� ������� rule_for_verb_inf ��� ����� "�� ����� ��������� ������"
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_verb_inf);
}

void InitRulesBeforeSimClauses( CFormatCaller& FormatCaller)
{
	// ������� ������  ������   ������ �������, ������� ������������ ������ �� �����������
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_names);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_web_addr);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_keyb);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_foreign_term);

	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_numbers);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_num_complex);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_numerals);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_num);
	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_anat_compar);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_adv_number_adverb);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_predik);	

	InitForSimilarAdj(FormatCaller);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_advs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_comp_adv);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_verb);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_approx_prep_noun);	

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_rank_surname);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_gen_chains);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_from_compar);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_selective_groups);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_preps);
}

void InitRulesAfterSimClauses( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups); //������������ ��� �������� ���� "���� � ���� � ����"
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_neg_verb);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_dir_obj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_verb_inf);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); //&&&
}

void InitSimpleSimilarRules( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_numerals);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_predik);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adv_adj);

	InitForSimilarAdj(FormatCaller);

	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_advs_comp_conj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_groups);
}

void InitGroupRulesForClause( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_partic_clause);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_whose_clause);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_adj_in_commas_preposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition); //???
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); //&&&
}

void InitSimilarNounGroupRules( CFormatCaller& FormatCaller )
{	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_num);
    FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_advs);
}

void InitSimpleRulesForAnalyticalVerbFormChecking( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_num);
    FormatCaller.m_vectorRulesNumbers.push_back(rule_for_number_noun);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_selective_groups);	
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_preps);
}
