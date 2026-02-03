cmake_minimum_required(VERSION 3.22)

project(MyMaya)

if (NOT EXISTS "${MAYA_LOCATION}" AND NOT EXISTS "$ENV{MAYA_LOCATION}")
  message(FATAL_ERROR "'MAYA_LOCATION' variable needs to be set either through env or CMake.")
endif()

# query devkit location from both env and regular cmake variables
if(DEFINED ENV{DEVKIT_LOCATION} AND NOT "$ENV{DEVKIT_LOCATION}" STREQUAL "")
    set(GLOBAL_INSTALL_PREFIX "$ENV{DEVKIT_LOCATION}")
elseif(DEFINED DEVKIT_LOCATION)
    set(GLOBAL_INSTALL_PREFIX "${DEVKIT_LOCATION}")
else()
    message(FATAL_ERROR "Set 'DEVKIT_LOCATION' as an env variable or in CMake to install directly into plug-in search path.")
endif()
# normalize the path if needed on windows
string(REPLACE "\\" "/" GLOBAL_INSTALL_PREFIX ${GLOBAL_INSTALL_PREFIX})
set(CMAKE_INSTALL_PREFIX "${GLOBAL_INSTALL_PREFIX}/plug-ins") # set the built-in install variable

if(DEFINED ENV{MAYA_LOCATION})
    set(GLOBAL_MAYA_LOCATION "$ENV{MAYA_LOCATION}")
elseif(DEFINED MAYA_LOCATION)
    set(GLOBAL_MAYA_LOCATION "${MAYA_LOCATION}")
else()
    message(FATAL_ERROR "Set 'MAYA_LOCATION' as an env variable or in CMake.")
endif()
string(REPLACE "\\" "/" GLOBAL_MAYA_LOCATION ${GLOBAL_MAYA_LOCATION}) # normalize path

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON) # ensure VS always includes `INSTALL` as a target

# set global variables
set(LSystem_SOURCE_DIR ${CMAKE_SOURCE_DIR}/LSystem/src) # base code
set(Lsystem_TARGET_NAME Lsystem)
set(LsystemMaya_TARGET_NAME LsystemMaya)

set (GLOBAL_TARGET_SUFFIX "_hotreload" CACHE STRING "Auto Hot-reload in Maya") # for auto-hot reload in Maya

set(GLOBAL_CMAKE_SOURCE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") # make a separate variable for this dir

# create a joined string variable for CMake modules
set(GLOBAL_CMAKE_MODULE_FILES
    ${GLOBAL_CMAKE_SOURCE_MODULE_PATH}/FindMaya.cmake
    ${GLOBAL_CMAKE_SOURCE_MODULE_PATH}/CoreUtils.cmake
)

if(CMAKE_CONFIGURATION_TYPES)
    set(GLOBAL_INSTALL_CONFIGURATION_ARGS --config $<CONFIG>)
else()
    set(GLOBAL_INSTALL_CONFIGURATION_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()

set(INSTALL_HELPER_TARGET_NAME "install_helper")
add_custom_target(${INSTALL_HELPER_TARGET_NAME} 
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_CURRENT_BINARY_DIR}" 
    ${GLOBAL_INSTALL_CONFIGURATION_ARGS} COMMENT "Helper installation target."
)

add_subdirectory(LSystem)
add_subdirectory(LSystemMaya)

add_dependencies(${INSTALL_HELPER_TARGET_NAME} ${LsystemMaya_TARGET_NAME})

set(DEPLOY_TARGET_NAME DEPLOY)
add_custom_target(${DEPLOY_TARGET_NAME} COMMENT "Build plugin, install, and run maya")

add_dependencies(${DEPLOY_TARGET_NAME} ${LsystemMaya_TARGET_NAME} ${INSTALL_HELPER_TARGET_NAME})

set_target_properties(${DEPLOY_TARGET_NAME} PROPERTIES
    VS_DEBUGGER_COMMAND "${GLOBAL_MAYA_LOCATION}/bin/maya.exe"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${DEPLOY_TARGET_NAME})
