#ifndef ger_thesaurus_h
#define ger_thesaurus_h

#include "../SynCommonLib/ThesaurusForSyntax.h"

class CGerThesaurusForSyntax  : public  CThesaurusForSyntax 
{
public:
	CGerThesaurusForSyntax(CSyntaxOpt* Opt);
protected:
	void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel);

};

#endif
