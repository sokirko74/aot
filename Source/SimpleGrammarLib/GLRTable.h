#ifndef GLRTable_h
#define GLRTable_h


#include "../common/utilit.h"
#include "LR-items.h"

struct CWorkRule;
class CWorkGrammar;


struct CGLRRuleInfo 
{
	uint32_t	m_RuleId;
	uint32_t	m_RuleLength;
	uint32_t	m_LeftPart;
	uint32_t	m_SynMainItemNo;
	uint32_t	m_RuleFeaturesNo;
	void	Init(const CWorkRule*);
	bool operator < (const CGLRRuleInfo& X) const
	{
		return m_RuleId < X.m_RuleId;
	};
};

struct CSLRCellBuild 
{
	bool						m_bShift;
	std::vector<size_t>				m_ReduceRules;
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
	uint32_t						m_ReduceRulesSetStart;
	int							m_GotoLine;
	
};




class CGLRTable  
{
	
	std::vector< std::vector < CSLRCellBuild > >		m_BuildTable;
	std::vector < CSLRCellWork >					m_WorkTable;
	

	void			ConvertBuildCellsToWordCells();
	size_t			GetRuleInfo (const CWorkRule*) const;
	size_t			m_WorkLineSize;

public:
	const CWorkGrammar*					m_pWorkGrammar;
	std::vector<CGLRRuleInfo>				m_RuleInfos;
	std::vector<uint32_t>						m_ReduceRuleSets;
	
	

	CGLRTable();
	
	bool BuildGLRTable();
	void PrintGLRTable(FILE* fp) const;
	
	bool ConvertAndSaveGLRTable(std::string FileName);
	bool LoadGLRTable(std::string FileName);
	bool SaveOldGLRTable(std::string FileName) const;

	const CSLRCellWork& GetCell(size_t  StateNo, size_t SymbolNo) const;
	uint32_t GetReduceRuleEnd(size_t  StateNo, size_t SymbolNo)  const;
};



#endif
