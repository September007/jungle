

add_executable(cbp2cmake src/ctc.cpp src/main.cpp)

##################  SOURCES

target_include_directories(cbp2cmake PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/src/protocal/cppout
)
file(GLOB_RECURSE PROTO_FILES src/protocal/cppout/*)
file(GLOB_RECURSE HEADERS src/include/*)
file(GLOB_RECURSE SOURCES src/*.cpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/protocal/cppout PREFIX "protobuf" FILES ${PROTO_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/include PREFIX "头文件" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "源文件" FILES ${SOURCES})

target_sources(cbp2cmake PUBLIC ${HEADERS} PRIVATE ${PROTO_FILES} ${SOURCES})


##################  COMPILE

add_compile_options(-s)
add_compile_definitions(RELATIVE_LOG_PATH)
##################  LINKS

find_package(Protobuf REQUIRED HINTS ${CMAKE_SOURCE_DIR}/extern/protobuf NO_DEFAULT_PATH)
find_package(absl REQUIRED HINTS ${CMAKE_SOURCE_DIR}/extern/protobuf NO_DEFAULT_PATH)
find_package(utf8_range REQUIRED HINTS ${CMAKE_SOURCE_DIR}/extern/protobuf NO_DEFAULT_PATH)
set(libs
    ${Protobuf_LIBRARIES}
    protobuf::libprotoc
    utf8_range::utf8_validity
    absl::strings
    absl::str_format_internal
    absl::status
    absl::synchronization
    absl::hash
    absl::raw_hash_set
)
target_link_libraries(cbp2cmake 
    PUBLIC boost_describe boost_json jungle::cpp_interface  
    PRIVATE ${libs})
