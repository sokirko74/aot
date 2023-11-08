#pragma once

#include "../../common/cortege.h"


class TCortegeContainer
{
	typedef TBasicCortege<3> CortegeType3;
	typedef TBasicCortege<10> CortegeType10;

	std::vector<CortegeType3>      m_Corteges3;
	std::vector<CortegeType10>     m_Corteges10;
protected:
	void	_AddCortege(const TCortege10& C);
	void   _InsertCortege(size_t index, const TCortege10& C);
	void		EraseCorteges(size_t start, size_t last);
	void		ClearCorteges();
	void		WriteCorteges(const char* CortegeFile) const;
	void		ReadCorteges(const char* CortegeFile);
public:
	BYTE m_MaxNumDom;

	TCortegeContainer(BYTE MaxNumDom);

	TCortege10* GetCortegePtr(size_t i);
	const TCortege10* GetCortegePtr(size_t i)  const;


	size_t	_GetCortegesSize()  const;
	void  ConcatOtherContainer(const TCortegeContainer& other);

};

