# SPDX-License-Identifier: LGPL-2.1-or-later

set(ColourGui_LIBS
    Colour
    FreeCADGui
    Part
)

qt_add_resources(Colour_QRC_SRCS Resources/Colour.qrc)

set(ColourGui_SRCS
    ${Colour_QRC_SRCS}
    AppColourGui.cpp
    Command.cpp
    Command.h
    PreCompiled.h
    RecentColours.cpp
    RecentColours.h
    SelectionObserver.cpp
    SelectionObserver.h
    ViewProviderColourMap.cpp
    ViewProviderColourMap.h
    Workbench.cpp
    Workbench.h
    LayerPanel.cpp
    LayerPanel.h
    MaterialPanel.cpp
    MaterialPanel.h
    ColourPickerDialog.cpp
    ColourPickerDialog.h
    PanelManager.cpp
    PanelManager.h
    ExportDialog.cpp
    ExportDialog.h
)

set(ColourGui_Scripts
    ../InitGui.py
)

set(ColourGuiIcon_SVG
    Resources/icons/ColourWorkbench.svg
)

add_library(ColourGui SHARED
    ${ColourGui_SRCS}
    ${ColourGui_Scripts}
    ${ColourGuiIcon_SVG}
)

if(FREECAD_USE_PCH)
    target_precompile_headers(ColourGui PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_LIST_DIR}/PreCompiled.h">
    )
endif(FREECAD_USE_PCH)

target_include_directories(
    ColourGui
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(ColourGui ${ColourGui_LIBS})

if(MSVC)
    target_compile_options(ColourGui PRIVATE /wd4251)
endif()

if(FREECAD_WARN_ERROR)
    target_compile_warn_error(ColourGui)
endif()

fc_target_copy_resource_flat(ColourGui
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Colour
    ${ColourGui_Scripts}
)

SET_BIN_DIR(ColourGui ColourGui /Mod/Colour)
SET_PYTHON_PREFIX_SUFFIX(ColourGui)

fc_copy_sources(ColourGui "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/Colour" ${ColourGuiIcon_SVG})

install(TARGETS ColourGui DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${ColourGuiIcon_SVG} DESTINATION "${CMAKE_INSTALL_DATADIR}/Mod/Colour/Resources/icons")
