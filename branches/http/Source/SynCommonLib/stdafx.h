// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef stdsynancommon_h
#define stdsynancommon_h


#pragma warning (disable : 4267)

#include "../common/util_classes.h"


#define MAX_WORD_LEN                    100

#pragma warning (disable : 4786)
#include "../LemmatizerLib/Lemmatizers.h"
#include "../LemmatizerLib/PLMLineCollection.h"

#include "../AgramtabLib/agramtab_.h"
#include "../RmlThesLib/Thesaurus.h"
#include "../StructDictLib/Ross.h"




#include <functional>
#include <map>
#include <list>




template<class II, class Pr> inline
	II microsoft_find_if(II F, II L, Pr P)
{
	for (; F != L; ++F)
		if (P(*F))
			break;

	return (F); 
}

typedef  size_t EClauseType;
typedef size_t EGroupType;

#endif
