#include "GerSynan.h"
#include "GerFormatCaller.h"


// Kanzler Gerhard Schröder
// Bundeskanzler Alexey Sokirko
// A-Techniker Gerhard Schroder
bool CGerFormatCaller :: format_for_rank_fam (CGroup& G)
{
	int i =  get_main_word(G.m_iFirstWord);
	if (!Wi.m_lemma) return false;
	if ( !Wi.HasFlag(fl_ranknoun) ) return false;
	
	size_t j = 0;
	const CGroup& main_gr = get_maximal_group(G.m_iFirstWord);
	j = main_gr.m_iLastWord+1;

	if ( j == sent.size() ) return false;

	const CGroup& next_gr =  get_maximal_group(j);

	if (	next_gr.m_GroupType!= gFAMILIE_GROUP 
		&&	next_gr.m_GroupType!= gSPNAME
		&&	next_gr.m_GroupType!= gPNAME
		)	  return false;

	G.m_iLastWord = next_gr.m_iLastWord;
	G.SetGrammems(  Wi.GetGrammems() );
	G.m_MainGroup = main_gr;
    G.m_GroupType = gBERUF_NP;
	create_syn_rel(G,i,j,gBERUF_NP);
	
	return true;
};

