#include "GerSynan.h"
#include "GerFormatCaller.h"


// example 
// amerikanische Soldaten und kurdische Kömpfer
// du, er und ich

bool CGerFormatCaller::can_be_part_of_similar_NP(const CSynPlmLine& L) const
{
	if (is_morph_noun(L)) return true;
	if (is_morph_adj(L) && L.has_lemma())
	{
		if (!strcmp (L.m_lemma,"ANDER") ) return true;
		if (!strcmp (L.m_lemma,"ÄHNLICH") ) return true;
	};


	return false;
};

bool CGerFormatCaller::format_for_similar_NP (CGroup& G)
{
	//  get all similar members using commas and conjunction
	CSimilarMembers Members;
	bool IsOnlyCommaDelimited;
	if (!get_similar_members(G.m_iFirstWord, Members, IsOnlyCommaDelimited, &CGerFormatCaller::can_be_part_of_similar_NP))
		return false;

	//  check common case
	G.SetGrammems ( Members[0]->GetGrammems() );
	for (size_t k  = 0; k<Members.size(); k++)
	{
		int j =  get_main_word_in_group(*Members[k]);
		G.SetGrammems( G.GetGrammems() & Wj.GetGrammems() );
	};

	// check common grammem 
	string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());
	if ((G.GetGrammems() & gAllCases) == 0) return false;
	G.SetGrammems (G.GetGrammems() | _QM(gPlural) );

	// creating group
	G.m_iLastWord =  Members.back()->m_iLastWord;

	G.m_bCommaDelimited = IsOnlyCommaDelimited;

	G.m_GroupType = gSIMILAR_NP;

	if (		G.m_bCommaDelimited
			&&	(G.m_iLastWord+1 < sent.size())
			&&	sent[G.m_iLastWord+1].HasFlag(fl_comma)
		)
	{
		/*
		 we convert SIMILAR_NP to APPOSITION, if it has only two members, and there is a comma after 
		 it.
		 for example:
		   Marine-Infanteristen sind in Tikrit, der Heimatstadt Saddam Husseins, einmarschiert. 
		*/

		G.m_GroupType = gAPPOSITION;
	};
	

	G.m_MainGroup = *Members[0];

	change_words_in_group_grammems(G, G.GetGrammems(), gAllCases);

	return true;
};



