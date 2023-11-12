#include "CortegeContainer.h"


TCortegeContainer::TCortegeContainer()
{
};



TCortege TCortegeContainer::GetCortegeCopy(size_t i) const 
{
	return m_Corteges[i];
};


const TCortege& TCortegeContainer::GetCortege(size_t i) const 
{
	return m_Corteges[i];
};



void TCortegeContainer::_AddCortege(const TCortege& C)
{
	m_Corteges.push_back(C);
};



void  TCortegeContainer::ConcatOtherContainer(const TCortegeContainer& other)
{
	m_Corteges.insert(m_Corteges.end(), other.m_Corteges.begin(), other.m_Corteges.end());
}


void  TCortegeContainer::_InsertCortege(size_t index, const TCortege& C)
{
	m_Corteges.insert(m_Corteges.begin() + index, C);
};



size_t  TCortegeContainer::_GetCortegesSize()  const
{
	return  m_Corteges.size();
};


void TCortegeContainer::ClearCorteges()
{
	m_Corteges.clear();
	
};


void TCortegeContainer::EraseCorteges(size_t start, size_t last)
{
	m_Corteges.erase(m_Corteges.begin() + start, m_Corteges.begin() + last);
};


void TCortegeContainer::WriteCorteges(const char* CortegeFile) const
{
	WriteVector<TCortege>(CortegeFile, m_Corteges);
};


void TCortegeContainer::ReadCorteges(const char* CortegeFile)
{
	ReadVector<TCortege>(CortegeFile, m_Corteges);
};


BYTE TCortegeContainer::GetCortegeFieldNo(size_t i) const
{
	return GetCortege(i).m_FieldNo;

};


BYTE TCortegeContainer::GetCortegeLeafId(size_t i) const
{
	return GetCortege(i).m_LeafId;
};


BYTE TCortegeContainer::GetCortegeBracketLeafId(size_t i) const
{
	return GetCortege(i).m_BracketLeafId;

};


