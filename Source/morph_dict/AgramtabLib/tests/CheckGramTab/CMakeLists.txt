project(CheckGramTab.cpp)

add_executable (CheckGramTab
        CheckGramTab.cpp
)

target_link_libraries(CheckGramTab 
    AgramtabLib
)

macro (TestGramTab lang)
    add_test(
        NAME Gramtab_${lang}
        COMMAND CheckGramTab ${lang}
    )

endmacro()

TestGramTab(Russian)
TestGramTab(German)
TestGramTab(English)
