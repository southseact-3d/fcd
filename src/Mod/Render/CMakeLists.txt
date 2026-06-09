# SPDX-License-Identifier: LGPL-2.1-or-later

add_subdirectory(App)
if(BUILD_GUI)
    add_subdirectory(Gui)
endif(BUILD_GUI)

set(Render_Scripts
    __init__.py
    Init.py
)

if(BUILD_GUI)
    list(APPEND Render_Scripts
        InitGui.py
    )
endif(BUILD_GUI)

install(
    FILES ${Render_Scripts}
    DESTINATION Mod/Render
)

install(
    DIRECTORY Resources
    DESTINATION Mod/Render
)
