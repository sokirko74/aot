// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#pragma once
#pragma warning (disable : 4786)

#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"



class CPlmLineCollection
{
public:
	StringVector			m_PlmItems;
	CLemmatizer*			m_pLemmatizer;

	CPlmLineCollection();
	~CPlmLineCollection();

	bool SaveToFile(std::string filename) const;
	bool ProcessPlmLines(const CGraphmatFile* piGraphmatFile);
	
};

inline bool CheckIfHomonymPlmLine(const char* strPlmLine)
{
	return( strPlmLine[0] == ' ') ;
}

