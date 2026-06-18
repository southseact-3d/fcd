# SPDX-License-Identifier: LGPL-2.1-or-later

include_directories(
    SYSTEM
    ${CMAKE_SOURCE_DIR}/src/3rdParty/
    ${RENDER_DEPS_INCLUDE_DIR}
)

set(RenderRaytracerGlobal_HDR
    Raytracer/RenderRaytracerGlobal.h
)

set(RenderRaytracer_SRCS
    Raytracer/RenderEngine.h
    Raytracer/RenderEngine.cpp
    Raytracer/PathTracer.h
    Raytracer/PathTracer.cpp
    Raytracer/BVH.h
    Raytracer/BVH.cpp
    Raytracer/RenderScene.h
    Raytracer/RenderScene.cpp
    Raytracer/ShadingSystem.h
    Raytracer/ShadingSystem.cpp
    Raytracer/RenderCameraRT.h
    Raytracer/RenderCameraRT.cpp
    Raytracer/Light.h
    Raytracer/Light.cpp
    Raytracer/TextureRT.h
    Raytracer/TextureRT.cpp
    Raytracer/OutputWriter.h
    Raytracer/OutputWriter.cpp
)

# Implementation units for stb and tinyexr (compiled into the RenderRaytracer target)
set(RenderDepsImpl_SRCS
    Raytracer/impl/stb_impl.cpp
    Raytracer/impl/tinyexr_impl.cpp
)

SOURCE_GROUP("Raytracer" FILES ${RenderRaytracer_SRCS})
SOURCE_GROUP("Raytracer/impl" FILES ${RenderDepsImpl_SRCS})

set(RenderGui_Python_SRCS
    Gui/RenderWorkbench.py
    Gui/RenderCommands.py
    Gui/TaskRenderSettings.py
    Gui/TaskSceneSettings.py
    Gui/TaskCameraSettings.py
    Gui/TaskMaterialEditor.py
    Gui/PanelRenderQueue.py
    Gui/PanelSceneBrowser.py
)

set(RenderGuiIcon_SVG
    Resources/Icons/RenderWorkbench.svg
)

add_library(RenderRaytracer SHARED
    ${RenderRaytracer_SRCS}
    ${RenderRaytracerGlobal_HDR}
    ${RenderDepsImpl_SRCS}
)

target_link_libraries(RenderRaytracer
    RenderApp
    Part
    FreeCADApp
    ${OCC_LIBRARIES}
)

SET_BIN_DIR(RenderRaytracer _RenderRaytracer /Mod/Render)

install(
    TARGETS RenderRaytracer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES ${RenderGui_Python_SRCS}
    DESTINATION Mod/Render/Gui
)

install(
    FILES ${RenderGuiIcon_SVG}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/Mod/Render/Resources/Icons"
)
