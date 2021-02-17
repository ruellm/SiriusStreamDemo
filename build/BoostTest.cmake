cmake_minimum_required(VERSION 3.11.4)

include(CMakeParseArguments)
include(CTest)

if (NOT "${SANITIZER}" STREQUAL "")
    set(DEFAULT_TEST_TIMEOUT 30)
else()
    set(DEFAULT_TEST_TIMEOUT 300)
endif()

if (IOS OR ANDROID)
    set(TEST_BINARIES_ARE_STATIC_LIBS 1)
else()
    set(TEST_BINARIES_ARE_STATIC_LIBS 0)
endif()

function(add_boost_tests)
    cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "PREFIX" # list of names of mono-valued arguments
        "COMPONENT;SOURCE_FILE_NAMES;SYSTEM_INCLUDE_PATHS;INCLUDE_PATHS;DEPENDENCY_LIBS"
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
# The following are only for debugging:
#    message("Provided sources are: ${PARSED_ARGS_SOURCE_FILE_NAMES}")
#    message("Provided includes are: ${PARSED_ARGS_INCLUDE_PATHS}")
#    message("Provided libs are: ${PARSED_ARGS_DEPENDENCY_LIBS}")
    if (NOT TEST_BINARIES_ARE_STATIC_LIBS)
        foreach(fileName ${PARSED_ARGS_SOURCE_FILE_NAMES})
            get_filename_component(TEST_BINARY_NAME ${fileName} NAME_WE)
            set(TEST_BINARY_NAME ${PARSED_ARGS_PREFIX}${TEST_BINARY_NAME})
                
            add_executable(${TEST_BINARY_NAME} ${fileName})

            set_target_properties(${TEST_BINARY_NAME}
                PROPERTIES COMPILE_DEFINITIONS "BOOST_TEST_MODULE=${TEST_BINARY_NAME}")

            target_include_directories(${TEST_BINARY_NAME}
                PRIVATE ${PARSED_ARGS_INCLUDE_PATHS}
                SYSTEM ${PARSED_ARGS_SYSTEM_INCLUDE_PATHS}
            )

            target_link_libraries(${TEST_BINARY_NAME} 
                                ${PARSED_ARGS_DEPENDENCY_LIBS}
            )

            if (IOS)
                set_xcode_properties(
                    TARGETS
                        ${TEST_BINARY_NAME}
                    PROPERTIES
                        MACOSX_BUNDLE_GUI_IDENTIFIER="com.paltalk.paltalkvideo"
                        CODE_SIGN_IDENTITY="iPhone Developer"
                        DEVELOPMENT_TEAM=${DEVELOPMENT_TEAM_ID}
                )
            endif()

            if (NOT EXISTS "${fileName}")
                message(FATAL " ${fileName} does not exist")
            endif()

            file(READ "${fileName}" SOURCE_FILE_CONTENTS)
            string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\)" 
                FOUND_TESTS ${SOURCE_FILE_CONTENTS})

            string(REGEX MATCHALL "BOOST_FIXTURE_TEST_CASE\\( *([A-Za-z_0-9]+) *, *[A-Za-z_0-9]+ *\\)" 
                FOUND_FIXTURE_TESTS ${SOURCE_FILE_CONTENTS})

            foreach(HIT ${FOUND_TESTS} ${FOUND_FIXTURE_TESTS})
                string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_NAME ${HIT})
                string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *, *[A-Za-z_0-9]+ *\\).*" "\\1" TEST_NAME ${TEST_NAME})

                    add_test(NAME "${TEST_BINARY_NAME}.${TEST_NAME}" 
                            COMMAND ${TEST_BINARY_NAME}
                            --build_info=yes
                            --catch_system_error=yes
                            --color_output=no
                            --detect_fp_exceptions=yes
                            # disabled for testing
                            --detect_memory_leaks=0
                            --log_level=all
                            --report_level=detailed
                            --run_test=${TEST_BINARY_NAME}/${TEST_NAME})

                    set_tests_properties("${TEST_BINARY_NAME}.${TEST_NAME}"
                        PROPERTIES
                            TIMEOUT ${DEFAULT_TEST_TIMEOUT}
                            CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 1048576
                            CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 1048576
                    ) 
            endforeach()
        endforeach(fileName)
    else()
        foreach(component ${PARSED_ARGS_COMPONENT})
            set(TEST_BINARY_NAME ${PARSED_ARGS_PREFIX}${component})
                
            add_library(${TEST_BINARY_NAME} STATIC
                ${PARSED_ARGS_SOURCE_FILE_NAMES}
            )

            target_include_directories(${TEST_BINARY_NAME}
                PRIVATE ${PARSED_ARGS_INCLUDE_PATHS}
                SYSTEM ${PARSED_ARGS_SYSTEM_INCLUDE_PATHS}
            )
        endforeach()
    endif()
endfunction(add_boost_tests)
