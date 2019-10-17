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
        input_handler.hpp)

# Tell the build system we have to use C++17 Standard
target_compile_features(rogue
    PRIVATE
        cxx_std_17)

# Provide some extra compiler options IF the compiler is MSVC
# - /W3 : enable all the warnings
# - /permissive- : tell msvc to work in conformance mode
# - /Zc:__cpluspluc : tell msvc to report C++ version correctly
# - /std:c++17 : tell msvc to use Standard C++ 17. CMake passes flag incorrectly
target_compile_options(rogue
    PRIVATE 
        $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /Zc:__cplusplus /std:c++17>)

# Provide some extra linker options IF the compiler is MSVC
# /ENTRY:mainCRTStartup : tell msvc we want to start in main() function.
target_link_options(rogue
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>)

# Define "DEBUG" if the Build type is Debug
# Define only for MSVC to silence deprecated header warnings
target_compile_definitions(rogue
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG _DEBUG>
        $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING>)

# Add the include directory for external dependencies to compiler 
# include path for rogue executable.
target_include_directories(rogue 
    PRIVATE 
        ${LIBTCOD_INCLUDE}
        ${EASYITERATOR_INCLUDE})

# Add the LIBTCOD library to linker lib path for rogue executable.
target_link_libraries(rogue 
    PRIVATE 
        fmt::fmt
        ${LIBTCOD_LIB})

# Add post build dependency for rogue executable.
add_dependencies(rogue LIBTCOD_BINS)
