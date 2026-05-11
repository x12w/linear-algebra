cmake_minimum_required(VERSION 3.16)

project(linear-algebra)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(LinearAlgebra INTERFACE)
add_library(BasicAlgebra INTERFACE)
target_include_directories(BasicAlgebra INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(LinearAlgebra INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(LinearAlgebra INTERFACE BasicAlgebra)

add_executable(test test.cpp)
target_link_libraries(test BasicAlgebra LinearAlgebra)
