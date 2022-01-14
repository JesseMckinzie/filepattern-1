cmake_minimum_required(VERSION 3.2)

project(pattern)
# Set source directory
set(SOURCE_DIR "src/pattern")
#Tell CMake that headers are also in SOURCE_DIR
include_directories(${SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/pybind11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

pybind11_add_module(backend MODULE ${SOURCES} "${SOURCE_DIR}/bindings.cpp" 
                                              "${SOURCE_DIR}/Pattern.cpp" 
                                              "${SOURCE_DIR}/InternalPattern.cpp" 
                                              "${SOURCE_DIR}/FilePattern.cpp"
                                              "${SOURCE_DIR}/StringPattern.cpp"
                                              "${SOURCE_DIR}/VectorPattern.cpp"
                                              "${SOURCE_DIR}/ExternalPattern.cpp"
                                              "${SOURCE_DIR}/ExternalFilePattern.cpp"
                                              "${SOURCE_DIR}/ExternalStringPattern.cpp"
                                              "${SOURCE_DIR}/ExternalVectorPattern.cpp"
                                              "${SOURCE_DIR}/stream.cpp"
                                              "${SOURCE_DIR}/fs_stream.cpp"
                                              "${SOURCE_DIR}/file_stream.cpp"
                                              "${SOURCE_DIR}/sort.cpp"
                                              "${SOURCE_DIR}/vector_parser.cpp"
                                            )

target_link_libraries(backend PRIVATE stdc++fs)