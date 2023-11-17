// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#pragma once
#pragma warning (disable : 4786)

#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"



class CLemmatizedText
{
public:
	StringVector			m_PlmItems;
	CLemmatizer*			m_pLemmatizer;

	CLemmatizedText();

	bool SaveToFile(std::string filename) const;
	void CreateFromTokemized(const CGraphmatFile* piGraphmatFile);
	
};

inline bool CheckIfHomonymPlmLine(const char* strPlmLine)
{
	return( strPlmLine[0] == ' ') ;
}

