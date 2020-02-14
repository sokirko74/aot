// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusFormatCaller.h"
#include "RusClause.h"
#include "RusSentence.h"


/*
	выбирает  тип морф. варианта из заданных vectorTypes со следующим предпочтением
	1. сначала берутс€ те типы, дл€ которых определена нормальна€ предикатна€ вершина;
	(if( node != -1 ))
	2. потом тип "“»–≈"
	3. потом пустыха, которой приравниваетс€  ќѕ”Ћ. 
*/



/*
	‘ункци€  удал€ет тип вершины ѕ–»„ј—“»≈, если это причастие вошло в группу неглавным словом
	‘ункци€  не делает этого дл€ клауз, которые уже были вложены как причатные обороты, а то получаетс€ 
	глупость: сначала вкладываем как причастный оборот, а потом удал€ем единственный оставшийс€ тип.
*/
void ChangeClauseTypeForParticiples(CClause& C)
{
	int ClauseNum = C.m_pSent->FindClauseIndexByPeriod(C);
	if (ClauseNum == -1) return;
	if (C.m_pSent->IsEnclosedClause(ClauseNum)) return;

	if (!C.HasType(PARTICIPLE_T)) return;
	int j = C.FindType(PARTICIPLE_T);
	int WordNo = C.m_vectorTypes[j].m_Root.m_WordNo;
	const CGroup* G = C.GetMaxLastHost(WordNo, C.m_vectorTypes[j].m_Root.m_HomonymNo);
	if (!G) return;

	int UnitNo = C.UnitNoByWordNo(WordNo);
	assert (UnitNo != -1);
	if (G->m_MainWordNo != WordNo)
		C.DeleteClauseType( j );
}



/*
здесь нельз€  использовать функцию WordNoByUnitNo, поскольку  синварианты еще не построены
*/
int CRusSentence::GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const
{
	int iHypPredk = 0;
	
	
	for (int i = 0; i < synVar.GetUnitsCount(); i++)
	{
		if ( synVar.GetUnitType(i) == EClause ) 
		{
			continue;
		};

		int iWord = synVar.m_SynUnits[i].m_SentPeriod.m_iFirstWord;
			
		const CSynHomonym& Hom = C.GetWords()[iWord].GetSynHomonym(synVar.GetHomNum(i));

		//значени€ части речи, которые может принимать предикат
		//глагол "ƒј¬ј“№" в императиве может сто€ть в одной клаузе с другим предикатом
		//"давайте будем дружить"
		//глаголы "—“јЌќ¬»“№—я", "ќ ј«ј“№—я" суть компоненты аналитических форм
		if ( C.GetOpt()->GetGramTab()->IsStrongClauseRoot( Hom.m_iPoses ) )

			if (!Hom.IsLemma("ƒј¬ј“№")&&
				!Hom.IsLemma("—“јЌќ¬»“№—я") &&
				!Hom.IsLemma("ќ ј«ј“№—я")&&
				!Hom.IsLemma("—“ј“№") &&
				!Hom.IsLemma("ћЌќ√ќ") &&
				!Hom.IsLemma("ћјЋќ"))
		{
				if ( C.m_pSent->FindFirstAuxVerb(iWord) == -1 )
					iHypPredk++;
		}
	}

	return iHypPredk;
};


void CRusSentence::AfterBuildGroupsTrigger(CClause& C)
{
	ChangeClauseTypeForParticiples(C);
};





