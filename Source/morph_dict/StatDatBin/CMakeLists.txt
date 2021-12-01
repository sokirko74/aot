declare_cmake_min_version()

project(StatDatBin)

add_executable (StatDatBin StatDatBin.cpp)

target_link_libraries(StatDatBin 
    LemmatizerBaseLib
)
