// BinaryDictionary.h : Declaration of the CBinaryDictionary

#ifndef __DERIVDIC_H_
#define __DERIVDIC_H_
#pragma warning(disable:4786)
#pragma warning(disable:4530)


#include "../common/utilit.h"



/**
Этот класс -- хранилище бызы данных словаря словобразования.
Бинарный файл устроен так:
Сначала идет текстовая часть:
N  -- число
афикс1
...
афиксN
Потом стоит символ 0 и далее в бинарном виде идут тройки UINTов
id1 id2 index

id1 и id2 -- ParadigmId слов из пары. 
index -- индекс афикса.

*/

enum AFIX_KIND
    {	DERIV_SUFFIX	= 0,
	DERIV_PREFIX	= 1,
	DERIV_SUP_ADJ	= 2
    } ;


class CDerivDictonary 
{
	struct deriv_pair
	{
		UINT id1, id2;
		UINT index;
		deriv_pair(UINT a, UINT b, UINT i = 0)
			:id1(a),  id2(b), index(i){}
		struct Less1{
			bool operator()(const deriv_pair &p1, const deriv_pair &p2) const{
				return p1.id1 < p2.id1;
			}
		};
		struct Less2{
			bool operator()(const deriv_pair &p1, const deriv_pair &p2) const{
				return p1.id2 < p2.id2;
			}
		};

	};

	std::vector<deriv_pair> vec1, vec2;
	typedef std::vector<deriv_pair>::const_iterator Iter;
	StringVector pref; // prefixes or suffixes -- afix


	//! read the database
	bool LoadFromFile(FILE* fp);

public:
	const string& GetAffixStr(DWORD index) const;

	bool Load(int TypeDict);
	//! find target by source. Return number of found targets.
	size_t SourceToTarget(UINT id, DwordVector &ids, DwordVector &afixes) const;
	//! find source by target. Return number of found sources.
	size_t TargetToSource(UINT id, DwordVector &ids, DwordVector &afixes) const;
	//! Number of afixes
	size_t AfixCount() const;

};

#endif 
