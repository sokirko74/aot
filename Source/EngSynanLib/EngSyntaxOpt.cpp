#include "StdAfx.h"
#include "EngSyntaxOpt.h"

const int eSyntaxGroupTypesCount = 13;
const char eSyntaxGroupTypes [eSyntaxGroupTypesCount][30] = 
{
	"DET_ADJ_NOUN", "ADJ_NOUN",  "PP",
	"NP_left", "DET_NP_left", "NP_right", "DET_NP_right",
	"SIMIL_NP", "SIMIL_ADJ", "MODIF_ADJ", "DIRECT_OBJ","PARTIC_CONSTR",
	"FOREIGN_GROUP"
};


CSyntaxOpt*  NewOptionsEnglish ()
{
	return new CEngSyntaxOpt (morphEnglish);
};

CEngSyntaxOpt :: CEngSyntaxOpt (MorphLanguageEnum langua) : CSyntaxOpt(langua)
{
	m_IndeclinableMask = 0;
	m_SyntaxGroupTypes.clear();
	for (size_t i=0; i < eSyntaxGroupTypesCount; i++)
		m_SyntaxGroupTypes.push_back(eSyntaxGroupTypes[i]);
}

CAgramtab* CEngSyntaxOpt::GetNewGramTab () const
{
	return	new CEngGramTab;
};

void CEngSyntaxOpt::DestroyOptions ()
{
	CSyntaxOpt::DestroyOptions();
};


bool CEngSyntaxOpt :: InitOptionsLanguageSpecific()
{
	return true;
}




bool CEngSyntaxOpt::is_firm_group(int GroupType) const
{
	return false;
}

bool CEngSyntaxOpt::IsGroupWithoutWeight(int GroupType, const char* cause) const
{
	return false;

};


bool CEngSyntaxOpt::IsSimilarGroup (int type) const
{
	return false;
};
