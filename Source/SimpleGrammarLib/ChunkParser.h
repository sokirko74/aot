#ifndef CHUNK_PARSER_H
#define CHUNK_PARSER_H

#include "../common/utilit.h"
#include <list>

#define YYDEBUG 1

#ifndef yyFlexLexerOnce
	#include <FlexLexer.h>
#endif


#include "ChunkNode.h"



class CChunkParser : // protected chunkFlexLexer
						protected yyFlexLexer
{
public:
	
	CChunkGrammar			m_ChunkGrammar;	
	const CAgramtab*		m_pGramTab;
	int						m_line_num;
	int                     m_CurrentSourceLineNo;
    std::string                  m_CurrentSourceFileName;

	void	CleanParser();
	int yylex (void* valp);
protected:
	int yylex();
	std::list<std::string>	m_Labels;
	
public:

	CChunkParser();
	bool	ParseGrammar(const char* src);
	bool	ParseGrammarInFile(std::string FileName, std::string RefererFile="");
	virtual ~CChunkParser();
	void yyerror(const char *a);
};

#endif
