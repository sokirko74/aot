// : interface for the JVisualSynAnParamBuilder class.
//
//////////////////////////////////////////////////////////////////////

#ifndef JVisualSynAnParamBuilder_h
#define JVisualSynAnParamBuilder_h

#if _MSC_VER > 1000
#pragma once
#endif 

#include "../common/SyntaxHolder.h"


struct SGroup
{
public:
	int m_W1;	
	int m_W2;	
	bool m_bIsGroup;
	string m_strDescr;
	bool operator < (const SGroup& gr) const
	{
		if( (m_W1 <= gr.m_W1) && (m_W2 >= gr.m_W2) )
			return true;

		if( m_W2 < gr.m_W1 )
			return true;

		return false;
	}
};


struct SSynVariant2Groups
{
public:
	vector< CSynUnit> m_SynUnits;
	vector<SGroup> m_Groups;

};

class CSyntaxHolder;
class JVisualSynAnParamBuilder  
{
public:
	const CSyntaxHolder* m_pSyntaxHolder;
	JVisualSynAnParamBuilder(const CSyntaxHolder* pSyntaxHolder);
	virtual ~JVisualSynAnParamBuilder();	
	string BuildStrParam(const CSentence& piSent);

protected:	
	string CodeWord(string strWord);
	string CodeSymbol(char ch);
	bool WriteWords(const CSentence& piSent, string& strRes, long StartWord, long LastWord);
	void GetTopClauses(const CSentence& piSent, vector<long>& topClauses);
	void AddVariants(vector<SSynVariant2Groups>& synVariants,long iClause,  const CSentence& piSent);
	void AddOneVariant(vector<SSynVariant2Groups>& synVariants,const CMorphVariant& piVar,  const CSentence& piSent, const CClause& Clause);
	void AddGroups(vector<SSynVariant2Groups>& synVariants, const CMorphVariant& piVar, const CClause& Clause);
	void MultVariants(vector<SSynVariant2Groups>& SynVariants1, vector<SSynVariant2Groups>& synVariants2);
	void MultTwoVariants(SSynVariant2Groups& var1, SSynVariant2Groups& var2, SSynVariant2Groups& res);
	void AddHomonym(vector<SSynVariant2Groups>& synVariants,  const CSynUnit& Unit);
	void AddGroup(vector<SSynVariant2Groups>& synVariants, const CGroup& piGroup);
	void WriteVariant(SSynVariant2Groups& var, string& strRes );


};

#endif 
