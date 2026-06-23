# SPDX-License-Identifier: LGPL-2.1-or-later

SET(Update_Scripts
    Init.py
    InitGui.py
    updater.py
    icon.svg
)

ADD_CUSTOM_TARGET(UpdateScripts ALL SOURCES ${Update_Scripts})

fc_target_copy_resource_flat(UpdateScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Update
    ${Update_Scripts}
)

INSTALL(
    FILES
        ${Update_Scripts}
    DESTINATION
        Mod/Update
)
