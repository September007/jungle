cmake_minimum_required(VERSION 3.2)

project(codeblocks_to_cmake
    LANGUAGES CXX C
    VERSION 1.1
    DESCRIPTION "trans .cbp file to CMakeLists")

add_executable(codeblocks_to_cmake codeblocks_to_cmake.cpp)

# set_property(TARGET codeblocks_to_cmake PROPERTY  INCLUDE_DIRECTORIES .)
target_link_libraries(codeblocks_to_cmake PRIVATE pugixml::pugixml)
target_link_libraries(codeblocks_to_cmake PRIVATE fmt::fmt-header-only)

# filesystem::proximate
set_property(TARGET codeblocks_to_cmake PROPERTY CXX_STANDARD 17)
target_include_directories(codeblocks_to_cmake PRIVATE
    .
    ../../extern/pugixml/srcs
    ../include)

target_sources(codeblocks_to_cmake PRIVATE codeblocks_to_cmake.cpp)

# on OSX only x64
# set_property(TARGET codeblocks_to_cmake PROPERTY OSX_ARCHITECTURES)
# source  property LANGUAGE
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/version.h"

    # @ONLY
)