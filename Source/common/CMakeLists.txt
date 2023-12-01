declare_cmake_min_version()

set(my_SOURCES
        Homonym.cpp
        LemWord.cpp
        PlmLine.cpp
        gra_descr.cpp
        Chunk.h
        cortege.h
        gra_descr.h
        Homonym.h
        http_server.h
        LemWord.h
        PlmLine.h
        cortege.cpp
)

if (COMPILE_WITH_HTTP)
    set(my_SOURCES ${my_SOURCES} http_server.cpp)
endif ()

add_library(aot_common ${my_SOURCES})

target_link_libraries(aot_common
        morphology_common
        )
