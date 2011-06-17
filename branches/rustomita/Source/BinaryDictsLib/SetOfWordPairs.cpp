// SetOfWordPairs.cpp : Implementation of CSetOfWordPairs
#include "stdafx.h"
#include "SetOfWordPairs.h"
#include "BinaryDictionary.h"


// PUT_LABELS_ARRAY


CSetOfWordPairs::CSetOfWordPairs()
{
}

void CSetOfWordPairs::SetRange(long _begin, long _end){
	begin = _begin;
	end = _end;
}

void CSetOfWordPairs::SetDictionary(CBinaryDictionary const *d, bool direct){
	dict = const_cast<CBinaryDictionary *>(d);
	is_direct = direct;
}


long CSetOfWordPairs::get_Count() const
{
	return end - begin;
}

long  CSetOfWordPairs::GetId(long index) const
{
	return is_direct
		? dict->GetDirectId(begin+index)
		: dict->GetIndirectId(begin+index);
}

long CSetOfWordPairs::GetFlag(long pair_index, long flag_index, string& text) const
{
	DWORD Res;
	dict->GetFlag(begin+pair_index, flag_index, text, is_direct, Res);
	return Res;
}

