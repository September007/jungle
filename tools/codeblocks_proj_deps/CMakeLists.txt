cmake_minimum_required(VERSION 3.20)

project(codeblocks_proj_dependencies 
            LANGUAGES CXX C
            VERSION 1.1 
            DESCRIPTION "analyze dependencies")

add_executable(codeblocks_proj_dependencies)
#set_property(TARGET codeblocks_proj_dependencies PROPERTY  INCLUDE_DIRECTORIES .)
target_link_libraries(codeblocks_proj_dependencies PRIVATE pugixml::pugixml)
target_link_libraries(codeblocks_proj_dependencies PRIVATE fmt::fmt-header-only)
# filesystem::proximate
set_property(TARGET codeblocks_proj_dependencies PROPERTY CXX_STANDARD 17)
include_directories(codeblocks_proj_dependencies PRIVATE 
    .
    ../include
    
    )

target_sources(codeblocks_proj_dependencies PRIVATE codeblocks_proj_dependencies.cpp codeblocks_proj_dependencies.h)
# on OSX only x64
# set_property(TARGET codeblocks_proj_dependencies PROPERTY OSX_ARCHITECTURES)
# source  property LANGUAGE

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/version.h"
   # @ONLY
)