# SPDX-License-Identifier: LGPL-2.1-or-later

add_executable(Colour_tests_run
    ColourMap.cpp
)

target_include_directories(Colour_tests_run PUBLIC
    ${CMAKE_BINARY_DIR}
)
