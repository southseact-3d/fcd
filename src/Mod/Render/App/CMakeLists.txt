# SPDX-License-Identifier: LGPL-2.1-or-later

include_directories(
    SYSTEM
    ${CMAKE_SOURCE_DIR}/src/3rdParty/
    ${RENDER_DEPS_INCLUDE_DIR}
)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(RenderApp_LIBS
    FreeCADApp
    Part
    Qt6::Core
)

set(RenderApp_SRCS
    RenderAppGlobal.h
    RenderMaterial.h
    RenderMaterial.cpp
    RenderCamera.h
    RenderCamera.cpp
    RenderEnvironment.h
    RenderEnvironment.cpp
    RenderSettings.h
    RenderSettings.cpp
    RenderQueue.h
    RenderQueue.cpp
)

SOURCE_GROUP("App" FILES ${RenderApp_SRCS})

add_library(RenderApp SHARED ${RenderApp_SRCS})

target_compile_definitions(RenderApp PRIVATE RenderApp_EXPORTS)

target_link_libraries(RenderApp ${RenderApp_LIBS})

SET_BIN_DIR(RenderApp _RenderApp /Mod/Render)

install(
    TARGETS RenderApp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
