#include "StdAfx.h"
#include "EngSentence.h"
#include "EngFormatCaller.h"


CSentence* NewSentenceEnglish (const CSyntaxOpt* pSyntaxOptions)
{
	return new CEngSentence(pSyntaxOptions);
};

CEngSentence::CEngSentence(const CSyntaxOpt* pSyntaxOptions)  : CSentence(pSyntaxOptions)
{

}

CEngSentence::~CEngSentence()
{

};

void CEngSentence::ReadNextFromPlmLinesLanguageSpecific()
{
};


void CEngSentence::ChooseClauseType(const  vector<SClauseType>& vectorTypes, CMorphVariant& V)
{
	int empty_type = -1; 

	for(int i = 0 ; i < vectorTypes.size() ; i++ )
	{
		if( !vectorTypes[i].m_Root.IsEmpty() )
		{
			int node = vectorTypes[i].m_Root.m_WordNo;
			int hom = vectorTypes[i].m_Root.m_HomonymNo;

			int iUnit = V.UnitNoByWordNo(node);
			assert (iUnit != -1);
			if( V.m_SynUnits[iUnit].m_iHomonymNum == hom )
			{
				V.m_ClauseTypeNo = i;
				return;
			}				
		}
		else
			empty_type = i;

	}
	V.m_ClauseTypeNo = empty_type;
}


bool CEngSentence::RunSyntaxInClauses(ESynRulesSet)
{
	try
	{
		if( m_pSyntaxOptions == NULL )
			return false;
		
		CEngFormatCaller FormatCaller(GetOpt());
		
		FormatCaller.AddAllRules( );

		for(int i = 0 ; i < GetClausesCount() ; i++ )
		{
			GetClause(i).BuildGroups(FormatCaller, true);		
			FormatCaller.Reset();
		}

		
		return true;
	}
	catch(...)
	{
		OutputErrorString("Failed RunSyntaxInClause");
		return false;
	}
}



void CEngSentence::DeleteHomOneToThousand()
{
}

void CEngSentence::AddWeightForSynVariantsInClauses()
{
	for (int i = 0; i < GetClausesCount(); i++)
	{
		CClause&  pClause = GetClause(i);
		
		pClause.AddVariantWeightWithHomOneToFifty();
	}
}



EClauseType CEngSentence::GetClauseTypeByAncodePattern (const CAncodePattern& Pattern) const
{
	
	return UnknownPartOfSpeech;
};



bool	CEngSentence::SetClauseBorderIfThereAreTwoPotentialPredicates(int FWrd, int LWrd)
{
	return false;
};


bool CEngSentence::BuildClauses()
{
	m_bPanicMode = IsPanicSentence();

	assert ( GetClausesCount() == 0 );

	FindGraPairs();

	if(! BuildInitialClauses() )
	{
		return false;

	};

	RunSyntaxInClauses(AllRules);
	assert ( IsValid() );

	return true;	
}






bool	CEngSentence::AllHomonymsArePredicates(const CSynWord& W) const
{
	return false;
};



bool CEngSentence::IsInitialClauseType(EClauseType x) const
{
	return  false;
};

bool CEngSentence::IsProfession(class CSynHomonym const &)const
{
	return false;
};

bool CEngSentence::CanBeRelativeAntecedent(class CSynHomonym const &)const
{
	return false;
};

bool CEngSentence::IsRelativSentencePronoun(int,int,int &)const 
{
	return false;
};

void CEngSentence::SolveAmbiguityUsingRuleForTwoPredicates(int,int)
{
	
};

void CEngSentence::InitHomonymLanguageSpecific(CSynHomonym& H, const CLemWord* pWord)
{

};

void CEngSentence::InitHomonymMorphInfo (CSynHomonym& H)
{
    H.InitAncodePattern( );
};



int CEngSentence::GetCountOfStrongRoots(class CClause const &,struct CMorphVariant const &)const
{
	return 0;
};

void CEngSentence::BuildSubjAndPredRelation(CMorphVariant& synVariant, long RootWordNo, EClauseType ClauseType)
{
	return;
};

void CEngSentence::AfterBuildGroupsTrigger(class CClause &)
{
};


bool CEngSentence::WordSchemeEqForThesaurus(class CSynHomonym const &,class CSynPlmLine const &)const
{
	return false;
};
