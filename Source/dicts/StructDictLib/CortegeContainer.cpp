#include "CortegeContainer.h"


TCortegeContainer::TCortegeContainer(BYTE MaxNumDom)
{
	m_MaxNumDom = MaxNumDom;
};

TCortege10* TCortegeContainer::_GetCortege(size_t i)
{
	if (m_MaxNumDom == 3)
		return (TCortege10*)(&m_Corteges3[i]);
	else
		return (TCortege10*)(&(m_Corteges10[i]));
};

const TCortege10* TCortegeContainer::_GetCortege(size_t i)  const
{
	if (m_MaxNumDom == 3)
		return (TCortege10*)(&m_Corteges3[i]);
	else
		return (TCortege10*)(&(m_Corteges10[i]));
};

void  TCortegeContainer::_AddCortege(const TCortege10& C)
{
	if (m_MaxNumDom == 3)
	{
		TBasicCortege<3> Q;
		Q = C;
		m_Corteges3.push_back(Q);;
	}
	else
		m_Corteges10.push_back(C);;
};

void  TCortegeContainer::ConcatOtherContainer(const TCortegeContainer& other)
{
	for (int i=0; i < other._GetCortegesSize(); i++)
	{
		TCortege10 C;
		C = *other._GetCortege(i);
		_AddCortege(C);
	};

}


void  TCortegeContainer::_InsertCortege(size_t index, const TCortege10& C)
{
	if (m_MaxNumDom == 3)
	{
		TBasicCortege<3> Q;
		Q = C;
		m_Corteges3.insert(m_Corteges3.begin() + index, Q);
	}
	else
		m_Corteges10.insert(m_Corteges10.begin() + index, C);
};


size_t  TCortegeContainer::_GetCortegesSize()  const
{
	if (m_MaxNumDom == 3)
		return  m_Corteges3.size();
	else
		return  m_Corteges10.size();
};

void TCortegeContainer::ClearCorteges()
{
	if (m_MaxNumDom == 3)
		m_Corteges3.clear();
	else
		m_Corteges10.clear();

};

void TCortegeContainer::EraseCorteges(size_t start, size_t last)
{
	if (m_MaxNumDom == 3)
		m_Corteges3.erase(m_Corteges3.begin() + start, m_Corteges3.begin() + last);
	else
		m_Corteges10.erase(m_Corteges10.begin() + start, m_Corteges10.begin() + last);
};

void TCortegeContainer::WriteCorteges(const char* CortegeFile) const
{
	if (m_MaxNumDom == 3)
		WriteVector<CortegeType3>(CortegeFile, m_Corteges3);
	else
		WriteVector<CortegeType10>(CortegeFile, m_Corteges10);
};

void TCortegeContainer::ReadCorteges(const char* CortegeFile)
{
	if (m_MaxNumDom == 3)
	{
		ReadVector<CortegeType3>(CortegeFile, m_Corteges3);
	}
	else
		ReadVector<CortegeType10>(CortegeFile, m_Corteges10);
};



