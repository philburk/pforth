# NAME/CMakeLists.txt
# Created by Robin Rowe 2020-05-01
# License: License Public Domain

cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Pforth)
message("Configuring ${PROJECT_NAME}...")
enable_testing()
if(WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
message("Warning: _CRT_SECURE_NO_WARNINGS")
endif(WIN32)
add_subdirectory(csrc)
link_libraries(${PROJECT_NAME}_lib)
if(NOT WIN32 AND NOT APPLE)
	link_libraries(rt pthread)
endif(NOT WIN32 AND NOT APPLE)

option(UNISTD "Enable libunistd" false)
if(UNISTD)
	set(LIBUNISTD_PATH /code/github/libunistd)
	if(WIN32)
		include_directories(${LIBUNISTD_PATH}/unistd)
		link_directories(${LIBUNISTD_PATH}/build/unistd/Release)
		link_libraries(libunistd)
	endif(WIN32)
endif(UNISTD)

option(FLTK "Enable FLTK" false)
if(FLTK)
	set(FLTK_PATH /Code/fltk)
	include_directories(${FLTK_PATH} ${FLTK_PATH}/fltk-build ${FLTK_PATH}/fltk-src/)
	link_directories(${FLTK_PATH}/fltk-src/build/lib/Debug)
	set(FLTK_LIBS
		fltk_formsd
		fltk_gld
		fltk_imagesd
		fltk_jpegd
		fltk_pngd
		fltk_zd
		fltkd
	)
	link_libraries(${FLTK_LIBS} comctl32)
endif(FLTK)

add_executable(pforth csrc/pf_main.c)
