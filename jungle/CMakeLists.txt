# CMakeList.txt: jungle 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
project(jungle VERSION 1.1 DESCRIPTION " some idiots ")
# set(JUNGLE_DIR ${CMAKE_CURRENT_LIST_DIR})
# set(CMAKE_CXX_STANDARD 20)
# 将源代码添加到此项目的可执行文件。


############## libs ###################################################################
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/include/jungle/version.h"
   # @ONLY
)

add_subdirectory(libs/core)
add_subdirectory(libs/cpp_interface)
add_executable (jungle "jungle.cpp" "jungle.h")
target_link_libraries( jungle PUBLIC Boost::describe)
