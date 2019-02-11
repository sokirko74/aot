#ifndef __SYNDICT_H_
#define __SYNDICT_H_

#include "../common/utilit.h"       // main symbols


class CSynDictionary 
{
	std::map<long, std::vector<long> > synonims;
	mutable std::map<long, std::vector<long> >::const_iterator curr_synset;
public:
	CSynDictionary();

	bool Load(string FileName);
	DWORD GetId(UINT Index) const;
	size_t FindSynonyms(DWORD  ParadigmId) const;
	
};

#endif 
