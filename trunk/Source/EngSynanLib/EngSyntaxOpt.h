#ifndef EngSynOpt_h
#define EngSynOpt_h

#include "../AgramtabLib/EngGramTab.h"
#include "../SynCommonLib/SyntaxInit.h"

class CEngSyntaxOpt :	public CSyntaxOpt
{
public:
    CEngSyntaxOpt (MorphLanguageEnum langua);
	void DestroyOptions ();


	bool InitOptionsLanguageSpecific();
	CAgramtab* GetNewGramTab () const;

	bool IsSimilarGroup (int type) const;
	bool IsGroupWithoutWeight(int GroupType, const char* cause) const;
	bool is_firm_group(int GroupType) const;

};


#endif
