# general utility function to make CMake module files visible in VS
function(_MAKE_SOURCE_GROUP_MSVC target files tree_source prefix)
    target_sources(${target} PRIVATE ${files})

    set_source_files_properties(${files} PROPERTIES HEADER_FILE_ONLY TRUE)

    source_group(TREE ${tree_source} PREFIX ${prefix} FILES ${files})
endfunction(_MAKE_SOURCE_GROUP_MSVC)