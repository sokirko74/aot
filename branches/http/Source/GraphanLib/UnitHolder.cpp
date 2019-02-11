// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include  "UnitHolder.h"
#include  "../common/gra_descr_primitives.h"


//=======================================================
//==========            CUnitHolder         =============
//=======================================================
CUnitHolder::CUnitHolder()
{
	m_Language  = morphRussian;
};



void CUnitHolder::SetState (size_t LB, size_t HB, WORD state) 
{
	for (size_t i=LB; i<HB; i++)
		m_Units[i].AddStatus(state);
}

bool  CUnitHolder::AreGrouped (size_t LB, size_t HB) const
{
	bool fl_res = true;

	for (size_t i=LB; i<HB; i++)
		fl_res = fl_res && m_Units[i].IsGrouped();

	return fl_res;
}

bool  CUnitHolder::HasGrouped (size_t LB, size_t HB) const
{
	for (size_t i=LB; i<HB; i++)
		if (m_Units[i].IsGrouped())
			return true;

	return false;
}

bool  CUnitHolder::HasAbbreviation (size_t LB, size_t HB) const
{
	for (size_t i=LB; i<HB; i++)
		if (m_Units[i].IsAbbreviation())
			return true;

	return false;
}

size_t CUnitHolder::PassSpace (size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && m_Units[i].IsSpace() ; i++);
	return  i;
}

size_t CUnitHolder::PPunctOrSoft(size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && (HasDescr (i,OPun) || m_Units[i].IsSoft()) ; i++);
	return  i;
};

size_t CUnitHolder::PPunct(size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && HasDescr (i,OPun) ; i++);
	return  i;
};


size_t CUnitHolder::FindSpace (size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && !m_Units[i].IsSpace() ; i++);
	return  i;
}


size_t CUnitHolder::BSpace (size_t i, size_t LB) const
{
	for ( ; (i>LB) && m_Units[i].IsSpace(); i--);

	return i;
}

size_t CUnitHolder::PSoft (size_t i, size_t HB) const
{
	if (i >= HB) return HB;

	for ( ;(i<HB) && m_Units[i].IsSoft(); i++);

	return i;
}

size_t CUnitHolder::BSoft (size_t i) const
{
	for ( ; (i>0) && m_Units[i].IsSoft(); i--);

	return i;
}


bool CUnitHolder::IsQuestionOrExclamationMarks (size_t i) const 
{
	return   (    (m_Units[i].GetTokenLength() > 0)
				&& (      ((unsigned  char)m_Units[i].GetToken()[0] == '?')
					||  ((unsigned  char)m_Units[i].GetToken()[0] == '!')
					)
			);
};

		                    
bool CUnitHolder::IsSentenceEndMark (size_t  i) const
{
	return			IsOneFullStop(i) 
				|| IsQuestionOrExclamationMarks(i)
				|| ((m_Units[i].GetTokenLength() == 1)  &&  ((BYTE)m_Units[i].GetToken()[0] == cEllipseChar))
				|| (
						(m_Units[i].GetTokenLength() >1)
					&&  (m_Units[i].GetTokenLength() <6)
					&&  ((unsigned  char)m_Units[i].GetToken()[0] ==  '.')
				)
				|| m_Units[i].IsTextAreaEnd();
};

bool CUnitHolder::IsOneCloseQuotationMark (size_t i) const
{
	if (i == 0) return false;
	BYTE z = (BYTE)m_Units[i].GetToken()[0];
	return	(m_Units[i].GetTokenLength() == 1)
		&&	(	( z == (BYTE)'"')
			||	( z == (BYTE)'»')
		); 
};

bool CUnitHolder::IsOneOpenQuotationMark (size_t i) const
{
	if (i == 0) return false;
	BYTE z = (BYTE)m_Units[i].GetToken()[0];
	return     (m_Units[i].GetTokenLength() == 1)
		&& (     (z == (BYTE)'"')
			|| (z == (BYTE)'«')
			); 
};



void CUnitHolder::SetDes(size_t x, Descriptors des)  
{
	m_Units[x].SetDes(des);
};


bool  CUnitHolder::IsHyphen(size_t x)  const 
{
	return  HasDescr(x,OHyp) && m_Units[x].GetTokenLength() == 1;
};


bool		CUnitHolder::is_latin_alpha (int ch) const 
{
		if (m_Language == morphGerman)
			return is_german_alpha(ch);
		else
			return is_english_alpha(ch);
};

bool		CUnitHolder::is_lowercase (int ch) const 
{
	if (m_Language == morphGerman)
		return is_german_lower(ch);
	else
		return is_russian_lower(ch) || is_english_lower(ch);

};

bool		CUnitHolder::is_uppercase (int ch) const 
{
	if (m_Language == morphGerman)
		return is_german_upper(ch);
	else
		return is_russian_upper(ch) || is_english_upper(ch);

};

bool CUnitHolder::StrSuperCompare (int UnitNo, const char* s) const 
{
	
	return		(s[m_Units[UnitNo].GetTokenLength()] == 0) 
			&&	!strscmp(m_Units[UnitNo].GetToken(), s, m_Units[UnitNo].GetTokenLength(), m_Language);
};


bool  CUnitHolder::IsOneAlpha(size_t x) const
{
	return  ((HasDescr(x,ORLE) || HasDescr(x,OLLE)) && (m_Units[x].GetTokenLength()==1))
		|| ( (m_Units[x].GetTokenLength()==1) &&  is_latin_alpha((unsigned char)m_Units[x].GetToken()[0]) );
};

bool  CUnitHolder::IsOneChar(size_t x, int i) const 
{
	return   m_Units[x].GetTokenLength() == 1 && m_Units[x].GetToken()[0] == i;
};


bool  CUnitHolder::FirstUpper(size_t x) const 
{
	return  HasDescr(x,OUp) || HasDescr(x,OUpLw); 
};

bool CUnitHolder::IsBulletWord (size_t x) const 
{
	return HasDescr(x,ODigits) || IsOneAlpha(x);
};

bool CUnitHolder::IsOneFullStop (size_t i) const
{
	return (m_Units[i].GetTokenLength() == 1)  &&  (m_Units[i].GetToken()[0] == '.');
};


//  i - индекс графемы в самом начале  строки.
// Смещаемся  до начала строки и смотрим, стоит ли перед этой строкой
// пустая строка. 

bool  CUnitHolder::EmptyLineBeforeGraph (size_t i, size_t HB) const
{
	if ((i == 0) || m_Units[i].IsSoft()) return false;
	size_t k = BSpace (i-1,0);
	if ((k == 0) || (!m_Units[k].IsEOLN()))  return false;
    if (   (m_Units[k].GetTokenLength()>2)
		|| (    (m_Units[k].GetTokenLength() == 2)
		        && (m_Units[k].GetToken()[0] == '\n')
		  	)
	    ) return true; 
	k--;
	if ( k ==  0 ) return false;
	k = BSpace ( k,0 ); 
	return m_Units[k].IsEOLN();
};


void CUnitHolder::FreeTable()
{
	ClearVector(m_Units);
	ClearVector(m_TokenBuf);
	m_FoundOborots.clear();
	m_FoundPageBreaks.clear();
}


void  CUnitHolder :: BuildUnitBufferUpper ()
{
	m_UnitBufUpper.clear();

	for (int i = 0; i<m_Units.size(); i++)
	{
		m_UnitBufUpper.insert(m_UnitBufUpper.end(), m_Units[i].GetToken(), m_Units[i].GetToken()+m_Units[i].GetTokenLength());
		m_UnitBufUpper.push_back(0);
	};
	GerEngRusMakeUpperTemplate(m_UnitBufUpper, m_Language, m_UnitBufUpper.size());

};



size_t CUnitHolder::GetUnitBufferSize() const 
{
	/*
	this function is used by DDC. We should decrement the returning value, since 
	we have added a null to the end of GetUnitBuf(). 
	Without this decrementing we have a broken index to bibliography  file.
	Generally, if the source file was plain, then CUnitHolder::GetUnitBufferSize()
	should return the real(!) size of this file in bytes.
	*/
	return GetUnitBuf().size() - 1;
};


const char*	CUnitHolder::GetUnitBufferStart() const 
{ 
	return &(*(GetUnitBuf().begin())); 
};

const char*	CUnitHolder::GetUnitUpperBufferStart() const 
{ 
	return &(*(m_UnitBufUpper.begin())); 
};

const char*	CUnitHolder::GetUppercaseToken(DWORD LineNo) const 
{ 
	size_t Offset = ( (GetUnits()[LineNo].GetToken()+LineNo) - GetUnitBufferStart());
	return GetUnitUpperBufferStart() + Offset; 
};

string  CUnitHolder::GetToken(DWORD LineNo) const 
{
	char s[CriticalTokenLength+1];
	strncpy (s,GetUnits()[LineNo].GetToken(), GetUnits()[LineNo].GetTokenLength());
	s[GetUnits()[LineNo].GetTokenLength()] = 0;
	return string(s);
};

	
size_t	CUnitHolder::GetTokensCount() const 
{
	return GetUnits().size();
}

DWORD	CUnitHolder::GetTokenInputOffset(DWORD LineNo) const 
{
	return GetUnits()[LineNo].GetInputOffset(); 
}

BYTE	CUnitHolder::GetTokenLength(DWORD LineNo) const 
{	
	return GetUnits()[LineNo].GetTokenLength(); 
};




void	CUnitHolder::InitTokenBuffer()
{
	FreeTable();

	// the first line of the graphematical table has a special meaining
	CGraLine C;
	m_Units.push_back(C);
	SetDes(0,OBeg);

	//  While  building gra-table we use pointers into m_TokenBuf, 
	// that's why we should allocate only once m_TokenBuf in the very beginning. 
	// No reallocations are possible.
	m_TokenBuf.resize(m_InputBuffer.size());
}



bool	CUnitHolder::InitInputBuffer(const string& S)
{
	m_InputBuffer.clear();
	// copy with terminated null 
	size_t len = S.length() + 1;
	m_InputBuffer.insert(m_InputBuffer.begin(), S.c_str(), S.c_str()+len);
	return m_InputBuffer.size() == len;
};


void	CUnitHolder::ClearInputBuffer()
{
	ClearVector(m_InputBuffer);
}

void	CUnitHolder::DeleteDescr(size_t LineNo, Descriptors d)
{
	m_Units[LineNo].DelDes(d);
};


CGraLine& CUnitHolder::GetUnit(size_t UnitNo) 
{ 
	return m_Units[UnitNo]; 
};

const vector<char>& CUnitHolder::GetUnitBuf() const 
{ 
	return m_TokenBuf; 
};

const vector<BYTE>& CUnitHolder::GetInputBuffer() const 
{ 
	return m_InputBuffer; 
};

void	CUnitHolder::AddUnit(const CGraLine& NewLine)
{
	m_Units.push_back(NewLine);  
};


void	CUnitHolder::ClearPairDescriptors(size_t StartLineNo, size_t EndLineNo)
{
	for (size_t LineNo=StartLineNo; LineNo<EndLineNo; LineNo++)
		for (size_t des=0; des<64; des++)
			if (des != OSentEnd)
			if (HasDescr(LineNo, des))
			if (IsFirstMemberOfPairDesciptor((Descriptors)des))
			{
				Descriptors dual_descr = GetSecondMemberByTheFirst((Descriptors)des);
				size_t i=LineNo;
				for (; i<LineNo+20; i++)
					if (HasDescr(i, dual_descr))
					{
						DeleteDescr(i,dual_descr);
						break;
					};
				//  we should find the dual descriptor in this range
				assert (i < LineNo+20);
					DeleteDescr(LineNo,(Descriptors)des);
			}
			else
				if (IsSecondMemberOfPairDesciptor((Descriptors)des))
				{
					Descriptors dual_descr = GetFirstMemberByTheSecond((Descriptors)des);
					int i=LineNo;
					for (; i>LineNo-20; i--)
						if (HasDescr(i, dual_descr))
						{
							DeleteDescr(i,dual_descr);
							break;
						};
					//  we should find the dual descriptor in this range
					assert (i > LineNo-20);
					DeleteDescr(LineNo,(Descriptors)des);
				};

		

};



void	CUnitHolder::MakeOneWord(size_t StartLineNo, size_t EndLineNo)
{
	if (StartLineNo+1 == EndLineNo) return;

	//  delete all pair descriptors which intersects with [StartLineNo,EndLineNo)
	ClearPairDescriptors(StartLineNo, EndLineNo);

	//  move "end of sentence" descriptors from all lines of [StartLineNo,EndLineNo) to StartLineNo
	for (size_t LineNo=StartLineNo+1; LineNo<EndLineNo; LineNo++)
		for (size_t i=0; i<64; i++)
			if	(		IsEndTextPeriodDescriptor((Descriptors)i) 
					&&	HasDescr(LineNo, i)
				)
			SetDes(StartLineNo, (Descriptors)i);

	//  add all hard lines to the first line, counting all spaces  in order to maintain the file offsets
	size_t SpacesLength = 0;
	for (size_t LineNo=StartLineNo+1; LineNo<EndLineNo; LineNo++)
	{
		if (!m_Units[LineNo].IsSoft())
		{
			char* out= const_cast<char*>(m_Units[StartLineNo].GetToken()) + m_Units[StartLineNo].GetTokenLength();
			strncpy(out, m_Units[LineNo].GetToken(),  m_Units[LineNo].GetTokenLength());
			GetUnit(StartLineNo).AddLength(m_Units[LineNo]);

			char* upper_out= const_cast<char*>(GetUppercaseToken(StartLineNo));
			strcat(upper_out, GetUppercaseToken(LineNo));

		}
		else
			SpacesLength += m_Units[LineNo].GetTokenLength();

	};


	//  if some spaces were found in (StartLineNo, EndLineNo)
	// then fill line StartLineNo+1 with the spaces
	if (SpacesLength > 0)
	{
		m_Units[StartLineNo+1].SetToken(m_Units[StartLineNo].GetToken()+m_Units[StartLineNo].GetTokenLength());
		m_Units[StartLineNo+1].MakeSpaces(SpacesLength);

		{	// update upper buffer
			char* upper_out= const_cast<char*>(GetUppercaseToken(StartLineNo+1));
			strncpy(upper_out, m_Units[StartLineNo+1].GetToken(),  m_Units[StartLineNo+1].GetTokenLength());
			upper_out[m_Units[StartLineNo+1].GetTokenLength()] = 0;
		};

		SetOborotNo(StartLineNo+1,-1);
		if (EndLineNo-StartLineNo > 2)
		{
			//  erasing NULLs from the upper m_UnitBufUpper, which were addeded after each line
			size_t upper_rest_offset =  GetUppercaseToken(StartLineNo+2) - GetUnitUpperBufferStart();
			m_UnitBufUpper.erase(m_UnitBufUpper.begin()+upper_rest_offset,m_UnitBufUpper.begin()+upper_rest_offset+(EndLineNo-StartLineNo-2));
		};

		//  deleting (StartLineNo+2, EndLineNo)
		m_Units.erase(m_Units.begin()+StartLineNo+2, m_Units.begin() + EndLineNo);
	}
	else
	{
		//  deleting (StartLineNo+1, EndLineNo)
		m_Units.erase(m_Units.begin()+StartLineNo+1, m_Units.begin() + EndLineNo);

		//  erasing NULLs from the upper m_UnitBufUpper, which were addeded after each line
		size_t upper_rest_offset = GetUppercaseToken(StartLineNo+1) - GetUnitUpperBufferStart();
		m_UnitBufUpper.erase(m_UnitBufUpper.begin()+upper_rest_offset,m_UnitBufUpper.begin()+upper_rest_offset+(EndLineNo-StartLineNo-1));
	}


	//  check uppercase buffer
	if (StartLineNo+2 < m_Units.size())
	{
		const char* check = GetUppercaseToken(StartLineNo+2);
		string s = GetToken(StartLineNo+2);
		RmlMakeUpper(s, m_Language);
		assert(check == s);
	};
};




void CUnitHolder::SetOborotNo(size_t LineNo, short OborotNo)
{
	if (OborotNo == -1)
		m_FoundOborots.erase(m_Units[LineNo].GetInputOffset());
	else
		m_FoundOborots[m_Units[LineNo].GetInputOffset()] = OborotNo;
}

short CUnitHolder::GetOborotNo(size_t LineNo) const
{
	if (LineNo == 0) return -1;
	map<size_t, short>::const_iterator it = m_FoundOborots.find(m_Units[LineNo].GetInputOffset());
	if ( it == m_FoundOborots.end() )
		return -1;
	else
		return it->second;
}

void	CUnitHolder::SetPageNumber(size_t LineNo, DWORD PageNumber)
{
	if (PageNumber == UnknownPageNumber)
		m_FoundPageBreaks.erase(m_Units[LineNo].GetInputOffset());
	else
		m_FoundPageBreaks[m_Units[LineNo].GetInputOffset()] = PageNumber;

}

DWORD	CUnitHolder::GetPageNumber(size_t LineNo) const
{
	map<size_t, DWORD>::const_iterator it = m_FoundPageBreaks.find(m_Units[LineNo].GetInputOffset());
	if ( it == m_FoundPageBreaks.end() )
		return UnknownPageNumber;
	else
		return it->second;

}
