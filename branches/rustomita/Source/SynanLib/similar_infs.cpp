// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"

/*
Примеры:
  пить и курить
  придти, увидеть, победить

встать и уйти

*/

/*
  Раньше при сборке инфинитивов проверялось согласование по виду. Теперь
   решили отказаться от этого из-за примеров:
   "Мне нужно встать и идти" (св и нс)
   "чтобы делать и сделать" (нс и св)
*/
bool CRusFormatCaller::format_for_similar_infs(CGroup& G)
{
	int i =  get_main_word(G.m_iFirstWord);

	if (!is_infinitive(Wi)) return false;
	int last_member = i;
	for (int k  = get_next_main_word(i);k != sent.size(); k  = get_next_main_word(k))
	{
		if (!IsSimilarConnector(Wk) )  
			break;
		bool bLastMember  = !Wk.HasFlag(fl_comma);
		if (bLastMember)
            G.m_bCommaDelimited = false;

		
		k =  get_next_main_word(k);
		if (k == sent.size()) 
			break;

		if (!is_infinitive(Wk)) 
		{
			break;
		};

		last_member = k;
		if (bLastMember) break;
		
	};
	if (last_member == i) return false;

	G.m_Cause = "Соч. группа инфинитивов, соединенная запятыми и простыми союзами";
	G.m_GroupType = SIMILAR_INFS;
	G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
	G.m_iLastWord = get_maximal_group(last_member).m_iLastWord;
	return true;
};

