

add_executable(cbp2cmake src/ctc.cpp src/main.cpp)

# #################  SOURCES
target_include_directories(cbp2cmake PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)
file(GLOB_RECURSE HEADERS src/include/*)
file(GLOB_RECURSE SOURCES src/*.cpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/include PREFIX "头文件" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "源文件" FILES ${SOURCES})

target_sources(cbp2cmake PUBLIC ${HEADERS} PRIVATE ${PROTO_FILES} ${SOURCES})

# #################  COMPILE
add_compile_options(-s)
add_compile_definitions(RELATIVE_LOG_PATH)
set_property(TARGET cbp2cmake PROPERTY CXX_STANDARD 20) # is_describe

# #################  LINKS
target_link_libraries(cbp2cmake PUBLIC boost_describe boost_json jungle::cpp_interface)
