# this is generator by codeblocks_to_cmake
# description:  trans .cbp file to CMakeLists
# version:      1.1
cmake_minimum_required (VERSION 3.20)
project(app 
    VERSION "1.1"
    DESCRIPTION "none" )
add_executable(app)

#-----------app
set_property(TARGET app PROPERTY RUNTIME_OUTPUT_DIRECTORY
			 ${CMAKE_CURRENT_LIST_DIR}/bin/Release) 
set_property(TARGET app PROPERTY SOURCES
	../3rd/3rd.c
	../3rd/3rd.cpp
	../include/out_dir.h
	main.cpp
	see_unit_options.h
	version_release.h)
set_property(TARGET app PROPERTY INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_LIST_DIR}/.
	${CMAKE_CURRENT_LIST_DIR}/../3rd)
set_property(TARGET app PROPERTY COMPILE_OPTIONS
	-O2
	-Wall
	-fexceptions)
set_property(TARGET app PROPERTY LINK_DIRECTORIES
	${CMAKE_CURRENT_LIST_DIR}/.)
set_property(TARGET app PROPERTY LINK_OPTIONS
	-s
	-l)

