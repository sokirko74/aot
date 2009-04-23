#ifndef _INCL_TRANSLATE_H_
#define _INCL_TRANSLATE_H_

#include "../SemanLib/SemStructureBuilder.h"

class RussianIntoEnglishTranslator
{
public:
	CSemStructureBuilder m_SemBuilder;
	RussianIntoEnglishTranslator();
	~RussianIntoEnglishTranslator();
	string GetJavaGraph();
	string translate(string russian, const string &po);
	string build_rus_graph(string russian, const string &po);
};

#endif //_INCL_TRANSLATE_H_