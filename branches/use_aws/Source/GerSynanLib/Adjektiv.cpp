#include "GerSynan.h"
#include "GerFormatCaller.h"

const size_t AdjModifCount = 4;
const string AdjModif[AdjModifCount] = 
{
	"GANZ", "BESONDERS", "SEHR",	"ZU"
};

bool CGerFormatCaller::CanBeAdjModifier (const CSynPlmLine& L) const
{
	if ( !(L.GetPoses() & (1<<gADV)) ) return false;
	if (!L.m_word_upper) return false;

	for (int i =0; i <AdjModifCount; i++)
		if (AdjModif[i] == L.m_word_upper)
		{
			return true;
		};
	return false;
};
// Examples
// sehr interessant
// ganz interessant

// besonders strenge
// zu konkreten Zielen // a good case of ambiguity
bool CGerFormatCaller::format_for_modif_adj_groups (CGroup& G)
{
	size_t j =  get_main_word(G.m_iFirstWord);
	if( get_maximal_group_no(j) != -1 )
		return false;
	if (!CanBeAdjModifier(Wj))
		return false;

	size_t i = j;
	if( ++i >= sent.size() )
		return false;

	int main_word = get_main_word(i);
	if( !is_morph_adj(sent[main_word]) )
		return false;

	G.m_iLastWord = get_maximal_group(i).m_iLastWord;
	G.m_GroupType = gMODIF_ADJ;
	G.SetGrammems( get_maximal_group(i).GetGrammems() );
	G.m_MainGroup =  get_maximal_group(i);
	create_syn_rel(G, get_main_word(G.m_iLastWord) , get_main_word(G.m_iFirstWord), gMODIF_ADJ);
	return true;
}


bool CGerFormatCaller::HaveEqualAdjEndungen(const CSynPlmLine& L1, const CSynPlmLine& L2) const
{
	if (!L1.m_word_upper || !L2.m_word_upper) return false;
	size_t l1 = strlen(L1.m_word_upper);
	size_t l2 = strlen(L2.m_word_upper);
	if (l1 < 2) return false;
	if (l2 < 2) return false;
	if (		!strcmp(L1.m_word_upper+l1-2,"EM")
			&&	!strcmp(L2.m_word_upper+l2-2,"EM")
		)
		return  true;
	if (		!strcmp(L1.m_word_upper+l1-2,"EN")
			&&	!strcmp(L2.m_word_upper+l2-2,"EN")
		)
		return  true;
	if (		!strcmp(L1.m_word_upper+l1-2,"ER")
			&&	!strcmp(L2.m_word_upper+l2-2,"ER")
		)
		return  true;
	if (		!strcmp(L1.m_word_upper+l1-2,"ES")
			&&	!strcmp(L2.m_word_upper+l2-2,"ES")
		)
		return  true;
	if (		!strcmp(L1.m_word_upper+l1-1,"E")
			&&	!strcmp(L2.m_word_upper+l2-1,"E")
		)
		return  true;
	return false;
};
// Examples:
// schwache oder  starke 
// starke  oder  sehr neue
bool CGerFormatCaller::format_for_similar_adjektive (CGroup& G)
{
	//  get all similar members using commas and conjunction
	CSimilarMembers Members;
	bool IsOnlyCommaDelimited;
	if (!get_similar_members(G.m_iFirstWord, Members, IsOnlyCommaDelimited, &CFormatCaller::is_left_noun_modifier))
		return false;

	//  check flexia of adjectives
	G.SetGrammems(  Members[0]->GetGrammems() );
	int first_main_word = get_main_word_in_group(*Members[0]);
	for (size_t k  = 0; k<Members.size(); k++)
	{
		int j =  get_main_word_in_group(*Members[k]);
		if (!HaveEqualAdjEndungen(sent[first_main_word], sent[j])) return false;
		G.SetGrammems( G.GetGrammems() & Wj.GetGrammems() );
	};

	// check common grammem 
	string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());
	if (!check_noun_or_adj_group_grammems(G.GetGrammems()) ) return false;

	// creating group
	G.m_iLastWord =  Members.back()->m_iLastWord;

	G.m_bCommaDelimited = IsOnlyCommaDelimited;

	G.m_GroupType = gSIMILAR_ADJ;

	G.m_MainGroup = *Members[0];

	change_words_in_group_grammems(G, G.GetGrammems(), gAllCases|gAllGenders);

	return true;
};
