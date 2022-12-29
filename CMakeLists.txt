cmake_minimum_required(VERSION 3.20)
project(SomeMathTask)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_CXX_STANDARD 20)
include(libraries/conan_paths.cmake)

find_package(CvPlot)

add_executable(SomeMathTask main.cpp)
target_link_libraries(${PROJECT_NAME} CvPlot::CvPlot)