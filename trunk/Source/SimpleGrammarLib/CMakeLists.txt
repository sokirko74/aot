cmake_minimum_required(VERSION 3.5)

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

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/chunker_yacc.cpp chunker_yacc.hpp
    DEPENDS ${PROJECT_SOURCE_DIR}/chunker.y
	COMMAND bison -d -ochunker_yacc.cpp chunker.y 
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
)

add_custom_command( OUTPUT ${PROJECT_SOURCE_DIR}/chunker_lex.cpp
    DEPENDS ${PROJECT_SOURCE_DIR}/chunker.l chunker_yacc.hpp
	COMMAND flex -ochunker_lex.cpp -d chunker.l
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
)


