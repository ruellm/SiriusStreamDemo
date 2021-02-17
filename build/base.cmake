cmake_minimum_required(VERSION 3.11.4)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:module /W4")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd\"4100\" /wd\"4127\" /wd\"4150\" \
        /wd\"4189\" /std:c++17 ")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang: Setting CMAKE_CXX_FLAGS. Was = ${CMAKE_CXX_FLAGS}")
    #?? TODO:  -fmodules-ts
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -Wall -Wno-long-long \
     -Wno-unused-variable -Wno-unused-function -Wno-variadic-macros \
     -Wno-gnu-zero-variadic-macro-arguments \
     -Wno-missing-braces -Wno-unused-private-field -Wno-language-extension-token \
     -Wno-gnu-statement-expression \
     -pedantic -fvisibility=default")

    # Only add warnings as errors if not using mobile platforms
    if (NOT IOS AND NOT ANDROID)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-c11-extensions ")
    elseif (ANDROID)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
    endif()

    message(STATUS "\tIs = ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "GNU: Setting CMAKE_CXX_FLAGS. Was = ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long \
     -Wno-unused-variable -Wno-unused-function -Wno-variadic-macros \
     -Wno-missing-braces -Wno-unknown-pragmas")

# TODO: -Werror -pedantic
     message(STATUS "\tIs = ${CMAKE_CXX_FLAGS}")
else ()
    message(FATAL_ERROR "Cannot detect compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if (ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-warn-shared-textrel")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_START_GROUP     "-Wl,--start-group")
    set(CMAKE_END_GROUP     "-Wl,--end-group")

    set(WHOLE_ARCHIVE "-Wl,-whole-archive")
    set(NO_WHOLE_ARCHIVE "-Wl,-no-whole-archive")
endif()

# Use the gold linker if it's available
if (UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
    if ("${ld_version}" MATCHES "GNU gold")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    endif()
endif()

set(SANITIZER "" CACHE STRING "Sanitizer to use for debugging. Specify via -DSANITIZER:STRING=undefined")

if (NOT "${SANITIZER}" STREQUAL "")
    message("Sanitizer enabled: ${SANITIZER}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -DUSING_SANITIZER=1 -fsanitize=${SANITIZER} -fsanitize-blacklist=${PROJECT_SOURCE_DIR}/sanitizers/${SANITIZER}-blacklist.txt")
else()
    message("Sanitizer not enabled: ${SANITIZER}")
endif()

if (DEFINED CODE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCODE_COVERAGE")
endif()
