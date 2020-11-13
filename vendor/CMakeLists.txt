# CMake build by James (Dr. Underscore)

cmake_minimum_required(VERSION 3.13)
project(MinHook)

add_library(MinHook STATIC
    minhook/src/buffer.c
    minhook/src/hook.c
    minhook/src/trampoline.c
    minhook/src/hde/hde32.c
    minhook/src/hde/hde64.c
    )

target_include_directories(MinHook PRIVATE minhook/src minhook/include)
