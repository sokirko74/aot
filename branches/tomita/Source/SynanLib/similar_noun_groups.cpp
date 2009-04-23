// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"

/*
Примеры:
 мама и папа
 импресарио и шоу
 денди и леди
 пальто и валенками
*/





// возращает длину найденного однородного ряда
size_t CRusFormatCaller::search_for_similar_noun_groups (const CGroup& FirstGroup, 
												 CGroup& G)
{
	size_t i;
	i = get_main_word_in_group(FirstGroup); 
	if (!Wi.is_syn_noun()) return 0;
	G.SetGrammems( FirstGroup.GetGrammems() );
	int k = FirstGroup.m_iLastWord+1;
	G.m_iFirstWord = FirstGroup.m_iFirstWord;
	G.m_iLastWord = FirstGroup.m_iLastWord;
	int CountOfCoordinators = 0;
	bool bAtLeastOneCommaExists = true;

	// итератор k всегда указывает на "сочинитель"(запятая, союз)
	// если последующая за "сочинителем" группа не именная, то цикл прерывается
	// если итератор k не указывает "сочинитель", то цикл прерывается
	// i не меняется

	while	(		(k+1 < sent.size()) 
				&&	CanConnectSimilar(Wk)
			)
	{
		//nim
		if ( !bAtLeastOneCommaExists && !Wk.HasFlag(fl_comma) && G.m_GroupType == SIMILAR_NOUN_GROUPS) break;

		if ( Wk.HasFlag(fl_comma) )
			bAtLeastOneCommaExists = true;
		else
			bAtLeastOneCommaExists = false;
		//nim

		const CGroup& F =  get_maximal_group (k + 1);
        int j = F.m_MainWordNo;
		if  (!Wj.is_syn_noun()) break;

		
		if (Wk.HasFlag(fl_comma))
		{
			//ограничения на построение группы, сочиненной по запятой; nim
			if ( Wi.has_grammem(rName)  != Wj.has_grammem(rName) ) break;
			if ( Wi.has_grammem(rSurName)  != Wj.has_grammem(rSurName) ) break;
			if ( is_morph_pronoun(Wi)      && !Wi.has_grammem(rNominativ) ) break;
			if ( is_morph_pronoun(Wj)      && !Wj.has_grammem(rNominativ) ) break;
			//ограничения на построение группы, сочиненной по запятой; nim
		}

		if ((G.GetGrammems() & F.GetGrammems() & rAllCases) == 0)
			break;

		G.SetGrammems(G.GetGrammems() & F.GetGrammems()); 
		CountOfCoordinators++;
		G.m_iLastWord = F.m_iLastWord;
		k = F.m_iLastWord+1;
	};

	/*
		приходится отдельно рассматривать случай, когда мы имеем конструкцию "Х,Y", например:
		"Полковник,взвод уже прибыл!"
		здесь нужно проверить, что ДОЛЖ(Х) != ДОЛЖ(Y), где ДОЛЖ(Х) равно истине, если Х имеет вышестоящий концепт ДОЛЖ.
	*/
	if (CountOfCoordinators == 1)
		if (bAtLeastOneCommaExists)
		{
				
			int j = get_main_word (G.m_iLastWord);

			// Чтобы  не строилась группа ОДНОР_ИГ в предложении "Полковник,взвод уже прибыл!
			// добавим следующую проверку
			if (Wi.HasFlag(fl_ranknoun) 	!= Wj.HasFlag(fl_ranknoun) ) 
				if ( Wi.has_grammem(rNominativ) && Wj.has_grammem(rNominativ) )
					return 0;
		};

	if (CountOfCoordinators > 0) 
		return G.m_iLastWord - FirstGroup.m_iFirstWord + 1;
	else
		return 0;
};

/*
 эта функция проверяет, что группа однородности разделяют только запятые.
 Если так, тогда запрещаются конструкции, типа
   "каждый, отец, мать " 
  т.е. запрещаем входить в бессоюзную однородную группу  местоимения-существительные,
  кроме личных местоимений. 
  Личные местоимения нужны для построения группы ПРИЛОЖЕНИЯ ("Я, старый большевик, ушел"),
  которая строится но основе ОДНОР_ИГ.
*/

static bool is_only_comma_with_pronoun_in_group(CRusFormatCaller& Caller, const CGroup &G)
{
   if (!G.m_bCommaDelimited) return false;	

	for ( int i = G.m_iFirstWord; i <= G.m_iLastWord; i++ )
	{
		if ( i > G.m_iFirstWord && i < G.m_iLastWord )
		{
			if (     Caller.sent[i].HasFlag(fl_comma)
				 &&  Caller.GetGramTab()->is_morph_pronoun(Caller.sent[i-1].GetPoses()) 
				 && !Caller.GetGramTab()->is_morph_personal_pronoun(Caller.sent[i-1].GetPoses(),Caller.sent[i-1].GetGrammems()) 
			   )
				return true;
		}
	}

	return false;
}


// Примеры 
// дом отца , выпивающего брата и красивой матери 
bool CRusFormatCaller::format_for_similar_noun_groups (CGroup& G)
{
  
 int i = get_maximal_group_no(G.m_iFirstWord); 
 const CGroup& gr = (i==-1) ? m_AtomicGroups[G.m_iFirstWord] :  GetGroups()[i]; 
 if (i == -1) i = GetGroups().size();
 size_t stack[20];
 size_t stack_size = 0;
 size_t max_similar_group_size = search_for_similar_noun_groups (gr, G);
 size_t max_i = GetGroups().size();
 bool bFound = false;
 bool bAtomic = false;

 // Итератор i указывает на группу, от которой алгоритм пытается
 // построить однородный ряд.
 // Если итератор i выходит за пределы, это означает, что ни от одной
 // группы нельзя построить однородный ряд. 
 while	(     (i < GetGroups().size()) 
		  &&  (GetGroups()[i].m_iLastWord == gr.m_iLastWord)
		  &&	(i >= 0)  
		) 
 {
	 size_t k =  search_for_similar_noun_groups (GetGroups()[i],G);
	 if (k > max_similar_group_size)
	 {
		 max_i = i;
		 max_similar_group_size = k;
		 bFound = true;
	 };
	 stack[stack_size++] = i;
	 assert (stack_size < 20);
	 if (i==0) 
	   i = GetGroups().size();
	 else 
	   i--;
 };

	if( !bFound && (max_similar_group_size <= 2) )
	{
		max_similar_group_size = search_for_similar_noun_groups (m_AtomicGroups[gr.m_iLastWord], G);  
		if( max_similar_group_size > 2 )
		{
			bFound = true;
			bAtomic = true;
		}
	}

  if ( max_similar_group_size > 2)
	{

		if( bAtomic )
		{			
			G.m_MainGroup.m_iFirstWord = gr.m_iLastWord;
			G.m_MainGroup.m_iLastWord = gr.m_iLastWord;
		}
		else
		{
			if (max_i < GetGroups().size()) // не атомарная группа 
			{
				search_for_similar_noun_groups (GetGroups()[max_i],G);
				G.m_MainGroup = GetGroups()[max_i];
			}
			else
			{
				search_for_similar_noun_groups (gr, G);//atomic_groups[G.m_iFirstWord],G);
				G.m_MainGroup = gr;
			};
		}

        G.m_iFirstWord =  G.m_MainGroup.m_iFirstWord;
		

		G.m_Cause = "группа ИГ, соединенная запятыми и простыми союзами, согл. по падежу ";
		G.m_GroupType = SIMILAR_NOUN_GROUPS;

        G.m_bCommaDelimited = is_only_comma_delimited(G);
		if ( is_only_comma_with_pronoun_in_group(*this, G) ) return false; 
		if ( is_Pronoun_P_in_similar_groups(G) ) return false;
		
		if( bFound )
			for (size_t k=0; k < stack_size; k++) 
				if   (   (max_i >= GetGroups().size())
					  || (stack[k] > max_i))
				{
				  if(GetGroups()[stack[k]].m_RuleNo == -1) //значит эта группа из тезауруса и разбивать ея нехорошо
					  return false;

				  if( GetOpt()->is_firm_group(GetGroups()[stack[k]].m_GroupType) )
					  return false;

				  m_GroupsToRebuild.push_back(GetGroups()[stack[k]]);
				}
		
		// не будем менять граммемы для ОДНОР_ИГ, которые построены на им., вин. -
		// слишком много ошибок 
		if ((G.GetGrammems() & ( _QM(rNominativ) | _QM(rAccusativ))) == 0)
			for(int i = G.m_iFirstWord ; i <= G.m_iLastWord ; )
			{
				const CGroup& gr = get_maximal_subgroup(G,i);
					
				if( (gr.m_GroupType == NOUN_ADJ) || (gr.size() == 1) )
					change_words_in_group_grammems(gr, G.GetGrammems(), rAllCases);
				else
				{
					int iw = get_main_word_in_group(gr);
					change_words_in_group_grammems(CPeriod(iw, iw), G.GetGrammems(), rAllCases);
					
				}
				i += (gr.size() + 1);		
				
			}
		
		
		return true;
	};

 return false;

};
