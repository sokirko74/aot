#include "stdafx.h"
#include "DictUnitInterp.h"
#include "RossHolder.h"

	

WORD CorrectAdjectiveWithA0SemFet (const CRossHolder* RossDoc, long UnitNo);


//===============================================================================
CRossInterp::CRossInterp() {
	m_DictType = NoneRoss;
	m_UnitNo = ErrUnitNo;
};

CRossInterp::CRossInterp(DictTypeEnum DictType, WORD UnitNo)  {
	m_DictType = DictType;
	m_UnitNo = UnitNo;
};
bool CRossInterp::operator == ( const CRossInterp& X ) const
{
	return (X.m_DictType == m_DictType) && (X.m_UnitNo == m_UnitNo);
};
CRossInterp& CRossInterp::operator = ( const CRossInterp& X ) 
{
	m_DictType = X.m_DictType;
	m_UnitNo = X.m_UnitNo;
	return *this;
};

//===============================================================================
CDictUnitInterp::CDictUnitInterp()  : CRossInterp() {
  	m_bNegativeForm = false;
	m_bPassiveForm = false;
	m_TerminId = -1;
};

CDictUnitInterp::CDictUnitInterp (DictTypeEnum   DictType,	WORD UnitNo) 
	: 		CRossInterp(DictType, UnitNo)
{
	m_bPassiveForm = false;
	m_bNegativeForm =  false;
	m_TerminId = -1;
};


CDictUnitInterp::CDictUnitInterp (const  CRossHolder* RossDoc, DictTypeEnum   DictType,	WORD UnitNo, bool bNegativeForm, bool bPassiveForm)
{
	m_DictType = DictType;
	m_UnitNo = UnitNo;
 	m_bNegativeForm = bNegativeForm;
	m_AOReference.m_UnitNo = CorrectAdjectiveWithA0SemFet(RossDoc, UnitNo);
	m_AOReference.m_DictType = DictType;
	m_bPassiveForm =   bPassiveForm;
	m_TerminId = -1;
};

CDictUnitInterp::CDictUnitInterp ( const CRossInterp& X ) 
{
	m_DictType = X.m_DictType;
	m_UnitNo = X.m_UnitNo;
	m_TerminId = -1;
	m_bPassiveForm = false;
};

bool CDictUnitInterp::operator == ( const CDictUnitInterp& X ) const
{
	return (X.m_DictType == m_DictType) && (X.m_UnitNo == m_UnitNo);
};

WORD  CDictUnitInterp::GetSemCorrectUnitNo () const 
{
	return  (m_AOReference.m_UnitNo != ErrUnitNo) ? m_AOReference.m_UnitNo : m_UnitNo;
};


// иначе выдает ErrUnitNo.
WORD CorrectAdjectiveWithA0SemFet (const CRossHolder* RossDoc, long UnitNo)
{
 if (UnitNo == ErrUnitNo) return ErrUnitNo;
 if (!RossDoc->GetRoss()->IsEmptyArticle(UnitNo))
  for (size_t i = RossDoc->GetRoss()->GetUnitStartPos(UnitNo); i<= RossDoc->GetRoss()->GetUnitEndPos(UnitNo); i++)
	{
	    TCortege C = GetCortege(RossDoc->GetRoss(),i);
		if ( C.m_FieldNo ==  RossDoc->SemFetFieldNo )
		{
			if (C.m_DomItemNos[0] == RossDoc->A0LexFunctNo)
			{
				string Word = RossDoc->GetDomItemStrInner(C.m_DomItemNos[1]);
				string S =  (C.m_DomItemNos[2] != -1)  ? RossDoc->GetDomItemStrInner(C.m_DomItemNos[2]) : "0";
				long MeanNum = atoi(S.c_str());
				if (MeanNum == 0) MeanNum = 1;
				long un = RossDoc->LocateUnit(Word.c_str(), MeanNum);
				if (un != ErrUnitNo)
					return un;
				break;
			};
		}
	};

 return ErrUnitNo;
};


