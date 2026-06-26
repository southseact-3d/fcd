# SPDX-License-Identifier: LGPL-2.1-or-later

# Fusion360Import is a pure-Python workbench - no C++ sources, no
# shared library. We only need to ship the Python files (and the
# icon) to the build/install tree.

set(Fusion360Import_Scripts
    __init__.py
    Init.py
    InitGui.py
    fusion360_mcp_client.py
    fusion360_extractor.py
    extractor.py
    sketch_rebuilder.py
    feature_rebuilder.py
    importer.py
    commands.py
    connection_dialog.py
    results_dialog.py
    progress_dialog.py
    help_dialog.py
    README.md
)

set(Fusion360Import_TestScripts
    Fusion360ImportTests/__init__.py
)

add_custom_target(Fusion360ImportScripts ALL
    SOURCES
        ${Fusion360Import_Scripts}
        ${Fusion360Import_TestScripts}
)

fc_target_copy_resource(Fusion360ImportScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Fusion360Import
    ${Fusion360Import_Scripts}
    ${Fusion360Import_TestScripts}
)

fc_target_copy_resource(Fusion360ImportScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/Fusion360Import
    ${Fusion360Import_Scripts}
    ${Fusion360Import_TestScripts}
)

fc_target_copy_resource(Fusion360ImportScripts
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icons
    ${CMAKE_BINARY_DIR}/Mod/Fusion360Import/Resources/icons
    Fusion360Import.svg
)

fc_target_copy_resource(Fusion360ImportScripts
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icons
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/Fusion360Import/Resources/icons
    Fusion360Import.svg
)

INSTALL(
    FILES ${Fusion360Import_Scripts}
    DESTINATION Mod/Fusion360Import
)

INSTALL(
    FILES ${Fusion360Import_TestScripts}
    DESTINATION Mod/Fusion360Import/Fusion360ImportTests
)

INSTALL(
    FILES Resources/icons/Fusion360Import.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Mod/Fusion360Import/Resources/icons
)