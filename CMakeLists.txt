cmake_minimum_required(VERSION 3.15)
project(MyProgram C)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -Wno-unused-but-set-variable")

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/include
)

# Internal sources
file(GLOB INTERNAL_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

# External sources
file(GLOB EXTERNAL_SOURCES ${PROJECT_SOURCE_DIR}/external/src/*.c)

# Main file
set(MAIN_SOURCE ${PROJECT_SOURCE_DIR}/main.c)

# Combine all sources
set(ALL_SOURCES
    ${MAIN_SOURCE}
    ${INTERNAL_SOURCES}
    ${EXTERNAL_SOURCES}
)

# Create executable
add_executable(program ${ALL_SOURCES})
