declare_cmake_min_version()

project(TestSemanCOM)


add_executable (TestSemanCOM TestSeman.cpp)

add_dependencies(TestSemanCOM  
    Seman
    SynAn
    Lemmatizer
    MAPost
    graphan
)


add_test(
    NAME Seman_COM_Russian 
    COMMAND TestSemanCOM test/Russian/text.txt
    WORKING_DIRECTORY .
)

add_test(
    NAME Seman_COM_Russian_cmp
    COMMAND ${CMAKE_COMMAND} -E compare_files test/Russian/canon.seman test/Russian/text.txt.seman
    WORKING_DIRECTORY .
)
