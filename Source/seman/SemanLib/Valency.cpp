#include "stdafx.h"

#include "Valency.h"
#include "struct_dict_holder.h"

CValency::CValency  () 
{
	m_RelationStr = "";
	m_Direction = A_C;
	m_IsAddition = false;
	m_LeafId = 0;
	m_bObligatory = false;
	m_BracketLeafId = 0;
	m_RossHolder = 0;
	m_UnitNo = ErrUnitNo;
	m_InstrAgent = false;
	m_RelationId = -1;
	m_bOptional = false;
	m_bObligatoryBecauseFirst = false;
};


CValency  :: CValency  (const TCortege C, const CStructDictHolder* Ross, uint16_t UnitNo)
{
	m_Direction  =  (C.GetItem(1) == Ross->SelfLabelNo) ? C_A : A_C;
	m_RelationStr	= Ross->GetDomItemStrWrapper(C.GetItem(0));
	BYTE leaf_item_no = (m_Direction == A_C) ? 1 : 2;
	std::string S = Ross->GetDomItemStrWrapper(C.GetItem(leaf_item_no));
	assert ( S.length() >	1 );
	assert(isdigit ((unsigned	char)S[1]) );
	m_LeafId = S[1] -	'0';
	S	= Ross->GetRoss()->GetSignat(C).GetFrmt();
	m_bOptional =	S.find("?")	!= string::npos;
	m_bObligatory	= S.find("!") != string::npos;
	m_bObligatoryBecauseFirst	= false;

	
	m_BracketLeafId =	0;

	m_RossHolder = Ross;
	m_IsAddition = false;
	m_UnitNo = UnitNo;
	m_InstrAgent = false;
	m_RelationId = -1;
};

CValency::CValency  (std::string  RelationStr, Valency_Direction Direction, const CStructDictHolder* Ross, BYTE LeafId, BYTE BracketLeafId, uint16_t UnitNo)
{
	m_RelationStr = RelationStr;
	m_Direction = Direction;
	m_IsAddition = false;
	m_bObligatory = false;
	m_bOptional = false;
	m_RossHolder =  Ross;
	m_LeafId = LeafId;
	m_BracketLeafId = BracketLeafId;
	m_UnitNo = UnitNo;
	m_InstrAgent = false;
	m_RelationId = -1;
	m_bObligatoryBecauseFirst = false;
};


bool CValency::IsFromDict () const 
{
	return m_RossHolder != 0;
};
bool CValency::operator == ( const CValency& X) const
{
	return (m_RelationStr == X.m_RelationStr); 
};
