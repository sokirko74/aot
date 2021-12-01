#ifndef _LEMMA_INFO_SERIALIZE_H
#define _LEMMA_INFO_SERIALIZE_H

#include  "morph_dict/common/bserialize.h"

inline size_t get_size_in_bytes (const CLemmaInfo& t)
{
	return		
				get_size_in_bytes(t.m_FlexiaModelNo)
			+   get_size_in_bytes(t.m_AccentModelNo)
			+   4; // CommonAncodeSize is 2 but we need to pad to 4 bytes
			;
};



inline size_t save_to_bytes(const CLemmaInfo& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_FlexiaModelNo, buf);
	buf += save_to_bytes(i.m_AccentModelNo, buf);
	buf += save_to_bytes((BYTE)i.m_CommonAncode[0], buf);
	buf += save_to_bytes((BYTE)i.m_CommonAncode[1], buf);
	buf += 2;
	return get_size_in_bytes(i);
}


inline size_t restore_from_bytes(CLemmaInfo& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_FlexiaModelNo, buf);
	buf += restore_from_bytes(i.m_AccentModelNo, buf);
	buf += restore_from_bytes(i.m_CommonAncode[0], buf);
	buf += restore_from_bytes(i.m_CommonAncode[1], buf);
	return get_size_in_bytes(i);
}


#endif
