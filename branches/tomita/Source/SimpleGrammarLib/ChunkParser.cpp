#include  "SimpleGrammarLib.h"
#include "ChunkParser.h"

#ifdef WIN32
	#include "chunk_yacc.h"
#else
	#include "unix_chunker_yacc.h"
#endif

#include <sstream>

extern int yydebug;

int yyparse (void *);

int yylex (void* valp, void* _p)
{
	CChunkParser* _prs =  (CChunkParser*)_p;

	return _prs->yylex(valp);
};


int CurrentSourceLineNo;
string CurrentSourceFileName;

void yyerror(const char *a)
{
	string error;
	if (a && strlen(a))
		error = a;
	else
		error = "error";

	fprintf(stderr, "%s at line %i in file %s\n",error.c_str(), CurrentSourceLineNo, CurrentSourceFileName.c_str());

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
	CurrentSourceLineNo = 1;
	if (src == NULL) return false;

	string  Query = src;
	istringstream 			QueryStream (Query.c_str());
	switch_streams((istream*)(&QueryStream), 0);
	int m_line_num = 1;
	if (yyparse((void*)this) != 0)
	{
		CleanParser();

		return false;
	}

	return true;

}
bool CChunkParser::ParseGrammarInFile(string FileName)
{	
	string Grammar;
	if (!LoadFileToString(FileName, Grammar))
	{
		bool bResult = false;
		if (!CurrentSourceFileName.empty())
		{
			FileName = GetPathByFile(CurrentSourceFileName) + FileName;
			bResult = LoadFileToString(FileName, Grammar);
		};
		if (!bResult)
		{
			fprintf(stderr, "cannot open %s\n", FileName.c_str());
			return false;
		};
	}
	string SaveName = CurrentSourceFileName;
	int SaveNumLine = CurrentSourceLineNo;

	CurrentSourceFileName = FileName;
	fprintf (stderr, "Loading %s\n", CurrentSourceFileName.c_str());
	bool bResult = ParseGrammar(Grammar.c_str());

	CurrentSourceFileName = SaveName;
	CurrentSourceLineNo = SaveNumLine;

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
		string s = buff;
		m_Labels.push_back(s);
		((YYSTYPE *)valp)->m_LabelPtr = & (m_Labels.back());
	}
	else
		((YYSTYPE *)valp)->m_LabelPtr = 0;

	return (iRetVal);
}
