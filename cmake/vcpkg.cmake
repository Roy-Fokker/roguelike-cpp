# Load VCPKG toolchain file
#    include(${vcpkg.cmake dir}/vcpkg.cmake)

if(DEFINED PROJECT_NAME)
    message(FATAL_ERROR "[Error]: Vcpkg.cmake must be included before the project() directive is first called.")
elseif(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()