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

set(CMAKE_CXX_SCAN_FOR_MODULES off)
set(CMAKE_CXX_STANDARD 23)

block()
	file(GLOB_RECURSE TARGET_SOURCES  src/*.cpp src/*.h) 
	add_executable (${CMAKE_PROJECT_NAME} WIN32 ) 
	target_sources(${CMAKE_PROJECT_NAME}  PRIVATE ${TARGET_SOURCES})
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "extern")
endblock()


block()
	add_library(ImguiLib OBJECT)
	file(GLOB src 
		"imgui_tiny_app/*.cpp"
		"imgui_tiny_app/misc/cpp/imgui_stdlib.cpp"
	)
	if(WIN32)
		list(APPEND src imgui_tiny_app/backends/imgui_impl_dx11.cpp)
		list(APPEND src imgui_tiny_app/backends/imgui_impl_win32.cpp)
	endif()
	target_sources(ImguiLib PRIVATE ${src})
	target_include_directories(ImguiLib PUBLIC "imgui_tiny_app") # PUBLIC - автоматически прокидываем дальше
	add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
	add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
	add_compile_definitions("$<$<CONFIG:Release>:IMGUI_DISABLE_DEMO_WINDOWS;IMGUI_DISABLE_DEBUG_TOOLS>")
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ImguiLib)
endblock()














