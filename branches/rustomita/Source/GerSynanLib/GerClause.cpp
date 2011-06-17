#include "GerSynan.h"
#include "GerFormatCaller.h"
#include "GerClause.h"
#include "GerSentence.h"


// The preferences  are the following:
//  1. if vectorTypes contains a not empty type, which suits to V  and which has no infinitive homonym, then it returns 
//    this type
//  2. Otherwise if vectorTypes contains a not empty type, which suits to V and which can be modal infinitiv ("sollen", "konnen"), then it returns 
//    this type
//  3. Otherwise if vectorTypes contains a not empty type, which suits to V and which can be infinitive, then it returns 
//    this type
//  4. Otherwise if vectorTypes contains a not empty type, which suits to V and which iz zu-infinitive, then it returns 
//    this type
//  5. Otherwise if vectorTypes contains an empty type, then it returns it.
// 
//  The point is that in the sentence " Ich will dass machen", 
// word "will" should be the root of the clause, not "machen".

void CGerSentence::ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V)
{

	int empty_type = -1; 
	int with_infinitive_homonym = -1;
	int zu_infinitive_homonym = -1;
	int with_modal_infinitive_homonym = -1;
	for(int i = 0 ; i < vectorTypes.size() ; i++ )
	{
		if( !vectorTypes[i].m_Root.IsEmpty() )
		{
			int WordNo = vectorTypes[i].m_Root.m_WordNo;
			int hom = vectorTypes[i].m_Root.m_HomonymNo;
			

			int iUnit = V.UnitNoByWordNo(WordNo);
			assert (iUnit != -1);
			if( V.m_SynUnits[iUnit].m_iHomonymNum == hom )
			{
				if 	(m_Words[WordNo].m_Homonyms[hom].HasPos (gZU_INFINITIV))
					zu_infinitive_homonym = i;
				else
				if	(m_Words[WordNo].m_Homonyms[hom].HasGrammem (gInfinitiv))
				{
					if (m_Words[WordNo].m_Homonyms[hom].HasGrammem (gModal))
						with_modal_infinitive_homonym = i;
					else
						with_infinitive_homonym = i;
				}
				else
				{
					if	(m_Words[WordNo].m_Homonyms[hom].HasPos(gVER))
					{
						V.m_ClauseTypeNo = i;
						return;
					}
					else
						with_infinitive_homonym = i;
				}
			}				
		}
		else
			empty_type = i;

	}

	if (with_modal_infinitive_homonym != -1)
		V.m_ClauseTypeNo = with_modal_infinitive_homonym;
	else
	if (with_infinitive_homonym != -1)
		V.m_ClauseTypeNo = with_infinitive_homonym;
	else
	if (zu_infinitive_homonym != -1)
		V.m_ClauseTypeNo = zu_infinitive_homonym;
	else
		V.m_ClauseTypeNo = empty_type;
}



int CGerSentence::GetCountOfStrongRoots(const CClause& C, const CMorphVariant& synVar) const
{
	int iHypPredk = 0;
	int Infinitives = 0;
	bool bInfinitiveConstruction = false;
	
	for (int i = 0; i < synVar.GetUnitsCount(); i++)
	{
		if ( synVar.GetUnitType(i) == EClause ) 
			continue;

		int iWord = synVar.m_SynUnits[i].m_SentPeriod.m_iFirstWord;
			
		const CSynHomonym& Hom = C.GetWords()[iWord].GetSynHomonym(synVar.GetHomNum(i));
		if ( C.GetOpt()->GetGramTab()->IsStrongClauseRoot( Hom.m_iPoses ) )
		{
				iHypPredk++;

				if (Hom.m_bInfinitiveConstruction)
					bInfinitiveConstruction = true;
				else
				if (Hom.HasGrammem(gInfinitiv) && Hom.HasPos(gVER))
					Infinitives++;

				
					
		};
	}

	if (iHypPredk > 1)
		if (bInfinitiveConstruction)
			iHypPredk -= max(1,  iHypPredk-Infinitives);


	return iHypPredk;
};


void CGerSentence::AfterBuildGroupsTrigger(CClause& C)
{
};





