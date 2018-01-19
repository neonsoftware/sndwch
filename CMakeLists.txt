# Cmake version
cmake_minimum_required(VERSION 3.5)

# Define project
project(sample C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# define  directories
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(TESTS ${CMAKE_SOURCE_DIR}/test)
add_subdirectory(${SRC})
add_subdirectory(${TESTS})
