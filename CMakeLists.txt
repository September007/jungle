
# if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
#     set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build_linux)
# else()
#     set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build_windows)
# endif()
# message("${CMAKE_BINARY_DIR}")
# message("build in ${CMAKE_HOST_SYSTEM_NAME}")
cmake_minimum_required(VERSION 3.2)


project("jungle")

set(Testing ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_CXX_FLAGS -g)

add_subdirectory(extern/boost)
add_subdirectory(extern/fmt)
add_subdirectory(extern/googletest)
add_subdirectory(extern/pugixml)
add_subdirectory(jungle)
add_subdirectory(tools)