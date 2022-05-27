cmake_minimum_required(VERSION 3.12)

project(ChaosMod)

add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/shv shv)
add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/minhook minhook)

file(GLOB ROOT_SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/Components/*.cpp ${PROJECT_SOURCE_DIR}/Effects/*.cpp
    ${PROJECT_SOURCE_DIR}/Memory/*.cpp ${PROJECT_SOURCE_DIR}/Util/*.cpp)
file(GLOB PATTERNS_SRC ${PROJECT_SOURCE_DIR}/../vendor/Patterns/Patterns.cpp)
add_library(ChaosMod MODULE ${ROOT_SRC} ${SRC} ${PATTERNS_SRC} ChaosMod.rc)

set_property(TARGET ChaosMod PROPERTY CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(COMPILE_OPTIONS -DUNICODE -D_UNICODE -DNOMINMAX)
else()
    set(COMPILE_OPTIONS -municode -fpermissive)
endif()
target_compile_options(ChaosMod PRIVATE ${COMPILE_OPTIONS})

target_include_directories(ChaosMod PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../vendor)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_options(ChaosMod PRIVATE -static)
endif()
target_link_directories(ChaosMod PRIVATE ${PROJECT_SOURCE_DIR}/../vendor/lua)
target_link_libraries(ChaosMod PRIVATE shv minhook lua54 winmm d3dcompiler)

target_precompile_headers(ChaosMod PRIVATE stdafx.cpp)

target_compile_definitions(ChaosMod PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
set_target_properties(ChaosMod PROPERTIES SUFFIX ".asi")