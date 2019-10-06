add_executable(rogue main.cpp)

target_compile_features(rogue PRIVATE cxx_std_17)

target_compile_options(rogue
                       PRIVATE 
                       $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /Zc:__cplusplus>)

target_compile_definitions(rogue
                           PRIVATE
                           $<$<CONFIG:Debug>:DEBUG _DEBUG>)



find_package(fmt REQUIRED)
target_link_libraries(rogue PRIVATE fmt::fmt)
