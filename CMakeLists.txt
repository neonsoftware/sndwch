# Cmake version
cmake_minimum_required(VERSION 3.5)

# Define project
project(sample C)
find_package(PkgConfig)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LibXml2)
find_package(LibYaml)

enable_testing()

# define  executable  names
# set(EXE  MyProgram)

# define  directories
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(TESTS ${CMAKE_SOURCE_DIR}/test)
#add_subdirectory(${SRC})
add_subdirectory(${TESTS})



