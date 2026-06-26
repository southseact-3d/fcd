# SPDX-License-Identifier: LGPL-2.1-or-later

set(Slicer_Scripts
    Init.py
    InitGui.py
    Slicer_Gui.py
)

set(Slicer_Core_Scripts
    SlicerCore/__init__.py
    SlicerCore/mesh_io.py
    SlicerCore/fdm_slicer.py
    SlicerCore/libslic3r_wrapper.py
    SlicerCore/resin_slicer.py
    SlicerCore/resin_formats.py
    SlicerCore/support_generator.py
    SlicerCore/hollowing.py
    SlicerCore/printer_profiles.py
    SlicerCore/gcode_parser.py
)

set(Slicer_UI_Scripts
    SlicerUI/__init__.py
    SlicerUI/slicer_task_panel.py
    SlicerUI/printer_selector_widget.py
    SlicerUI/material_selector_widget.py
    SlicerUI/slicer_preview_widget.py
    SlicerUI/support_paint_widget.py
    SlicerUI/gcode_preview_widget.py
    SlicerUI/slicer_output_widget.py
    SlicerUI/slicer_preferences.py
    SlicerUI/cavity_result_dialog.py
)

set(Slicer_Profiles
    Profiles/fdm_printers/generic.json
    Profiles/fdm_printers/bambu_x1c.json
    Profiles/fdm_printers/creality_ender3.json
    Profiles/fdm_printers/prusa_mk3s.json
    Profiles/fdm_printers/voron_2.4.json
    Profiles/fdm_materials/PLA.json
    Profiles/fdm_materials/ABS.json
    Profiles/fdm_materials/PETG.json
    Profiles/fdm_materials/TPU.json
    Profiles/resin_printers/generic.json
    Profiles/resin_printers/anycubic_photon_mono_x.json
    Profiles/resin_printers/elegoo_mars3.json
    Profiles/resin_materials/generic_resin.json
)

set(Slicer_Resources
    Resources/icons/Slicer_Workbench.svg
)

add_custom_target(SlicerScripts ALL
    SOURCES ${Slicer_Scripts} ${Slicer_Core_Scripts} ${Slicer_UI_Scripts} ${Slicer_Profiles} ${Slicer_Resources}
)

fc_target_copy_resource(SlicerScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Slicer
    ${Slicer_Scripts})

fc_target_copy_resource(SlicerScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Slicer
    ${Slicer_Core_Scripts})

fc_target_copy_resource(SlicerScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Slicer
    ${Slicer_UI_Scripts})

fc_target_copy_resource(SlicerScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Slicer
    ${Slicer_Profiles})

fc_target_copy_resource(SlicerScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Slicer
    ${Slicer_Resources})

install(
    FILES
        ${Slicer_Scripts}
    DESTINATION
        Mod/Slicer
)

install(
    FILES
        ${Slicer_Core_Scripts}
    DESTINATION
        Mod/Slicer/SlicerCore
)

install(
    FILES
        ${Slicer_UI_Scripts}
    DESTINATION
        Mod/Slicer/SlicerUI
)

install(
    FILES
        ${Slicer_Profiles}
    DESTINATION
        Mod/Slicer/Profiles
)
