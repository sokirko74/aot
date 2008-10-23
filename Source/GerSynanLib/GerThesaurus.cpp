
#include "GerSynan.h"
#include "GerThesaurus.h"
#include "../SynCommonLib/SynPlmLine.h"



CThesaurusForSyntax* NewThesaurusForSyntaxGerman (CSyntaxOpt* Opt)
{
	return new CGerThesaurusForSyntax(Opt);
};

CGerThesaurusForSyntax::CGerThesaurusForSyntax(CSyntaxOpt* Opt): CThesaurusForSyntax(Opt)
{
};



void CGerThesaurusForSyntax::AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel)
{
	return;
	
}
