# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.17)
project(BrownNoise VERSION 1.0 LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC)
    # Заставляет MSVC интерпретировать исходники и строки как UTF-8
    add_compile_options("/utf-8")
endif()

if (NOT TARGET sfml)
    message(STATUS "Fetching sfml")
    include(FetchContent)
    FetchContent_Declare(sfml GIT_REPOSITORY https://github.com/SFML/SFML GIT_TAG 3.0.2)
    FetchContent_MakeAvailable(sfml)
endif()

set(BUILD_SHARED_LIBS false)
set(SFML_USE_STATIC_STD_LIBS true)

file(  
    GLOB_RECURSE 
    TARGET_SOURCES  
    src/*.cpp
) 


set(CMAKE_CXX_SCAN_FOR_MODULES off)
set(CMAKE_CXX_STANDARD 23)

add_executable (
    ${CMAKE_PROJECT_NAME} 
    ${TARGET_SOURCES} 
) 

target_link_libraries(${CMAKE_PROJECT_NAME} SFML::Graphics SFML::Audio)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "extern")













