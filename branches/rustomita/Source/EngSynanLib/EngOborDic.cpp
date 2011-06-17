#include "StdAfx.h"
#include "EngOborDic.h"
#pragma warning(disable:4786) 




COborDic* NewOborDicEnglish (CSyntaxOpt* Opt)
{
	return new CEngOborDic(Opt);
};

static const size_t CoordConjTypesCount = 21;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{"AND", false},
    {"OR", false},
	{"AS", false},
	{"BECAUSE", false},
	{"BEFORE", false},
	{"IF", false},
	{"SINCE", false},
	{"SO", false},
	{"THAT", false},
	{"THOUGH", false},
	{"UNLESS", false},
	{"UNTIL", false},
	{"WHEN", false},
	{"WHILE", false},
	{"WHO", false},
	{"WHAT", false},
	{"WHERE", false},
	{"HOW", false},
	{"WHETHER", false},
	{"WHILST", false}
};

CEngOborDic::CEngOborDic(CSyntaxOpt* Opt) : COborDic(Opt)
{
	m_SimpleCoordConj.clear();
	for (size_t i=0; i < CoordConjTypesCount;i++)
		m_SimpleCoordConj.push_back(CoordConjTypes[i]);

}

static long GetItemNoByItemStr(const CDictionary* piOborDic, const char* ItemStr, const char* _DomStr) 
{
	BYTE DomNo = piOborDic->GetDomenNoByDomStr(_DomStr);
    return piOborDic->GetItemNoByItemStr(ItemStr, DomNo);
}







bool CEngOborDic::ReadOborDic (const CDictionary* piOborDic)
{
	try
	{
		if(piOborDic == NULL )
			return false;

		m_SimpleSubConj.clear();
		
	}
	catch(...)
	{		
		return false;
	}
	return true; 	
};




