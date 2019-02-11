#include "GerSynan.h"
#include "GerFormatCaller.h"



// example 
// bei Tisch
// zu Hause
// bei mir
bool CGerFormatCaller::format_for_preps (CGroup& G)
{
	 QWORD depend_cases;	
	 int j = check_first_part_of_prep_noun (G, depend_cases, gPREP_NOUN);
	 if (j == -1 ) return false;

	 int k = get_main_word( get_maximal_group(j).m_iLastWord + 1);
	 if( k >= sent.size() )
		 return false;
		
	 int i_group = get_maximal_group_no(k);
 
	 if (	(    !Wk.is_morph_noun() 
			  && !(Wk.GetPoses() & _QM(gPRONOMEN))
			) 
		 ) 
		 return false;


	const CGroup& MaxGrp = get_maximal_group(k);

	QWORD grammems;
	if( i_group != -1 )
		grammems = GetGroups()[i_group].GetGrammems();
	else	
		grammems = Wk.GetGrammems();


	if ((depend_cases & grammems & gAllCases)  == 0)
		 return false;

	if (		((depend_cases & grammems & gAllCases)  == _QM(gNominativ) )
			&&	((grammems & _QM(gPlural)) == 0)
		)
	return false;

	G.m_MainGroup.m_iFirstWord = G.m_iFirstWord;
	G.m_MainGroup.m_iLastWord = j; 
	G.SetGrammems (grammems & depend_cases); 
	G.m_iLastWord = j + get_maximal_group_size(k);

	change_words_in_group_grammems(CPeriod(get_maximal_group(G.m_iLastWord).m_iFirstWord, get_main_word(G.m_iLastWord)),depend_cases, gAllCases);
	if( i_group != -1 )
		change_group_grammems(*GetGroupPtr(i_group),depend_cases, gAllCases);

	W1.m_FoundPrepDependCases = (depend_cases & grammems & gAllCases);

	G.m_GroupType = gPREP_NOUN;
	return true;
};

