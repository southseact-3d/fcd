# The Qt for Python project officially recommends using pip to install Shiboken, so we expect to find Shiboken in the
# site-packages directory. FreeCAD also requires shiboken6_generator, so this find script also locates that package
# and ensures the inclusion of its include directory when using the pip finding mechanism


# -------------------------------------------------------------------
# Ubuntu / KDE Neon workaround:
# The apt-installed libpyside6-dev package ships a Shiboken6Config.cmake
# that points SHIBOKEN_INCLUDE_DIR at /usr/lib/python3/dist-packages/shiboken6/include —
# but that directory does NOT contain sbkversion.h. The sbkversion.h
# header lives in the separate shiboken6-generator package, which is
# pip-installable. We always look for the generator (via pip OR apt) and
# append its include directory so sbkversion.h is found regardless of
# which installation method was used.
# -------------------------------------------------------------------

find_package(Shiboken6 CONFIG QUIET)
if(NOT Shiboken6_FOUND)
    if(NOT Shiboken6_INCLUDE_DIR AND TARGET Shiboken6::Shiboken6)
        get_property(Shiboken6_INCLUDE_DIR TARGET Shiboken6::Shiboken6 PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    endif()
    if(NOT Shiboken6_INCLUDE_DIR)
        find_pip_package(Shiboken6)
        if (Shiboken6_FOUND)
            set(SHIBOKEN_LIBRARY ${Shiboken6_LIBRARIES})
            set(SHIBOKEN_MAJOR_VERSION 6)
            set(SHIBOKEN_FOUND ON)
        endif()
        # The include directory we actually want is part of shiboken6-generator
        find_pip_package(shiboken6_generator)
        if (shiboken6_generator_FOUND)
            set(SHIBOKEN_INCLUDE_DIR ${shiboken6_generator_INCLUDE_DIRS})
        endif()
    endif()
else()
    set(SHIBOKEN_INCLUDE_DIR ${Shiboken6_INCLUDE_DIRS})
    set(SHIBOKEN_LIBRARY ${Shiboken6_LIBRARIES})
    set(SHIBOKEN_FOUND ON)
endif()

# -------------------------------------------------------------------
# Always try to locate shiboken6-generator's include directory and
# append it to SHIBOKEN_INCLUDE_DIR. This is the directory that
# contains sbkversion.h. Without it, builds fail on Ubuntu 24.04 with
# KDE Neon's libpyside6-dev package (which omits sbkversion.h from the
# main shiboken6 include dir).
#
# Look in:
#   1. pip-installed shiboken6_generator (most reliable)
#   2. /usr/include/shiboken6 (Debian/Ubuntu alt location)
#   3. Anywhere under /usr/lib/python3*/dist-packages/shiboken6_generator
# -------------------------------------------------------------------
if(SHIBOKEN_FOUND)
    # Try pip first
    find_pip_package(shiboken6_generator)
    if(shiboken6_generator_FOUND AND shiboken6_generator_INCLUDE_DIRS)
        list(APPEND SHIBOKEN_INCLUDE_DIR ${shiboken6_generator_INCLUDE_DIRS})
    endif()

    # Belt-and-braces: scan for sbkversion.h in known locations and add
    # the containing directory to SHIBOKEN_INCLUDE_DIR if it isn't there
    # already. This handles the case where pip install was used but
    # find_pip_package didn't pick it up (different Python or pip layout).
    file(GLOB _sbk_candidate_dirs
        "/usr/lib/python3*/dist-packages/shiboken6_generator/include"
        "/usr/lib/python3*/site-packages/shiboken6_generator/include"
        "/usr/local/lib/python3*/dist-packages/shiboken6_generator/include"
        "/usr/local/lib/python3*/site-packages/shiboken6_generator/include"
        "/usr/include/shiboken6"
    )
    foreach(_dir ${_sbk_candidate_dirs})
        if(EXISTS "${_dir}/sbkversion.h")
            list(FIND SHIBOKEN_INCLUDE_DIR "${_dir}" _already)
            if(_already EQUAL -1)
                list(APPEND SHIBOKEN_INCLUDE_DIR "${_dir}")
                message(STATUS "FindShiboken6: appended sbkversion.h provider: ${_dir}")
            endif()
        endif()
    endforeach()

    # Final sanity check: verify sbkversion.h is reachable through
    # SHIBOKEN_INCLUDE_DIR. If not, emit a warning so the user knows
    # what's missing.
    set(_sbk_found FALSE)
    foreach(_dir ${SHIBOKEN_INCLUDE_DIR})
        if(EXISTS "${_dir}/sbkversion.h")
            set(_sbk_found TRUE)
            break()
        endif()
    endforeach()
    if(NOT _sbk_found)
        message(WARNING
            "FindShiboken6: sbkversion.h not found in any SHIBOKEN_INCLUDE_DIR "
            "(${SHIBOKEN_INCLUDE_DIR}). Build will fail on files that "
            "#include <sbkversion.h> (e.g. src/Gui/PythonWrapper.cpp). "
            "Fix: pip install shiboken6-generator (matching your PySide6 version)."
        )
    endif()
endif()
