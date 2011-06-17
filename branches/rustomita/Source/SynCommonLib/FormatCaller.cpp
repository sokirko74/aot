// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "FormatCaller.h"
#include "Clause.h"



CFormatCall::CFormatCall()  
{
	m_format = NULL; 
};

CFormatCall::CFormatCall(CFormatFunc _format, const char*  _name, int _dir) 
{ 
	m_format = _format;
	m_name =  _name;
	m_direction = _dir;
};






void	CFormatCaller::ChangeGroupType(CGroup& G, int NewGroupType)
{
	//  we should assign a new GroupId, otherwise there will be no appropriate update 
	// in  CClause::m_mapGroups
    for (int i=0; i< m_Relations.size(); i++)
        if (m_Relations[i].is_part_of(G) && m_Relations[i].type == G.m_GroupType)
            m_Relations[i].type = NewGroupType;

	G.m_GroupType = NewGroupType;
    
	
};

bool CFormatCaller::try_and_step_forward(CFormatCall* FC, CGroup& G, int rule)
{
  int save_rebuild_groups_size = m_GroupsToRebuild.size();

  if (   ((*this).*(FC->m_format))(G) )
  {
    // если во множестве m_GroupsToRebuild найдена группа,
	// которая построена с помощью тезауруса (СCroup::rule==-1),
	// тогда считается, что правило не отработало, поскольку такие группы 
	// перестраивать нельзя
    bool rebuild_groups_has_firm_group = false;

    if (save_rebuild_groups_size != m_GroupsToRebuild.size())
	  for (size_t k = 0; k<m_GroupsToRebuild.size(); k++)
		  if (m_GroupsToRebuild[k].m_RuleNo == -1)
			  rebuild_groups_has_firm_group = true;
  
    if (    find_group(G)  == -1 
		&& !rebuild_groups_has_firm_group
	   )
    { 
		// deleting all groups which should be rebuilt
	    for (size_t k = save_rebuild_groups_size; k<m_GroupsToRebuild.size(); k++)
              del_group_by_period(m_GroupsToRebuild[k]);

		// init group properties
		G.m_RuleNo = rule;
		SetMainWordNo(G);

		// inserting new group
		create_group(G);

		
		rml_TRACE("%s (%i, %i)  is created\n", GetOpt()->GetGroupNameByIndex(G.m_GroupType), G.m_iFirstWord, G.m_iLastWord);
		
		return true;
	}
  }
  
  /*
    до этого места мы доходим только тогда, когда  не удалось построить
	группу. Но при этом, возможно, форматка посчитала, что группу построить нужно, а 
	здесь, в этой процедуре группу отбраковали. Если форматка хотела 
	еще перестроить некоторые группы, тогда нужно отказаться от перестойки, поскольку
	сама форматка не применилась.
	Надо удалить только те группы, которые хотела перестроить именно эта форматка
	(они лежат за итератором save_rebuild_groups_size).
  */

  for (size_t k = save_rebuild_groups_size; k<m_GroupsToRebuild.size(); )
	  m_GroupsToRebuild.erase(m_GroupsToRebuild.begin()+k);


  return false;
};


int CFormatCaller::main_analyse()
{
	int WordNo;
	size_t i;

	CFormatCall FormatCall;
    CGroup Group;
	try
	{
		for (i=0; i < m_vectorRulesNumbers.size(); i++)
		{
			FormatCall = FormatCalls.find(m_vectorRulesNumbers[i])->second;
			for (	(FormatCall.m_direction == FROM_LEFT) ?  WordNo=0 : WordNo = sent.size() - 1; 
					(FormatCall.m_direction == FROM_LEFT) ?  (WordNo < sent.size()) : (WordNo >= 0) ; )

			{
				int s = get_maximal_group_no(WordNo);
				if( (s != -1 ) && (GetGroups()[s].m_iFirstWord != WordNo) )
				{
                    if (FormatCall.m_direction == FROM_LEFT)
                    {
                        WordNo = GetGroups()[s].m_iLastWord+1;
                        if (WordNo >= sent.size()) break;
                    }
                    else
                    {
                        WordNo = GetGroups()[s].m_iFirstWord;
                    }
					continue;
				}
					
                Group.Reset();
                Group.m_iFirstWord = WordNo;	
        		if (try_and_step_forward (&FormatCall,Group,m_vectorRulesNumbers[i]) )
                {
          		    // updating output pointer  to next word to be analysed
		            if (FormatCall.m_direction == FROM_LEFT) 
			            WordNo += Group.size();
		            else
			            WordNo--; 
                }
                else
                {
                     if (FormatCall.m_direction == FROM_LEFT)  
                    	  WordNo++; 
                    else 
	                        WordNo--; 
                }


                while (!m_GroupsToRebuild.empty())
				{
				  const CGroup& G = m_GroupsToRebuild.back(); 	
				  char rule = G.m_RuleNo;
				  CFormatCall& FormatCall = FormatCalls.find(rule)->second;
				  /*
				   если начало перестраиваемой группы указывает 
				   в середину другой группы Х, тогда начало надо сдвинуть на начало
				   группы Х, поскольку нельзя запускать 
				   try_and_step_forward от середины группы
				  */
				  const CGroup& LastHost  = get_maximal_group (G.m_iFirstWord);

				  /*
				   вообще говоря, может возникнуть ситуация, когда
				   try_and_step_forward, который вызывается в этом цикле, 
				   увеличит размер m_GroupsToRebuild,
				   тогда последний элемент этого массива изменится (ссылка G).
				   Поэтому удалить последний элемент нужно уже сейчас.	
				   Такое происходит во фразе  "случае сомнения, неуверенности, незнания"
				   где группы строятся в следующей последовательности:
				        groups				m_GroupsToRebuild
				     1. ГЕНИТ_ИГ(0,1)            0
					 2. ГЕНИТ_ИГ(0,1)			 0	
						ПРИЛОЖЕНИЕ(0,3)
					 3. ГЕНИТ_ИГ(0,1)		 ПРИЛОЖЕНИЕ(0,3)
					    ОДНОР_ИГ(3,5)
					 4. ОДНОР_ИГ(1,5)		 ГЕНИТ_ИГ(0,1)
						ОДНОР_ИГ(3,5)
					 5.	ОДНОР_ИГ(1,5)			0		 
						ОДНОР_ИГ(3,5)
						ГЕНИТ_ИГ(0,1)
					прим. ПРИЛОЖЕНИЕ - это разновидность  ОДНОР_ИГ, которая 
					не имеет собственной форматки
				  */
				  m_GroupsToRebuild.erase(m_GroupsToRebuild.begin()+m_GroupsToRebuild.size()-1);
				  
                  Group.Reset();
                  Group.m_iFirstWord = LastHost.m_iFirstWord;	
				  try_and_step_forward (&FormatCall,Group, rule);
				}

			};
		}
	}
	catch(...)
	{
		string strMsg;
		strMsg += " Failed main_analyse: \n";
		strMsg += "word: ";
		if (sent[WordNo].get_word())
			strMsg += sent[WordNo].get_word();

		strMsg += " \n clause: ";
		string strMsg1;
		get_full_sentence(strMsg1);
		strMsg += strMsg1;	 	 
		strMsg += "\n Syntax rule ";
		strMsg += FormatCall.m_name;
		GetOpt()->OutputErrorString(strMsg.c_str());
	}


	return 1;
}


int FindInList(const char* arrWords, int iLen, const char* word_upper)
{
	if (!word_upper) return -1;
	for(int j = 0 ; j < iLen ; j++ )
		if( !strcmp(word_upper, arrWords + (j * MAX_WORD_LEN)))
			return j;		

	return -1;

}





bool  CFormatCaller::create_groups_from_termin(const SFoundTermin& FoundTermin)
{ 
	if (FoundTermin.size()==1) return false;

	const CGroups* pModel = FoundTermin.m_pModel;

    vector<size_t> ThesCoord2Real;

    int iCoef = FoundTermin.m_iFirstWord;
    int Delta = iCoef;
    for (size_t i=FoundTermin.m_iFirstWord; i<= FoundTermin.m_iLastWord; i++) {
        ThesCoord2Real.push_back(i);
        if (sent[i].HasFlag(fl_oborot1)) 
            for (; i<=FoundTermin.m_iLastWord; i++) 
                if (sent[i].HasFlag(fl_oborot2))
                    break;
    }

	CIntVector groups_num;
	int GroupNo = 0;
	for(; GroupNo < pModel->GetGroups().size() ; GroupNo++ )
	{
		CGroup new_group;		
		const CGroup& group = pModel->GetGroups()[GroupNo];
		int save_first_word = ThesCoord2Real[group.m_iFirstWord];
		bool bGoodGroup = true;
		int RuleNo = GetRuleByGroupTypeForThesaurus(group.m_GroupType);
		if (RuleNo != -1)
		{
			CFormatCall& FormatCall = (*FormatCalls.find(RuleNo)).second;
			CFormatCall* pFormatCall = &FormatCall; 
            CGroup Gr;
            Gr.m_iFirstWord = save_first_word;	
			bGoodGroup = try_and_step_forward(pFormatCall, Gr, -1);
		}
		else
		{
            new_group.m_iFirstWord = ThesCoord2Real[group.m_iFirstWord];
			new_group.m_iLastWord = ThesCoord2Real[group.m_iLastWord];
			new_group.m_GroupType = group.m_GroupType;
			new_group.m_MainGroup.m_iFirstWord = ThesCoord2Real[group.m_MainGroup.m_iFirstWord];
			new_group.m_MainGroup.m_iLastWord  = ThesCoord2Real[group.m_MainGroup.m_iLastWord];
			SetMainWordNo(new_group);
			if( new_group.m_MainGroup.size() == 1 )
			{	
				assert( (new_group.m_MainGroup.m_iFirstWord >= 0) && ( new_group.m_MainGroup.m_iFirstWord < sent.size() ) );
				new_group.SetGrammems(sent[new_group.m_MainGroup.m_iFirstWord].GetGrammems());
			}
			else
			{
				int ii = find_group(new_group.m_MainGroup);
				assert( (ii >=0) && (ii != -1));
				new_group.SetGrammems (GetGroups()[ii].GetGrammems());
			}

				
			create_group(new_group);
				
		}
			
		if( !bGoodGroup )
		{
			for(int k = 0 ; k < GroupNo ; k++ )
			{
				CGroup group = pModel->GetGroups()[k];
				group.m_iFirstWord += iCoef;
				group.m_iLastWord += iCoef;
				int ii = find_group(group);
				if( ii != -1 )
				{
					del_group(ii);
					k--;
				}
			}
			return false;
		}

		if( (GroupNo == pModel->GetGroups().size() - 1)  && !GetGroups().empty() )
		{
			CGroup& G = *GetGroupPtr(GetGroups().size() - 1);
			G.m_iTerminID = FoundTermin.m_TerminId;
			G.m_iThesType = FoundTermin.m_ThesType;
		}	
	} //  end of for


	return GroupNo == pModel->GetGroups().size();
}


bool CFormatCaller::format_for_gra_fork(CGroup& G, int GroupType, Descriptors fl_start, Descriptors fl_end, bool bFirstWordIsMain )
{
	
    if (!sent[G.m_iFirstWord].has_des(fl_start)) return false;

	int FirstWord = -1;

	for(int i = G.m_iFirstWord ; i < sent.size(); i++)
	{
		if( get_maximal_group_size(i) > 1)
			return false;		

		if (sent[i].HasFlag(fl_le) || sent[i].HasFlag(fl_ile))
			FirstWord = i;


		if (	sent[i].has_des(fl_end) )
		{
			G.m_iLastWord = i;
			G.m_GroupType = GroupType;
			int MainWord = bFirstWordIsMain ? G.m_iFirstWord : G.m_iLastWord;
			/*
			 if the main word is not a lexeme and the group contains at least 
			 one lexeme then declare the first lexeme the main word. It is done for coordination 
			 in the sentence, for example for family-groups
			*/
			if (!sent[MainWord].HasFlag(fl_le) && !sent[MainWord].HasFlag(fl_ile))
				if (FirstWord != -1)
					MainWord = FirstWord;

			const CGroup& Main = get_maximal_group( MainWord );
			G.m_MainGroup = Main;
			for (size_t i  = G.m_iFirstWord;  i < G.m_iLastWord;  i++)
				if (MainWord != i)
					create_syn_rel(G,MainWord, i, GroupType);
				
			G.SetGrammems (Main.GetGrammems());

			
			return true;
		}
	};
	return false;
}

bool CFormatCaller::format_for_web_addr(CGroup& G)
{
	return format_for_gra_fork(G, GetOpt()->m_WWWGroupType, OFile1, OFile2, true);
}


// for example "ctrl-z"
bool CFormatCaller::format_for_keyb(CGroup& G)
{
	return format_for_gra_fork(G, GetOpt()->m_KEYBGroupType, OKey1, OKey2, true);
}


// process graphematical marks FAM1-FAM2
bool CFormatCaller:: format_for_FAM1_FAM2(CGroup& G)
{
	return format_for_gra_fork(G, GetOpt()->m_NameGroupType, OFAM1, OFAM2, false);
};



int CFormatCaller::FindEndOfExpression(int StartWordNo) const 
{

	for(size_t j = StartWordNo; j < sent.size() ; j++ )
	{
		if( Wj.HasFlag(fl_oborot2) ) 
			return j;	 

		if( Wj.HasFlag(fl_oborot1) && (j > StartWordNo ) ) 
			break;
	};

	assert(false);
	return -1;

};

int CFormatCaller::check_first_part_of_prep_noun (const CGroup& G, QWORD& depend_cases, int PREP_NOUN_GROUP_TYPE) const 
{
	
	if( get_maximal_group(G.m_iFirstWord).m_GroupType == PREP_NOUN_GROUP_TYPE )
		return -1;

	if ( G.m_iFirstWord >= sent.size()-1)  return -1;

	if (W1.get_oborot_ptr() && W1.HasFlag(fl_oborot1) && W1.get_oborot_ptr()->HasPartOfSpeech(GetOpt()->m_Preposition) ) 
	{
		// multiword preposition
		depend_cases = W1.get_oborot_ptr()->m_AllPossibleDependCases;
		return FindEndOfExpression(G.m_iFirstWord);
	}
	else
	{
		// single-word preposition
		if (		W1.get_simple_preps()
			&&		!W1.get_simple_preps()->empty()
			)
		{
			depend_cases = GetOpt()->m_pOborDic->m_Entries[(*W1.get_simple_preps())[0]].m_AllPossibleDependCases;
			return G.m_iFirstWord;

		}
	};

	return -1;
};





// process oborotEXPR1-EXPR1
bool CFormatCaller::format_for_oborots(CGroup& G)
{
	G.m_MainGroup = - 1;
	if (!sent[G.m_iFirstWord].HasFlag(fl_oborot1)) return false;
    if (get_maximal_group(G.m_iFirstWord).m_GroupType == GetOpt()->m_OborotGroupType) return false;

	for (G.m_iLastWord = G.m_iFirstWord; G.m_iLastWord < sent.size(); G.m_iLastWord++)
		if (sent[G.m_iLastWord].HasFlag(fl_oborot2) )
			break;
	
	if ( G.m_iLastWord == sent.size() ) return false; 
	if (G.m_iLastWord == G.m_iFirstWord)return false; 

	G.m_MainGroup = G.m_iFirstWord;	

	G.m_GroupType = GetOpt()->m_OborotGroupType;
	G.SetGrammems (sent[G.m_iLastWord].GetGrammems());
	
	// no relations  for  oborots, since they should  be fixed groups

	return true;
};



struct  DobleConjLess
{
	
	bool operator () (const SDoubleConj& X, const char* word_upper) const 
	{
			assert (X.m_FirstPart.size() > 0);
			assert (word_upper);
			return strcmp(X.m_FirstPart[0].m_item, word_upper) < 0;
	};
	bool operator () (const char* word_upper, const SDoubleConj& X ) const 
	{
			assert (X.m_FirstPart.size() > 0);
			assert (word_upper);
			return strcmp(word_upper, X.m_FirstPart[0].m_item) < 0;
	};
	bool operator () (const SDoubleConj& X1, const SDoubleConj& X2 ) const 
	{
			assert (X1.m_FirstPart.size() > 0);
			assert (X2.m_FirstPart.size() > 0);
			return strcmp(X1.m_FirstPart[0].m_item, X2.m_FirstPart[0].m_item) < 0;
	};

};




/*
this syntax rule finds a disruptive conjunction and builds a group with it.
for example:
  zwar ein paar Gefechte, aber keinen nennenswerten Widerstand.  (German)
  "как Иван, так и Мария" (Russian)
we compare m_Entries of a sisruptive conjunction with a wordform, since lemma can be different,
It is done for the following Russian example:
    "будь то доступ, или глава",
  in this example "будь" is lemmatized as "быть".

*/


bool CFormatCaller::format_for_disrupt_conj(CGroup& G)
{
	// we use a small vector for speed
	CSmallVector<int, 6> WordsOfConj;
	if (sent[G.m_iFirstWord].m_UnitType != EWord) return false;

	// ===   Russian part
	if (GetOpt()->m_Language == morphRussian)
	{
		/*
			наречия не могут наичинать  разрывный союз. 
			Если убрать это условие, тогда  во фразе "как посылать , так и получать"
			будет собираться группа с разрывным союзом, когда  "как" является наречием,
			возникают две параллельные группы с разрывным союзом.
		*/
		if (is_morph_adv(W1)) return false;
	};
	// ===  end

	// searching for  the first part of a conjunction in GetOpt()->m_pOborDic->m_DisruptConj
	vector<SDoubleConj>::const_iterator it = lower_bound (GetOpt()->m_pOborDic->m_DisruptConj.begin(), GetOpt()->m_pOborDic->m_DisruptConj.end(), sent[G.m_iFirstWord].get_upper_word(),DobleConjLess());

	// checking all conjunction with the same first part
	for ( ;
		      (it != GetOpt()->m_pOborDic->m_DisruptConj.end()) 
		   && sent[G.m_iFirstWord].is_word_upper(it->m_FirstPart[0].m_item);
		   it++)
	{		
		const SDoubleConj& vConj = *it;
		long CurrWordNo  = G.m_iFirstWord;
        if ((string(vConj.m_FirstPart[0].m_item) == "КАК") && !sent[G.m_iFirstWord].HasPOS(GetOpt()->m_Conjunction))
            break;

		/*
			we should invoke a special rule for repeating conjunction
			(only Russian )
		*/
		if ( vConj.m_bRepeating )
			if ( create_repeating_disrupt_conj(G, vConj) )
				return true;
			else
				continue;
		
		
		WordsOfConj.Clear();

		// saving parts of cojunction
		WordsOfConj.Add(CurrWordNo);

		// the first part of a conjunction can contain many words, we should
		// check these words, starting from the second word
		// since the first word was already checked
		CurrWordNo++;
		int l = 1;
		for (; l < vConj.m_FirstPart.size(); l++)
		{
			if (CurrWordNo == sent.size()) break;
			if ( !sent[CurrWordNo].is_word_upper(vConj.m_FirstPart[l].m_item) )
				break;


			// saving parts of cojunction
			WordsOfConj.Add(CurrWordNo);
			CurrWordNo++;
		};

		
		// if the whole first part was not equal to the current period of text, then exit 
		if (l < vConj.m_FirstPart.size()) continue;

		// if there is the end of the sentence after the first part, then exit
		if (CurrWordNo == sent.size()) continue;

		
		// pass the syntax group after the first part of the conjunction
		const CGroup& FirstGr = get_maximal_group(CurrWordNo);
		CurrWordNo = FirstGr.m_iLastWord+1;
		if (CurrWordNo == sent.size()) continue;

		// there can be a a comma after this  group 
		if ( sent[CurrWordNo].HasFlag(fl_comma) ) 
			CurrWordNo++;

		if (CurrWordNo == sent.size()) continue;

		//  should be an atomic group or oborot
		const CGroup&  SecondConjPartGroup = get_maximal_group(CurrWordNo);
		if	(		(	SecondConjPartGroup.size()!= 1 )
				&& 	(SecondConjPartGroup.m_GroupType != GetOpt()->m_OborotGroupType)
			) 
			continue;

		//  checking the second  part of the current conjunction
		for (l = 0; l < vConj.m_SecondPart.size(); l++)
		{
			if (CurrWordNo == sent.size()) break;
			if ( !sent[CurrWordNo].is_word_upper(vConj.m_SecondPart[l].m_item) )
				break;
			WordsOfConj.Add(CurrWordNo);
			CurrWordNo++;
		};

		// if the second  part differs  from the text, then exit
		if (l < vConj.m_SecondPart.size()) continue;

		// if there is the end of the sentence after the second part, then exit
		if (CurrWordNo >= sent.size()) continue;

		
		//  can be a parenthesis
		if (sent[CurrWordNo].HasFlag(fl_oborot1) )
        {
            assert ( sent[CurrWordNo].get_oborot_ptr() != 0 );
			if (sent[CurrWordNo].get_oborot_ptr()->m_bParenthesis)
				CurrWordNo = FindEndOfExpression(CurrWordNo)+1;
        }
		
        // if there is the end of the sentence after the second part, then exit
		if (CurrWordNo >= sent.size()) continue;

		//  can be a particle
		if (GetGramTab()->IsSimpleParticle(sent[CurrWordNo].get_lemma(), sent[CurrWordNo].GetPoses()))
			CurrWordNo = get_next_main_word (CurrWordNo);

		if (CurrWordNo == sent.size()) continue;


		/*
			function create_disrupt_conj_group creates a group for disruptive conjunction and
			checks  the morphological equality of the two compound parts. 
			If part of speeches are not equal, then we cannot create a group. Here are
			examples of such non-equality:
				"Не красивая, а огород". (Russian)
				"Er kauft einesteils Autos, andernteils kauft er Fahrrдder" (German)

		*/
		const CGroup* pSecondGr = create_disrupt_conj_group(G, &FirstGr, &(get_maximal_group(CurrWordNo)),true);

		
		if ( !pSecondGr )
		{
			// no morphologically equal group is found
			continue;
		};

		// saving  OborotId for future proceedings
		sent[G.m_iFirstWord].m_DisruptOborotId = vConj.m_iOborNum;


		// create a relation from the first word  of the conjunction to 
		// the main word of the first group
		create_syn_rel(G, G.m_iFirstWord, get_main_word_in_group(FirstGr), G.m_GroupType);


		/*
		 create chain relation from the one conjunction part to another .
		 This realations  will be uses to  create fixed groups for semantics
		*/
		for (l = 1; l < WordsOfConj.m_ItemsCount ; l++)
			create_syn_rel(G, WordsOfConj.m_Items[l-1], WordsOfConj.m_Items[l], GetOpt()->m_DisruptConjRelation);

		/*
			We should  rebuild some groups, which now are invalid, because of the second compound group.
			Examples
			Russian: "и красивый, и жалкий человек".	 Здесь был нарушена группа "жалкий человек"
			German: Er kauft weder alte noch moderne Fahrrдder. 
		*/
		
		for (l = GetGroups().size()-1; l >= 0; l--)
				if	(		GetGroups()[l].m_iFirstWord == pSecondGr->m_iFirstWord 
						&&	GetGroups()[l].m_iLastWord > pSecondGr->m_iLastWord
					)
					m_GroupsToRebuild.push_back(GetGroups()[l]);


		return true;
	}

	return false;
};



/*

This function creates group for  repeating conjunctions.  A repeating  conjunction can 
connect two or more groups while a normal disruptive conjunction can connect only 
two groups.

German examples
	Er kauft teils Autos, teils Fahrrдder

Russian examples:
   и красивый, и строгий человек
   и Петя, и Ваня
   и в лесу , и в огороде
   либо в лес, либо домой
   то очень долго идти, то пилить дрова, то курить
*/
bool CFormatCaller::create_repeating_disrupt_conj(CGroup& G, const SDoubleConj& pConj)
{
	// we use a small vector for speed
    const size_t MaxConjCount = 20;
	// all conjunction parts
	CSmallVector<int,MaxConjCount> WordsOfConj;

	// all groups which are connected by the conjunction
	CSmallVector<const CGroup*,MaxConjCount> DisruptGroups; 

	long CurrWordNo = G.m_iFirstWord;

	// the first part of a conjunction can contain many words, we should
	// check these words, starting from the second word
	// since the first word was already checked
	for (;  CurrWordNo < sent.size(); CurrWordNo)
	{
		int l = 0;
		for (; l < pConj.m_FirstPart.size(); l++)
		{
			if (CurrWordNo == sent.size()) break;
			if ( !sent[CurrWordNo].is_word_upper(pConj.m_FirstPart[l].m_item) )
				break;
			WordsOfConj.Add(CurrWordNo);
			CurrWordNo++;
		};
		// if the whole first part was not equal to the current period of text, then exit 
		if (l < pConj.m_FirstPart.size()) break;


		// if there is the end of the sentence after the first part, then exit
		if (CurrWordNo == sent.size()) continue;

		const CGroup& CurrGr = get_maximal_group(CurrWordNo);
		CurrWordNo = CurrGr.m_iLastWord+1;
		DisruptGroups.Add(&CurrGr);

		// threre should be a comma after the group
		if (CurrWordNo == sent.size()) break;
		if ( !sent[CurrWordNo].HasFlag(fl_comma) ) break;
		CurrWordNo++;

	};

	// at least two groups  should be found
	if ( DisruptGroups.size() < 2 ) return false; 

	{
		//  checking morphological equality of the connected groups
		long CountMorphEqualGroupPairs = 0;
		for ( int jj = 1; jj < DisruptGroups.size(); jj++ )
		{
			// Function create_disrupt_conj_group checks the morphologically equaluty.
			// We should compare all groups to the first by part of speech, if an error is found
			// then break the process of comparing by the last "good" group.
			if ( !create_disrupt_conj_group(G, DisruptGroups[0], DisruptGroups[jj], true) ) break;

			CountMorphEqualGroupPairs++;


			//  adding groups  which should be rebuild
			bool bShouldRebuild = false;
			for (int m = GetGroups().size()-1; m >= 0; m--)
				if	(		GetGroups()[m].m_iFirstWord == DisruptGroups[jj]->m_iFirstWord 
						&&	GetGroups()[m].m_iLastWord > G.m_iLastWord
					)
				{
					m_GroupsToRebuild.push_back(GetGroups()[m]);
					bShouldRebuild = true;
				}

			//  if a group to rebuild was found then we should cut the row of groups here
			//  because there is no hope to prolong it any more, even though some items of 
			//  repeating conjunction  were found after it
			if ( bShouldRebuild ) break;
		}

		if ( CountMorphEqualGroupPairs == 0 ) return false;

		if (CountMorphEqualGroupPairs+1 != DisruptGroups.size())
            if ((CountMorphEqualGroupPairs+1)*pConj.m_FirstPart.size() < MaxConjCount)
		    {
			    // resing the vector of the connected groups.
			    DisruptGroups.m_ItemsCount =  CountMorphEqualGroupPairs+1;
			    WordsOfConj.m_ItemsCount = (CountMorphEqualGroupPairs+1)*pConj.m_FirstPart.size();
		    };
	};
	
	sent[G.m_iFirstWord].m_DisruptOborotId = pConj.m_iOborNum;

	// create a relation from the first word  of the conjunction to 
	// the main word of the first group
	create_syn_rel(G, G.m_iFirstWord, get_main_word_in_group(*DisruptGroups[0]), G.m_GroupType);

	/*
		create chain relation from the one conjunction part to another .
		This realations  will be uses to  create fixed groups for semantics
	*/
	for (long l = 1; l < WordsOfConj.m_ItemsCount ; l++)
			create_syn_rel(G, WordsOfConj.m_Items[l-1], WordsOfConj.m_Items[l], GetOpt()->m_DisruptConjRelation);
		


	// ===   Russian part
	if (GetOpt()->m_Language == morphRussian)
	{
		/*
		случай "и", "или", "да", когда перед первой компонентой группы данный союз факультативен. 
		пробуем распространить группу влево, Например:
			"Иван, или Мария, или Петр."
		*/
		if	(		( G.m_iFirstWord > 2  ) 
				&&	sent[G.m_iFirstWord-1].HasFlag(fl_comma) 
			)
		{
			const CGroup* GroupWithoutConj = &(get_maximal_group(G.m_iFirstWord-2));
			CGroup GroupTest = G; 	

			/*
				последний параметр в create_disrupt_conj_group должен быть false, поскольку 
				здесь нужно запретить перестройку групп
			*/
			if ( create_disrupt_conj_group(GroupTest, DisruptGroups[0], GroupWithoutConj, false) ) 
			{
				create_syn_rel(G, G.m_iFirstWord, get_main_word_in_group(*GroupWithoutConj), G.m_GroupType);
				G.m_iFirstWord = GroupWithoutConj->m_iFirstWord;
			};
		};
	};


	return true;
	
}


const CGroup* CFormatCaller::create_disrupt_conj_group(CGroup& G, const CGroup* pFirstGr, const CGroup* pSecondGr, bool pbSearchSubGroups) 
{
	size_t main_word_in_first_group = get_main_word_in_group(*pFirstGr);
	size_t main_word_in_second_group = get_main_word_in_group(*pSecondGr);
	/*
	 first check the case "entweder zu Hause  oder dort", when there is no formal coordination 
	 by part of speech
	*/
	if	(	(		 (pFirstGr->m_GroupType == GetOpt()->m_PrepNounGroupType)
				&&	is_morph_adv(sent[main_word_in_second_group])
		    )
		  ||(    (pSecondGr->m_GroupType == GetOpt()->m_PrepNounGroupType)
			  && is_morph_adv(sent[main_word_in_first_group])
			)
		)
	{
		
	}
	else
	{
		/*
		  if pbSearchSubGroups==true, then we can search a appropriate group in subgroups 
		  It follows, that by creating the main disruptive group we can make invalid 
		  group pSecondGr or some of its children.
		*/
		if (pbSearchSubGroups)
		{
			/*
			 going  through all groups which are started from the  the first word of pSecondGr (including pSecondGr), 
			 if the some subgroup is morphologically equal to the frist group,
			 then  we say, we have found the result. If the found group was not pSecondGr (a topmost group),
			 then we should rebuild all parents of the found group
			 For example, "entweder kleines oder altes Baby",  here group "altes Baby" is  
			 invalid, because there will be a non-empty intersection between "entweder kleines oder altes".
			*/
			int k = get_maximal_group_no(pSecondGr->m_iFirstWord);
			bool bFound = false;
			if (k != -1)
				for (; k >= 0; k--)
				{
					if (GetGroups()[k].m_iFirstWord == pSecondGr->m_iFirstWord)
					{
						int iWrd = get_main_word_in_group(GetGroups()[k]);
						if (sent[iWrd].GetPoses() == sent[main_word_in_first_group].GetPoses()) 
						{
							pSecondGr = &(GetGroups()[k]);
							main_word_in_second_group = get_main_word_in_group(*pSecondGr);
							bFound = true;
							break;
						}
					}
				}
			if (!bFound)
			{
				// if no appropriate multiword group is found then check atomic group
				if ( sent[pSecondGr->m_iFirstWord].GetPoses() == sent[main_word_in_first_group].GetPoses())
				{
						main_word_in_second_group = pSecondGr->m_iFirstWord;
						pSecondGr = &(m_AtomicGroups[pSecondGr->m_iFirstWord]);

				};


			};
		}

		//  exit, if no no appropriate group is found 
		if (sent[main_word_in_second_group].GetPoses() != sent[main_word_in_first_group].GetPoses()) 
				return 0;
	   /*
		check coordination by case if groups are NP
	   */
		const CSynPlmLine& W_1 = sent[main_word_in_first_group];
		const CSynPlmLine& W_2 = sent[main_word_in_second_group];
		if (   W_1.is_morph_noun() 
			|| is_morph_pronoun(W_1)
		   )
		   if (!GetGramTab()->GleicheCase(W_2.m_gramcodes, W_1.m_gramcodes))
				return 0;


	   /*
		check coordination by case, number and gender if groups are AdjP
	   */
		if ( is_left_noun_modifier(W_1)   )
			if (!GetGramTab()->GleicheGenderNumberCase(W_2.m_type_gram_code, W_2.m_gramcodes, W_1.m_gramcodes))
				return 0;

		
	}

	G.m_GroupType = GetOpt()->m_DisruptConjGroupType;
	G.m_iLastWord = pSecondGr->m_iLastWord;
	G.m_MainGroup = *pFirstGr;
	G.SetGrammems ( (sent[main_word_in_first_group].GetGrammems() & sent[main_word_in_second_group].GetGrammems()));

	create_syn_rel(G, G.m_iFirstWord, main_word_in_second_group, G.m_GroupType);

	return pSecondGr;
}



bool CFormatCaller::is_left_noun_modifier (const CSynPlmLine& L) const
{
	return		GetOpt()->GetGramTab()->is_left_noun_modifier (L.GetPoses(), L.GetGrammems()) 
			||	L.HasFlag(fl_digit);;
};



bool CFormatCaller::is_morph_noun(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->IsMorphNoun(L.GetPoses());
};
bool CFormatCaller::is_syn_noun(const CSynPlmLine& L) const
{
	return L.is_syn_noun();
};
bool CFormatCaller::is_morph_adj(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_adj(L.GetPoses());
};
bool CFormatCaller::is_morph_article(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_article(L.GetPoses());
};
bool CFormatCaller::is_morph_adv(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_adv(L.GetPoses());
};
bool CFormatCaller::is_morph_participle(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_participle(L.GetPoses());
};
bool CFormatCaller::is_morph_predk(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_predk(L.GetPoses());
};
bool CFormatCaller::is_verb_form(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_verb_form(L.GetPoses());
};
bool CFormatCaller::is_morph_pronoun(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_pronoun(L.GetPoses());
};

bool CFormatCaller::is_numeral(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_numeral(L.GetPoses());
};
bool CFormatCaller::is_infinitive(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_infinitive(L.GetPoses());
};
bool CFormatCaller::is_morph_pronoun_adjective(const CSynPlmLine& L) const
{
	return GetOpt()->GetGramTab()->is_morph_pronoun_adjective(L.GetPoses());
};
