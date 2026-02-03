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

add_subdirectory(LSystem)
add_subdirectory(LSystemMaya)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT INSTALL)
