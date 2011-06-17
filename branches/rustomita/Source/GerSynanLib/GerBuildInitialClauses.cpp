#include "GerSynan.h"
#include "GerSentence.h"


bool	CGerSentence::IsInitialClauseType(EClauseType ClauseType) const
{
	if (ClauseType == VERBSATZ_T) return true;
	if (ClauseType == INFINITIVSATZ_T) return true;
	return false;
};

bool	CGerSentence::IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int& HomonymNo) const
{
	string W = m_Words[WordNo].m_strUpperWord;

	for (HomonymNo =0; HomonymNo<m_Words[WordNo].m_Homonyms.size(); HomonymNo++)
	{
		const CSynHomonym& H = m_Words[WordNo].m_Homonyms[HomonymNo];
		if (H.IsLemma("DER") && H.HasPos(gPRONOMEN)) 
		{
			if (		(W == "DEREN") 
					||	(W == "DESSEN")
					||	(W == "DENEN")
				) return true;
		}
		else
			// der europäischen Staaten, insbesondere auch diejenigen des deutschen Reiches  keineswegs geeignet ist, 
			if (		H.IsLemma("DERJENIGE") 
					||	H.IsLemma("WELCHER") 
				)
			{

			}
			else
				continue;


		// should  be close to the beginning of the clause
		if (WordNo - ClauseStartWordNo > 3) continue;
		int i = WordNo-1;
		for (; i>=ClauseStartWordNo; i--)
			if	(		!m_Words[i].m_bComma 
                    &&	!m_Words[i].HasDes(OOpn)
					// "der europäischen Staaten, insbesondere auch diejenigen des deutschen Reiches  keineswegs geeignet ist, "
					// : "auch", "sozusagen", "inbesondere"...
					&&	!m_Words[i].FindLemma("AUCH")
					&&	!m_Words[i].FindLemma("SOZUSAGEN")
					&&	!m_Words[i].FindLemma("INSBESONDERE")
				) 
			{
				int Prp = m_Words[i].GetHomonymByPOS(gPRP);
				if (Prp == -1)
					break;
				// "zu den Gründen"
				if  (!( m_Words[i].m_Homonyms[Prp].m_iGrammems & H.m_iGrammems & gAllCases))
					break;

			};

		if (i >= ClauseStartWordNo) continue;

		return true;
	};

	return false;
};




/*
this function deletes all strong homonyms of ambiguous words, if there is an unambiguous strong root
in period [iFirstWord, iLastWord]. This period will be a clause. 
*/
void CGerSentence::SolveAmbiguityUsingRuleForTwoPredicates(int iFirstWord, int iLastWord)
{
	for(int POS = 0 ; POS < GetGerGramTab()->GetPartOfSpeechesCount(); POS++)
	if ( GetGerGramTab()->IsStrongClauseRoot(1 << POS) )
	{
		int iWord = FindWordWithOneHomonym(iFirstWord, iLastWord, POS);

		if (iWord == -1) continue;

		for(int j = iFirstWord ; j <= iLastWord ; j++ )
		{

			if( j == iWord ) continue;

			for(int k = 0 ; k < GetGerGramTab()->GetPartOfSpeechesCount(); k++)
				if ( GetGerGramTab()->IsStrongClauseRoot(1 << k) )
					m_Words[j].KillHomonymOfPartOfSpeech(k);

		}

		break;	
	}
};
