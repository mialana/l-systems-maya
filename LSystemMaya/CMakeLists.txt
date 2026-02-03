cmake_minimum_required(VERSION 3.22)

project(${LsystemMaya_TARGET_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include cmake module files
include(${GLOBAL_CMAKE_SOURCE_MODULE_PATH}/CoreUtils.cmake)

# Add source files to the project
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cylinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LSystemCmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PluginMain.cpp
    ${${Lsystem_TARGET_NAME}_SOURCE_FILES}
)


# Add header files to the project
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cylinder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LSystemCmd.h
    ${LSystem_SOURCE_DIR}/LSystem.h
    ${LSystem_SOURCE_DIR}/matrix.h
    ${LSystem_SOURCE_DIR}/vec.h
    ${${Lsystem_TARGET_NAME}_HEADER_FILES}
)

set(CMAKE_MODULE_PATH ${GLOBAL_CMAKE_SOURCE_MODULE_PATH})
find_package(Maya REQUIRED)

add_library(${LsystemMaya_TARGET_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${LsystemMaya_TARGET_NAME} PRIVATE Maya::Maya)
target_include_directories(${LsystemMaya_TARGET_NAME} 
    PUBLIC ${LSystem_SOURCE_DIR}
    PRIVATE Maya::Maya
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
)


_MAKE_SOURCE_GROUP_MSVC(${LsystemMaya_TARGET_NAME} "${GLOBAL_CMAKE_MODULE_FILES}" 
    "${GLOBAL_CMAKE_SOURCE_MODULE_PATH}" "cmake_modules" "ON" "ON")

_MAKE_SOURCE_GROUP_MSVC(${LsystemMaya_TARGET_NAME} "${${Lsystem_TARGET_NAME}_HEADER_FILES}" 
    "${LSystem_SOURCE_DIR}" "core/Header Files" "OFF" "OFF"
)
_MAKE_SOURCE_GROUP_MSVC(${LsystemMaya_TARGET_NAME} "${${Lsystem_TARGET_NAME}_SOURCE_FILES}" 
    "${LSystem_SOURCE_DIR}" "core/Source Files" "OFF" "OFF"
)

MAYA_PLUGIN(${LsystemMaya_TARGET_NAME})

# install directly into plug-in path
install(TARGETS ${LsystemMaya_TARGET_NAME} ${MAYA_TARGET_TYPE} DESTINATION "plug-ins")
