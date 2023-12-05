// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include  "UnitHolder.h"
#include  "../../common/gra_descr.h"



//=======================================================
//==========            CUnitHolder         =============
//=======================================================
CUnitHolder::CUnitHolder()
{
	m_Language  = morphRussian;
};

void CUnitHolder::SetState (size_t LB, size_t HB, uint16_t state) 
{
	for (size_t i=LB; i<HB; i++)
		m_Tokens[i].AddStatus(state);
}

bool  CUnitHolder::AreGrouped (size_t LB, size_t HB) const
{
	bool fl_res = true;

	for (size_t i=LB; i<HB; i++)
		fl_res = fl_res && m_Tokens[i].IsGrouped();

	return fl_res;
}

bool  CUnitHolder::HasGrouped (size_t LB, size_t HB) const
{
	for (size_t i=LB; i<HB; i++)
		if (m_Tokens[i].IsGrouped())
			return true;

	return false;
}

bool  CUnitHolder::HasAbbreviation (size_t LB, size_t HB) const
{
	for (size_t i=LB; i<HB; i++)
		if (m_Tokens[i].IsAbbreviation())
			return true;

	return false;
}

size_t CUnitHolder::PassSpace (size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && m_Tokens[i].IsSpace() ; i++);
	return  i;
}

size_t CUnitHolder::PPunctOrSoft(size_t i, size_t HB) const
{
	if (i >= HB) return HB;
	for ( ; (i<HB)  && (HasDescr (i,OPun) || m_Tokens[i].IsSoft()) ; i++);
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
	for ( ; (i<HB)  && !m_Tokens[i].IsSpace() ; i++);
	return  i;
}


size_t CUnitHolder::BSpace (size_t i, size_t LB) const
{
	if (i == 0) {
		return 0;
	}
	i--;
	for ( ; (i>LB) && m_Tokens[i].IsSpace(); i--);

	return i;
}

size_t CUnitHolder::PSoft (size_t i, size_t HB) const
{
	if (i >= HB) return HB;

	for ( ;(i<HB) && m_Tokens[i].IsSoft(); i++);

	return i;
}

size_t CUnitHolder::BSoft (size_t i) const
{
	if (i == 0) {
		return 0;
	}

	i--;

	for ( ; (i>0) && m_Tokens[i].IsSoft(); i--);

	return i;
}


bool CUnitHolder::IsQuestionOrExclamationMarks (size_t i) const 
{
	return   (    (m_Tokens[i].GetTokenLength() > 0)
				&& (      ((unsigned  char)m_Tokens[i].GetToken()[0] == '?')
					||  ((unsigned  char)m_Tokens[i].GetToken()[0] == '!')
					)
			);
};

		                    
bool CUnitHolder::IsSentenceEndMark (size_t  i) const
{
	return			IsOneFullStop(i) 
				|| IsQuestionOrExclamationMarks(i)
				|| ((m_Tokens[i].GetTokenLength() == 1)  &&  ((BYTE)m_Tokens[i].GetToken()[0] == cEllipseChar))
				|| (
						(m_Tokens[i].GetTokenLength() >1)
					&&  (m_Tokens[i].GetTokenLength() <6)
					&&  ((unsigned  char)m_Tokens[i].GetToken()[0] ==  '.')
				)
				|| m_Tokens[i].IsTextAreaEnd();
};

bool CUnitHolder::IsOneCloseQuotationMark (size_t i) const
{
	if (i == 0) return false;
	BYTE z = (BYTE)m_Tokens[i].GetToken()[0];
	return	(m_Tokens[i].GetTokenLength() == 1)
		&&	(	( z == (BYTE)'"')
			||	( z == CloseQuoteMark1251)
		); 
};

bool CUnitHolder::IsOneOpenQuotationMark (size_t i) const
{
	BYTE z = (BYTE)m_Tokens[i].GetToken()[0];
	return     (m_Tokens[i].GetTokenLength() == 1)
		&& (     (z == (BYTE)'"')
			|| (z ==OpenQuoteMark1251)
			); 
};



void CUnitHolder::SetDes(size_t x, Descriptors des)  
{
	m_Tokens[x].SetDes(des);
};

bool CUnitHolder::HasDescr(size_t i, Descriptors descr)  const {
	return m_Tokens[i].HasDes(descr); 
}






//  i - пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ  пїЅпїЅпїЅпїЅпїЅпїЅ.
// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ  пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
// пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ. 

bool  CUnitHolder::EmptyLineBeforeGraph (size_t i, size_t HB) const
{
	if ((i == 0) || m_Tokens[i].IsSoft()) return false;
	size_t k = BSpace (i, 0);
	if (!m_Tokens[k].IsEOLN())  return false;
    if (   (m_Tokens[k].GetTokenLength()>2)
		|| (    (m_Tokens[k].GetTokenLength() == 2)
		        && (m_Tokens[k].GetToken()[0] == '\n')
		  	)
	    ) return true; 
	k--;
	if ( k ==  0 ) return false;
	k = BSpace (k + 1, 0); 
	return m_Tokens[k].IsEOLN();
};


void CUnitHolder::FreeTable()
{
	m_Tokens.clear();
}


const std::string&  CUnitHolder::GetToken(uint32_t LineNo) const 
{
	return m_Tokens[LineNo].GetToken();
};

const std::string&  CUnitHolder::GetUpperString(uint32_t LineNo) const
{
	return m_Tokens[LineNo].GetTokenUpper();
};

	
size_t	CUnitHolder::GetTokensCount() const 
{
	return m_Tokens.size();
}

uint32_t	CUnitHolder::GetTokenInputOffset(uint32_t LineNo) const 
{
	return GetUnits()[LineNo].GetInputOffset(); 
}

BYTE	CUnitHolder::GetTokenLength(uint32_t LineNo) const 
{	
	return GetUnits()[LineNo].GetTokenLength(); 
};


void	CUnitHolder::InitInputText(const std::string& utfString)
{
	m_InputText = convert_from_utf8(utfString.c_str(), m_Language);

	if (m_InputText.length() > 0x500000)
	{
		throw CExpc("Graphan cannot process files larger than 5 MB");
	};

	// to process it as a null-terminated string
	std::replace(m_InputText.begin(), m_InputText.end(), '\x0', '\x1');
};


void	CUnitHolder::ClearInputBuffer()
{
	m_InputText.clear();
}

void	CUnitHolder::DeleteDescr(size_t LineNo, Descriptors d)
{
	m_Tokens[LineNo].DelDes(d);
};


CGraLine& CUnitHolder::GetUnit(size_t UnitNo) 
{ 
	return m_Tokens[UnitNo]; 
};


const std::vector<CGraLine>& CUnitHolder::GetUnits() const
{ 
	
	return m_Tokens; 
};

std::string CUnitHolder::GetTokenUtf8(size_t line_no) const {
	return m_Tokens[line_no].GetTokenUtf8();
}

void	CUnitHolder::ClearPairDescriptors(size_t start_token_no, size_t end_token_no)
{
	for (size_t t=start_token_no; t<end_token_no; t++)
		for (uint8_t des=0; des<64; des++)
			if (des != OSentEnd)
			if (HasDescr(t, (Descriptors)des))
			if (IsFirstMemberOfPairDesciptor((Descriptors)des))
			{
				Descriptors dual_descr = GetSecondMemberByTheFirst((Descriptors)des);
				size_t i=t;
				for (; i<t+20; i++)
					if (HasDescr(i, dual_descr))
					{
						DeleteDescr(i,dual_descr);
						break;
					};
				//  we should find the dual descriptor in this range
				assert (i < t+20);
					DeleteDescr(t,(Descriptors)des);
			}
			else
				if (IsSecondMemberOfPairDesciptor((Descriptors)des))
				{
					Descriptors dual_descr = GetFirstMemberByTheSecond((Descriptors)des);
					int i=t;
					for (; i>t-20; i--)
						if (HasDescr(i, dual_descr))
						{
							DeleteDescr(i,dual_descr);
							break;
						};
					//  we should find the dual descriptor in this range
					assert (i > t-20);
					DeleteDescr(t,(Descriptors)des);
				};

		

};


/*
 all descriptors which end a text period like a sentence or a paragraph
*/
static bool IsEndTextPeriodDescriptor(Descriptors d)
{
	return     (d == CS_Undef)
		|| (d == CS_Simple)
		|| (d == CS_Parent)
		|| (d == CS_Quasi)
		|| (d == CS_Heading)
		|| (d == CS_Explan)
		|| (d == OSentEnd);
};


void	CUnitHolder::MakeOneWord(size_t start_token_no, size_t end_token_no)
{
	if (start_token_no+1 == end_token_no) return;

	//  delete all pair descriptors which intersects with [start_token_no,end_token_no)
	ClearPairDescriptors(start_token_no, end_token_no);

	//  move "end of sentence" descriptors from all lines of [start_token_no,end_token_no) to start_token_no
	for (size_t t=start_token_no + 1; t<end_token_no; t++)
		for (size_t i=0; i<64; i++)
			if	(		IsEndTextPeriodDescriptor((Descriptors)i) 
					&&	HasDescr(t, (Descriptors)i)
				)
			SetDes(start_token_no, (Descriptors)i);

	//  add all hard lines to the first line, counting all spaces  in order to maintain the file offsets
	size_t spaces_len = 0;
	for (size_t t=start_token_no + 1; t<end_token_no; t++)
	{
		if (!m_Tokens[t].IsSoft())
		{
			GetUnit(start_token_no).AppendToken(m_Tokens[t]);
		}
		else {
			spaces_len += m_Tokens[t].GetTokenLength();
		}
	};


	//  if some spaces were found in (start_token_no, end_token_no)
	// then fill line start_token_no+1 with the spaces
	if (spaces_len > 0)
	{
		m_Tokens[start_token_no + 1].MakeSpaces(spaces_len);
		GetUnit(start_token_no + 1).SetOborot(-1, false);
		m_Tokens.erase(m_Tokens.begin() + start_token_no + 2, m_Tokens.begin() + end_token_no);
	}
	else
	{
		m_Tokens.erase(m_Tokens.begin() + start_token_no + 1, m_Tokens.begin() + end_token_no);
	}
};


bool CUnitHolder::IsOneFullStop(size_t i) const
{
	return (GetUnits()[i].GetTokenLength() == 1) && (GetUnits()[i].GetToken()[0] == '.');
};
