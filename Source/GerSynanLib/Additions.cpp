#include "GerSynan.h"
#include "GerFormatCaller.h"

// examples
// Bruttoinlandsproduktes (BIP)
// Biedenkopf (CDU)
// Herrmann (43)

bool CGerFormatCaller::format_for_additions (CGroup& G)
{
	const CGroup& MainGroup = get_maximal_group(G.m_iFirstWord);
	int i = get_main_word_in_group(MainGroup);
	if (!is_morph_noun(sent[i])) return false;
	int j = MainGroup.m_iLastWord+1;
	if (j == sent.size()) return false;

	if (!sent[j].is_word_upper("(")) return false;

	j++;
	if (j == sent.size()) return false;
	if (		!sent[j].HasFlag(fl_register_BB) 
			&&	!sent[j].HasFlag(fl_digit)
		) 
	return false;

	j++;
	if (j == sent.size()) return false;
	if (!sent[j].is_word_upper(")") ) return false;


	G.m_GroupType = gADDITION;
	G.m_MainGroup = MainGroup;
	G.m_iLastWord = j;
	G.SetGrammems( MainGroup.GetGrammems() );
	create_syn_rel(G, i, j-1,  gADDITION);
	return true;
};