#pragma once

#include "morph_dict/common/utilit.h"
#include "LemmatizedText.h"
#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

class CLemTextCreator
{
protected:
	MorphLanguageEnum m_Language;
	CGraphmatFile	m_Graphan;
public:
	CLemmatizedText	m_LemText;

	CLemTextCreator(MorphLanguageEnum l);

	void InitGraphan();

	bool BuildLemText(std::string str, bool bFile, int& CountOfWords);
};


