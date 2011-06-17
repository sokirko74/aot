// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


bool is_two_conj_without_comma_in_group(const CRusFormatCaller& C, CGroup& G)
{
	int iconj = 0;

	for ( int i = G.m_iFirstWord; i <= G.m_iLastWord; i++ )
	{
		if (C.sent[i].HasFlag(fl_comma)) return false;
        int conj_no = C.sent[i].get_coord_conj_no();
		if( conj_no !=  -1)  
			if( !C.GetOpt()->m_pOborDic->GetCoordConjs()[conj_no].should_be_after_comma )
				iconj++;
	}

	if (iconj >= 2) return true;
	
	return false;
}

/*
Примеры:
  много и мало
  меньше или больше
*/


bool CRusFormatCaller::format_for_similar_advs(CGroup& G)
{

	int i = get_main_word (G.m_iFirstWord);
	if( !is_morph_adv(Wi) )
		return false;

	G.m_bCommaDelimited = true;
	bool bInterrogative = Wi.has_grammem(rInterrogative);
	bool bDemonstrative = Wi.has_grammem(rDemonstrative);
	int last_member = i;
	

	for (int k = get_next_main_word(i); k != sent.size(); k = get_next_main_word(k))
	{
		if( !CanConnectSimilar(Wk))	break;	
		bool bLastConj  = !Wk.HasFlag(fl_comma);
		if (bLastConj)
			G.m_bCommaDelimited = false;

		k = get_next_main_word(k);
		if (k == sent.size()) break;

		if( !is_morph_adv(Wk) ) break;
		if (bInterrogative != Wk.has_grammem(rInterrogative)) break;
		if (bDemonstrative != Wk.has_grammem(rDemonstrative)) break;

		last_member = k;
		if (bLastConj)  break;
	}

	if ( last_member == i ) return false;

	G.m_GroupType = SIMILAR_ADVS;
	G.m_MainGroup = get_maximal_group(G.m_iFirstWord);	 
	G.m_iLastWord = get_maximal_group(last_member).m_iLastWord;
	return true;

};

//правило вызывается только при первичном разбиении на клаузы;
//правило строит только те однородные ряды, в которых есть соч. союзы;  
bool CRusFormatCaller::format_for_similar_advs_comp_conj(CGroup& G) 
{
	if (format_for_similar_advs(G))
	{
		if ( G.m_bCommaDelimited ) return false;
		return true;
	}
	return false;
}

