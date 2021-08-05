cmake_minimum_required(VERSION 3.14)
project(socket)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) 

aux_source_directory(src SOURCE_COMMON)
aux_source_directory(server SOURCE_SERVER)
aux_source_directory(client SOURCE_CLIENT)

include_directories(include)

add_executable(server ${SOURCE_SERVER} ${SOURCE_COMMON})
add_executable(client ${SOURCE_CLIENT} ${SOURCE_COMMON})
