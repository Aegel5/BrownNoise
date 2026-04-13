# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.17)
project(BrownNoise VERSION 1.0 LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC)
    # Заставляет MSVC интерпретировать исходники и строки как UTF-8
    add_compile_options("/utf-8")
    add_compile_definitions(NOMINMAX)
endif()


file(  
    GLOB_RECURSE 
    TARGET_SOURCES  
    src/*.cpp
    src/*.h
) 


set(CMAKE_CXX_SCAN_FOR_MODULES off)
set(CMAKE_CXX_STANDARD 23)

add_executable (
    ${CMAKE_PROJECT_NAME} 
    ${TARGET_SOURCES} 
) 

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "extern")














