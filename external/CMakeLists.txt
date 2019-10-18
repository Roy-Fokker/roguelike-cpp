## SPDX-License-Identifier: MIT

# LIBTCOD
## Create a FILE-local variable for LIBTCOD directory
set(LIBTCOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libtcod-1.14.0-x86_64-msvc")

## Create LIBTCOD_INCLUDE variable for Include directory.
## Ensure it's available to global cmake build (CACHE PATH).
set(LIBTCOD_INCLUDE "${LIBTCOD_DIR}/include/" CACHE PATH "Path to Libtcod Include")

## Tell CMake to find libtcod.lib file, and add that to LIBTCOD_LIB global variable.
find_library(LIBTCOD_LIB libtcod ${LIBTCOD_DIR})

## Create a Custom Build task, which will copy 
## LIBTCOD dll, SDL2 dll, and font image to Output Path
add_custom_target(LIBTCOD_BINS
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBTCOD_DIR}/libtcod.dll ${EXECUTABLE_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBTCOD_DIR}/sdl2.dll ${EXECUTABLE_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBTCOD_DIR}/data/fonts/arial10x10.png ${EXECUTABLE_OUTPUT_PATH}
)
