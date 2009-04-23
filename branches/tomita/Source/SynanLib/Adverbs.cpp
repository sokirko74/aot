// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"

// Примеры:
//    гораздо умнее
//    значительно круче и сильнее 

bool CRusFormatCaller::format_for_comp_adv (CGroup& G)
{
	G.m_iLastWord  = G.m_iFirstWord;

	if ( G.m_iFirstWord >= sent.size() - 1)  return false;

    if ( !has_item( GetOpt()->m_pCompAdvList, W1.get_word()) ) return false;

	int I = get_minimal_group(G.m_iFirstWord+1);

	if   (I == -1)
       if (        W2.has_grammem(rComparative)
            && (is_morph_adj(W2) || W2.HasPOS(ADV))
           )
			{   G.m_iLastWord++;

	            G.m_MainGroup = G.m_iLastWord;
				create_syn_rel(G, G.m_iFirstWord, G.m_iLastWord, COMP_ADV);				
			    G.m_Cause = " Аналитическая форма сравнительной степени прил. или наречия.";}
	   else;
    else
      if  (    (GetGroups()[I].m_GroupType == SIMILAR_ADJS)
		    && sent[GetGroups()[I].m_MainGroup.m_iFirstWord].has_grammem(rComparative))
	  {   
		  G.m_iLastWord = GetGroups()[I].m_iLastWord;
	      G.m_MainGroup = GetGroups()[I].m_MainGroup.m_iFirstWord;
		  create_syn_rel(G, G.m_iFirstWord,get_main_word_in_group(GetGroups()[I]), COMP_ADV);
		  G.m_Cause = " Аналитическая форма сравнительной степени соч. группы прил.";
	  }



   G.m_GroupType = COMP_ADV;
   return G.m_iFirstWord != G.m_iLastWord;
}

bool IsUZH_Particle (const CSynPlmLine& W)
{
    return W.is_lemma("УЖ") && W.HasPOS(PARTICLE);
};


// Примеры:
//     очень хороший
//	 весьма близкий и родной (ПРИЛАГАТЕЛЬНОЕ)
//     абсолютно и относительно непознаваемый (ПРИЧАСТИЕ)
//     весьма далеко  (НАРЕЧИЕ)
//     особенно интересно (ПРЕДИКАТИВ)

bool CRusFormatCaller::format_for_adv_adj(CGroup& G)
{
	try 
	{
		int i = get_main_word (G.m_iFirstWord);
		if (IsUZH_Particle(Wi))
		{
			i = get_next_main_word (i);
			if (i == sent.size()) return false;
		}
        if (!Wi.is_adv_adj()) return false;
		if (!Wi.can_syn_depend_on_adj() ) return false;

		int j = get_next_main_word (i);
		if (j == sent.size()) return false;

		//  проходим частицу, если она встретилась
		if (GetGramTab()->IsSimpleParticle(Wj.get_lemma(), Wj.GetPoses()))
			j = get_next_main_word (j);
		if (j == sent.size()) return false;

		if( Wj.has_grammem(rComparative) )	 return false;

		if ( !is_morph_adj(Wj) && !is_morph_participle(Wj)  )
			return false;


		G.m_Cause = "Слова степени (типа \"очень\") с группой прилагательных или причастий";
		G.m_GroupType = ADV_ADJ;

		G.m_MainGroup = get_maximal_group(j);
		G.m_iLastWord = G.m_MainGroup.m_iLastWord;
		for (size_t k=get_main_word(G.m_iFirstWord); k != j; k  = get_next_main_word(k))
			create_syn_rel(G, j, k, ADV_ADJ);
		return true;
	}
	catch (...)
	{
		throw CExpc ("CRusFormatCaller::format_for_adv_adj exception");
	};
};

//примеры
//очень много
//очень многие
//уж очень многие
//очень же многие
bool CRusFormatCaller::format_for_adv_number_adverb (CGroup& G)
{
	int i = get_main_word (G.m_iFirstWord);

	if (IsUZH_Particle(Wi))
		i = get_next_main_word (i);

	if (i == sent.size()) return false;

	if( !is_morph_adv(Wi) )
		return false;

	if( !Wi.can_syn_depend_on_adv() )
		return false;

	int j = get_next_main_word (i);
	if (j == sent.size()) return false;
	//  проходим частицу, если она встретилась
	if (GetGramTab()->IsSimpleParticle(Wj.get_lemma(), Wj.GetPoses()))
		j = get_next_main_word (j);
	if (j == sent.size()) return false;

	if (!has_item (GetOpt()->m_pNumberAdverbsList, Wj.get_lemma())) 
		return false;

	G.m_GroupType = ADV_NUM_ADV;

	G.m_MainGroup = get_maximal_group(j);
	G.m_iLastWord = G.m_MainGroup.m_iLastWord;
	for (size_t k=get_main_word(G.m_iFirstWord); k != j; k  = get_next_main_word(k))
		create_syn_rel(G, j, k, ADV_NUM_ADV);

	return true;
}


//Примеры:
//	очень хорошо, весьма сносно, "уж очень хорошо", "очень уж  хорошо"
//  но не надо строить группу НАР_НАР в "Слишком уж"
bool CRusFormatCaller::format_for_adv_adv(CGroup& G)
{
	int i = get_main_word (G.m_iFirstWord);

	if ( IsUZH_Particle(Wi) )
		i = get_next_main_word (i);

	if (i == sent.size()) return false;
		
	if( !is_morph_adv(Wi) )
		return false;

    if( Wi.has_grammem(rInterrogative))   //*is_word_upper("КАК") )
		return false;

	if( !Wi.can_syn_depend_on_adv() )
		return false;
	int j = get_next_main_word (i);
	if (j == sent.size()) return false;
	//  проходим частицу, если она встретилась
	if (GetGramTab()->IsSimpleParticle(Wj.get_lemma(), Wj.GetPoses()))
		j = get_next_main_word (j);
	if (j == sent.size()) return false;

	if( !is_morph_adv(Wj) )
		return false;

	if (Wj.is_lemma("УЖ"))
		return false;

     if( Wj.has_grammem(rInterrogative))   
		return false;

	G.m_GroupType = ADV_ADV;	
	G.m_MainGroup = get_maximal_group(j);
	G.m_iLastWord = G.m_MainGroup.m_iLastWord;
	for (size_t k=get_main_word(G.m_iFirstWord); k != j; k  = get_next_main_word(k))
		create_syn_rel(G, j, k, ADV_ADV);
	return true;
}

//правило для наречия с предикативом: "очень интересно";
bool CRusFormatCaller::format_for_adv_predik(CGroup& G)
{
	int i = get_main_word (G.m_iFirstWord);

	if ( IsUZH_Particle(Wi) )
		i = get_next_main_word (i);
	
	if (i == sent.size()) return false;

	if( !is_morph_adv(Wi) )
		return false;

	int j = get_next_main_word (i);
	if (j == sent.size()) return false;

	//  проходим частицу, если она встретилась
	if (GetGramTab()->IsSimpleParticle(Wj.get_lemma(), Wj.GetPoses()))
		j = get_next_main_word (j);
	if (j == sent.size()) return false;

	if ( ( !is_morph_predk(Wj) ) ||	(Wj.m_UnitType == EClause))
		return false;

	G.m_GroupType = ADV_PREDIK;	
	G.m_MainGroup = get_maximal_group(j);
	G.m_iLastWord = G.m_MainGroup.m_iLastWord;
	for (size_t k=get_main_word(G.m_iFirstWord); k != j; k  = get_next_main_word(k))
		create_syn_rel(G, j, k, ADV_PREDIK);
	return true;
}


// Примеры:
//		хорошо жить
//		хорошо или плохо жить
//		хорошо петь и плясать
//		хорошо или плохо жить и плясать

bool CRusFormatCaller::format_for_adv_verb(CGroup& G)
{
	const CGroup& G1 = get_maximal_group(G.m_iFirstWord);
	if (G1.m_iLastWord+1 >= sent.size()) return false;
	const CGroup& G2 = get_maximal_group(G1.m_iLastWord+1);
    
    const CSynPlmLine& ww = sent[G1.m_MainWordNo];
	if( GetOpt()->m_pOborDic->FindSubConj (ww.get_lemma() ) != -1 )
		return false;

	if (G1.m_GroupType == OBOROTS) return false;

	
	if  (!is_morph_adv(sent[G1.m_MainWordNo])  ) return false;
	if    (!is_verb_form(sent[G2.m_MainWordNo]) || sent[G2.m_MainWordNo].m_UnitType == EClause)  return false;
	G.m_iLastWord = G2.m_iLastWord;
	G.m_GroupType = ADV_VERB;
    create_syn_rel(G, G2.m_MainWordNo, G1.m_MainWordNo, ADV_VERB);
	G.m_MainGroup =  G2;
	return true;
}

