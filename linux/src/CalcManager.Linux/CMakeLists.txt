# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# Builds the Windows Calculator CalcManager engine as libCalcManager.so.
# Upstream sources (../../../src/CalcManager) are compiled UNMODIFIED; all
# Linux differences live in this directory (shims/ + calc_manager_c_api.*).
cmake_minimum_required(VERSION 3.20)

project(CalcManagerLinux VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Upstream engine root (read-only reference; never edited by this port).
get_filename_component(ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/CalcManager" ABSOLUTE)
if(NOT EXISTS "${ENGINE_ROOT}/CalculatorManager.h")
    message(FATAL_ERROR "Engine sources not found at ${ENGINE_ROOT}")
endif()

# Shims MUST come first so <ppltasks.h> / <intsafe.h> resolve to ours,
# not to any system header of the same name.
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/shims"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${ENGINE_ROOT}"
)

file(GLOB ENGINE_SOURCES
    "${ENGINE_ROOT}/*.cpp"
    "${ENGINE_ROOT}/CEngine/*.cpp"
    "${ENGINE_ROOT}/Ratpack/*.cpp"
)
# pch.cpp only exists for MSVC precompiled headers; it compiles to nothing
# useful here and would just slow the build.
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/pch\\.cpp$")

add_library(CalcManager SHARED
    ${ENGINE_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/calc_manager_c_api.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_converter_bridge.cpp"
)

target_compile_options(CalcManager PRIVATE
    -Wall -Wextra
    -Wno-deprecated-declarations
    # "Header Files" contains a space; MSVC-era code also trips these:
    -Wno-unused-parameter
    # MSVC builds get the STL baseline via pch.h; force our portable prelude
    # instead (shims/linux_prelude.h). Upstream sources stay untouched.
    -include "${CMAKE_CURRENT_SOURCE_DIR}/shims/linux_prelude.h"
)

set_target_properties(CalcManager PROPERTIES
    OUTPUT_NAME "CalcManager"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# --- Smoke test (CTest): drives the real engine through the frozen C ABI -----
enable_testing()

add_executable(calc_manager_smoke tests/smoke_test.cpp)
target_link_libraries(calc_manager_smoke PRIVATE CalcManager)
target_include_directories(calc_manager_smoke PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME calc_manager_smoke COMMAND calc_manager_smoke)
