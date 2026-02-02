cmake_minimum_required(VERSION 3.22)

project(${Lsystem_TARGET_NAME})

# Add source files to the project
set(${LsystemMaya_TARGET_NAME}_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec.cpp
)

# Add header files to the project
set(${LsystemMaya_TARGET_NAME}_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LSystem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vec.h
)

# Create the executable
add_executable(${Lsystem_TARGET_NAME} ${LsystemMaya_TARGET_NAME}_SOURCE_FILES ${LsystemMaya_TARGET_NAME}_HEADER_FILES)

# Set additional configurations for the executable
target_include_directories(${Lsystem_TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set runtime library for Debug|x64
set_property(TARGET ${Lsystem_TARGET_NAME} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
)
