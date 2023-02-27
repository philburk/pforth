# pforth/csrc/CMakeLists.txt
# Extended by Phil Burk 2021-10-31
# License: BSD Zero

file(STRINGS sources.cmake SOURCES)

if(WIN32)
set(PLATFORM stdio/pf_fileio_stdio.c win32_console/pf_io_win32_console.c )
endif(WIN32)

if(UNIX OR APPLE)
set(PLATFORM posix/pf_io_posix.c stdio/pf_fileio_stdio.c)
endif(UNIX OR APPLE)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
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
target_compile_definitions(${PROJECT_NAME}_lib PRIVATE PF_SUPPORT_FP)

# Compile the same library but with an option for the static dictionary.
add_library(${PROJECT_NAME}_lib_sd STATIC ${SOURCES} ${PLATFORM})
target_compile_definitions(${PROJECT_NAME}_lib_sd PRIVATE PF_STATIC_DIC)
target_compile_definitions(${PROJECT_NAME}_lib_sd PRIVATE PF_SUPPORT_FP)
