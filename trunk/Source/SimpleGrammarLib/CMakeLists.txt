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
${PROJECT_SOURCE_DIR}/chunker_yacc.cpp
${PROJECT_SOURCE_DIR}/chunker_lex.cpp
)

target_link_libraries(SimpleGrammarLib
    aot_common
    LemmatizerLib
)

if (WIN32) 
    include_directories (AFTER ${WIN_FLEX_FOLDER})
endif()

message ("bison_tool = " ${BISON_TOOL})
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/chunker_yacc.cpp ${PROJECT_SOURCE_DIR}/chunker_yacc.hpp
    DEPENDS chunker.y
	COMMAND ${BISON_TOOL} -d -ochunker_yacc.cpp chunker.y 
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
)

add_custom_command( OUTPUT ${PROJECT_SOURCE_DIR}/chunker_lex.cpp
    DEPENDS chunker.l chunker_yacc.hpp                              
	COMMAND ${FLEX_TOOL} -ochunker_lex.cpp -d chunker.l
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
)



