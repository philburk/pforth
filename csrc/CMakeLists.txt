# pforth/csrc/CMakeLists.txt
# Created by Robin Rowe 2020-05-01
# License: License Public Domain

file(STRINGS sources.cmake SOURCES)

if(WIN32)
set(PLATFORM stdio/pf_fileio_stdio.c win32_console/pf_io_win32_console.c )
endif(WIN32)

if(UNIX OR APPLE)
set(PLATFORM posix/pf_io_posix.c stdio/pf_fileio_stdio.c)
endif(UNIX OR APPLE)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(
#        --std=c89
        -fsigned-char
        -fno-builtin
        -fno-unroll-loops
        -pedantic
        -Wcast-qual
        -Wall
        -Werror
        -Wwrite-strings
        -Winline
        -Wmissing-prototypes
        -Wmissing-declarations
        )
endif()

add_library(${PROJECT_NAME}_lib ${SOURCES} ${PLATFORM})
