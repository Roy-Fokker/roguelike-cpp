# Add directories for libtcod
set(LIBTCOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libtcod-1.14.0-x86_64-msvc")

set(LIBTCOD_INCLUDE "${LIBTCOD_DIR}/include/" CACHE PATH "Path to Libtcod Include")
find_library(LIBTCOD_LIB libtcod ${LIBTCOD_DIR})

add_custom_target(LIBTCOD_BINS
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBTCOD_DIR}/libtcod.dll ${EXECUTABLE_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBTCOD_DIR}/sdl2.dll ${EXECUTABLE_OUTPUT_PATH}
)
