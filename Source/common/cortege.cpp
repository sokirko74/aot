
#include "common/cortege.h"
#include "morph_dict/common/utilit.h"
#include "morph_dict/common/bserialize.h"


TCortege::TCortege()
{
	m_SignatIdAndEqualFlag = UnknownSignatId;
	m_FieldNo = ErrUChar;
	m_LeafId = 0;
	m_BracketLeafId = 0;
	for (size_t i = 0; i < MaxNumDom;i++)
		SetItem(i, EmptyDomItemId);
};

bool TCortege::is_null(BYTE i) const {
	return ::is_null(m_DomItemNos[i]);
}


int TCortege::GetItem(size_t index)  const
{
	assert(index < MaxNumDom);
	return m_DomItemNos[index];
};


int TCortege::SetItem(size_t index, dom_item_id_t Value)
{
	assert(index < MaxNumDom);
	return m_DomItemNos[index] = Value;
};


BYTE TCortege::GetSignatId()  const
{
	return m_SignatIdAndEqualFlag & ~128;
};

void TCortege::SetSignatId(BYTE SignatId)
{
	m_SignatIdAndEqualFlag = SignatId | (128 & m_SignatIdAndEqualFlag);
};

bool TCortege::IsUsedForInclusion() const
{
	return (m_SignatIdAndEqualFlag & 128) > 0;
};

void TCortege::SetEqual()
{
	m_SignatIdAndEqualFlag |= 128;
};

const  char* TCortege::GetFieldFormat() const
{
	return IsUsedForInclusion() ? _FieldFormatEqual : _FieldFormat;

};

bool TCortege::HasEqualItems(const TCortege& X)  const
{
	for (BYTE i = 0; i < MaxNumDom;i++)
		if (GetItem(i) != X.GetItem(i))
			return false;
	return true;
};

bool TCortege::EqualCortege(const TCortege& X)  const
{
	return    (m_FieldNo == X.m_FieldNo)
		&& (m_SignatIdAndEqualFlag == X.m_SignatIdAndEqualFlag)
		&& (m_LevelId == X.m_LevelId)
		&& (m_LeafId == X.m_LeafId)
		&& (m_BracketLeafId == X.m_BracketLeafId)
		&& HasEqualItems(X);
};

bool TCortege::IsEqualWithWildCard(const TCortege& X, dom_item_id_t WildCardDomItemNo) const
{
	if (!((m_FieldNo == X.m_FieldNo)
		&& ((m_LevelId == ErrUChar)
			|| (X.m_LevelId == ErrUChar)
			|| (m_LevelId == X.m_LevelId)
			)
		&& ((m_LeafId == ErrUChar)
			|| (X.m_LeafId == ErrUChar)
			|| (m_LeafId == X.m_LeafId)
			)
		&& ((m_BracketLeafId == ErrUChar)
			|| (X.m_BracketLeafId == ErrUChar)
			|| (m_BracketLeafId == X.m_BracketLeafId)
			)

		)) return false;

	for (size_t i = 0; i < MaxNumDom;i++)
		if ((GetItem(i) != X.GetItem(i))
			&& (GetItem(i) != WildCardDomItemNo)
			&& (X.GetItem(i) != WildCardDomItemNo)
			)
			return false;

	return true;
};


size_t TCortege::get_size_in_bytes() const {
	return	::get_size_in_bytes(m_FieldNo)
		+ ::get_size_in_bytes(m_SignatIdAndEqualFlag)
		+ ::get_size_in_bytes(m_LevelId)
		+ ::get_size_in_bytes(m_LeafId)
		+ ::get_size_in_bytes(m_BracketLeafId)
		+ ::get_size_in_bytes(m_DomItemNos[0]) * MaxNumDom
		;

}

size_t TCortege::save_to_bytes(BYTE* buf) const
{
	buf += ::save_to_bytes(m_FieldNo, buf);
	buf += ::save_to_bytes(m_SignatIdAndEqualFlag, buf);
	buf += ::save_to_bytes(m_LevelId, buf);
	buf += ::save_to_bytes(m_LeafId, buf);
	buf += ::save_to_bytes(m_BracketLeafId, buf);

	for (int j = 0; j < MaxNumDom; j++)
		buf += ::save_to_bytes(m_DomItemNos[j], buf);

	return get_size_in_bytes();
};


size_t TCortege::restore_from_bytes(const BYTE* buf)
{
	buf += ::restore_from_bytes(m_FieldNo, buf);
	buf += ::restore_from_bytes(m_SignatIdAndEqualFlag, buf);
	buf += ::restore_from_bytes(m_LevelId, buf);
	buf += ::restore_from_bytes(m_LeafId, buf);
	buf += ::restore_from_bytes(m_BracketLeafId, buf);
	for (int j = 0; j < MaxNumDom; j++)
		buf += ::restore_from_bytes(m_DomItemNos[j], buf);

	return get_size_in_bytes();
};

void TCortege::shift_left(BYTE start) {
	for (BYTE k = start; k < MaxNumDom - 1; ++k) {
		SetItem(k, GetItem(k + 1));
	}
	SetItem(MaxNumDom - 1, EmptyDomItemId);
}


