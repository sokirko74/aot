#include "GerSynan.h"
#include "GerFormatCaller.h"
#include "GerClause.h"

CFormatCaller* NewFormatCallerGerman(const CSyntaxOpt* Opt)
{
	return new CGerFormatCaller(Opt);
};


CGerFormatCaller::CGerFormatCaller(const CSyntaxOpt* Opt) : CFormatCaller(Opt) 
{
};



int   CGerFormatCaller::GetRuleByGroupTypeForThesaurus(int GroupType) const
{
	return -1;
};

void	CGerFormatCaller::AddAllRules()
{
};

void CGerFormatCaller::AddSimpleSimilarRules( )
{
	
};









