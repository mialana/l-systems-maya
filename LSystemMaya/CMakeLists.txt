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

set(MEL_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/bin/LSystem_gui.mel)

# install directly into plug-in path
install(TARGETS ${LsystemMaya_TARGET_NAME} ${MAYA_TARGET_TYPE} DESTINATION "plug-ins")

foreach(mel_script IN LISTS MEL_SCRIPTS)
    get_filename_component(mel_script_name "${mel_script}" NAME)
    get_filename_component(mel_script_name_no_ext "${mel_script}" NAME_WE)

    set(mel_script_output_path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/${mel_script_name})

    add_custom_target(${mel_script_name_no_ext} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${mel_script}
                ${mel_script_output_path}
    )

    target_sources(${mel_script_name_no_ext}
        PRIVATE ${mel_script}
    )

    set_source_files_properties(${mel_script} 
        TARGET_DIRECTORY ${mel_script_name_no_ext}
        PROPERTIES HEADER_FILE_ONLY TRUE
    )

    add_dependencies(${LsystemMaya_TARGET_NAME} ${mel_script_name_no_ext})

    install(FILES "${mel_script_output_path}" DESTINATION "plug-ins") #install

endforeach()