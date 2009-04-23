// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"









bool CRusFormatCaller :: format_for_rank_surname (CGroup& G)
{
    const CGroup& main_gr = get_maximal_group(G.m_iFirstWord);
    int i =  main_gr.m_MainWordNo;
	if (!Wi.get_lemma()) return false;
	if ( !Wi.HasFlag(fl_ranknoun) ) return false;
	
	size_t j = main_gr.m_iLastWord+1;
	if ( j == sent.size() ) return false;

	j = get_main_word(j);

	if ( !Wj.has_grammem( rSurName ) && !Wj.has_grammem( rName ) ) return false;

	if ( !GetGramTab()->GleicheCaseNumber(Wi.m_gramcodes, Wj.m_gramcodes) ) return false;

	G.m_iLastWord = get_maximal_group(j).m_iLastWord;
	G.SetGrammems( Wi.GetGrammems() );
	G.m_MainGroup = main_gr;
    G.m_GroupType = RANK_NAMES;
	G.m_Cause = "Правила сборки Должность+ФИО";   
	create_syn_rel(G,i,j,RANK_NAMES);
	
	return true;
};


bool CRusFormatCaller :: format_for_adj_in_commas_preposition(CGroup& G)
{
	int i = get_main_word (G.m_iFirstWord);
	if (i != G.m_iFirstWord) return false;
	if (!is_morph_pronoun(Wi)) return false;
	if (i < 2) return false; 
	if (!sent[i-1].HasFlag(fl_comma) ) return false;
	size_t j = i-2;
	const CGroup& MaxGrp = get_maximal_group(j);	

	if (MaxGrp.m_iLastWord == j && MaxGrp.m_iFirstWord == j)
		j = j;
	else
		j = MaxGrp.m_MainWordNo;

	if (!is_morph_adj(Wj)) return false;

	bool vB = false;
	if (MaxGrp.m_iLastWord == 0)
		vB = true;
	else
	 if (MaxGrp.m_iFirstWord >0 && sent[MaxGrp.m_iFirstWord-1].HasFlag(fl_comma)) 
		 vB = true;
	
	if ( GetGramTab()->GleicheGenderNumberCase(Wi.m_type_gram_code, Wi.m_gramcodes, Wj.m_gramcodes) )
	{
		G.m_GroupType = NOUN_ADJ;
		G.m_iLastWord = i;
		G.m_iFirstWord = MaxGrp.m_iFirstWord;
		G.m_MainGroup.m_iFirstWord =  i;
		G.m_MainGroup.m_iLastWord =  i;
		G.SetGrammems( Wi.GetGrammems() );
		return true;
	}
	return false;
};

// Правила для "черная и белая ракетки; черные шкаф и стул"
bool CRusFormatCaller::format_for_plural_noun_adj(CGroup& G)
{
	const CGroup& AdjMaxGrp = get_maximal_group(G.m_iFirstWord);
	size_t adj_main_word = get_main_word_in_group(AdjMaxGrp);
	const CSynPlmLine& Adj = sent[adj_main_word];

	
	size_t noun_main_word = get_next_main_word(adj_main_word);
	if (noun_main_word == sent.size()) return false;
	const CSynPlmLine& Noun = sent[noun_main_word];
	const CGroup& NounMaxGrp = get_maximal_group(noun_main_word);

	if (!Adj.m_gramcodes || !Noun.m_gramcodes) return false; 

	bool bCase1 = 
		(		(AdjMaxGrp.m_GroupType == SIMILAR_ADJS)
			&&	(Noun.GetGrammems() & _QM(rPlural)) 
			&&  Noun.is_morph_noun( ) 
			&&  (NounMaxGrp.size() == 1)
			&&	GetGramTab()->GleicheCase(Adj.m_gramcodes, Noun.m_gramcodes)
		);

	bool bCase2 = 
		(
				(NounMaxGrp.m_GroupType == SIMILAR_NOUN_GROUPS)
			&&	Noun.is_morph_noun( ) 
			&&	(Adj.GetGrammems() & _QM(rPlural)) 
			&&	(Noun.GetGrammems() & _QM(rSingular))
			&&	AdjMaxGrp.size() == 1
			&&	is_morph_adj(Adj)
			&&	GetGramTab()->GleicheCase(Adj.m_gramcodes,Noun.m_gramcodes)
		);
		
	if (bCase1 || bCase2)
	{
		G.m_GroupType = NOUN_ADJ;
		G.m_iFirstWord = AdjMaxGrp.m_iFirstWord;
		G.m_iLastWord = NounMaxGrp.m_iLastWord;
		G.m_MainGroup = NounMaxGrp;
		G.SetGrammems( NounMaxGrp.GetGrammems() );
		create_syn_rel(G, noun_main_word, adj_main_word, NOUN_ADJ);
		return true;
	}
	
	return false;		
};

// Правила для \\Человек этот, неожиданно увидев нечто впечатляющее, отпрянул, но ничего страшного не произошло\\.
bool CRusFormatCaller::format_for_noun_adj_postposition(CGroup& G)
{
	int i = get_main_word (G.m_iFirstWord);
	if (!Wi.is_morph_noun()) return false;
	if (Wi.m_UnitType != EWord) return false;

	int j = get_next_main_word(i);
	if (j == sent.size()) return false;
	if (!is_morph_adj(Wj)) return false;
	if (Wj.m_UnitType != EWord) return false;
	
	if (! GetGramTab()->GleicheGenderNumberCase(Wi.m_type_gram_code, Wi.m_gramcodes, Wj.m_gramcodes) ) return false;

	G.m_GroupType = NOUN_ADJ_POSTPOSITION;
	G.m_iLastWord = get_maximal_group(j).m_iLastWord;
	G.m_MainGroup  = get_maximal_group(i);
	G.SetGrammems( Wi.GetGrammems() );
	create_syn_rel(G, i, j, NOUN_ADJ_POSTPOSITION);
	return true;		
};

// Правила для обособ. прил. в постпозиции\\Этим вопросом, давно и надолго забытым, \\.
bool CRusFormatCaller::format_for_noun_detached_adj_postposition(CGroup& G)
{
	size_t i, j;
	i = G.m_iFirstWord;
	const CGroup& MaxGrp = get_maximal_group(i);
	if (MaxGrp.m_iFirstWord != MaxGrp.m_iLastWord)
		i = MaxGrp.m_MainWordNo;
	else
		i = MaxGrp.m_iFirstWord;
	if ( !Wi.is_morph_noun() && !is_morph_pronoun(Wi)) return false;
	j = MaxGrp.m_iLastWord + 1;
	if (j == sent.size()) return false;
	if (!Wj.HasFlag(fl_comma)) return false;
	j++;
	if (j == sent.size()) return false;
	const CGroup& MaxGrpAdj = get_maximal_group(j);
	if (MaxGrpAdj.m_iFirstWord != MaxGrpAdj.m_iLastWord)
		j = MaxGrpAdj.m_MainWordNo;
	else
		j = MaxGrpAdj.m_iFirstWord;
	if (!is_morph_adj(Wj)) return false;
	if (    !(MaxGrpAdj.m_iLastWord == sent.size()-1) 
		&&  !sent[MaxGrpAdj.m_iLastWord+1].HasFlag(fl_comma)
	   )
		return false;
	bool bGr = false;
	if (  GetGramTab()->GleicheGenderNumberCase(Wi.m_type_gram_code, Wi.m_gramcodes,Wj.m_gramcodes) )
		bGr = true;

	if ((Wi.GetGrammems() & (1 <<rPlural)) && (Wj.GetGrammems() & (1 <<rSingular)))
		if ( GetGramTab()->GleicheCase(Wi.m_gramcodes, Wj.m_gramcodes))
			bGr = true;

	if ((Wi.GetGrammems() & (1 <<rSingular)) && (Wj.GetGrammems() & (1 <<rPlural)))
		if (GetGramTab()->GleicheCase(Wi.m_gramcodes, Wj.m_gramcodes))
			bGr = true;

	if (bGr == true)
	{
		G.m_GroupType = NOUN_DETACHED_ADJ;
		G.m_iFirstWord = MaxGrp.m_iFirstWord;
		G.m_iLastWord = MaxGrpAdj.m_iLastWord;
		G.m_MainGroup = MaxGrp;
		G.SetGrammems( Wi.GetGrammems() );
		create_syn_rel(G, i, j, NOUN_DETACHED_ADJ);
		return true;
	}
		
	return false;
};

// Правила для БОЛЕЕ или МЕНЕЕ + прил или кр.прил
bool CRusFormatCaller::format_for_anat_compar(CGroup& G)
{
	//более  извращенная  любительница
	size_t i = G.m_iFirstWord;
	const CGroup& MaxGrp = get_maximal_group(i);
	if (MaxGrp.m_iFirstWord != MaxGrp.m_iLastWord) return false;

    if (   !Wi.is_word_upper("БОЛЕЕ") 
		&& !Wi.is_word_upper("МЕНЕЕ") 
	   ) return false;
	size_t j = i + 1;
	if (j >= sent.size()) return false;
	if (get_maximal_group_size(j) != 1) return false;
	if (is_morph_adj(Wj) || is_morph_participle (Wj))
	{
		G.m_GroupType = ANAT_COMPAR;
		G.m_iLastWord = j;
		G.m_MainGroup.m_iFirstWord = j;
		G.m_MainGroup.m_iLastWord = j;
		G.SetGrammems( Wj.GetGrammems() );
		create_syn_rel(G, j, i, ANAT_COMPAR);
		return true;		
	}

	return false;		
};
