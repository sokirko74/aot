#ifndef rus_thesaurus_h
#define rus_thesaurus_h

#include "../SynCommonLib/ThesaurusForSyntax.h"

class CRusThesaurusForSyntax  : public  CThesaurusForSyntax 
{
public:
	CRusThesaurusForSyntax(CSyntaxOpt* Opt);
protected:
	void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel);

};

#endif
