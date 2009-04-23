// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"




/*
Примеры:
  новый или старый 
  молодые и старые 
  желтые, синие  и красные 
  означающий и означаемый
  означавший и означенный 

*/
bool CRusFormatCaller::format_for_similar_adjs(CGroup& G)
{
	int i =  get_main_word(G.m_iFirstWord);

	if (!is_left_noun_modifier (Wi) ) return false;
	G.SetGrammems( Wi.GetGrammems() );
	G.m_iLastWord = G.m_iFirstWord;
	G.m_bCommaDelimited = true;
	bool bHasPossesive = GetOpt()->GetGramTab()->is_morph_pronoun_adjective(Wi.GetPoses());
	bool bFound = false;
	for (int k  = get_next_main_word(i);k != sent.size(); k  = get_next_main_word(k))
	{
		if (!IsSimilarConnector(Wk) || !is_free(k) )  break;
		bool bLastMember  = !Wk.HasFlag(fl_comma);
		if (bLastMember)
			G.m_bCommaDelimited = false;

		k =  get_next_main_word(k);
		if (k == sent.size()) break;

		bHasPossesive = bHasPossesive || GetOpt()->GetGramTab()->is_morph_pronoun_adjective(Wk.GetPoses());

		if (	!is_left_noun_modifier (Wk) 
			|| 	(FindInList((const char*)g_strAdjModif, g_AdjModifCount, Wk.get_lemma()) != -1)
			) 
			break;

		bool bCheckCoordination = (	(		(		Wi.has_grammem(rPlural) 
												&&	Wk.has_grammem(rPlural) 
											)	
										||	(G.GetGrammems() & Wk.GetGrammems() & rAllGenders)
									) 
								&&  (G.GetGrammems() & Wk.GetGrammems() & rAllCases)
								);

		if (!bCheckCoordination) break;

		bFound = true;
		G.m_iLastWord = get_maximal_group(k).m_iLastWord;
		if (bLastMember) break;
	};

	//  не найдено хотя бы еше одного члена группы
	if (!bFound) return false;


/*	 если группу однородности разделяют только запятые и в группу входит хотя бы одно 
притяж. местоимение, то запрещаем такую группу:
   *"мой, хороший дом" 
   должен быть  
   "мой хороший дом"
*/  
	if (bHasPossesive && G.m_bCommaDelimited) return false;

	G.m_GroupType = SIMILAR_ADJS;
	G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
	change_words_in_group_grammems(G, G.GetGrammems(), rAllCases|rAllGenders);


	return true;
}


// Примеры:
//   круче и сильнее 
bool CRusFormatCaller::format_for_similar_comp_adjs(CGroup& G)
{
	if (!is_morph_adj(W1) || !W1.has_grammem(rComparative)  ) return false;

	for (G.m_iLastWord = G.m_iFirstWord;
		(G.m_iLastWord+2 < sent.size()) 
		&&	(is_morph_adj(sent[G.m_iLastWord+2]))
		&&	(sent[G.m_iLastWord+2].has_grammem(rComparative))
		&&	(is_free(G.m_iLastWord+2))
		&&	IsSimilarConnector(sent[G.m_iLastWord+1]);
	G.m_iLastWord+=2);


	if (G.m_iLastWord != G.m_iFirstWord)
	{
		G.m_bCommaDelimited = is_only_comma_delimited(G);
		G.m_Cause = "Соч. группа срав. прилагательных, соединенная запятыми и простыми союзами";
		G.m_GroupType = SIMILAR_ADJS;
		G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
		return true;
	}
	else
		return false;

};

