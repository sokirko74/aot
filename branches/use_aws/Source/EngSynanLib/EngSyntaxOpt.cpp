#include "StdAfx.h"
#include "EngSyntaxOpt.h"
#include "EngSentence.h"
#include "EngOborDic.h"

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

CSentence* CEngSyntaxOpt::NewSentence() const {
	return new CEngSentence(this);
};

CAgramtab *CEngSyntaxOpt::NewGramTab() const {
    return new CEngGramTab();
};

CLemmatizer *CEngSyntaxOpt::NewLemmatizer() const {
    return new CLemmatizerEnglish();
};

COborDic * CEngSyntaxOpt::NewOborDic(const CSyntaxOpt* opt)  {
    return new CEngOborDic(opt);
};

class CEngThesaurusForSyntax  : public  CThesaurusForSyntax
{
public:
	CEngThesaurusForSyntax(const CSyntaxOpt* Opt) : CThesaurusForSyntax(Opt) {};
protected:
	void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel) {return;};
};

CThesaurusForSyntax* CEngSyntaxOpt::NewThesaurus(const CSyntaxOpt* opt) {
    return new CEngThesaurusForSyntax(opt);
};


CEngSyntaxOpt :: CEngSyntaxOpt (MorphLanguageEnum langua) : CSyntaxOpt(langua)
{
	m_IndeclinableMask = 0;
	m_SyntaxGroupTypes.clear();
	for (size_t i=0; i < eSyntaxGroupTypesCount; i++)
		m_SyntaxGroupTypes.push_back(eSyntaxGroupTypes[i]);
}


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
