cmake_minimum_required(VERSION 3.31)
project(ChaosMod)

file(GLOB ROOT_SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/Components/*.cpp ${PROJECT_SOURCE_DIR}/Effects/*.cpp
    ${PROJECT_SOURCE_DIR}/Memory/*.cpp ${PROJECT_SOURCE_DIR}/Util/*.cpp)
file(GLOB PATTERNS_SRC ${PROJECT_SOURCE_DIR}/../vendor/Patterns/Patterns.cpp)
add_library(ChaosMod MODULE ${ROOT_SRC} ${SRC} ${PATTERNS_SRC} ChaosMod.rc)

file (STRINGS ${PROJECT_SOURCE_DIR}/../version.txt MOD_VERSION)
configure_file(${PROJECT_SOURCE_DIR}/InfoTEMPLATE.h.in ${PROJECT_SOURCE_DIR}/Info.h @ONLY)

target_precompile_headers(ChaosMod PUBLIC stdafx.cpp)

set_target_properties(ChaosMod PROPERTIES SUFFIX ".asi")
set_target_properties(ChaosMod PROPERTIES CXX_STANDARD 20)
set_target_properties(ChaosMod PROPERTIES CXX_SCAN_FOR_MODULES OFF)

target_compile_definitions(ChaosMod PUBLIC $<$<CONFIG:Debug>:CHAOSDEBUG> NDEBUG WIN32_LEAN_AND_MEAN SOL_ALL_SAFETIES_ON SOL_DEFAULT_PASS_ON_ERROR)

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
    target_compile_options(ChaosMod PUBLIC /Zi /GT /W2 /WX /bigobj -DUNICODE -D_UNICODE -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    target_link_options(ChaosMod PUBLIC /DEBUG /OPT:REF /OPT:ICF)
else()
    target_compile_options(ChaosMod PUBLIC -municode -Wa,-mbig-obj -flarge-source-files -Werror -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable -Wno-maybe-uninitialized -Wno-array-bounds -Wno-unused-but-set-variable)
    target_link_options(ChaosMod PUBLIC -static)
endif()
