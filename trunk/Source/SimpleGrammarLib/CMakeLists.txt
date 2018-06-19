cmake_minimum_required(VERSION 3.6)

project(SimpleGrammarLib)


add_library(SimpleGrammarLib
AhoKorasick.cpp
ChunkNode.cpp
ChunkParser.cpp
FirstAndFollow.cpp
GLR.cpp
GLRTable.cpp
GetMaxCoverage.cpp
GrammarItem.cpp
LR-items.cpp
Loader.cpp
Parser.cpp
SimpleGrammarLib.cpp
SolvePeriodAmbiguity.cpp
SourceGrammarParser.cpp
chunker_yacc.cpp
chunker_lex.cpp
)

target_link_libraries(SimpleGrammarLib
    aot_common
    LemmatizerLib
)

add_custom_command(OUTPUT chunker_yacc.cpp chunker_yacc.hpp
    DEPENDS chunker.y
	COMMAND bison -d -ochunker_yacc.cpp chunker.y 
    VERBATIM
)

add_custom_command( OUTPUT chunker_lex.cpp
    DEPENDS chunker.l chunker_yacc.hpp
	COMMAND flex -ochunker_lex.cpp -d chunker.l
    VERBATIM
)


