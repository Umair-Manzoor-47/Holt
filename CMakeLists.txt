cmake_minimum_required(VERSION 3.24)

project(holt LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(holt_project INTERFACE)

target_include_directories(
        holt_project
        INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(holt_project INTERFACE cxx_std_20)

target_compile_options(
        holt_project
        INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -fno-exceptions
)

add_executable(
        holt
        src/holt_main.cpp
)

target_link_libraries(
        holt
        PRIVATE holt_project
)

set_target_properties(
        holt
        PROPERTIES
            OUTPUT_NAME "holt"
            OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
)

add_executable(
        holt_trades
        tools/holt_trades.cpp
)

target_link_libraries(
        holt_trades
        PRIVATE holt_project
)

set_target_properties(
        holt_trades
        PROPERTIES
            OUTPUT_NAME "holt_trades"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

add_executable(
        holt_liquidity
        tools/holt_liquidity.cpp
)

target_link_libraries(
        holt_liquidity
        PRIVATE holt_project
)

set_target_properties(
        holt_liquidity
        PROPERTIES
        OUTPUT_NAME "holt_liquidity"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)