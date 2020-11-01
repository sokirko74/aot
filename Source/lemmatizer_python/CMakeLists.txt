find_program(SWIG_EXECUTABLE 
   NAMES swig.exe
   HINTS C:/tmp/swigwin-4.0.2/swigwin-4.0.2
)
message (SWIG_EXECUTABLE)

find_package(SWIG 4.0 COMPONENTS python)
INCLUDE(${SWIG_USE_FILE})

find_package (Python3 COMPONENTS Interpreter Development)
INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
message (${Python3_LIBRARIES})
message (${Python3_INCLUDE_DIRS})

SET(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(aot.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(aot.i PROPERTIES SWIG_FLAGS "-includeall")

swig_add_library(lemmatizer_python 
   TYPE SHARED 
   LANGUAGE python 
   SOURCES aot.i morphology_module.cpp)

target_link_libraries(lemmatizer_python  
    LemmatizerLib
)
swig_link_libraries(lemmatizer_python 
  ${Python3_LIBRARIES}
)
if (BUILD_DICTS)
    add_dependencies (lemmatizer_python MorphDicts)
endif()

add_test(
    NAME LemmatizerPython_Russian 
    COMMAND python3  ${CMAKE_CURRENT_SOURCE_DIR}/test_russian.py
    WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}
)
