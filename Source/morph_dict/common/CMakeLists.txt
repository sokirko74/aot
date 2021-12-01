declare_cmake_min_version()

set(my_SOURCES
        ../AgramtabLib/AncodePattern.cpp
        ../LemmatizerBaseLib/rus_numerals.cpp
        utilit.cpp
)

add_library(morphology_common  ${my_SOURCES})

target_link_libraries(morphology_common
    iconv_subset
)
