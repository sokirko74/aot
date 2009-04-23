// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "Clause.h"
#include "Sentence.h"



/*
	возвращает истину, если был удален хотя бы один синтаксический вариант
*/
bool CClause::KillHomonymsCoverage( )
{
	

	bool bResult = false;
	if (m_SynVariants.empty()) return bResult;
	long BestWeight = m_SynVariants.begin()->m_iWeight;

	
	//удаляем все варианты, у которых вес меньше самого лучшего
	int VarsCount  = m_SynVariants.size();
	for (SVI pSynVar = m_SynVariants.begin(); pSynVar != m_SynVariants.end(); )
	{
		const CMorphVariant& V = *pSynVar;
		if (pSynVar->m_iWeight < BestWeight)
		{
			pSynVar = EraseMorphVariant(pSynVar );
		}
		else
			pSynVar++;
	};

	rml_TRACE ("KillHomonymsCoverage left only %i from %i synvariants in %s\n", m_SynVariants.size(), VarsCount, GetTraceStr().c_str());

	
	//  удаляем все омонимы, который не входят в оставшиеся варианты
	for(int WordNo = m_iFirstWord ; WordNo <= m_iLastWord ; WordNo++ )
	{
		WordNo = PassSubClause(WordNo);
		if (WordNo > m_iLastWord) break; 		
        if (m_SynVariants.begin()->IsInSubclause(WordNo)) continue;
        GetWords()[WordNo].SetHomonymsDel(true);
		for (CSVI pSynVar = m_SynVariants.begin(); pSynVar != m_SynVariants.end(); pSynVar++)
		{
            int iHom = pSynVar->m_SynUnits[UnitNoByWordNo(WordNo)].m_iHomonymNum;
            CSynHomonym & H = GetWords()[WordNo].m_Homonyms[iHom];
			H.m_bDelete =  false;
		};
	};

    m_pSent->DontDeleteExpr2ifExpr1Exists(*this);

	for(int WordNo = m_iFirstWord ; WordNo <= m_iLastWord ; WordNo++ )
	{
		size_t SaveHomonymsCount = GetWords()[WordNo].m_Homonyms.size();
		DeleteMarkedHomonyms(WordNo);

		if (SaveHomonymsCount > GetWords()[WordNo].m_Homonyms.size())
			bResult = true;
    }

	return bResult;
};

