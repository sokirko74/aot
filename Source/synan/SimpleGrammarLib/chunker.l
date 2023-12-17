%{

//#define chunk_query_l
#define YY_NO_UNISTD_H
#include "ChunkParser.h"
#include "chunker_yacc.hpp"



%}
%option c++
%option noyywrap
%option yyclass="CChunkParser"
 
 
ws			[ \t\r]+
punct		[.:,?!-_]
punct_without_dash		[.:,?!_]
op_or		"|" 
open_bracket "["
close_bracket "]"
chunk_name		([A-Za-z0-9_\-])+
comments %%.*
attrib (hom)|(type)|(filename)|(grm)|(type_grm)|(root)|(register)|(form)|(det)|(has_one_grm)|(atomic)|(graphem)
attrib_var \$[0-9]\.({attrib})
include_direc "#include"



%x ATTRIB_VALUE_COND
%x WORD_FORM_COND
%x OUT_OF_RULE

%%

"\""	BEGIN(ATTRIB_VALUE_COND);
<ATTRIB_VALUE_COND>[^"]* return CHUNK_ATTRIB_VALUE;
<ATTRIB_VALUE_COND>"\""	BEGIN(INITIAL);


"'"	BEGIN(WORD_FORM_COND);
<WORD_FORM_COND>[^']* return CHUNK_WORD_FORM;
<WORD_FORM_COND>"'"	BEGIN(INITIAL);


"->" return CHUNK_RULE_PART_DELIM;
":=" return CHUNK_ATTR_ASSIGNMENT;
"="  return CHUNK_ATTR_EQU_CHAR;

{attrib} return CHUNK_ATTR;
{attrib_var} return CHUNK_ATTR_VARIABLE;
{op_or}		return CHUNK_OR;
{chunk_name}		return CHUNK_NAME;
{include_direc} return CHUNK_INCLUDE;

;   return CHUNK_RULE_DELIM;
"?" return CHUNK_FACULTATIVE;

{comments}	/* skip blanks and tabs */;
{ws}    /* skip blanks and tabs */;
\n  {
		m_CurrentSourceLineNo++;
	};
.			return ((int)YYText()[0]);

%%

