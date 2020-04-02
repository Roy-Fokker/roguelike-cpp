## SPDX-License-Identifier: MIT

# Find the fmtlib package installed by vcpkg.
# fmtlib can be installed by running following command in shell
# => vcpkg install fmt cppitertools
find_package(fmt REQUIRED)

# Define the executable we want to build.
add_executable(rogue)

# Source files used to build rogue.exe
target_sources(rogue
    PRIVATE
        main.cpp
        console.cpp
        console.hpp
        game_entity.cpp
        game_entity.hpp
        game_actions.cpp
        game_actions.hpp
        game_map.cpp
        game_map.hpp
        input_handler.cpp
        input_handler.hpp
        game_ui.cpp
        game_ui.hpp)

# Add the include directory for external dependencies to compiler 
# include path for rogue executable.
target_include_directories(rogue 
    PRIVATE 
        ${LIBTCOD_INCLUDE})

# Add project_configuration for MSVC settings,
# lib fmt and LIBTCOD to rogue executable.
target_link_libraries(rogue 
    PRIVATE 
        project_configuration
        fmt::fmt
        ${LIBTCOD_LIB})

# Add post build dependency for rogue executable.
add_dependencies(rogue LIBTCOD_BINS)
