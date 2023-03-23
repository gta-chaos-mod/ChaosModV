cmake_minimum_required(VERSION 3.13)

project(ChaosMod)

add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/shv shv)
add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/minhook minhook)
if (WITH_DEBUG_PANEL_SUPPORT)
    set(USE_ZLIB OFF CACHE BOOL "Enable zlib support")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/IXWebSocket ixwebsocket)
endif ()

file(GLOB ROOT_SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/Components/*.cpp ${PROJECT_SOURCE_DIR}/Effects/*.cpp
    ${PROJECT_SOURCE_DIR}/Memory/*.cpp ${PROJECT_SOURCE_DIR}/Util/*.cpp)
file(GLOB PATTERNS_SRC ${PROJECT_SOURCE_DIR}/../vendor/Patterns/Patterns.cpp)
add_library(ChaosMod MODULE ${ROOT_SRC} ${SRC} ${PATTERNS_SRC} ChaosMod.rc)

set_property(TARGET ChaosMod PROPERTY CXX_STANDARD 20)

if (WITH_DEBUG_PANEL_SUPPORT)
    target_compile_definitions(ChaosMod PRIVATE WITH_DEBUG_PANEL_SUPPORT)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ChaosMod PRIVATE -DUNICODE -D_UNICODE -DNOMINMAX /Zi)
    target_link_options(ChaosMod PRIVATE /DEBUG /OPT:REF /OPT:ICF)
else()
    target_compile_options(ChaosMod PRIVATE -municode -fpermissive)
    target_link_options(ChaosMod PRIVATE -static)
endif()

target_include_directories(ChaosMod PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../vendor ${PROJECT_SOURCE_DIR}/../vendor/websocketpp)

set(link_libs shv minhook lua54 winmm d3dcompiler xinput)

if (WITH_DEBUG_PANEL_SUPPORT)
    set(link_libs ${link_libs} ixwebsocket wsock32 ws2_32)
endif ()

target_link_directories(ChaosMod PRIVATE ${PROJECT_SOURCE_DIR}/../vendor/lua)
target_link_libraries(ChaosMod PRIVATE ${link_libs})

target_precompile_headers(ChaosMod PRIVATE stdafx.cpp)

target_compile_definitions(ChaosMod PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
set_target_properties(ChaosMod PROPERTIES SUFFIX ".asi")
