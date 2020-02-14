// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#ifndef __UNITSHOLDER_H_
#define __UNITSHOLDER_H_

#include "graline.h"       


class CUnitHolder
{
	vector<CGraLine>		m_Units;
	vector<char>			m_TokenBuf;
	vector<char>			m_UnitBufUpper;
	vector<BYTE>			m_InputBuffer;

	map<size_t, short>		m_FoundOborots;
	map<size_t, DWORD>		m_FoundPageBreaks;
public:
	const vector<CGraLine>& GetUnits() const	{ return m_Units; 	};
	CGraLine& GetUnit(size_t UnitNo);
	const vector<char>& GetUnitBuf() const;
	const vector<BYTE>& GetInputBuffer() const;

	MorphLanguageEnum		m_Language;

	CUnitHolder();
	// graphematical descriptors
	void	SetState (size_t LB, size_t HB, WORD state);
	void	SetDes(size_t x, Descriptors des);
	bool	HasDescr (size_t i, int descr)  const	{ return (m_Units[i].GetDescriptors() & _QM (descr)) > 0; }


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
	bool	IsHyphen(size_t x)  const; 
	bool	is_latin_alpha (int ch) const; 
	bool	is_lowercase (int ch) const; 
	bool	is_uppercase (int ch) const; 
	bool	StrSuperCompare (int UnitNo, const char* s) const; 
	bool	IsOneAlpha(size_t x) const;
	bool	IsOneChar(size_t x, int i) const; 
	bool	IsOneULet(size_t x) const; 
	bool	FirstUpper(size_t x) const; 
	bool	IsBulletWord (size_t x) const; 
	bool	IsOneFullStop (size_t i) const;
	bool	EmptyLineBeforeGraph (size_t i, size_t HB) const;
	bool	IsQuestionOrExclamationMarks (size_t i) const; 
	bool	IsSentenceEndMark (size_t  i) const;
	bool	IsOneCloseQuotationMark (size_t i) const;
	bool	IsOneOpenQuotationMark (size_t i) const;


	void	FreeTable();
	void	BuildUnitBufferUpper ();
	void	InitTokenBuffer();
	bool	InitInputBuffer(const string& S);	
	void	ClearInputBuffer();
	void	AddUnit(const CGraLine& NewLine);
	const char*	GetUnitBufferStart() const;
	const char*	GetUnitUpperBufferStart() const;
	const char*	GetUppercaseToken(DWORD LineNo) const;
	string		GetToken(DWORD LineNo) const;
	size_t		GetTokensCount() const;
	DWORD		GetTokenInputOffset(DWORD LineNo) const;
	BYTE		GetTokenLength(DWORD LineNo) const;



	size_t		GetUnitBufferSize() const;
	void	DeleteDescr(size_t LineNo, Descriptors d);

	void		MakeOneWord(size_t StartLineNo, size_t EndLineNo);
	void		ClearPairDescriptors(size_t StartLineNo, size_t EndLineNo);



	void	SetOborotNo(size_t LineNo, short OborotNo);
	short	GetOborotNo(size_t LineNo) const;

	void	SetPageNumber(size_t LineNo, DWORD PageNumber);
	DWORD	GetPageNumber(size_t LineNo) const;
};



#endif
