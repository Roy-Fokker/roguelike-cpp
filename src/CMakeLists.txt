find_package(fmt REQUIRED)

add_executable(rogue main.cpp)

target_compile_features(rogue PRIVATE cxx_std_17)

target_compile_options(rogue
                       PRIVATE 
                       $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /Zc:__cplusplus /std:c++17>)

target_compile_definitions(rogue
                           PRIVATE
                           $<$<CONFIG:Debug>:DEBUG _DEBUG>)

target_include_directories(rogue PRIVATE ${LIBTCOD_INCLUDE})

target_link_libraries(rogue PRIVATE fmt::fmt ${LIBTCOD_LIB})

add_dependencies(rogue LIBTCOD_BINS)


