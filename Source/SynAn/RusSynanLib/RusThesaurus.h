#ifndef rus_thesaurus_h
#define rus_thesaurus_h

#include "synan/SynCommonLib/ThesaurusForSyntax.h"

class CRusThesaurusForSyntax  : public  CThesaurusForSyntax 
{
public:
	CRusThesaurusForSyntax(const CSyntaxOpt* Opt);
protected:
	void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel);

};

#endif
