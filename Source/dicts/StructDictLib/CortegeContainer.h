#pragma once

#include "../../common/cortege.h"


class TCortegeContainer
{
	std::vector<TCortege>     m_Corteges;
	

protected:
	void	_AddCortege(const TCortege& C);
	void   _InsertCortege(size_t index, const TCortege& C);
	void		EraseCorteges(size_t start, size_t last);
	void		ClearCorteges();

	void		WriteCorteges(const char* CortegeFile) const;
	void		ReadCorteges(const char* CortegeFile);
public:
	TCortegeContainer();

	TCortege GetCortegeCopy(size_t i) const;
	const TCortege& GetCortege(size_t i) const ;


	size_t	_GetCortegesSize()  const;
	void  ConcatOtherContainer(const TCortegeContainer& other);

	BYTE		GetCortegeFieldNo(size_t i) const;
	BYTE		GetCortegeLeafId(size_t i) const;
	BYTE		GetCortegeBracketLeafId(size_t i) const;

};

