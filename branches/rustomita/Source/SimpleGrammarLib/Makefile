include ../common/common_mak
LibName = libSimpleGrammar$(ct)


objects = \
Loader.o$(ct) \
Parser.o$(ct) \
LR-items.o$(ct) \
GLR.o$(ct) \
GLRTable.o$(ct) \
GetMaxCoverage.o$(ct) \
AhoKorasick.o$(ct) \
unix_chunker_yacc.o$(ct) \
unix_chunker_lex.o$(ct) \
ChunkNode.o$(ct) \
ChunkParser.o$(ct) \
GrammarItem.o$(ct) \
SolvePeriodAmbiguity.o$(ct) \
FirstAndFollow.o$(ct) \
SourceGrammarParser.o$(ct) 


include ../common/common_lib_mak


unix_chunker_yacc.cpp unix_chunker_yacc.h :     chunker.y 
	bison -d        -ounix_chunker_yacc_tmp.cpp chunker.y 
	sed -f ../common/process_yacc.sed < unix_chunker_yacc_tmp.cpp  > unix_chunker_yacc.cpp
	mv unix_chunker_yacc_tmp.hpp  unix_chunker_yacc.h
	-rm unix_chunker_yacc_tmp.cpp

unix_chunker_lex.cpp : chunker.l unix_chunker_yacc.h
	flex -ounix_chunker_lex.cpp -d chunker.l 

