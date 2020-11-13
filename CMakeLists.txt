# CMake build by James (Dr. Underscore)

cmake_minimum_required(VERSION 3.13)
project(ChaosModV)

add_subdirectory(vendor)
add_subdirectory(ChaosMod)

target_link_directories(ChaosMod PRIVATE vendor/scripthookv/lib)
