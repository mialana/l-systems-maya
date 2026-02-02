cmake_minimum_required(VERSION 3.22)

project(${LsystemMaya_TARGET_NAME})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add source files to the project
set(${LsystemMaya_TARGET_NAME}_SOURCE_FILES
src/cylinder.cpp
src/LSystemCmd.cpp
src/PluginMain.cpp
${LSystem_SOURCE_DIR}/LSystem.cpp
${LSystem_SOURCE_DIR}/vec.cpp
)


# Add header files to the project
set(${LsystemMaya_TARGET_NAME}_HEADER_FILES
    src/cylinder.h
    src/LSystemCmd.h
    ${LSystem_SOURCE_DIR}/LSystem.h
    ${LSystem_SOURCE_DIR}/matrix.h
    ${LSystem_SOURCE_DIR}/vec.h
)

find_package(Maya REQUIRED)

add_library(${LsystemMaya_TARGET_NAME} SHARED ${LsystemMaya_TARGET_NAME}_SOURCE_FILES ${LsystemMaya_TARGET_NAME}_HEADER_FILES)

target_link_libraries(${LsystemMaya_TARGET_NAME} PRIVATE Maya::Maya)
target_include_directories(${LsystemMaya_TARGET_NAME} 
    PUBLIC ${LSystem_SOURCE_DIR}
    PRIVATE Maya::Maya
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
)
MAYA_PLUGIN(${LsystemMaya_TARGET_NAME})

install(TARGETS ${LsystemMaya_TARGET_NAME} ${MAYA_TARGET_TYPE} DESTINATION plug-ins/${MAYA_VERSION})