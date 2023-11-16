#include "StructEntry.h"
#include "morph_dict//common/bserialize.h"

// ссылки на словaрную статью, лежащую в массиве Ross :: Cortege
const int InitialStartPos = 5000000;
const int InitialEndPos = -1;
const	uint32_t	EntryStrSize = 60;
const uint32_t EntryAuthorStrSize = 30;

CStructEntry::CStructEntry(std::string entry_str, int mean_num)
{
	m_StartCortegeNo = InitialStartPos;
	m_LastCortegeNo = InitialEndPos;
	SetEntryStr(entry_str);
	m_MeanNum = mean_num;
	m_bSelected = true;
}

const std::string& CStructEntry::GetEntryStr() const {
	return m_EntryStr;
}

void CStructEntry::SetEntryStr(const std::string& s)  {
	m_EntryStr = s;
	if (m_EntryStr.length() >= EntryStrSize) {
		throw CExpc("Dict Entry %s is longer than %i bytes", m_EntryStr, EntryStrSize);
	}
}

const std::string& CStructEntry::GetAuthorStr() const {
	return m_AuthorStr;
}


size_t CStructEntry::get_size_in_bytes() const
{
	return ::get_size_in_bytes(m_EntryId) + EntryStrSize + ::get_size_in_bytes(m_MeanNum) +
		::get_size_in_bytes(m_StartCortegeNo) + ::get_size_in_bytes(m_LastCortegeNo) + ::get_size_in_bytes((BYTE)m_bSelected) +
		EntryAuthorStrSize;
};

size_t CStructEntry::save_to_bytes( BYTE* buf) const
{
	buf += ::save_to_bytes(m_EntryId, buf);
	strcpy((char*)buf, m_EntryStr.c_str());
	buf += EntryStrSize;
	buf += ::save_to_bytes(m_MeanNum, buf);
	buf += ::save_to_bytes(m_StartCortegeNo, buf);
	buf += ::save_to_bytes(m_LastCortegeNo, buf);
	buf += ::save_to_bytes((BYTE)m_bSelected, buf);
	strcpy((char*)buf, m_AuthorStr.c_str());
	buf += EntryAuthorStrSize;
	return get_size_in_bytes();
};

size_t CStructEntry::restore_from_bytes(const BYTE* buf)
{
	buf += ::restore_from_bytes(m_EntryId, buf);
	m_EntryStr = (char*)buf;
	buf += EntryStrSize;
	buf += ::restore_from_bytes(m_MeanNum, buf);
	buf += ::restore_from_bytes(m_StartCortegeNo, buf);
	buf += ::restore_from_bytes(m_LastCortegeNo, buf);
	buf += ::restore_from_bytes((BYTE&)m_bSelected, buf);
	m_AuthorStr = (char*)buf;
	return get_size_in_bytes();
};


bool CStructEntry::operator == (const CStructEntry& X) const
{
	return    (m_EntryStr == X.m_EntryStr)
		&& (m_MeanNum == X.m_MeanNum);
}

bool CStructEntry::operator < (const CStructEntry& X) const
{
	int i = m_EntryStr.compare(X.m_EntryStr);
	return (i < 0) || ((i == 0) && (m_MeanNum < X.m_MeanNum));
}

bool CStructEntry::HasEmptyArticle() const { 
	return m_StartCortegeNo == InitialStartPos; 
};

void CStructEntry::MakeEmpty()  {
	m_StartCortegeNo = InitialStartPos;
	m_LastCortegeNo = InitialStartPos;
};

void CStructEntry::SetUnitAuthor(std::string s) {
	m_AuthorStr = s;
	if (m_AuthorStr.length() >= EntryAuthorStrSize) {
		throw CExpc("Dict Entry %s is longer than %i bytes", m_AuthorStr, EntryAuthorStrSize);
	}

}

size_t get_size_in_bytes(const CStructEntry& t)
{
	return t.get_size_in_bytes();
};

size_t save_to_bytes(const CStructEntry& i, BYTE* buf)
{
	return i.save_to_bytes(buf);
};

size_t restore_from_bytes(CStructEntry& i, const BYTE* buf)
{
	return i.restore_from_bytes(buf);
};


