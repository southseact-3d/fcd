# SPDX-License-Identifier: LGPL-2.1-or-later

set(Colour_LIBS
    FreeCADApp
)

if(FREECAD_USE_OPENCASCADE)
    list(APPEND Colour_LIBS
        ${OpenCASCADE_LIBRARIES}
    )
endif()

set(Colour_SRCS
    AppColour.cpp
    AppColourPy.cpp
    BRepTessellator.cpp
    BRepTessellator.h
    ColourMap.cpp
    ColourMap.h
    ColourMapObject.cpp
    ColourMapObject.h
    Exporter.cpp
    Exporter.h
    PreCompiled.h
    SketchRasterizer.cpp
    SketchRasterizer.h
    TextureCompositor.cpp
    TextureCompositor.h
    UVAtlas.cpp
    UVAtlas.h
    XAtlasWrapper.cpp
    XAtlasWrapper.h
)

set(Colour_Scripts
    ../Init.py
)

add_library(Colour SHARED ${Colour_SRCS} ${Colour_Scripts})

if(FREECAD_USE_PCH)
    target_precompile_headers(Colour PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:"${CMAKE_CURRENT_LIST_DIR}/PreCompiled.h">
    )
endif(FREECAD_USE_PCH)

target_include_directories(
    Colour
    PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/3rdparty/xatlas/source
)

target_link_libraries(Colour ${Colour_LIBS})

if(MSVC)
    target_compile_options(Colour PRIVATE /wd4251)
endif()

if(FREECAD_WARN_ERROR)
    target_compile_warn_error(Colour)
endif()

fc_target_copy_resource_flat(Colour
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Colour
    ${Colour_Scripts}
)

SET_BIN_DIR(Colour Colour /Mod/Colour)
SET_PYTHON_PREFIX_SUFFIX(Colour)

install(TARGETS Colour DESTINATION ${CMAKE_INSTALL_LIBDIR})
