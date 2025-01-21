cmake_minimum_required(VERSION 3.13)

project(ChaosMod)

file(GLOB ROOT_SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/Components/*.cpp ${PROJECT_SOURCE_DIR}/Effects/*.cpp
    ${PROJECT_SOURCE_DIR}/Memory/*.cpp ${PROJECT_SOURCE_DIR}/Util/*.cpp)
file(GLOB PATTERNS_SRC ${PROJECT_SOURCE_DIR}/../vendor/Patterns/Patterns.cpp)
add_library(ChaosMod MODULE ${ROOT_SRC} ${SRC} ${PATTERNS_SRC} ChaosMod.rc)

target_precompile_headers(ChaosMod PUBLIC stdafx.cpp)

set_target_properties(ChaosMod PROPERTIES SUFFIX ".asi")
set_target_properties(ChaosMod PROPERTIES CXX_STANDARD 20)

target_compile_definitions(ChaosMod PUBLIC $<$<CONFIG:Debug>:CHAOSDEBUG>)
target_compile_definitions(ChaosMod PUBLIC NDEBUG)

set(include_dirs ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../vendor ${PROJECT_SOURCE_DIR}/../vendor/lua/include)
set(link_libs shv minhook lua54 winmm d3dcompiler xinput)

add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/shv shv)
add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/minhook minhook)
if (WITH_DEBUG_PANEL_SUPPORT)
    set(include_dirs ${include_dirs} ${PROJECT_SOURCE_DIR}/../vendor/IXWebSocket)
    set(link_libs ${link_libs} wsock32 ws2_32 ixwebsocket)

    target_compile_definitions(ChaosMod PUBLIC WITH_DEBUG_PANEL_SUPPORT)

    set(USE_ZLIB OFF CACHE BOOL "Enable zlib support")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../vendor/IXWebSocket ixwebsocket)
endif ()

target_include_directories(ChaosMod PUBLIC ${include_dirs})
target_link_directories(ChaosMod PUBLIC ${PROJECT_SOURCE_DIR}/../vendor/lua)
target_link_libraries(ChaosMod PUBLIC ${link_libs})

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ChaosMod PUBLIC -DUNICODE -D_UNICODE -DNOMINMAX /Zi /GT)
    target_link_options(ChaosMod PUBLIC /DEBUG /OPT:REF /OPT:ICF)
else()
    target_compile_options(ChaosMod PUBLIC -municode)
    target_link_options(ChaosMod PUBLIC -static)
endif()