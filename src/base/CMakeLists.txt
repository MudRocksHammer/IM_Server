cmake_minimum_required(VERSION 3.10)

project(base)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find source and header files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.cc)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/*.h)

# Extract unique include directories
set(INCLUDE_DIRS "")
foreach(HEADER ${HEADERS})
    get_filename_component(DIR ${HEADER} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# Debugging info
# message(STATUS "Source files: ${SOURCES}")
# message(STATUS "Include directories: ${INCLUDE_DIRS}")

find_package(OpenSSL REQUIRED)
find_package(Boost CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./lib/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE pthread OpenSSL::Crypto)

target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wextra -rdynamic -O3 -fPIC -ggdb -Wno-deprecated
        -Werror -Wno-unused-function -Wno-builtin-macro-redefined
        -Wno-deprecated-declarations)