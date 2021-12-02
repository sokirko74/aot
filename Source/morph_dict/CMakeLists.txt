project(morphology)


set (CMAKE_CXX_STANDARD 17)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message ("compile debug")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -D_DEBUG")
    endif()

    enable_testing()
else()
    message ("compile release")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    endif()
endif()


add_subdirectory (contrib/iconv_subset)
add_subdirectory (common)
add_subdirectory (AgramtabLib)
add_subdirectory (MorphWizardLib)
add_subdirectory (LemmatizerBaseLib)
add_subdirectory (TestLem)
add_subdirectory (MorphGen)
add_subdirectory (StatDatBin)
add_subdirectory (WordDatBin)
