## SPDX-License-Identifier: MIT

# Root Cmake file.
# Sets up overall project configuration.

# Require that minimum CMake version is 3.15.0
cmake_minimum_required(VERSION 3.15.0)

# Ensure that VCPKG toolchain is loaded by CMake. 
include(cmake/vcpkg.cmake)

# Project Name (rogue), Language(C++) and current version(0.1.0).
project(rogue
        LANGUAGES CXX 
        VERSION 0.1.0)

# Project configuration for MSVC
add_library(project_configuration INTERFACE)
include(cmake/msvc_settings.cmake)
set_msvc_project_configuration(project_configuration)

# Must have ISO Standard C++, no vendor extensions.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place build output to build/bin directory
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/")

# Subfolders part of this build
add_subdirectory(external)
add_subdirectory(src)
