#pragma once

#include "morph_dict/common/utilit.h"
#include "LemmatizedText.h"
#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

class CGraphanAndMorphanHolder: public CMorphanHolder
{
public:
	CGraphmatFile	m_Graphan;
	CLemmatizedText	m_LemText;

	CGraphanAndMorphanHolder();
	~CGraphanAndMorphanHolder();

	bool GetMorphology(std::string str, bool bFile, int& CountOfWords);
    void LoadGraphanAndLemmatizer(MorphLanguageEnum langua);
};

extern bool ProcessHyphenWords(const CLemmatizer* lemmatizer, CGraphmatFile* piGraphmatFile);

