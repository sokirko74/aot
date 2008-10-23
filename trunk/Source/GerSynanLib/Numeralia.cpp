#include "GerSynan.h"
#include "GerFormatCaller.h"


// Mindestens 43 Menschen 
// ungefahr 2 Franken 
// hochstens zwei 
// bis zu 7 Jahren
// etwa sieben Jahren
// wenigstens drei Mitgliedern
// zirka zwei Milliarden
// fruhestens drei Monate 



const size_t NumeralModifCount = 9;
const string NumeralModif[NumeralModifCount] = 
{
	"UNGEFÄHR", "HÖCHSTENS", "ETWA",	"WENIGSTENS",
	"ZIRKA", "FRÜHESTENS", "SPÄTESTENS", "MINDESTENS",
	"RUND"
};


bool CanBeNumeralModifier (const CSynPlmLine& L) 
{
	if ( !(L.GetPoses() & (1<<gADV)) ) return false;
	if (!L.m_word_upper) return false;


	for (int j =0; j <NumeralModifCount; j++)
		if (NumeralModif[j] == L.m_word_upper)
		{
			return true;
		};


	return false;
};


bool CGerFormatCaller :: format_for_adv_zal (CGroup& G)
{
	size_t i =  get_main_word(G.m_iFirstWord);
	if( get_maximal_group_no(i) != -1 )
		return false;

	if (CanBeNumeralModifier(Wi))
	{
		i++;
	}
	else
		if (		(i+1 < sent.size())
				&&	sent[i].is_lemma("BIS")
				&&	sent[i+1].is_lemma("ZU")
			)
		{
			i+=2;
		}
	else
			return false;

	if( i >= sent.size() )
		return false;


	int main_word = get_main_word(i);
	if( !IstZahlWort(sent[main_word]) )
		return false;

	G.m_iLastWord = get_maximal_group(i).m_iLastWord;
	G.m_GroupType = gMODIF_NUMER;
	G.SetGrammems(  get_maximal_group(i).GetGrammems() );
	G.m_MainGroup =  get_maximal_group(i);
	create_syn_rel(G, get_main_word(G.m_iLastWord) , get_main_word(G.m_iFirstWord), gMODIF_NUMER);
	return true;


};
