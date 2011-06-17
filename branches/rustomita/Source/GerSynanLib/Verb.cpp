#include "GerSynan.h"
#include "GerFormatCaller.h"

// Examples:
//		neulich ausgebrannt // "neulich" is an adverb 
//		gut machen	// "gut" is an adjective

// but not 
// *sehr machen 
// *wann machen

bool CGerFormatCaller::format_for_verb_modif(CGroup& G)
{
	const CGroup& G1 = get_maximal_group(G.m_iFirstWord);
	if (G1.m_iLastWord+1 >= sent.size()) return false;
	const CGroup& G2 = get_maximal_group(G1.m_iLastWord+1);

	const CSynPlmLine& ww = sent[get_main_word(G.m_iFirstWord)];

	// to prohibit "wann machen" as a group
	if( GetOpt()->m_pOborDic->FindSubConj (ww.m_lemma ) != -1 )
		return false;

	// to prohibit "sehr machen"  as a group
	if( CanBeAdjModifier (ww ))
		return false;


	int i = get_main_word(G1.m_iFirstWord);
	if  (		!is_morph_adv(sent[i]) 
			&&	!	(		(sent[i].GetPoses() & (1<<gADJ))
						&&	(sent[i].has_grammem(gPredikBenutz))
					)
		) return false;
	int j = get_main_word(G2.m_iFirstWord);
	if    (!is_verb_form(sent[j]) || sent[j].m_UnitType == EClause)  return false;
	G.m_iLastWord = G2.m_iLastWord;
	G.m_GroupType = gVERB_MODIF;
	create_syn_rel(G, j, i,  gVERB_MODIF);
	G.m_MainGroup =  G2;
	return true;
}
