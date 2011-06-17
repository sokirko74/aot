// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "StdSynan.h"
#include "RusFormatCaller.h"


bool CRusFormatCaller::CanConnectSimilar(const CSynPlmLine& L) const
{
	if( L.HasFlag(fl_comma) )
		return true;
	else
        if (L.HasPOS(CONJ))
        {
            int conj_no  = L.get_coord_conj_no();
				if( conj_no  !=  -1)  
					if( !GetOpt()->m_pOborDic->GetCoordConjs()[conj_no].should_be_after_comma )
						return true;
        }

	return false;
};


bool CRusFormatCaller::is_Pronoun_P_in_similar_groups(const CGroup& G)
{
	size_t i;

	for (i = G.m_iFirstWord; i <= G.m_iLastWord; i++)
	{
        if (Wi.HasPOS(PRONOUN_P)) 
		{
			const CGroup& GrPron_P = get_maximal_group(i);
			if (GrPron_P.m_iLastWord == G.m_iLastWord || GrPron_P.m_iFirstWord == G.m_iFirstWord)
				if (GrPron_P.m_iFirstWord == GrPron_P.m_iLastWord)
					return true;
				else
					if (GrPron_P.m_MainWordNo == i)
						return true;
		}
	}

	return false;

};


