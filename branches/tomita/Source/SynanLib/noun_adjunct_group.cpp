// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"



// Он, честный человек, ушел в горы.
bool CRusFormatCaller::format_for_noun_adjunct_group (CGroup& G)
{
	const CGroup& MaxGrp = get_maximal_group(G.m_iFirstWord);

	/*
	 конвертирует группу ОДНОР_ИГ а группу ПРИЛОЖЕНИЕ, если она состоит только из 
	 двух членов, разделенных запятой, и сразу  после  ОДНОР_ИГ идет запятая.
	 Он, честный человек, ушел в горы.
	*/

	if	(		(MaxGrp.m_GroupType == SIMILAR_NOUN_GROUPS)
			&&	(MaxGrp.m_bCommaDelimited)
			&&	(MaxGrp.m_iLastWord+1 < sent.size())
			&&	sent[MaxGrp.m_iLastWord+1].HasFlag(fl_comma)
		)
		{
			const CGroup& Child1 = get_maximal_subgroup(MaxGrp, G.m_iFirstWord);
			if (Child1.m_iLastWord < MaxGrp.m_iLastWord) 
			{
				const CGroup& Child2 = get_maximal_subgroup(MaxGrp, Child1.m_iLastWord + 2);
				if (Child2.m_iLastWord == MaxGrp.m_iLastWord) 
				{
					CGroup& D = *GetGroupPtr(get_maximal_group_no(G.m_iFirstWord));
					ChangeGroupType(D, NOUN_ADJUNCT);
					return false;
				};
			};
		};

	/*
		строим группу приложения с нуля
	*/
	int i = get_main_word(G.m_iFirstWord); 
	if (!Wi.is_syn_noun()) return false;

	int j = get_next_main_word(i); 
	if (j == sent.size()) return false;
	if (!Wj.HasFlag(fl_comma))  return false;


	j = get_next_main_word(j); 
	if (j == sent.size()) return false;
	if  (!Wj.is_syn_noun()) return false;


	j = get_next_main_word(j);
	if (j == sent.size()) return false;
	if (!Wj.HasFlag(fl_comma))  return false;

	G.m_iLastWord = j -1;
	G.m_GroupType = NOUN_ADJUNCT;
	G.m_MainGroup = MaxGrp;
	G.SetGrammems( MaxGrp.GetGrammems() );
	
	return true;
};
