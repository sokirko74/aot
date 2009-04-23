#ifndef GLRTable_h
#define GLRTable_h


#include "../common/utilit.h"
#include "LR-items.h"

struct CWorkRule;
class CWorkGrammar;


struct CGLRRuleInfo 
{
	DWORD	m_RuleId;
	DWORD	m_RuleLength;
	DWORD	m_LeftPart;
	DWORD	m_SynMainItemNo;
	DWORD	m_RuleFeaturesNo;
	void	Init(const CWorkRule*);
	bool operator < (const CGLRRuleInfo& X) const
	{
		return m_RuleId < X.m_RuleId;
	};
};

struct CSLRCellBuild 
{
	bool						m_bShift;
	vector<size_t>				m_ReduceRules;
	int							m_GotoLine;
	CSLRCellBuild()
	{
		m_GotoLine = -1;
		m_bShift = false;
	};
};


struct CSLRCellWork 
{
	bool						m_bShift;
	DWORD						m_ReduceRulesSetStart;
	int							m_GotoLine;
	
};




class CGLRTable  
{
	
	vector< vector < CSLRCellBuild > >		m_BuildTable;
	vector < CSLRCellWork >					m_WorkTable;
	

	void			ConvertBuildCellsToWordCells();
	size_t			GetRuleInfo (const CWorkRule*) const;
	size_t			m_WorkLineSize;

public:
	const CWorkGrammar*					m_pWorkGrammar;
	vector<CGLRRuleInfo>				m_RuleInfos;
	vector<DWORD>						m_ReduceRuleSets;
	
	

	CGLRTable();
	
	bool BuildGLRTable();
	void PrintGLRTable(FILE* fp) const;
	
	bool ConvertAndSaveGLRTable(string FileName);
	bool LoadGLRTable(string FileName);
	bool SaveOldGLRTable(string FileName) const;

	const CSLRCellWork& GetCell(size_t  StateNo, size_t SymbolNo) const;
	DWORD GetReduceRuleEnd(size_t  StateNo, size_t SymbolNo)  const;
};



#endif
