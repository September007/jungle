

add_executable(cbp2cmake cbp2cmake.cpp)

target_include_directories(cbp2cmake PUBLIC . include)
target_link_libraries(cbp2cmake PUBLIC boost_describe boost_json)