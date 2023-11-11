#pragma once

#include "../../common/cortege.h"


class TCortegeContainer
{
	std::vector<TCortege3>      m_Corteges3;
	std::vector<TCortege10>     m_Corteges10;
	

protected:
	void	_AddCortege3(const TCortege3& C);
	void	_AddCortege10(const TCortege10& C);
	void	_AddCortege(const TCortegeContainer& other, size_t cortege_no);

	void   _InsertCortege(size_t index, const TCortege10& C);
	void		EraseCorteges(size_t start, size_t last);
	void		ClearCorteges();
	void		WriteCorteges(const char* CortegeFile) const;
	void		ReadCorteges(const char* CortegeFile);
public:
	BYTE m_MaxNumDom;

	TCortegeContainer(BYTE MaxNumDom);

	TCortege10 GetCortegeCopy(size_t i);


	size_t	_GetCortegesSize()  const;
	void  ConcatOtherContainer(const TCortegeContainer& other);

};

