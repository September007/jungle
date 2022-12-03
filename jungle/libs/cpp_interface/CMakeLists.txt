
set(libname  jungle_cpp_interface)
add_library(${libname} INTERFACE)
add_library(jungle::cpp_interface ALIAS ${libname})
target_include_directories(${libname} INTERFACE include ${jungle_SOURCE_DIR}/include)
target_link_libraries(${libname} INTERFACE boost_describe jungle::core)

if(Testing)
    foreach(test test)
        add_executable(${libname}_${test} test/${test}.cpp)
        target_link_libraries(${libname}_${test} PRIVATE ${libname})    
    endforeach()
    
endif()