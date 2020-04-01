# Setup configuration specific to MSVC

function(set_msvc_project_configuration project_name)
    option(MSVC_CXX_17_MODE "Enable C++17" TRUE)
    option(MSVC_CXX_LATEST  "Enable C++Latest" FALSE)

    set(MSVC_COMPILER_FLAGS
            /W3             # Enable all warnings
            /permissive-    # enable conformance mode
            /Zc:__cplusplus # report C++ version correctly
    )

    set(MSVC_LINKER_FLAGS
        /ENTRY:mainCRTStartup # use int main() as entry point.
    )

    set(MSVC_COMPILE_DEFINITIONS
            UNICODE _UNICODE    # Windows.h use UNICODE
            _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING # disable C header complaints
            NOMINMAX            # Don't use Windows.h min/max
            WIN32_LEAN_AND_MEAN # Trim it down.
    )

    if(MSVC_CXX_17_MODE)
        set(MSVC_COMPILER_FLAGS 
                ${MSVC_COMPILER_FLAGS}
                /std:c++17  # use C++ 17 
        )
    elseif(MSVC_CXX_LATEST)
        set(MSVC_COMPILER_FLAGS 
                ${MSVC_COMPILER_FLAGS}
                /std:c++latest  # use C++ Latest features (20?)
        )
    endif()

    if (MSVC)
        message("Enable MSVC configuraiton.")
        target_compile_options(${project_name}
            INTERFACE
                ${MSVC_COMPILER_FLAGS}
        )

        target_compile_definitions(${project_name}
            INTERFACE
                ${MSVC_COMPILE_DEFINITIONS}
        )

        target_link_options(${project_name}
            INTERFACE
                ${MSVC_LINKER_FLAGS}
        )
    endif()
endfunction(set_msvc_project_configuration project_name)
