
#ifndef MorphologyHolder_H
#define MorphologyHolder_H

#include "morph_dict/common/utilit.h"
#include "PLMLineCollection.h"
#include "graphan/GraphanLib/GraphmatFile.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

class CGraphanAndMorphanHolder: public CMorphanHolder
{
public:
	bool							m_bTimeStatis;
	CPlmLineCollection				m_PlmLines;
	CGraphmatFile					m_Graphan;

	CGraphanAndMorphanHolder();
	~CGraphanAndMorphanHolder();

	bool GetMorphology(std::string str, bool bFile, int& CountOfWords);
    void LoadGraphanAndLemmatizer(MorphLanguageEnum langua);
};

extern bool ProcessHyphenWords(const CLemmatizer* lemmatizer, CGraphmatFile* piGraphmatFile);

#endif
