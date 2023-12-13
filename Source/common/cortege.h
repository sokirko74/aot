// ==========  This file is under  LGPL, the GNU Lesser General Public License
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
	return ((uint32_t)dom_no << 24) | inner_item_index;

}

const int MaxNumDom = 3;

class TCortege {

	BYTE m_SignatIdAndEqualFlag;  // reference to Ross :: Fields[m_FieldNo].m_Signats
	dom_item_id_t  m_DomItemNos[MaxNumDom];


public:
	BYTE m_FieldNo;      // reference to Ross :: Fields
	BYTE m_LevelId;
	BYTE m_LeafId;
	BYTE m_BracketLeafId;



	TCortege();

	int GetItem(size_t index)  const;
	int SetItem(size_t index, dom_item_id_t Value);
	bool is_null(BYTE i) const;
	BYTE GetSignatId()  const;
	void SetSignatId(BYTE SignatId);
	bool IsUsedForInclusion() const;
	void SetEqual();
	const  char* GetFieldFormat() const;
	bool HasEqualItems(const TCortege& X) const;
	bool EqualCortege(const TCortege& X) const;
	bool IsEqualWithWildCard(const TCortege& X, dom_item_id_t WildCardDomItemNo) const;
	size_t get_size_in_bytes() const;
	size_t save_to_bytes(BYTE* buf) const;
	size_t restore_from_bytes(const BYTE* buf);

};


inline size_t get_size_in_bytes (const TCortege& t)
{
	return	t.get_size_in_bytes();
};


inline size_t save_to_bytes(const TCortege& i, BYTE* buf)
{
	return i.save_to_bytes(buf);
};


inline size_t restore_from_bytes(TCortege& i, const BYTE* buf)
{
	return i.restore_from_bytes(buf);
};

