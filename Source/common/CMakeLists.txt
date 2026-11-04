declare_cmake_min_version()

set(my_SOURCES
        Chunk.h
        http_server.h
        test_corpus.h
        test_corpus.cpp
)

if (COMPILE_WITH_HTTP)
    set(my_SOURCES ${my_SOURCES} http_server.cpp)
endif ()

add_library(aot_common ${my_SOURCES})

target_link_libraries(aot_common
        morphology_common
        )
