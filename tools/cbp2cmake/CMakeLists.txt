
set(libname cbp2cmake) 
add_executable(${libname} src/ctc.cpp src/main.cpp)

# #################  SOURCES
target_include_directories(${libname} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)
file(GLOB_RECURSE HEADERS src/include/*)
file(GLOB_RECURSE SOURCES src/*.cpp src/agent/cpp/*.cpp)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/include PREFIX "头文件" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX "源文件" FILES ${SOURCES})

target_sources(${libname} PUBLIC ${HEADERS} PRIVATE ${PROTO_FILES} ${SOURCES})

# #################  COMPILE
message("CMAKE_CXX_COMPILER_ID:${CMAKE_CXX_COMPILER_ID}")
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang") #https://www.cnblogs.com/pandamohist/p/13556546.html
    add_compile_options(-s -Wnon-c-typedef-for-linkage)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_compile_options(
        /wd:4819 # unicode file 
        
    )
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
endif()
add_compile_definitions(RELATIVE_LOG_PATH)
set_property(TARGET ${libname} PROPERTY CXX_STANDARD 20) # default compriser

# #################  LINKS
target_link_libraries(${libname} PUBLIC boost_describe boost_json jungle::cpp_interface)

if(Testing)
    foreach(test data_convert)
        add_executable(${libname}_${test} test/${test}.cpp)
        target_include_directories(${libname}_${test} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/include)
        target_link_libraries(${libname}_${test} PRIVATE  boost_describe boost_json jungle::cpp_interface )
    endforeach()
    target_link_libraries(${libname}_data_convert PRIVATE gtest)
    #set_target_properties(${libname}_data_convert PROPERTIES CXX_STANDARD 20)
    
endif()