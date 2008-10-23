// SynDictionary.cpp : Implementation of CSynDictApp and DLL registration.
#pragma  warning (disable:4786)
#pragma  warning (disable:4530)
#include "stdafx.h"
#include <fstream>
#include "SynDictionary.h"

/////////////////////////////////////////////////////////////////////////////
//

CSynDictionary::CSynDictionary() 
{
}

bool CSynDictionary::Load(string FileName)
{
	// TODO: Add your implementation code here
	long pos=0;
	long  all_synsets_count;
	try {

		string fname = FileName;
		ifstream dict(fname.c_str(), std::ios::in | std::ios::binary);
		
		dict.read((char*)&all_synsets_count, sizeof(long));
		long cur, dummy;
		long cnt;
		
		for (pos=0; pos < all_synsets_count; pos++)
		{
			dict.read((char*)&cur, sizeof(long));
			dict.read((char*)&cnt, sizeof(long));
				
			for(unsigned int i = 0; i < cnt; i++)
			{
				dict.read((char*)&dummy, sizeof(long));
				synonims[cur].push_back(dummy);
			}
		}
		return true;
	}
	catch (...)
	{
		fprintf (stderr, "Cannot read synset %i from (%i) %s\n", pos, all_synsets_count, FileName.c_str());
		return false;
	};

	
}

size_t CSynDictionary::FindSynonyms(DWORD ParadigmId) const
{
	// TODO: Add your implementation code here
    curr_synset = synonims.find(ParadigmId);
    if(curr_synset == synonims.end()) return 0;
	return curr_synset->second.size();
}

DWORD CSynDictionary::GetId(UINT Index) const
{
	// TODO: Add your implementation code here
	if(curr_synset == synonims.end()) throw CExpc ("bad index in synonyms vector");
	return curr_synset->second[Index];
}
