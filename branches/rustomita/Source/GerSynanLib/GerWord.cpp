#include "GerSynan.h"

#include "GerWord.h"

#include "../SynCommonLib/SynPlmLine.h"
#include "GerSentence.h"


// should be without "bleiben"
const size_t PerfeckAnomalieWordsCount = 11;
const char PerfeckAnomalieWords[PerfeckAnomalieWordsCount][MAX_WORD_LEN] = 
{"MÜSSEN", "KÖNNEN", "DÜRFEN","MÖGEN", "WOLLEN", "SOLLEN","HÖREN","LASSEN","SEHEN", "HELFEN", "LEHREN"};


void CGerSentence::InitHomonymMorphInfo (CSynHomonym& H)
{
    H.InitAncodePattern(  );
	H.m_bPerfectAnomalie = FindInList((const char*)PerfeckAnomalieWords, PerfeckAnomalieWordsCount,H.m_strLemma.c_str()) != -1 && H.HasGrammem(gInfinitiv);
	H.m_bInfinitiveConstruction = H.m_bPerfectAnomalie || ( H.IsLemma("BLEIBEN") && !H.HasPos(gSUB));
	H.m_bAdjWithActiveValency = H.HasPos(gADJ) && binary_search(GetOpt()->m_pAdjPrp->begin(), GetOpt()->m_pAdjPrp->end(), H.m_strLemma);
};


void CGerSentence::InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord)
{


};



bool CGerSentence::WordSchemeEqForThesaurus(const CSynHomonym& Homonym, const CSynPlmLine& word_scheme) const
{
	return false;	
}



bool is_article_for_weak_declination(const CSynHomonym& H) 
{

	if (H.HasPos(gART)	&&	H.IsLemma("DER"))	return true;
	if (H.IsLemma("DERSELBE")) return true;
	if (H.IsLemma("DERJENIGE")) return true;
	if (H.IsLemma("DIESER")) return true;
	if (H.IsLemma("JEDER")) return true;
	if (H.IsLemma("JEDWEDER")) return true;
	if (H.IsLemma("JEGLICHER")) return true;
	if (H.IsLemma("ALLER")) return true;
	if (H.IsLemma("MANCHER")) return true;
	return false;
};


bool is_article_for_mixed_declination(const CSynHomonym& H) 
{
	
	if (H.HasPos(gART)	&&	H.IsLemma("EIN"))		return true;
	if (H.IsLemma("KEIN")) return true;
	if (H.HasPos(gPRO_BEG) &&  H.HasGrammem(gPossessiv)) return true;

	return false;
}


const size_t PrepArticlesCount = 16;
string PrepArticles[PrepArticlesCount] = { "AM","ANS","AUFS","BEIM","DURCHS",
"FÜRS", "HINTERM","HINTERS","IM","INS","UMS","VOM","VORM","VORS","ZUM","ZUR" };


bool IsPrepositionArticle(const char* WordUpper, QWORD& Grammems)
{
	if (!WordUpper) return false;
	Grammems = (_QM(gSingular) | gAllGenders);
	int len = strlen(WordUpper);
	if (len == 0) return false;
	if (WordUpper[len-1] == 'S')
		Grammems |= _QM(gAkkusativ);
	else
		if (WordUpper[len-1] == 'M')
			Grammems |= _QM(gDativ);
		else
			return false;
	return binary_search(PrepArticles, PrepArticles + PrepArticlesCount,string(WordUpper));
};


