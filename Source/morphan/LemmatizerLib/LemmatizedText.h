// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#pragma once
#pragma warning (disable : 4786)

#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"
#include "LemWord.h"


class CLemmatizedText
{
public:
	std::vector<CLemWord>	m_LemWords;
	CAgramtab* m_pGramTab;
	CLemmatizer* m_pLemmatizer;

	CLemmatizedText(CAgramtab* pGramTab=nullptr, CLemmatizer* pLemmatizer=nullptr);

	bool SaveToFile(std::string filename) const;
	void CreateFromTokemized(const CGraphmatFile* piGraphmatFile);
	
};

inline bool CheckIfHomonymPlmLine(const std::string& strPlmLine)
{
	return strPlmLine[0] == ' ';
}

