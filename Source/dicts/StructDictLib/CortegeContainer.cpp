#include "CortegeContainer.h"


TCortegeContainer::TCortegeContainer(BYTE MaxNumDom)
{
	m_MaxNumDom = MaxNumDom;
};

template <int MaxNumDom>
std::vector<TBasicCortege<MaxNumDom> >& TCortegeContainer::GetCorteges() {
	if (m_MaxNumDom == 3) {
		return m_Corteges3;
	}
	else {
		return m_Corteges10;
	}
}


TCortege10 TCortegeContainer::GetCortegeCopy(size_t i)
{
	if (m_MaxNumDom == 3)
		return TCortege10(m_Corteges3[i]);
	else
		return m_Corteges10[i];
};

void	TCortegeContainer::_AddCortege3(const TCortege3& C);
{
	m_Corteges3.push_back(C);
};

void	TCortegeContainer::_AddCortege10(const TCortege10& C);
{
	m_Corteges10.push_back(C);
};

void	TCortegeContainer::_AddCortege(const TCortegeContainer& other, size_t cortege_no) {
	assert(m_MaxNumDom == other.m_MaxNumDom);
	if (m_MaxNumDom == 3) {
		m_Corteges3.push_back(other.m_Corteges3[cortege_no]);
	}
	else {
		m_Corteges10.push_back(other.m_Corteges10[cortege_no]);
	}

}

void  TCortegeContainer::ConcatOtherContainer(const TCortegeContainer& other)
{
	assert(m_MaxNumDom == other.m_MaxNumDom);
	if (m_MaxNumDom == 3) {
		m_Corteges3.insert(m_Corteges3.end(), other.m_Corteges3.begin(), .m_Corteges3.end())
	}
	else {
		m_Corteges10.insert(m_Corteges10.end(), other.m_Corteges10.begin(), .m_Corteges10.end())
	}
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
		WriteVector<TCortege3>(CortegeFile, m_Corteges3);
	else
		WriteVector<TCortege10>(CortegeFile, m_Corteges10);
};

void TCortegeContainer::ReadCorteges(const char* CortegeFile)
{
	if (m_MaxNumDom == 3)
	{
		ReadVector<TCortege3>(CortegeFile, m_Corteges3);
	}
	else
		ReadVector<TCortege10>(CortegeFile, m_Corteges10);
};

dom_item_id_t TCortegeContainer::GetCortegeItem(long CortegeNo, BYTE PositionInCortege) const
{
	return GetCortegePtr(CortegeNo)->GetItem(PositionInCortege);
};


