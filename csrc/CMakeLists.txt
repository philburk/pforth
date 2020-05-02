# pforth/csrc/CMakeLists.txt
# Created by Robin Rowe 2020-05-01
# License: License Public Domain

file(STRINGS sources.cmake SOURCES)
if(WIN32)
set(PLATFORM stdio/pf_fileio_stdio.c
win32_console/pf_io_win32_console.c )
endif(WIN32)

add_library(${PROJECT_NAME}_lib ${SOURCES} ${PLATFORM})
