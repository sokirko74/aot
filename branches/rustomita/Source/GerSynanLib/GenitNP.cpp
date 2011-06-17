#include "GerSynan.h"
#include "GerFormatCaller.h"


// example 
// die Eroberung Bagdads
// der Norden des Irak
// We use here Constraint that if the Genitivattribut has other cases except rGenitiv,
//  then Genitivattribut should have a determiner or an adjective. 
// The  same rule is used in Project ROSA (Robuste SprachAnalyse)

bool CGerFormatCaller::format_for_genit_NP (CGroup& G)
{
	const CGroup& MainGroup = get_maximal_group(G.m_iFirstWord);
	int i = get_main_word_in_group(MainGroup);
	if (!is_morph_noun(sent[i])) return false;

	if (MainGroup.m_iLastWord+1 == sent.size()) return false;

	const CGroup& GenitGroup = get_maximal_group(MainGroup.m_iLastWord+1);
	int j = get_main_word_in_group(GenitGroup);
	//if ( (GenitGroup.GetGrammems() & gAllCases) != _QM(gGenitiv) ) return false;
	if ( !(GenitGroup.GetGrammems() & _QM(gGenitiv)) ) return false;

	if	(		!is_morph_noun(sent[j]) 
			&&	!sent[j].HasFlag(fl_adjective_as_noun)
		) 
	{
		return false;
	};

	if ( (GenitGroup.GetGrammems() & gAllCases) != _QM(gGenitiv) ) 
	{
		if	(		(GenitGroup.m_GroupType != gDET_ADJ_NOMEN) 
				&&	(GenitGroup.m_GroupType != gADJ_NOMEN) 
			)
			return false;

		if	(		(GenitGroup.size() == 2)
				&&	IstZahlWort(sent[GenitGroup.m_iFirstWord])
			)
			return false;
	};
	
	
	
	G.m_GroupType = gGENIT_NP;
	G.m_MainGroup = MainGroup;
	G.m_iLastWord = GenitGroup.m_iLastWord;
	G.SetGrammems( MainGroup.GetGrammems() );
	create_syn_rel(G, i, j,  gGENIT_NP);
	

	return true;
};


/*
The rule finds all noun groups which stay in preposition to their main groups.
This Rule should be called after for postpositon genitives in order not to create a mess.

Examples:
	des Vaters Novelle
	des geistigen Vaters Verliebtheit
	des burgerlichen Vaters Brust 
	des Vaters alte Jacke

*/

bool CGerFormatCaller::format_for_genit_in_preposition(CGroup& G)
{
	const CGroup& GenitGroup = get_maximal_group(G.m_iFirstWord);
	int i = get_main_word_in_group(GenitGroup);
	if (!is_morph_noun(sent[i])) return false;

	if (GenitGroup.m_iLastWord+1 == sent.size()) return false;
	if ( (GenitGroup.GetGrammems() & gAllCases) != _QM(gGenitiv) )  return false;

	const CGroup& MainGroup = get_maximal_group(GenitGroup.m_iLastWord+1);
	int j = get_main_word_in_group(MainGroup);
	if	(!is_morph_noun(sent[j]) ) 	return false;

	G.m_GroupType = gGENIT_PRE;
	G.m_MainGroup = MainGroup;
	G.m_iLastWord = MainGroup.m_iLastWord;
	G.SetGrammems( MainGroup.GetGrammems() );
	create_syn_rel(G, j, i,  gGENIT_NP);
	return true;
};
