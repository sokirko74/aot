#include "GerSynan.h"
#include "GerFormatCaller.h"


const size_t CitiesCount = 4;
const string Cities[CitiesCount] = 
{
	"STADT", "HAUPTSTADT", "HAFENSTADT","KÜSTENSTADT"
};


bool CanBeCityType (const CSynPlmLine& L) 
{
	if ( !(L.GetPoses() & (1<<gSUB)) ) return false;
	if (!L.m_word_upper) return false;

	for (int i =0; i <CitiesCount; i++)
		if (Cities[i] == L.m_word_upper)
		{
			return true;
		};
	return false;
};

bool CGerFormatCaller :: format_for_geo_names (CGroup& G)
{
	size_t j =  get_main_word(G.m_iFirstWord);
	if (!CanBeCityType(Wj))
		return false;
	size_t i = j;

	if( ++i >= sent.size() )
		return false;

	int main_word = get_main_word(i);
	if (main_word == -1) return false;

	if	(		!sent[main_word].HasFlag(fl_morph_predicted)
			&&	!sent[main_word].has_grammem(gStd)	
		)
		return false;

	G.m_iLastWord = get_maximal_group(i).m_iLastWord;
	G.m_GroupType = gGEOGR;
	G.SetGrammems(  get_maximal_group(j).GetGrammems() );
	G.m_MainGroup =  get_maximal_group(j);
	create_syn_rel(G, j, main_word, gGEOGR);
	return true;

};
