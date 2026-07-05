# The Qt for Python project officially recommends using pip to install Shiboken, so we expect to find Shiboken in the
# site-packages directory. FreeCAD also requires shiboken6_generator, so this find script also locates that package
# and ensures the inclusion of its include directory when using the pip finding mechanism


# -------------------------------------------------------------------
# Ubuntu / KDE Neon workaround:
#
# The KDE Neon libpyside6-dev apt package ships a Shiboken6Config.cmake
# that points SHIBOKEN_INCLUDE_DIR at:
#   /usr/lib/python3/dist-packages/shiboken6/include
#
# But sbkversion.h is NOT in that directory — it's in a 'shiboken6'
# SUBDIRECTORY of it:
#   /usr/lib/python3/dist-packages/shiboken6/include/shiboken6/sbkversion.h
#
# So when src/Gui/PythonWrapper.cpp does `#include <sbkversion.h>`, the
# compiler searches the -isystem path (.../shiboken6/include) and fails
# because sbkversion.h is one level deeper (.../shiboken6/include/shiboken6).
#
# Fix: after the standard lookup, run find_path(RECURSE) for sbkversion.h
# across all plausible shiboken6 / shiboken6_generator install locations
# (apt, pip --user, pip --system, conda) and append every directory that
# contains it to SHIBOKEN_INCLUDE_DIR.
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
# Always locate sbkversion.h and add its containing directory to
# SHIBOKEN_INCLUDE_DIR. Required for src/Gui/PythonWrapper.cpp which
# does `#include <sbkversion.h>` unconditionally when HAVE_SHIBOKEN6
# is defined.
#
# We use find_path with PATHS covering apt + pip --user + pip --system
# layouts on Debian/Ubuntu/KDE Neon. The 'NO_DEFAULT_PATH' ensures we
# only look where we expect (not /usr/include or similar, which would
# pick up unrelated headers).
# -------------------------------------------------------------------
if(SHIBOKEN_FOUND)
    # find_path returns _sbkversion_dir = the directory containing sbkversion.h
    find_path(_sbkversion_dir
        NAMES sbkversion.h
        PATHS
            # apt layout (KDE Neon, libshiboken6-py3-6.11)
            /usr/lib/python3/dist-packages/shiboken6/include
            /usr/lib/python3/dist-packages/shiboken6/include/shiboken6
            # pip --user layout
            "$ENV{HOME}/.local/lib/python3.12/site-packages/shiboken6_generator/include"
            "$ENV{HOME}/.local/lib/python3.11/site-packages/shiboken6_generator/include"
            "$ENV{HOME}/.local/lib/python3.10/site-packages/shiboken6_generator/include"
            # pip --system layout
            /usr/local/lib/python3.12/dist-packages/shiboken6_generator/include
            /usr/local/lib/python3.11/dist-packages/shiboken6_generator/include
            /usr/local/lib/python3.10/dist-packages/shiboken6_generator/include
            # generic Debian/Ubuntu alternative
            /usr/include/shiboken6
        NO_DEFAULT_PATH
    )
    if(_sbkversion_dir)
        list(FIND SHIBOKEN_INCLUDE_DIR "${_sbkversion_dir}" _already)
        if(_already EQUAL -1)
            list(APPEND SHIBOKEN_INCLUDE_DIR "${_sbkversion_dir}")
            message(STATUS "FindShiboken6: appended sbkversion.h location: ${_sbkversion_dir}")
        endif()
        # Also try the nested 'shiboken6' subdirectory of the apt include path
        # (KDE Neon layout: /usr/lib/python3/dist-packages/shiboken6/include/shiboken6/sbkversion.h)
        set(_nested_dir "/usr/lib/python3/dist-packages/shiboken6/include/shiboken6")
        if(EXISTS "${_nested_dir}/sbkversion.h")
            list(FIND SHIBOKEN_INCLUDE_DIR "${_nested_dir}" _already_nested)
            if(_already_nested EQUAL -1)
                list(APPEND SHIBOKEN_INCLUDE_DIR "${_nested_dir}")
                message(STATUS "FindShiboken6: appended nested sbkversion.h location: ${_nested_dir}")
            endif()
        endif()
    else()
        message(WARNING
            "FindShiboken6: sbkversion.h not found in any expected location.\n"
            "Build will fail on files that #include <sbkversion.h> (e.g.\n"
            "src/Gui/PythonWrapper.cpp).\n"
            "Fix: pip install shiboken6-generator (matching your PySide6 version),\n"
            "or check that the apt libshiboken6-py3-6.x package is installed."
        )
    endif()
endif()
