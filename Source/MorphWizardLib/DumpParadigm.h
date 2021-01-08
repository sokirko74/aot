#pragma once

#include <string>

class MorphoWizard;

//----------------------------------------------------------------------------
struct CMorphSession
{
	std::string		m_UserName;
	std::string		m_SessionStart;
	std::string		m_LastSessionSave;

	bool operator  == (const  CMorphSession& X) const;
	bool		ReadFromString(const std::string& s);
	std::string		ToString() const;
	void		SetEmpty();
	bool		IsEmpty() const;
};


class CDumpParadigm
{
	bool	ReadFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors);

public:
	std::string			m_TypeGrammemsStr;
	std::string			m_PrefixesStr;
	std::string			m_SlfStr;
	std::string			m_AuthorStr;
	CMorphSession		m_Session;
	int					m_FirstSlfLineNo;
	const MorphoWizard* m_pWizard;

	CDumpParadigm(const MorphoWizard* wizard) : m_pWizard(wizard) {};
	void	SetEmpty();
	bool	SaveToFile(FILE* fp) const;
	bool	SaveHeaderToFile(FILE* fp) const;
	bool	ReadNextParadigmFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors);

};
