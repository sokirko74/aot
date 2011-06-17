// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


// Примеры:
// самая красивая
// такая большая

bool CRusFormatCaller::format_for_modif_adj_groups (CGroup& G)
{
    if (get_maximal_group_no(G.m_iFirstWord) != -1) return  false;    
    int j =  G.m_iFirstWord;

    if( FindInList((const char*)g_strAdjModif, g_AdjModifCount, Wj.get_lemma()) == -1)
        return false;


    if( j+1 >= sent.size() )
        return false;
    const CGroup& H = get_maximal_group(j+1);
    if( !is_morph_adj(sent[H.m_MainWordNo]) )
            return false;

    if( !GetGramTab()->GleicheGenderNumberCase(sent[H.m_MainWordNo].m_type_gram_code, sent[H.m_MainWordNo].m_gramcodes, Wj.m_gramcodes ) )
        return false;

    G.m_iLastWord = H.m_iLastWord;
    G.m_GroupType = MODIF_ADJ;
    G.SetGrammems (H.GetGrammems() & sent[j].GetGrammems());
    G.m_MainGroup =  H;
    change_words_in_group_grammems(G, G.GetGrammems(), (rAllNumbers | rAllCases));  
    create_syn_rel(G, H.m_MainWordNo, j , MODIF_ADJ);
    return true;
}


// Примеры:
//    новый стол // род, число, падеж
//    красная кровать // род, число, падеж
//    большие дома // число, падеж
//    очень красивый стол
//    большой и очень красивый стол
//    новые и совсем ненужные столы и стулья
//    моя вторая мама

bool CRusFormatCaller::format_for_noun_groups (CGroup& G)
{
	const CGroup& FirstChild = get_maximal_group(G.m_iFirstWord);
	if (FirstChild.m_iFirstWord != G.m_iFirstWord) return false;
	int i =  get_main_word_in_group(FirstChild);
	if (!is_left_noun_modifier (Wi)) return false;
	if( Wi.is_lemma("КОТОРЫЙ") )
	  		return false;
	string debug_str;
	bool bFound_VSE = false; //слово "всe" начинает группу
	bool bFoundParticiple = false; //нашли дст., пе, причастие
	bool bAdjShouldBeInNominativOrGenitiv = false; //чтобы собрать "две красивых девочки"
	//gleiche_for_small_numbers
	int NounGroupNo;

	if(		Wi.get_upper_word()
		&&	!strcmp(Wi.get_upper_word(), "ВСЕ")
		&&    Wi.is_lemma("ВЕСЬ")
		&&	(get_maximal_group_size(i) == 1)
		)
	bFound_VSE = true;

	if	(		Wi.has_grammem(rActiveVoice)
			&&	Wi.has_grammem(rTransitive)
            &&  Wi.HasPOS(PARTICIPLE) 
		)
		bFoundParticiple = true;

  

  
	if ((G.m_iFirstWord+1) >= sent.size()) return false;

	// собираем группу "сам себя"
	int j;
	if (    Wi.is_lemma( "САМ") )
	{
		G.m_iLastWord = get_maximal_group(G.m_iFirstWord).m_iLastWord + 1; 
		if (G.m_iLastWord < sent.size())
		{
			NounGroupNo = get_maximal_group_no(G.m_iLastWord);		
            j = (NounGroupNo == -1 ) ? G.m_iLastWord : GetGroups()[NounGroupNo].m_MainWordNo;
			if (   Wj.is_lemma("СЕБЯ") 
				|| Wj.is_lemma("ТЫ") 
				|| Wj.is_lemma("Я") 
				|| Wj.is_lemma("МЫ")  // "сами мы не местные"
				)
				if ((Wi.GetGrammems() & Wj.GetGrammems() & rAllCases) > 0)
				{
					i = j;
					G.SetGrammems (Wi.GetGrammems() & Wj.GetGrammems() & (rAllCases | rAllGenders));
					goto CreateGroup;
				};
		};
	};
	   

	// нахождение последовательности П1 ... Пn С, где Пi - прилагательное или группа прилагательных,
	// а С - ИГ.
	// Согласование по падежу, числу и роду будет проверяться в другом цикле.
	G.SetGrammems ( Wi.GetGrammems() );
	for (	G.m_iLastWord = get_maximal_group(G.m_iFirstWord).m_iLastWord + 1; 
			G.m_iLastWord < sent.size(); 
			G.m_iLastWord  = get_maximal_group(G.m_iLastWord).m_iLastWord + 1 
		)
	{

			int i_gr = get_maximal_group_no(G.m_iLastWord);		
            i = (i_gr == -1) ? G.m_iLastWord:GetGroups()[i_gr].m_MainWordNo;

			if( Wi.is_lemma("КОТОРЫЙ") )
	  			return false;

			if( i_gr != -1 ) 
			{
				G.SetGrammems (G.GetGrammems() &  GetGroups()[i_gr].GetGrammems());
			}
			else
			{
				G.SetGrammems( G.GetGrammems() & Wi.GetGrammems() );
				if( GetGramTab()->IsSimpleParticle(Wi.get_lemma(), Wi.GetPoses()) && (i > G.m_iFirstWord) )			
					continue;
				
			}

			if ( !is_left_noun_modifier (Wi)) break;

			if	(		Wi.has_grammem(rActiveVoice) 
					&&	Wi.has_grammem(rTransitive) 
					&&	Wi.HasPOS(PARTICIPLE) 
				)
				bFoundParticiple = true;
		
	}; //  end of for by  "G.m_iLastWord"


	if (G.m_iLastWord < sent.size())
		G.m_iLastWord = get_maximal_group(G.m_iLastWord).m_iLastWord;

	/*
		если дошли до конца или в конце не стоит существительного 
		тогда пробуем собрать группу "все это" ("это" - не является синтаксическим cуществительным)
			или группу "все лишнее"
	*/
	if (G.m_iLastWord >= sent.size() || !Wi.is_syn_noun())
	{
		//  если самое первое слово ИГ было "всe"
		if( bFound_VSE )
		{
			// возвращаемся на одно слово назад
			G.m_iLastWord--;	
			if( G.size() != 2 )
				return false;
			i = G.m_iLastWord;
			if( ( get_maximal_group_size(i) == 1) && GetGramTab()->IsSimpleParticle(Wi.get_lemma(), Wi.GetPoses()) )
				G.m_iLastWord++;

			if (G.m_iLastWord >= sent.size()) return false;

			if( get_maximal_group_size(G.m_iLastWord) > 1)
				return false;

			if( sent[G.m_iLastWord].m_UnitType != EWord)
				return false;

			i = get_main_word(G.m_iLastWord);			
		
			if( !is_left_noun_modifier (Wi) )	 
				return false;

			if( !is_left_noun_modifier (Wi) )	 
				return false;
		}
		else
			return false;
	}



	if( bFoundParticiple )
		if( (Wi.GetGrammems() & _QM(rAccusativ)) && (G.GetGrammems() & _QM(rAccusativ)) )
			return false;
  

	// Берем группу, которая могла быть построена на найденном существительном,
	// группа может прийти из тезауруса или ЧИСЛ-СУЩ, например: "первые пять человек",

	size_t FirstWordOfNounGroup;  
	NounGroupNo = get_maximal_group_no(i);
	if( NounGroupNo != -1)
	{
		FirstWordOfNounGroup = GetGroups()[NounGroupNo].m_iFirstWord;
		G.SetGrammems ( GetGroups()[NounGroupNo].GetGrammems() );
	}
	else
	{
		FirstWordOfNounGroup = i;
		G.SetGrammems( Wi.GetGrammems() );
	};

	if( G.m_iFirstWord > 0 )
		if(is_small_number_group(G.m_iFirstWord - 1) )	  
		{
			QWORD dummy;
			gleiche_for_small_numbers(i, G.m_iFirstWord - 1, false, dummy, bAdjShouldBeInNominativOrGenitiv);		  
		}

	// Согласование по падежу, числу и роду проверяться здесь.
	for (j = G.m_iFirstWord; j <  FirstWordOfNounGroup; j = get_maximal_group(j).m_iLastWord + 1)
	{
		const CGroup& LastHost = get_maximal_group(j);
		int  MainWordNo = LastHost.m_MainWordNo;

		if( (LastHost.size() == 1) && GetGramTab()->IsSimpleParticle(Wj.get_lemma(), Wj.GetPoses()) )				
				continue;



		if ( !bAdjShouldBeInNominativOrGenitiv )
		{
			//  проверяем согласование между прилагательным и существительным
			QWORD CommonGrams = GetGramTab()->GleicheGenderNumberCase(Wi.m_type_gram_code, Wi.m_gramcodes, sent[MainWordNo].m_gramcodes); 
			if (!CommonGrams)
				if	(		NounGroupNo != -1 
						&& GetGroups()[NounGroupNo].m_GroupType == NUMERAL_NOUN
						&& (sent[MainWordNo].GetGrammems() &  G.GetGrammems() & rAllNumbers)					
						&& (sent[MainWordNo].GetGrammems() &  G.GetGrammems() & rAllCases)
					) 
					CommonGrams = sent[MainWordNo].GetGrammems();
				else
					return false;
				


			//  если главный элемент в ПРИЛ_СУЩ является однородным рядом, тогда нужно проверить, что
			//  данное прилагательное согласовано со всеми членами однородного ряда по падежу (по числу и роду может быть не согласовано)
			//  иначе вся фразы "Советской гавани, он" покрывается одним ПРИЛ_СУЩ 
			if (NounGroupNo != -1)
				if (GetGroups()[NounGroupNo].m_GroupType == SIMILAR_NOUN_GROUPS)
					if ( (Wi.GetGrammems() & GetGroups()[NounGroupNo].GetGrammems() & rAllCases) == 0)
						return false;

			G.SetGrammems( G.GetGrammems() & CommonGrams);
		}
		else
		{
			if	(			(			!(sent[MainWordNo].GetGrammems() &  _QM(rGenitiv))	
									&&	!(sent[MainWordNo].GetGrammems() &  _QM(rNominativ)) 
							)
						||	!(	sent[MainWordNo].GetGrammems() &  _QM(rPlural)) 
					
				)
					return false;				

			G.SetGrammems(G.GetGrammems() & sent[MainWordNo].GetGrammems());
		};

		
	}  // end of cycle by "j"

	if( bAdjShouldBeInNominativOrGenitiv )
		if( NounGroupNo == -1 )
			G.SetGrammems( Wi.GetGrammems());
		else
			G.SetGrammems( GetGroups()[NounGroupNo].GetGrammems() );


	debug_str = GetGramTab()->GrammemsToStr(G.GetGrammems());
	debug_str = GetGramTab()->GrammemsToStr(W2.GetGrammems());

	G.m_Cause = "ИГ, согласованная по роду, числу и падежу";

	if (is_morph_pronoun(Wi)) return false; 


CreateGroup:

	if ((G.GetGrammems() & rAllNumbers) == _QM(rSingular))
		change_words_in_group_grammems(G, G.GetGrammems(), (rAllNumbers | rAllGenders | rAllCases));  
	else
		change_words_in_group_grammems(G, G.GetGrammems(), (rAllNumbers | rAllCases));  

	G.m_MainGroup =  get_maximal_group(i);
	debug_str = GetGramTab()->GrammemsToStr(W2.GetGrammems());
	G.m_iLastWord = G.m_MainGroup.m_iLastWord;
	G.m_GroupType = NOUN_ADJ;
	debug_str = GetGramTab()->GrammemsToStr(G.GetGrammems());
  
    {
        size_t k  = G.m_iFirstWord; 
        size_t last = get_maximal_group(G.m_iLastWord).m_iFirstWord;
        while (k < last)
        {	
            const CGroup& H = get_maximal_group(k);
            create_syn_rel(G, i, H.m_MainWordNo, NOUN_ADJ);           		
            k += H.size();
        }

    }

	return true;
};
             

bool  CRusFormatCaller::format_for_partic_clause(CGroup& G)
{
	int j;

	int i = get_main_word (G.m_iFirstWord);
	if (i != G.m_iFirstWord) return false;
	if (Wi.m_UnitType == EWord) return false;
	if (Wi.m_ClauseType != PARTICIPLE_T) return false;

	if (i == 0) return false;
	const CGroup& MaxGrp = get_maximal_group(i-1);

	for (j = MaxGrp.m_iLastWord; j >= MaxGrp.m_iFirstWord; j--)
	{
		assert(j != -1);

		if ( !Wj.is_morph_noun()) continue;
		if ( Wj.HasFlag(fl_ile)) continue;
		
		int m = -1;
		for (int f = 0; f < GetGroups().size(); f++)
			if (GetGroups()[f].m_iFirstWord <= j && GetGroups()[f].m_iLastWord == i-1)
			{
				m = f;
				break;
			}
		int varMainWrd; 
		if (m != -1)
			varMainWrd = GetGroups()[m].m_MainWordNo;
		else
			varMainWrd = j;


		if (		GetGramTab()->GleicheGenderNumberCase(Wi.m_type_gram_code, Wi.m_gramcodes, Wj.m_gramcodes ) 
			||		(		sent[MaxGrp.m_iFirstWord].HasFlag(fl_small_number)
						&&	((Wi.GetGrammems() &  ( (1 << rGenitiv)  | (1 << rPlural))) == ( (1 << rGenitiv)  | 	(1 << rPlural)) )
					)
			)
		 if (varMainWrd == j || j == i-1)
		 {
			G.m_GroupType = NOUN_PARTICIPLE;
			G.m_iLastWord = get_maximal_group(G.m_iFirstWord).m_iLastWord;
			if (varMainWrd == j && m != -1)
				G.m_iFirstWord = GetGroups()[m].m_iFirstWord;
			else
				G.m_iFirstWord = j;
			G.m_MainGroup.m_iFirstWord =  G.m_iFirstWord;
			if (varMainWrd == j && m != -1)
				G.m_MainGroup.m_iLastWord = GetGroups()[m].m_iLastWord;
			else
				G.m_MainGroup.m_iLastWord =  j;

			G.SetGrammems( Wj.GetGrammems() );

			for (m = GetGroups().size()-1; m >= 0; m--)
				if (   GetGroups()[m].m_iFirstWord < G.m_iFirstWord 
					&& GetGroups()[m].m_iLastWord == i-1
				   )
					m_GroupsToRebuild.push_back(GetGroups()[m]);
			return true;
		 }

		 if (m != -1 && GetGroups()[m].m_iLastWord == i-1)
		   if (GetGroups()[m].m_GroupType == SIMILAR_NOUN_GROUPS)
			   if (GetGramTab()->GleicheCase(Wi.m_gramcodes,Wj.m_gramcodes))
			{
				G.m_GroupType = NOUN_PARTICIPLE;
				G.m_iLastWord = G.m_iFirstWord;
				G.m_iFirstWord = GetGroups()[m].m_iFirstWord;
				G.m_MainGroup.m_iFirstWord =  GetGroups()[m].m_iFirstWord;
				G.m_MainGroup.m_iLastWord =  GetGroups()[m].m_iLastWord;
				G.SetGrammems( GetGroups()[m].GetGrammems() );
				for (m = GetGroups().size()-1; m >= 0; m--)
					if (GetGroups()[m].m_iFirstWord < G.m_iFirstWord && GetGroups()[m].m_iLastWord == i-1)
						m_GroupsToRebuild.push_back(GetGroups()[m]);
				return true;
			}		
	}

	return false;	
};

bool  CRusFormatCaller::format_for_whose_clause(CGroup& G) //правило построения группы с придаточным определительным;	
{
	int j;
	int i = get_main_word (G.m_iFirstWord);
	if (i != G.m_iFirstWord) return false;
	if (Wi.m_UnitType == EWord) return false;
	if (!Wi.HasFlag(fl_relative_clause)) return false;

	if (i == 0) return false;
	const CGroup& MaxGrp = get_maximal_group(i-1);

	for (j = MaxGrp.m_iLastWord; j >= MaxGrp.m_iFirstWord; j--)
	{
		assert(j != -1);
		if ( !Wj.is_morph_noun()) continue;

		
		int m = -1;
		for (int f = 0; f < GetGroups().size(); f++)
			if (GetGroups()[f].m_iFirstWord <= j && GetGroups()[f].m_iLastWord == i-1)
			{
				m = f;
				break;
			}
		int varMainWrd; 
		if (m != -1)
			varMainWrd = GetGroups()[m].m_MainWordNo;
		else
			varMainWrd = j;

		if ( GetGramTab()->GleicheGenderNumber(Wi.m_gramcodes, Wj.m_gramcodes ) )
		 if (varMainWrd == j || j == i-1)
		 {
			G.m_GroupType = WHOSE_SUBORDINATE;
			G.m_iLastWord = G.m_iFirstWord;
			if (varMainWrd == j && m != -1)
				G.m_iFirstWord = GetGroups()[m].m_iFirstWord;
			else
				G.m_iFirstWord = j;
			G.m_MainGroup.m_iFirstWord =  G.m_iFirstWord;
			if (varMainWrd == j && m != -1)
				G.m_MainGroup.m_iLastWord = GetGroups()[m].m_iLastWord;
			else
				G.m_MainGroup.m_iLastWord =  j;
			G.SetGrammems( Wj.GetGrammems() );
			for (m = GetGroups().size()-1; m >= 0; m--)
			{

				if (GetGroups()[m].m_iFirstWord < G.m_iFirstWord && GetGroups()[m].m_iLastWord == i-1)
					m_GroupsToRebuild.push_back(GetGroups()[m]);
			};
			return true;
		 }

		 if (m != -1 && GetGroups()[m].m_iLastWord == i-1)
		   if (GetGroups()[m].m_GroupType == SIMILAR_NOUN_GROUPS)
			if (GetGramTab()->GleicheCase(Wi.m_gramcodes, Wj.m_gramcodes) )
			{
				G.m_GroupType = WHOSE_SUBORDINATE;
				G.m_iLastWord = G.m_iFirstWord;
				G.m_iFirstWord = GetGroups()[m].m_iFirstWord;
				G.m_MainGroup.m_iFirstWord =  GetGroups()[m].m_iFirstWord;
				G.m_MainGroup.m_iLastWord =  GetGroups()[m].m_iLastWord;
				G.SetGrammems ( GetGroups()[m].GetGrammems() );
				for (m = GetGroups().size()-1; m >= 0; m--)
					if (GetGroups()[m].m_iFirstWord < G.m_iFirstWord && GetGroups()[m].m_iLastWord == i-1)
						m_GroupsToRebuild.push_back(GetGroups()[m]);
				return true;
			}		
	}

	return false;	
};
