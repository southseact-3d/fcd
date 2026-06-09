macro(SetupRenderDeps)

    if(NOT BUILD_RENDER)
        return()
    endif()

    message(STATUS "Setting up Render workbench dependencies...")

    # Internet connectivity check (same pattern as SetupLibFmt)
    message(STATUS "Checking for connection to GitHub (for Render deps)...")
    if(WIN32)
        set(_render_ping_command "ping /n 1 /w 3 github.com")
    else()
        set(_render_ping_command "ping -c 1 -W 3 github.com")
    endif()
    execute_process(
        COMMAND ${_render_ping_command}
        RESULT_VARIABLE _render_NO_CONNECTION
    )
    if(_render_NO_CONNECTION GREATER 0)
        set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
        message(WARNING "NO INTERNET CONNECTION: Using disconnected mode for FetchContent updates (Render deps)")
    else()
        message(STATUS "GitHub connection established for FetchContent (Render deps)")
        set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    endif()

    include(FetchContent)
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
    endif()

    # stb: single-header image libraries (public domain)
    # stb doesn't tag releases; pin to a specific commit for reproducibility
    FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )

    # tinyexr: single-file EXR reader/writer (MIT license)
    FetchContent_Declare(tinyexr
        GIT_REPOSITORY https://github.com/syoyo/tinyexr.git
        GIT_TAG        v1.0.10
        GIT_SHALLOW    TRUE
    )

    # miniz: single-file zlib replacement (MIT license)
    FetchContent_Declare(miniz
        GIT_REPOSITORY https://github.com/richgel999/miniz.git
        GIT_TAG        3.0.2
        GIT_SHALLOW    TRUE
    )

    # Download all three
    FetchContent_GetProperties(stb)
    if(NOT stb_POPULATED)
        FetchContent_Populate(stb)
    endif()

    FetchContent_GetProperties(tinyexr)
    if(NOT tinyexr_POPULATED)
        FetchContent_Populate(tinyexr)
    endif()

    FetchContent_GetProperties(miniz)
    if(NOT miniz_POPULATED)
        FetchContent_Populate(miniz)
    endif()

    # Set up output directory
    set(RENDER_DEPS_DIR "${CMAKE_BINARY_DIR}/RenderDeps")
    file(MAKE_DIRECTORY "${RENDER_DEPS_DIR}/include")

    # Copy headers to the output directory
    file(COPY_FILE "${stb_SOURCE_DIR}/stb_image.h"       "${RENDER_DEPS_DIR}/include/stb_image.h")
    file(COPY_FILE "${stb_SOURCE_DIR}/stb_image_write.h" "${RENDER_DEPS_DIR}/include/stb_image_write.h")
    file(COPY_FILE "${tinyexr_SOURCE_DIR}/tinyexr.h"     "${RENDER_DEPS_DIR}/include/tinyexr.h")
    file(COPY_FILE "${miniz_SOURCE_DIR}/miniz.h"          "${RENDER_DEPS_DIR}/include/miniz.h")

    # Set include path for consumers
    set(RENDER_DEPS_INCLUDE_DIR "${RENDER_DEPS_DIR}/include" CACHE PATH "Render workbench 3rdParty include dir")

    message(STATUS "Render deps downloaded:")
    message(STATUS "  stb:     ${stb_SOURCE_DIR}")
    message(STATUS "  tinyexr: ${tinyexr_SOURCE_DIR}")
    message(STATUS "  miniz:   ${miniz_SOURCE_DIR}")
    message(STATUS "  includes: ${RENDER_DEPS_INCLUDE_DIR}")

endmacro()
