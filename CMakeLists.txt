# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.20)

project ("jungle")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_CXX_FLAGS  -g)
add_subdirectory(extern/pugixml)
add_subdirectory(extern/fmt)
add_subdirectory(tools)
add_subdirectory(jungle)