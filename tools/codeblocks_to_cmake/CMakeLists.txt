cmake_minimum_required(VERSION 3.20)

project(codeblocks_to_cmake)

add_executable(codeblocks_to_cmake codeblocks_to_cmake.cpp codeblocks_to_cmake.h)
#set_property(TARGET codeblocks_to_cmake PROPERTY  INCLUDE_DIRECTORIES .)
target_link_libraries(codeblocks_to_cmake PRIVATE pugixml::pugixml)
target_link_libraries(codeblocks_to_cmake PRIVATE fmt::fmt-header-only)
# filesystem::proximate
set_property(TARGET codeblocks_to_cmake PROPERTY CXX_STANDARD 17)
include_directories(codeblocks_to_cmake PRIVATE ../../extern/pugixml/src)

# on OSX only x64
# set_property(TARGET codeblocks_to_cmake PROPERTY OSX_ARCHITECTURES)
# source  property LANGUAGE
