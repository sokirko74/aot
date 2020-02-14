// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"
#include "RusClause.h"


CRusFormatCaller::CRusFormatCaller(const CSyntaxOpt* Opt) : CFormatCaller(Opt) 
{
	#define InsertRule(_rule, _member, _group_name, _direction)  FormatCalls.insert(CFormatCallMap::value_type( _rule ,  CFormatCall ((CFormatFunc)&CRusFormatCaller:: _member , _group_name, _direction )));

	InsertRule ( rule_for_numbers, format_for_numbers, "�����", FROM_LEFT);
	InsertRule ( rule_for_num_complex, format_for_num_complex,"����_����", FROM_LEFT);
	InsertRule ( rule_for_similar_numerals, format_for_similar_numerals,"�����_����", FROM_LEFT);
	InsertRule ( rule_for_noun_adjuncts, format_for_noun_adjunct_group,"����������", FROM_LEFT);
	InsertRule ( rule_for_noun_num, format_for_noun_num,"���_����", FROM_LEFT);
	InsertRule ( rule_for_names, format_for_FAM1_FAM2, "���", FROM_LEFT);
	InsertRule ( rule_for_adv_adj, format_for_adv_adj,"���_����", FROM_LEFT);
	InsertRule ( rule_for_similar_adjs, format_for_similar_adjs,"�����_����", FROM_LEFT);
	InsertRule ( rule_for_modif_adj, format_for_modif_adj_groups,"�����_����", FROM_LEFT);
	InsertRule ( rule_for_similar_advs, format_for_similar_advs,"�����_���", FROM_LEFT);
	InsertRule ( rule_for_similar_advs_comp_conj, format_for_similar_advs_comp_conj,"�����_���", FROM_LEFT);
	InsertRule ( rule_for_similar_infs, format_for_similar_infs,"�����_���", FROM_LEFT);
	InsertRule ( rule_for_similar_comp_adjs, format_for_similar_comp_adjs,"�����_����", FROM_LEFT);
	InsertRule ( rule_for_web_addr, format_for_web_addr,"��_�����", FROM_LEFT);
	InsertRule ( rule_for_comp_adv, format_for_comp_adv,"�����_�������", FROM_LEFT);
	InsertRule ( rule_for_adv_verb, format_for_adv_verb,"�����_������", FROM_LEFT);
	InsertRule ( rule_for_noun_groups, format_for_noun_groups,"����_���", FROM_LEFT);
	InsertRule ( rule_for_number_adverb, format_for_number_adverb,"���_����_���", FROM_LEFT);
	InsertRule ( rule_for_number_noun, format_for_number_noun,"����_���", FROM_LEFT);
	InsertRule ( rule_for_approx_noun_number, format_for_approx_noun_number,"�������_��", FROM_LEFT);
	 
	InsertRule ( rule_for_rank_surname, format_for_rank_surname,"����_���", FROM_LEFT);
	InsertRule ( rule_for_gen_chains, format_for_gen_chains,"�����_��", FROM_LEFT);

	InsertRule ( rule_for_from_compar, format_for_from_compar,"�������",FROM_LEFT);
	 
	InsertRule ( rule_for_preps, format_for_preps,"��", FROM_LEFT);
	InsertRule ( rule_for_prep_IZ, format_for_prep_IZ,"�� (��)", FROM_LEFT);
	 
	InsertRule ( rule_for_selective_groups, format_for_selective_groups,"�����_��", FROM_LEFT);
	InsertRule ( rule_for_similar_noun_groups, format_for_similar_noun_groups,"�����_��", FROM_LEFT);
	InsertRule ( rule_for_neg_verb,format_for_neg_verb,"���_�����", FROM_LEFT);
	InsertRule ( rule_for_dir_obj, format_for_dir_obj,"����_���", FROM_LEFT);
	InsertRule ( rule_for_instr_obj, format_for_instr_obj,"�����_���", FROM_LEFT);
	InsertRule ( rule_for_dir_obj_rev, format_for_dir_obj_rev,"����_���_���", FROM_LEFT);
	InsertRule ( rule_for_verb_inf, format_for_verb_inf,"���_����_���", FROM_LEFT);
	InsertRule ( rule_for_adv_adv, format_for_adv_adv,"���_���", FROM_LEFT);
	InsertRule ( rule_adv_number_adverb, format_for_adv_number_adverb,"���_���_����", FROM_LEFT);
	InsertRule ( rule_for_approx_prep_noun, format_for_approx_noun_prep,"�������_��", FROM_LEFT);
	InsertRule ( rule_for_partic_clause, format_for_partic_clause,"����_���",FROM_RIGHT);
	InsertRule ( rule_for_whose_clause, format_for_whose_clause,"������_���",FROM_RIGHT);
	InsertRule ( rule_for_adj_in_commas_preposition, format_for_adj_in_commas_preposition,"����-���",FROM_RIGHT);
	InsertRule ( rule_for_plural_noun_adj, format_for_plural_noun_adj,"����-���",FROM_LEFT);
	InsertRule ( rule_for_noun_adj_postposition, format_for_noun_adj_postposition,"����_�������",FROM_LEFT);
	InsertRule ( rule_for_noun_detached_adj_postposition, format_for_noun_detached_adj_postposition,"���_���_����",FROM_LEFT);
	InsertRule ( rule_for_keyb, format_for_keyb,"���",FROM_LEFT);
	InsertRule ( rule_for_disrupt_conj, format_for_disrupt_conj,"������_����",FROM_LEFT);
	InsertRule ( rule_for_adv_predik, format_for_adv_predik,"���_������", FROM_LEFT);
	InsertRule ( rule_for_anat_compar, format_for_anat_compar,"����_�����", FROM_LEFT);
	InsertRule ( rule_for_foreign_term, format_for_foreign_term,"������_����",FROM_LEFT);
	InsertRule ( rule_for_preps_adj, format_for_preps_adj,"�� (��� ��������������)", FROM_LEFT);
	InsertRule ( rule_for_oborots, format_for_oborots,"������", FROM_LEFT);
	

#undef InsertRule
};


 
int   CRusFormatCaller::GetRuleByGroupTypeForThesaurus(int GroupType) const
{
	switch (GroupType) 
	{
		case NOUN_ADJ:		return rule_for_noun_groups;
		case PREP_NOUN: 	return rule_for_preps;
		default: return -1;
	};
};



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

static void InitForSimilarAdj( CFormatCaller& FormatCaller)
{
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_adjs);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_modif_adj);
	FormatCaller.m_vectorRulesNumbers.push_back(rule_for_similar_adjs);
};

void CRusFormatCaller::AddAllRules()
{
	// ������� ������  ������   ������ �������, ������� ������������ ������ �� �����������
	m_vectorRulesNumbers.push_back(rule_for_names);
	m_vectorRulesNumbers.push_back(rule_for_web_addr);
	m_vectorRulesNumbers.push_back(rule_for_keyb);
	m_vectorRulesNumbers.push_back(rule_for_oborots);
	

	m_vectorRulesNumbers.push_back(rule_for_foreign_term);
	m_vectorRulesNumbers.push_back(rule_for_num_complex);
	m_vectorRulesNumbers.push_back(rule_for_numbers);
	m_vectorRulesNumbers.push_back(rule_for_similar_numerals);
	m_vectorRulesNumbers.push_back(rule_for_noun_num);
	
	m_vectorRulesNumbers.push_back(rule_for_anat_compar);
	m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	m_vectorRulesNumbers.push_back(rule_adv_number_adverb);
	m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	m_vectorRulesNumbers.push_back(rule_for_adv_predik);	

	InitForSimilarAdj(*this);

	m_vectorRulesNumbers.push_back(rule_for_similar_advs);
	m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	m_vectorRulesNumbers.push_back(rule_for_comp_adv);
	m_vectorRulesNumbers.push_back(rule_for_adv_verb);

	m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);
	m_vectorRulesNumbers.push_back(rule_for_approx_prep_noun);	


	m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_number_noun); // "��������� ����� 40 ������ ������������"
	m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);	
	m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_rank_surname);
	m_vectorRulesNumbers.push_back(rule_for_gen_chains);
	m_vectorRulesNumbers.push_back(rule_for_from_compar);
	m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	m_vectorRulesNumbers.push_back(rule_for_selective_groups);
	m_vectorRulesNumbers.push_back(rule_for_preps);
	m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups); //������������ ��� �������� ���� "���� � ���� � ����"
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj); // ����� ������� "������ ���� � ����"
	m_vectorRulesNumbers.push_back(rule_for_neg_verb);
	m_vectorRulesNumbers.push_back(rule_for_dir_obj);
	m_vectorRulesNumbers.push_back(rule_for_instr_obj);
	
	m_vectorRulesNumbers.push_back(rule_for_dir_obj_rev);
	m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	m_vectorRulesNumbers.push_back(rule_for_verb_inf);
	m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); 
	m_vectorRulesNumbers.push_back(rule_for_preps_adj); 

	m_vectorRulesNumbers.push_back(rule_for_noun_adjuncts);

	//  ��� ��� �������� ������� rule_for_gen_chains ��� ����� "������������� ������ � ����� �������"
	m_vectorRulesNumbers.push_back(rule_for_gen_chains);

	//  ��� ��� �������� ������� rule_for_verb_inf ��� ����� "�� ����� ��������� ������"
	m_vectorRulesNumbers.push_back(rule_for_verb_inf);

	//  ��� �����������   "�� ������� � ���, � ������."
	m_vectorRulesNumbers.push_back(rule_for_dir_obj);
}

void CRusFormatCaller::AddRulesBeforeSimClauses( )
{
	// ������� ������  ������   ������ �������, ������� ������������ ������ �� �����������
	m_vectorRulesNumbers.push_back(rule_for_names);
	m_vectorRulesNumbers.push_back(rule_for_web_addr);
	m_vectorRulesNumbers.push_back(rule_for_keyb);	
	m_vectorRulesNumbers.push_back(rule_for_oborots);
	m_vectorRulesNumbers.push_back(rule_for_foreign_term);

	
	m_vectorRulesNumbers.push_back(rule_for_num_complex);
	m_vectorRulesNumbers.push_back(rule_for_numbers);
	m_vectorRulesNumbers.push_back(rule_for_similar_numerals);	
	m_vectorRulesNumbers.push_back(rule_for_noun_num);
	
	m_vectorRulesNumbers.push_back(rule_for_anat_compar);
	m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	m_vectorRulesNumbers.push_back(rule_adv_number_adverb);
	m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	m_vectorRulesNumbers.push_back(rule_for_adv_predik);	

	InitForSimilarAdj(*this);

	m_vectorRulesNumbers.push_back(rule_for_similar_advs);
	m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	m_vectorRulesNumbers.push_back(rule_for_comp_adv);
	m_vectorRulesNumbers.push_back(rule_for_adv_verb);

	m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_approx_noun_number);
	m_vectorRulesNumbers.push_back(rule_for_approx_prep_noun);	

	m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_number_noun); // "��������� ����� 40 ������ ������������"
	m_vectorRulesNumbers.push_back(rule_for_number_adverb);/**/
	m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_rank_surname);
	m_vectorRulesNumbers.push_back(rule_for_gen_chains);	
	m_vectorRulesNumbers.push_back(rule_for_from_compar);	
	m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	m_vectorRulesNumbers.push_back(rule_for_selective_groups);	
	m_vectorRulesNumbers.push_back(rule_for_preps);
}

void CRusFormatCaller::AddRulesAfterSimClauses( )
{
	m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups); //������������ ��� �������� ���� "���� � ���� � ����"
	m_vectorRulesNumbers.push_back(rule_for_neg_verb);
	m_vectorRulesNumbers.push_back(rule_for_dir_obj);
	m_vectorRulesNumbers.push_back(rule_for_instr_obj);
	
	m_vectorRulesNumbers.push_back(rule_for_dir_obj_rev);
	m_vectorRulesNumbers.push_back(rule_for_verb_inf);
	m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); //&&&
}

void CRusFormatCaller::AddSimpleSimilarRules( )
{
	m_vectorRulesNumbers.push_back(rule_for_oborots);
	m_vectorRulesNumbers.push_back(rule_for_similar_numerals);
	m_vectorRulesNumbers.push_back(rule_for_adv_adj);
	m_vectorRulesNumbers.push_back(rule_for_adv_adv);
	m_vectorRulesNumbers.push_back(rule_for_adv_predik);
	m_vectorRulesNumbers.push_back(rule_for_adv_adj);

	InitForSimilarAdj(*this);

	m_vectorRulesNumbers.push_back(rule_for_similar_advs_comp_conj);
	m_vectorRulesNumbers.push_back(rule_for_similar_infs);
	m_vectorRulesNumbers.push_back(rule_for_similar_comp_adjs);
	m_vectorRulesNumbers.push_back(rule_for_noun_groups);
}

void CRusFormatCaller::AddGroupRulesForClause( )
{
	m_vectorRulesNumbers.push_back(rule_for_partic_clause);
	m_vectorRulesNumbers.push_back(rule_for_whose_clause);
	m_vectorRulesNumbers.push_back(rule_for_adj_in_commas_preposition);
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition); //???
	m_vectorRulesNumbers.push_back(rule_for_disrupt_conj); //&&&
}

void CRusFormatCaller::AddSimilarNounGroupRules(  )
{	
	m_vectorRulesNumbers.push_back(rule_for_oborots);
	m_vectorRulesNumbers.push_back(rule_for_noun_num);
    m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	m_vectorRulesNumbers.push_back(rule_for_noun_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_noun_detached_adj_postposition);
	m_vectorRulesNumbers.push_back(rule_for_similar_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_similar_advs);
}

void CRusFormatCaller::AddSimpleRulesForAnalyticalVerbFormChecking( )
{
	m_vectorRulesNumbers.push_back(rule_for_oborots);
	m_vectorRulesNumbers.push_back(rule_for_noun_num);
    m_vectorRulesNumbers.push_back(rule_for_number_noun);
	m_vectorRulesNumbers.push_back(rule_for_noun_groups);
	m_vectorRulesNumbers.push_back(rule_for_plural_noun_adj);
	m_vectorRulesNumbers.push_back(rule_for_prep_IZ);
	m_vectorRulesNumbers.push_back(rule_for_selective_groups);	
	m_vectorRulesNumbers.push_back(rule_for_preps);
}



bool CRusFormatCaller::IsSimilarConnector(const CSynPlmLine& L) const
{
    if (L.HasFlag(fl_comma) ) return true;
    if (!L.HasPOS(CONJ)) return false;
    int conj_no = L.get_coord_conj_no();
	return		(conj_no!= -1)  &&	!GetOpt()->GetOborDic()->GetCoordConjs()[conj_no].should_be_after_comma;
		

};
void CRusFormatCaller::BuildOborotGroups() 
{
    CIntVector save = m_vectorRulesNumbers;
    m_vectorRulesNumbers.clear();
    m_vectorRulesNumbers.push_back(rule_for_oborots);
    main_analyse();
    swap (m_vectorRulesNumbers,  save);

}


