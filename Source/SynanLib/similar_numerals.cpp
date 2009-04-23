// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


/*
Примеры:
	2,3 и 4
	два и три
	но не второй и третий - это однор. прил.
*/

bool CRusFormatCaller::format_for_similar_numerals(CGroup& G)
{
	int i =  get_main_word(G.m_iFirstWord);
	if( !is_numeral(Wi) && !Wi.HasFlag(fl_digit) )
	return false;

	G.SetGrammems( Wi.GetGrammems() );
	if( Wi.HasFlag(fl_digit) )
		G.SetGrammems( rAllCases );

	G.m_iLastWord = G.m_iFirstWord;

	for (size_t k  = G.m_iFirstWord+get_maximal_group_size(i); 
					k < sent.size()-1; 
					k = G.m_iLastWord+1)
	{
		if (!IsSimilarConnector(Wk))  break;
		int j =  get_main_word(k+1);
		if( !is_numeral(Wj) && !Wj.HasFlag(fl_digit) ) break;

		if( is_numeral(Wj) )
			if( !( G.GetGrammems() & Wj.GetGrammems() & rAllCases) )
				break;

		if( is_numeral(Wj) )
			G.SetGrammems( G.GetGrammems() &  Wj.GetGrammems() );

		G.m_iLastWord =  k + get_maximal_group_size(j);
	}

	if (G.m_iLastWord != G.m_iFirstWord)
	{
		G.m_GroupType = SIMILAR_NUMERALS;
		G.m_MainGroup = get_maximal_group(G.m_iFirstWord);
		return true;
	}

	return false;
}
