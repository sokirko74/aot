// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once

#include <assert.h>
#include "morph_dict/common/utilit.h"
#include "morph_dict/common/bserialize.h"

const BYTE ErrUChar	= 254;

const BYTE UnknownSignatId = 16;


const char _FieldFormat[] = "%-8s= %s";
const char _FieldFormatEqual[] = "%-8s== %s";


typedef uint32_t dom_item_id_t;

const uint32_t EmptyDomItemId = 0xffffffff;

inline BYTE get_dom_no(dom_item_id_t item_id) {
	return item_id >> 24;
}

inline uint32_t get_inner_item_index(dom_item_id_t item_id) {
	return item_id & 0x00ffffff;
}

inline bool is_null(dom_item_id_t item_id) {
	return item_id == EmptyDomItemId;
}

inline dom_item_id_t build_item_id(BYTE dom_no, uint32_t inner_item_index) {
	return (24 << dom_no) | inner_item_index;

}

template <int MaxNumDom>
class TBasicCortege {

	BYTE m_SignatIdAndEqualFlag;  // reference to Ross :: Fields[m_FieldNo].m_Signats


public:
	BYTE m_FieldNo;      // reference to Ross :: Fields
	BYTE m_LevelId;
	BYTE m_LeafId;
	BYTE m_BracketLeafId;
	dom_item_id_t  m_DomItemNos[MaxNumDom];



	TBasicCortege()
	{
		m_SignatIdAndEqualFlag = UnknownSignatId;
		m_FieldNo = ErrUChar;
		m_LeafId = 0;
		m_BracketLeafId = 0;
		for (size_t i = 0; i < MaxNumDom;i++)
			SetItem(i, EmptyDomItemId);
	};

	template<int MaxNumDom1>
	TBasicCortege(const TBasicCortege<MaxNumDom1>& X)
	{
		m_FieldNo = X.m_FieldNo;
		m_LeafId = X.m_LeafId;
		m_BracketLeafId = X.m_BracketLeafId;
		m_LevelId = X.m_LevelId;
		SetSignatId(X.GetSignatId());
		if (X.IsUsedForInclusion()) {
			SetEqual();
		}

		int i = 0;
		for (; i < min(MaxNumDom1, MaxNumDom); i++)
			SetItem(i, X.GetItem(i));

		for (; i < MaxNumDom; i++)
			SetItem(i, EmptyDomItemId);
	};

	int GetItem(size_t index)  const
	{
		assert(index < MaxNumDom);
		return m_DomItemNos[index];
	};

	int SetItem(size_t index, dom_item_id_t Value)
	{
		assert(index < MaxNumDom);
		return m_DomItemNos[index] = Value;
	};

	BYTE GetMaxNumDom() const {
		return MaxNumDom;
	}


	bool is_null(BYTE i) const {
		return ::is_null(m_DomItemNos[i]);
	}

	BYTE GetSignatId()  const
	{
	  return m_SignatIdAndEqualFlag & ~128;
	};

	void SetSignatId(BYTE SignatId) 
	{
		m_SignatIdAndEqualFlag = SignatId | (128 & m_SignatIdAndEqualFlag);
	};

	bool IsUsedForInclusion() const
	{
		return (m_SignatIdAndEqualFlag & 128) > 0;
	};

	void SetEqual()
	{
		m_SignatIdAndEqualFlag |= 128;
	};

	const  char* GetFieldFormat() const
	{
	  return IsUsedForInclusion() ? _FieldFormatEqual : _FieldFormat;

	};

	bool HasEqualItems(const TBasicCortege&  X, BYTE _MaxNumDom)  const 
	{  
		for (BYTE i=0; i< _MaxNumDom;i++)
			if (GetItem(i) != X.GetItem(i))
				return false;
		return true;
	};

	bool EqualCortege(const TBasicCortege& X, BYTE _MaxNumDom)  const 
	{  
	  return    (m_FieldNo  == X.m_FieldNo)  
	             && (m_SignatIdAndEqualFlag == X.m_SignatIdAndEqualFlag)
				 && (m_LevelId  == X.m_LevelId)
				 && (m_LeafId   == X.m_LeafId)
				 && (m_BracketLeafId   == X.m_BracketLeafId)
				 && HasEqualItems (X, _MaxNumDom);
	};

	bool IsEqualWithWildCard(const TBasicCortege& X, uint16_t WildCardDomItemNo, BYTE _MaxNumDom) const
	{  
		if    (!(   (m_FieldNo  == X.m_FieldNo)  
			 && (      (m_LevelId == ErrUChar)
					|| (X.m_LevelId == ErrUChar)
					|| (m_LevelId == X.m_LevelId)
				)
			 && (      (m_LeafId == ErrUChar)
					|| (X.m_LeafId == ErrUChar)
					|| (m_LeafId == X.m_LeafId)
				)
			 && (      (m_BracketLeafId == ErrUChar)
					|| (X.m_BracketLeafId == ErrUChar)
					|| (m_BracketLeafId == X.m_BracketLeafId)
				)

			 )) return false;

		for (size_t i=0; i< _MaxNumDom;i++)
		   if (     (GetItem(i)  != X.GetItem(i))
				&&  (GetItem(i)  != WildCardDomItemNo)
				&&  (X.GetItem(i) != WildCardDomItemNo)
			  )
		   return false; 

		return true;
	};

	size_t get_size_in_bytes() const {
		return	::get_size_in_bytes(m_FieldNo)
			+ ::get_size_in_bytes(m_SignatIdAndEqualFlag)
			+ ::get_size_in_bytes(m_LevelId)
			+ ::get_size_in_bytes(m_LeafId)
			+ ::get_size_in_bytes(m_BracketLeafId)
			+ ::get_size_in_bytes(m_DomItemNos[0]) * MaxNumDom
			;

	}

	size_t save_to_bytes(BYTE* buf) const
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


	size_t restore_from_bytes (const BYTE* buf)
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

	void shift_left(BYTE start=0) {
		for (BYTE k = start; k < MaxNumDom - 1; ++k) {
			SetItem(k, GetItem(k + 1));
		}
		SetItem(MaxNumDom - 1, EmptyDomItemId);
	}



};


template <int MaxNumDom>
size_t get_size_in_bytes (const TBasicCortege<MaxNumDom>& t)
{
	return	t.get_size_in_bytes();
};

template <int MaxNumDom>
size_t save_to_bytes(const TBasicCortege<MaxNumDom>& i, BYTE* buf)
{
	return i.save_to_bytes(buf);
};


template <int MaxNumDom>
size_t restore_from_bytes(TBasicCortege<MaxNumDom>& i, const BYTE* buf)
{
	return i.restore_from_bytes(buf);
};

typedef TBasicCortege<3> TCortege3;
typedef TBasicCortege<10> TCortege10;

