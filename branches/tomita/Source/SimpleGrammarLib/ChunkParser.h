#ifndef CHUNK_PARSER_H
#define CHUNK_PARSER_H

#include "../common/utilit.h"
#include <list>

#define YYDEBUG 1

#ifndef chunk_query_l
	#include "MyFlexLexer.h"
	//#include <C:/cygwin/usr/include/FlexLexer.h>
#endif


#include "ChunkNode.h"



class CChunkParser : // protected chunkFlexLexer
						protected yyFlexLexer
{
public:
	
	CChunkGrammar			m_ChunkGrammar;	
	const CAgramtab*		m_pGramTab;
	int						m_line_num;
	
	void	CleanParser();
	int yylex (void* valp);
protected:
	int yylex();
	list<string>	m_Labels;
	
public:

	CChunkParser();
	bool	ParseGrammar(const char* src);
	bool	ParseGrammarInFile(string FileName);
	virtual ~CChunkParser();
	void yyerror(const char *a);
};

#endif
