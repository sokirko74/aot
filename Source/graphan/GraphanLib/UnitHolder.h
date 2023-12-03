// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#ifndef __UNITSHOLDER_H_
#define __UNITSHOLDER_H_

#include "graline.h"       


class CUnitHolder
{
protected: 
	std::vector<CGraLine> m_Tokens;
	std::string	m_InputText;
	CGraLine& GetUnit(size_t UnitNo);

public:
	const std::vector<CGraLine>& GetUnits() const;
	std::string GetTokenUtf8(size_t line_no) const;

	MorphLanguageEnum		m_Language;

	CUnitHolder();
	// graphematical descriptors
	void	SetState (size_t LB, size_t HB, uint16_t state);
	void	SetDes(size_t x, Descriptors des);
	bool	HasDescr(size_t i, Descriptors descr) const;


	// group graphematical descriptors
	bool	AreGrouped (size_t LB, size_t HB) const;
	bool	HasGrouped (size_t LB, size_t HB) const;
	bool	HasAbbreviation (size_t LB, size_t HB) const;
	

	// moving inside the graphematical table
	size_t	PassSpace (size_t i, size_t HB) const;
	size_t	PPunctOrSoft(size_t i, size_t HB) const;
	size_t	PPunct(size_t i, size_t HB) const;
	size_t	FindSpace (size_t i, size_t HB) const;
	size_t	BSpace (size_t i, size_t LB = 0) const;
	size_t	PSoft (size_t i, size_t HB) const;
	size_t	BSoft (size_t i) const;

	// simple predicates	
	bool	StrSuperCompare (int UnitNo, const char* s) const; 
	bool	EmptyLineBeforeGraph (size_t i, size_t HB) const;
	bool	IsQuestionOrExclamationMarks (size_t i) const; 
	bool	IsSentenceEndMark (size_t  i) const;
	bool	IsOneCloseQuotationMark (size_t i) const;
	bool	IsOneOpenQuotationMark (size_t i) const;


	void	FreeTable();
	void	InitInputText(const std::string& S);	
	void	ClearInputBuffer();
	const std::string& GetToken(uint32_t LineNo) const;
	const std::string& GetUpperString(uint32_t LineNo) const;
	size_t	GetTokensCount() const;
	uint32_t GetTokenInputOffset(uint32_t LineNo) const;
	BYTE	GetTokenLength(uint32_t LineNo) const;


	void	DeleteDescr(size_t LineNo, Descriptors d);

	void		MakeOneWord(size_t StartLineNo, size_t EndLineNo);
	void		ClearPairDescriptors(size_t StartLineNo, size_t EndLineNo);
	bool IsOneFullStop(size_t i) const;


};



#endif
