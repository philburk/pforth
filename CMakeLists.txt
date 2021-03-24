# NAME/CMakeLists.txt
# Original file by Robin Rowe 2020-05-01
# License: License Public Domain

# The process of building pForth involves several steps.
#
#    cmake .
#    make
#
# Finish building the Forth dictionary and save it as "pforth.dic".
#
#    cd fth
#    ./pforth -i system.fth
#
# The next time you run pforth, the precompiled pforth.dic file will
# be loaded automatically.
#
#    ./pforth

cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put pforth in the fth folder so we can load the Forth code more easily.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fth)

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

add_executable(pforth csrc/pf_main.c)
