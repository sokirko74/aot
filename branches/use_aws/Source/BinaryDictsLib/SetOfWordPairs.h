// SetOfWordPairs.h : Declaration of the CSetOfWordPairs

#ifndef __SETOFWORDPAIRS_H_
#define __SETOFWORDPAIRS_H_

#include "../common/utilit.h"


/////////////////////////////////////////////////////////////////////////////
// CSetOfWordPairs
class CBinaryDictionary;
class CSetOfWordPairs 
{
	int begin, end;
	const CBinaryDictionary *dict;
	bool is_direct;
	void SetRange(long _begin, long _end);
	void SetDictionary(CBinaryDictionary const *d, bool direct);
	friend class CBinaryDictionary;
public:

	CSetOfWordPairs();

// ISetOfWordPairs
public:
	long GetFlag(long pair_index, long flag_index, string& text) const;
	long GetId(long index) const;
	long get_Count() const;
};

#endif //__SETOFWORDPAIRS_H_
