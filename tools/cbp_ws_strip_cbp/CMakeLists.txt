project(cbp_ws_strip_cbp 
            LANGUAGES CXX C
            VERSION 1.1 
            DESCRIPTION "analyze dependencies")

add_executable(cbp_ws_strip_cbp cbp_ws_strip_cbp.cpp)

# set_property(TARGET codeblocks_to_cmake PROPERTY  INCLUDE_DIRECTORIES .)
target_link_libraries(cbp_ws_strip_cbp PRIVATE pugixml::pugixml)
target_link_libraries(cbp_ws_strip_cbp PRIVATE fmt::fmt-header-only)

# filesystem::proximate
set_property(TARGET cbp_ws_strip_cbp PROPERTY CXX_STANDARD 17)
target_include_directories(cbp_ws_strip_cbp PRIVATE
    .
    ../../extern/pugixml/srcs
    ../include)

target_sources(cbp_ws_strip_cbp PRIVATE cbp_ws_strip_cbp.cpp)
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/version.h"
   # @ONLY
)