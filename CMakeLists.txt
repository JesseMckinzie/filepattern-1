cmake_minimum_required(VERSION 3.2)

#set(CMAKE_CXX_COMPILER "/usr/local/bin/g++") #For mac only

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.8
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      message(FATAL_ERROR "GCC version must be at least 8.0!")
  endif()
endif()

project(filepattern2)
# Set source directory
set(SOURCE_DIR "src/filepattern2/cpp")
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
                                              "${SOURCE_DIR}/fs_stream.cpp"
                                              "${SOURCE_DIR}/sort.cpp"
                                              "${SOURCE_DIR}/vector_parser.cpp"
                                            )

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(backend PRIVATE stdc++fs)
endif()
