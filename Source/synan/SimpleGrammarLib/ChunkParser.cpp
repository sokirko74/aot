#include  "SimpleGrammarLib.h"
#include "ChunkParser.h"

#include "chunker_yacc.hpp"

#include <sstream>

extern int yydebug;

int yyparse (CChunkParser*);

int yylex (void* valp, CChunkParser* _prs)
{
	return _prs->yylex(valp);
};


void yyerror(CChunkParser* _prs, const char *a)
{
	std::string error;
	if (a && strlen(a))
		error = a;
	else
		error = "error";

	fprintf(stderr, "%s at line %i in file %s\n", error.c_str(), _prs->m_CurrentSourceLineNo, _prs->m_CurrentSourceFileName.c_str());

};


CChunkParser::CChunkParser() 
	:	yyFlexLexer(0, 0)
{
}


CChunkParser::~CChunkParser()
{
	CleanParser();	
}

bool CChunkParser::ParseGrammar(const char* src)
{
	//yydebug  = 1;
	//yy_flex_debug = 1;
	m_CurrentSourceLineNo = 1;
	if (src == NULL) return false;

	std::string  Query = src;
	std::stringstream QueryStream (Query.c_str());
	switch_streams((std::istream*)(&QueryStream), 0);
	int m_line_num = 1;
	if (yyparse(this) != 0)
	{
		CleanParser();

		return false;
	}

	return true;

}
bool CChunkParser::ParseGrammarInFile(std::string FileName, std::string RefererFile)
{	
	std::string Grammar;
	if (FileExists(FileName.c_str())) {
		Grammar = LoadFileToString(FileName);
	}
	else
	{
		if (!RefererFile.empty())
		{
			FileName = GetParentPath(RefererFile) + FileName;
			Grammar = LoadFileToString(FileName);
		};
	}
	m_CurrentSourceFileName = FileName;
	LOGI << "Loading" <<  m_CurrentSourceFileName;
	bool bResult = ParseGrammar(Grammar.c_str());
	return bResult;
};	

void CChunkParser::CleanParser()
{
	m_Labels.clear();
}

int CChunkParser::yylex(void* valp)
{
	int iRetVal = yylex();
	const char* __s = YYText();
	int len = YYLeng();

	if	(		(iRetVal == CHUNK_NAME) 
			||	(iRetVal == CHUNK_ATTRIB_VALUE) 
			||	(iRetVal == CHUNK_WORD_FORM)
			||	(iRetVal == CHUNK_ATTR_VARIABLE)
			||	(iRetVal == CHUNK_ATTR)
			
		)
	{
		assert (YYLeng() < 1000-1);
		char buff[1000];
		strncpy (buff, YYText(), YYLeng());
		buff[YYLeng()] = 0;
		std::string s = buff;
		m_Labels.push_back(s);
		((YYSTYPE *)valp)->m_LabelPtr = & (m_Labels.back());
	}
	else
		((YYSTYPE *)valp)->m_LabelPtr = 0;

	return (iRetVal);
}
