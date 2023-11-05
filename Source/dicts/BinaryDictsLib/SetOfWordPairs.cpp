#include "stdafx.h"
#include "SetOfWordPairs.h"
#include "BinaryDictionary.h"


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

bool CSetOfWordPairs::IsNormalLanguage(long pair_index) const
{
	return dict->IsNormalLanguage(begin + pair_index, is_direct);
}

part_of_speech_t CSetOfWordPairs::GetSimplifiedPartOfSpeech(long pair_index) const {
    return dict->GetSimplifiedPartOfSpeech(begin + pair_index, is_direct);
}
