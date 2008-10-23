#include "StdAfx.h"
#include "EngFormatCaller.h"


CFormatCaller* NewFormatCallerEnglish(const CSyntaxOpt* Opt)
{
	return new CEngFormatCaller(Opt);
};


CEngFormatCaller::CEngFormatCaller(const CSyntaxOpt* Opt) : CFormatCaller(Opt) 
{
#define InsertRule(_rule, _member, _group_name, _direction)  FormatCalls.insert(CFormatCallMap::value_type( _rule ,  CFormatCall ((CFormatFunc)&CEngFormatCaller:: _member , _group_name, _direction )));


#undef InsertRule
};



int   CEngFormatCaller::GetRuleByGroupTypeForThesaurus(int GroupType) const
{
	return -1;
};

void	CEngFormatCaller::AddAllRules()
{
	AddSimpleSimilarRules();
};

void CEngFormatCaller::AddSimpleSimilarRules( )
{
	
};




