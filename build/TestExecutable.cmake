cmake_minimum_required(VERSION 3.11.4)

include(CMakeParseArguments)

function(add_test_executables)
    cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "PREFIX" # list of names of mono-valued arguments
        "SOURCE_FILE_NAMES;SYSTEM_INCLUDE_PATHS;INCLUDE_PATHS;DEPENDENCY_LIBS"
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.mycompany.test")
    set(MACOSX_BUNDLE_PRODUCT_NAME "test")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")

    foreach(fileName ${PARSED_ARGS_SOURCE_FILE_NAMES})
        get_filename_component(TEST_EXECUTABLE_NAME ${fileName} NAME_WE)
        set(TEST_EXECUTABLE_NAME ${PARSED_ARGS_PREFIX}${TEST_EXECUTABLE_NAME})
        add_executable(${TEST_EXECUTABLE_NAME} ${fileName})
        target_include_directories(${TEST_EXECUTABLE_NAME}
            PRIVATE ${PARSED_ARGS_INCLUDE_PATHS}
            SYSTEM ${PARSED_ARGS_SYSTEM_INCLUDE_PATHS}
        )
        target_link_libraries(${TEST_EXECUTABLE_NAME} 
                            ${PARSED_ARGS_DEPENDENCY_LIBS})
    endforeach(fileName)
endfunction(add_test_executables)
