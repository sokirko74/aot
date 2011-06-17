// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"




/*
 к обвиняемому и потерпевшему
 О ленивом я не хочу говорить.
 никак сегодня не натягивало на сорок .
*/
bool CRusFormatCaller::format_for_preps_adj (CGroup& G)
{
	// падежи предлога 
	QWORD depend_cases;	
	// стоит ли на первом месте предлог?
	int j = check_first_part_of_prep_noun (G, depend_cases, PREP_NOUN);
	// не стоит - выходим
	if (j == -1 ) return false;
	// k - главное слово группы после предлога
	int k = get_next_main_word( j );
	if( k == sent.size() )
	 return false;
	if (sent[k].m_UnitType != EWord)
		return false;

	// берем только прилагательные 
	if ( ( Wk.GetPoses() & ( (1 << ADJ_FULL) |  (1 << PARTICIPLE) |  (1 << NUMERAL)) )  == 0) return false;



	if ((depend_cases & Wk.GetGrammems() )  == 0) return false;

	// специальная  проверка для  в+И_мн
	if (		((depend_cases & Wk.GetGrammems() & rAllCases)  == _QM(rNominativ) )
			&&	((Wk.GetGrammems() & _QM(rPlural)) == 0)
		)
	return false;

	
	G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
	G.SetGrammems( Wk.GetGrammems() & depend_cases );  

	
	const CGroup& LastGroup = get_maximal_group(k);

	G.m_iLastWord = LastGroup.m_iLastWord;
	change_words_in_group_grammems(CPeriod(LastGroup.m_iFirstWord, get_main_word_in_group(LastGroup)),depend_cases, rAllCases);

	int i_group = get_maximal_group_no(k);
	if( i_group != -1)
		change_group_grammems(*GetGroupPtr(i_group), depend_cases, rAllCases);

	W1.m_FoundPrepDependCases  =  depend_cases & Wk.GetGrammems() & rAllCases;
	G.m_Cause = "Предложная группа c прилагательным";

	//  saving grammems
	sent[G.m_iFirstWord].SetGrammems(G.GetGrammems());
	G.m_GroupType = PREP_NOUN;
	return true;
};

/*
 Примеры:
	 в столе 
	 в красивой вазе 
	 в ногах и руках 
	 в выделенной прошлой весной комнате
*/

bool CRusFormatCaller::format_for_preps (CGroup& G)
{
	 QWORD depend_cases;	
	 int j = check_first_part_of_prep_noun (G, depend_cases, PREP_NOUN);
	 if (j == -1 ) return false;

	 int k = get_maximal_group(j).m_iLastWord + 1;
	 if( k >= sent.size() )
		 return false;

     int i_group = get_maximal_group_no(k);
     k = (i_group ==-1) ? k : GetGroups()[i_group].m_MainWordNo;

	 if (Wk.m_UnitType != EWord)
		 return false;
		
 
	 if (	(    !Wk.is_syn_noun() 
              && !Wk.HasPOS(PRONOUN_PREDK ) 
			  && !(   (i_group != -1) 
				   && (GetGroups()[i_group].m_GroupType == NUMERAL_NOUN)
				  ) 
			  && !Wk.HasPOS(PRONOUN)
			) 
		 && !Wk.HasFlag(fl_ile) 
		 && !((i_group != -1) && (GetGroups()[i_group].m_GroupType == SELECTIVE_GR ) ) 
		 ) 
		 return false;


	/*
	Следующий "if (!Wk.is_syn_noun())" добавил я, чтобы бралась 
	фраза  
	"я разговаривал о том, что ты сказал"
	"Первый номер сообщает , в какой из нескольких больших географических зон находится BBS"
	*/
	
	if (!Wk.is_syn_noun()	)
	if	(	    is_morph_pronoun_adjective(Wk) 
			&&  i_group == -1
			&& !sent[G.m_iFirstWord].is_lemma("ИЗ") 
			&& !Wk.is_lemma("КОТОРЫЙ") 
		) 
		return false; 

	QWORD grammems;
	if( i_group != -1 )
	{
		grammems = GetGroups()[i_group].GetGrammems();
	}
	else	
	{
		grammems = Wk.GetGrammems();
		if( Wk.HasFlag(fl_ile) )
			grammems = GetMaxQWORD();
	}


	if ((depend_cases & grammems & rAllCases)  == 0)
	{

		 /*
		  предлог "по" может управлять номинативом, например:
		  "по две девочки" или "по двадцать долларов"
		  Сейчас надо проверить этот случай.
		 */
		if (    i_group != -1  
			 && (GetGroups()[i_group].m_GroupType == NUMERAL_NOUN) 
			 && (GetGroups()[i_group].GetGrammems() & _QM(rNominativ ) ) 
			 && ( sent[G.m_iFirstWord].is_lemma("ПО") || sent[G.m_iFirstWord].is_lemma("НА"))
			) 
		;
		else
		 return false;
	};

	// специальная  проверка для  в+И_мн
	if (		((depend_cases & grammems & rAllCases)  == _QM(rNominativ) )
			&&	((grammems & _QM(rPlural)) == 0)
		)
	return false;

	/*
		запретить:
			"в  его",                    (надо "в него")
			"над им",                    (надо "над ним")
			и т.д.
		Короче, в предложной группе для личных местоимений он, она, оно
		выбирать только те, которые начинаются на согласный.
	*/
	if (GetGramTab()->is_morph_personal_pronoun(Wk.GetPoses(),Wk.GetGrammems())) 
		if (is_upper_vowel(Wk.get_upper_word()[0],morphRussian))
			return false;


	G.m_MainGroup.m_iFirstWord = G.m_iFirstWord;
	G.m_MainGroup.m_iLastWord = j; 

	if ( !(depend_cases & grammems) )
		G.SetGrammems(_QM(rDativ ) );
	else
		G.SetGrammems( grammems & depend_cases); 

	G.m_iLastWord = j + get_maximal_group_size(k);
	change_words_in_group_grammems(CPeriod(get_maximal_group(G.m_iLastWord).m_iFirstWord, get_main_word(G.m_iLastWord)),depend_cases, rAllCases);
	if( i_group != -1 )
		change_group_grammems(*GetGroupPtr(i_group), depend_cases, rAllCases);
	


	if ( !(depend_cases & grammems) ) 
		W1.m_FoundPrepDependCases = _QM(rDativ ); //  почему датив? Сокирко ноябрь, 2002
	else
		W1.m_FoundPrepDependCases = (depend_cases & grammems & rAllCases);

	G.m_GroupType = PREP_NOUN;
	return true;
};

bool CRusFormatCaller::format_for_prep_IZ (CGroup& G)
{
	const CSynPlmLine& first_word_plm = sent[G.m_iFirstWord];
	if (!first_word_plm.is_lemma("ИЗ")) return false;
	return format_for_preps(G);
};


