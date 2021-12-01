// BinaryDictionary.cpp : Implementation of CBinaryDictionary
#include "stdafx.h"
#include "morph_dict/common/utilit.h"
#include "DerivDict.h"



bool CDerivDictonary::LoadFromFile(FILE* fp)
{
	uint32_t pref_size;
	fscanf (fp, "%i\n",&pref_size);
	if (pref_size == 0) return false;
	pref.reserve(pref_size);
	int i;
	for(i = 0; i < pref_size; i++){
		char s[200];
		fgets(s, 200, fp);
		std::string q = s;
		Trim(q);
		if (q.empty()) return false;
		pref.push_back(q);
	}

	while(true)
	{
		uint32_t id1, id2, index;
		fread((char*)&id1, 1, sizeof(uint32_t), fp);
		fread((char*)&id2, 1, sizeof(uint32_t), fp);
		fread((char*)&index, 1, sizeof(uint32_t), fp);
		if(feof(fp)) break;
		vec1.push_back(deriv_pair(id1, id2, index));
	}
	vec2 = vec1;
	std::sort(vec1.begin(), vec1.end(), deriv_pair::Less1());
	std::sort(vec2.begin(), vec2.end(), deriv_pair::Less2());
	return true;
}

size_t CDerivDictonary::SourceToTarget(uint32_t id, DwordVector &ids, DwordVector &afixes) const
{
	afixes.clear();
	ids.clear();

	std::pair<Iter, Iter> range = std::equal_range
		(vec1.begin(), vec1.end(), deriv_pair(id, 0), deriv_pair::Less1());

	for(Iter it = range.first; it != range.second; ++it){
		ids.push_back(it->id2);
		afixes.push_back(it->index);
	}
	return ids.size();		
}

//! find source by target. Return number of found sources.
size_t CDerivDictonary::TargetToSource(uint32_t id, DwordVector &ids, DwordVector &afixes) const
{
	afixes.clear();
	ids.clear();
	std::pair<Iter, Iter> range = std::equal_range
		(vec2.begin(), vec2.end(), deriv_pair(0, id), deriv_pair::Less2());

	for(Iter it = range.first; it != range.second; ++it){
		ids.push_back(it->id1);
		afixes.push_back(it->index);
	}
	return ids.size();		
}


//! Number of afixes
size_t CDerivDictonary::AfixCount() const
{
	return pref.size();
}


bool CDerivDictonary::Load(int TypeDict)
{
	bool bResult = false;
	try {

		std::string Path;
		if (TypeDict == DERIV_SUP_ADJ)
			Path = GetRegistryString("Software\\Dialing\\RUSSIAN_DERIV_DICT\\SupAdjDictPath");
		else
		if (TypeDict == DERIV_PREFIX)
			Path = GetRegistryString("Software\\Dialing\\RUSSIAN_DERIV_DICT\\PrefixDictPath");
		else
			Path = GetRegistryString("Software\\Dialing\\RUSSIAN_DERIV_DICT\\SuffixDictPath");

		FILE * fp = fopen (Path.c_str(), "rb");
		if (fp)
		{
			if ( LoadFromFile(fp) )
				bResult = true;

			fclose(fp);
		};
		
	}
	catch (...)
	{
		
	};
	return bResult;
}


const std::string& CDerivDictonary::GetAffixStr(uint32_t index) const
{
	return pref[index];
}
