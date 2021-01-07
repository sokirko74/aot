#pragma once
#include "list"


class CQueryHistory 
{
	CString m_IniFilePath;
public:
	std::list<CString>	m_LastQueries;

	CQueryHistory();
	void LoadHistory();
	void SaveHistory() const;
	void ChangeHistory(CString query);
};
