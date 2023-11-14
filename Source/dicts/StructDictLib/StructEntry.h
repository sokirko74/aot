#pragma once 

#include "morph_dict/common/utilit.h"


// Индекс  - uint16_t
class CStructEntry {
	std::string	m_EntryStr;
	std::string	m_AuthorStr;

public:
	int		m_EntryId;
	BYTE	m_MeanNum;
	int		m_StartCortegeNo;
	int		m_LastCortegeNo;
	bool	m_bSelected;


	CStructEntry(std::string entry_str="", int _MeanNum = 0);
	
	const std::string& GetEntryStr() const;
	void SetEntryStr(const std::string& s);

	const std::string& GetAuthorStr() const;
	void SetUnitAuthor(std::string s);

	bool HasEmptyArticle() const;
	void MakeEmpty() const;

	bool operator==(const CStructEntry& X) const;
	bool operator<(const CStructEntry& X) const;
	

	size_t get_size_in_bytes() const;
	size_t save_to_bytes(BYTE* buf) const;
	size_t restore_from_bytes(const BYTE* buf);

};

extern size_t get_size_in_bytes(const CStructEntry& t);
extern size_t save_to_bytes(const CStructEntry& i, BYTE* buf);
extern size_t restore_from_bytes(CStructEntry& i, const BYTE* buf);
