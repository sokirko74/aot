
#include "StdSynan.h"
#include "RusSentence.h"

void InitCopulList(StringVector& v_CopulList)
{
	v_CopulList.clear();
	v_CopulList.push_back("как");
	v_CopulList.push_back("словно");
	v_CopulList.push_back("будто");
	v_CopulList.push_back("что");
	v_CopulList.push_back("точно");
}

bool HasAdjInNom(const CSynWord& _W)
{

	for (int i = 0; i < _W.m_Homonyms.size(); i++)
		if (_W.m_Homonyms[i].HasPos(ADJ_FULL) 
			&& _W.m_Homonyms[i].HasGrammem(rNominativ))
			return true;
		
	return false; 

}


bool HasNounInGen(const CSynWord& _W)
{

	for (int i = 0; i < _W.m_Homonyms.size(); i++)
		if (		_W.m_Homonyms[i].IsSynNoun()
				&&	_W.m_Homonyms[i].HasGrammem(rGenitiv)
			)
			return true;
		
	return false; 

}


bool HasNounInNomSgPl(const CSynWord &W)
{
	for (int i = 0; i < W.m_Homonyms.size(); i++)
		if ( W.m_Homonyms[i].IsSynNoun() ) 
			if ( W.m_Homonyms[i].HasGrammem(rNominativ) &&
				 W.m_Homonyms[i].HasGrammem(rSingular) && 
				 W.m_Homonyms[i].HasGrammem(rPlural) )
				return true;
		
	return false; 
}


CSynWord CreateDash(CSentence* pSent)
{
	CSynWord W (pSent);
	CSynHomonym NewDashHom (pSent);
	NewDashHom.SetLemma("-");

    W.SetWordStr("-", pSent->GetOpt()->m_Language);
	W.m_bDash = true;
	W.AddDes(OPun);
    W.AddDes(OHyp);
	W.m_bArtificialCreated = true;
	W.m_Homonyms.push_back(NewDashHom);
	return W;
};


void CRusSentence::BuildDash(int iClauseNum, int iWrdAfter, bool bCopul)
{
	SClauseType type;
	CClause&  pClause = GetClause(iClauseNum);

	if (bCopul)
	{
		pClause.ChangeAllClauseTypesToOneType(SClauseType(COPUL_T, -1,-1));		

		return;
	}

	if ( m_Words[iWrdAfter].IsInOborot() )
			return;

	if (pClause.IsInTermin(iWrdAfter)) return;

	if (IsBetweenGraphematicalPairDescriptors(iWrdAfter)) return;

	int DashWordNo = iWrdAfter + 1;
	m_Words.insert(m_Words.begin() + DashWordNo, CreateDash(this));

	RecalculateIndicesAfterInsertingWord(DashWordNo);
	pClause.ChangeAllClauseTypesToOneType(SClauseType(DASH_T, DashWordNo, 0) );		
}


void CRusSentence::TryToRebuildDashInClause()
{
	StringVector v_CopulList;
	InitCopulList(v_CopulList);

	for (int ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++)
	{
		CClause&  pClause = GetClause(ClauseNo);

		if (!pClause.m_vectorTypes.empty()) 
			continue;

		bool bHasDash = false;
		if ( pClause.m_iFirstWord != 0 )
			for ( int tt = pClause.m_iFirstWord; tt <= pClause.m_iLastWord; tt++ )
				if ( m_Words[tt].m_bDash  )
				{
					bHasDash = true;
					break;
				}

		if ( bHasDash ) continue;


		int j = pClause.m_iFirstWord;
		for (; j <= pClause.m_iLastWord; j++)
			if (	!m_Words[j].GetSynHomonym(0).IsLemma("У")
				&&	m_Words[j].GetHomonymByPOS(PREP) != -1 
				)
				 break;

		if ( j < pClause.m_iLastWord ) continue;

		vector<int> Noun_Nom, Adj_Nom, Eto, Noun_Gen, Copul_Words, Noun_NomSgPl;
		int Prep_U = -1;

		for (j = pClause.m_iFirstWord; j <= pClause.m_iLastWord; j++)
		{
			if ( m_Words[j].GetSynHomonym(0).IsLemma("ЭТО")	)
			{
				Eto.push_back(j);
				continue;
			}

			for (int ll = 0; ll < v_CopulList.size(); ll++)
				if ( m_Words[j].m_strWord ==  v_CopulList[ll].c_str() )
				{
					Copul_Words.push_back(j);
					continue;
				}

			if ( m_Words[j].GetHomonymsCount() > 1 ) continue;

			if ( m_Words[j].GetSynHomonym(0).IsLemma("КОТОРЫЙ") )
				continue;

			if (		HasNounInNom( m_Words[j]) 
                && 	!m_Words[j].HasDes(OFAM2) 
					)
			{
				Noun_Nom.push_back(j);
				continue;
			
			}
			if (	HasAdjInNom(m_Words[j]) && !m_Words[j].HasDes(OFAM2) )
			{
				Adj_Nom.push_back(j);
				continue;
			}

			if ( m_Words[j].GetSynHomonym(0).IsLemma("У") )
			{
				Prep_U = j;
				continue;
			}

			if ( HasNounInGen(m_Words[j]) )
				Noun_Gen.push_back(j);

			if ( HasNounInNomSgPl( m_Words[j] ) )
				Noun_NomSgPl.push_back(j);
			
		}
		
		if (0 == Noun_Nom.size() && 0 == Eto.size() && 
			0 == Adj_Nom.size() && -1 == Prep_U)
			continue;

		if (Noun_Nom.size() > 1)
		{
			BYTE up_let = m_Words[Noun_Nom[1]].m_strWord[0];
			if (!is_russian_upper(up_let))
			{
				int k = 0;
				for (; k < Copul_Words.size(); k++)
					if (Copul_Words[k] > Noun_Nom[0] && Copul_Words[k] < Noun_Nom[1])
					{
						BuildDash(ClauseNo, -1, true);
						break;
					}

				if ( k == Copul_Words.size())
					BuildDash(ClauseNo, Noun_Nom[0]);
			}

			continue;
		}

		if ( Noun_Nom.size() > 0 && Eto.size() > 0 )
		{
			QWORD tormoz = _QM(rNeutrum) |  _QM(rSingular) |  _QM(rNominativ); 
			for (int k = 0; k < Eto.size() && k < Noun_Nom.size(); k++)
				//  без "тормоза" во фразе "это облако" восстановится тире
				if	(!m_Words[Noun_Nom[k]].GetSynHomonym(0).HasSetOfGrammemsExact(tormoz) )
					if (Noun_Nom[k] > Eto[k])
					{
						BuildDash(ClauseNo, Eto[k]);
						continue;
					}
		}		

		bool bPersCl, bEncCl;
		bPersCl = bEncCl = false;

		int cc = ClauseNo-1;
		for (; ClauseNo > 1 && cc >= 0; cc--)
		{
			CClause&  pClauseBefore = GetClause(cc);
		
			if (	pClauseBefore.HasType(VERB_PERS_T) 
					&& pClauseBefore.HasLeftStarter() 
				)
			{
					bEncCl = true;
					continue;
			}

			if ( pClauseBefore.HasType(PARTICIPLE_T) || pClauseBefore.HasType(ADVERB_PARTICIPLE_T) )
			{
					bEncCl = true;
					continue;
			}

			if ( pClauseBefore.HasType(DASH_T) )
				break;

			if (		(		!pClauseBefore.HasType(INFINITIVE_T) 
							&&  !pClauseBefore.m_vectorTypes.empty()
						)
					||  !(pClauseBefore.m_vectorTypes.size() == 1) 
				)
			{
				if (bEncCl)
					bPersCl = true;

				break;
			}

		}

		if (bPersCl) continue;

		bPersCl = bEncCl = false;

		for (cc = ClauseNo+1; ClauseNo < GetClausesCount()-2 && cc < GetClausesCount(); cc++)
		{
			CClause&  pClauseAfter = GetClause(cc);
		
			if (	pClauseAfter.HasType(VERB_PERS_T) 
				&&	pClauseAfter.HasLeftStarter() 
				)
			{
					bEncCl = true;
					continue;
			}

			if ( pClauseAfter.HasType(PARTICIPLE_T) || pClauseAfter.HasType(ADVERB_PARTICIPLE_T) )
			{
					bEncCl = true;
					continue;
			}

			if ( pClauseAfter.HasType(DASH_T) )
				break;

			if (	(			!pClauseAfter.HasType(INFINITIVE_T) 
						&& 		!pClauseAfter.m_vectorTypes.empty()
					) 
				||  !(pClauseAfter.m_vectorTypes.size() == 1) 
				)
			{
				if (bEncCl)
					bPersCl = true;

				break;
			}

		}
		
		if (bPersCl) continue;

		if ( Noun_Nom.size() > 0 && Adj_Nom.size() > 0 )
		{
			for (int k = 0; k < Noun_Nom.size() && k < Adj_Nom.size(); k++)
				if (Noun_Nom[k] < Adj_Nom[k])
						BuildDash(ClauseNo, Noun_Nom[k]);

			continue;
		}		
		
		if (-1 != Prep_U && Noun_Gen.size() > 0 && ( Noun_Nom.size() > 0 || Noun_NomSgPl.size() > 0 ) )
		{		
			for (int m = 0; m < Noun_Gen.size(); m++)
				if (Prep_U < Noun_Gen[m])
				{
					Prep_U = Noun_Gen[m];
					break;
				}
			if ( Noun_Nom.size() > 0 )
			{
				if ( Prep_U > Noun_Nom[0] )
						BuildDash(ClauseNo, Noun_Nom[0]);
				else
						BuildDash(ClauseNo, Prep_U);
				continue;
			}

			int iNomSgPl = -1;
			for ( int tt = 0; tt < Noun_NomSgPl.size(); tt++ )
				if ( Prep_U != Noun_NomSgPl[tt] )
				{
					iNomSgPl = Noun_NomSgPl[tt];
					break;
				}

			if ( -1 != iNomSgPl )
				if ( Prep_U > iNomSgPl )
						BuildDash(ClauseNo, iNomSgPl);
				else
						BuildDash(ClauseNo, Prep_U);
		}

	}
	
}



