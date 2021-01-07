#include "stdafx.h"
#include "QueryHistory.h"

const int HISTORY_SIZE = 10;

CQueryHistory::CQueryHistory()
{
	m_IniFilePath = CString(getenv("USERPROFILE")) + _T("\\morphwizard.ini");
}

void CQueryHistory::LoadHistory()
{
	const size_t bufferSize = 1024;
	TCHAR buffer[bufferSize];
	for (int i = 1; i < HISTORY_SIZE; ++i)
	{
		if (!GetPrivateProfileString(_T("History"), IntToStr(i), nullptr, buffer, bufferSize, (const TCHAR*)m_IniFilePath)) {
			break;
		};
		m_LastQueries.push_back(buffer);
	}
}


void CQueryHistory::SaveHistory() const
{
	if (!::WritePrivateProfileSection(_T("History"), _T(""), (const TCHAR*)m_IniFilePath)) {
		ErrorMessage(GetLastErrorAsString());
	}
	else {

		int i = 1;
		for (auto strVal : m_LastQueries)
		{
			if (!::WritePrivateProfileString(_T("History"), IntToStr(i), strVal, (const TCHAR*)m_IniFilePath)) {
				ErrorMessage(GetLastErrorAsString());
				break;
			}
			++i;
		}
	}
}


void CQueryHistory::ChangeHistory(CString query)
{
	if (!m_LastQueries.empty())
		if (m_LastQueries.front() == query)
			return;

	m_LastQueries.push_front(query);
	if (m_LastQueries.size() > HISTORY_SIZE)
		m_LastQueries.pop_back();
	
}


