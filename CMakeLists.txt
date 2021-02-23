cmake_minimum_required(VERSION 3.5)

project(SiriusStreamDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan
# NOTE: ONLY QT6 supports CONAN
if(NOT EXISTS "build/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/memsharded/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${PROJECT_SOURCE_DIR}/build/conan.cmake")
endif()

include("${PROJECT_SOURCE_DIR}/build/conan.cmake")
include(build/base.cmake)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    conan_cmake_run(CONANFILE conan/conanfile_windows.txt
            CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
            BASIC_SETUP CMAKE_TARGETS
            BUILD missing)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

   # starting with Catalina 10.15 /usr/local/include are deprecated, set new sdk location
   add_compile_options(-isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)

   conan_cmake_run(CONANFILE conan/conanfile_osx.txt
           CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
           BASIC_SETUP CMAKE_TARGETS
           BUILD missing)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    conan_cmake_run(CONANFILE conan/conanfile_linux.txt
            CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
            BASIC_SETUP CMAKE_TARGETS
            BUILD missing)
endif ()

# Conan version should be 1.32 and above
conan_basic_setup()

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# Custom 3rd party dependencies
include_directories(3rd_party/SiriusStreamSDK/include)
include_directories(3rd_party/x264/include)
include_directories(3rd_party/h264/include)
include_directories(3rd_party/speex-1.2rc1/include)
include_directories(3rd_party/portaudio/include)

if (CMAKE_BUILD_TYPE MATCHES  "Debug")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        link_directories(3rd_party/x264/lib/win64/debug)
        link_directories(3rd_party/h264/lib/win64/debug)
        link_directories(3rd_party/SiriusStreamSDK/lib/win64/debug)
        link_directories(3rd_party/speex-1.2rc1/lib/win64/debug)
        link_directories(3rd_party/portaudio/lib/win64/debug)

    #elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        # add here
    #elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            # add here
    endif()
else()
  if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        link_directories(3rd_party/x264/lib/win64/release)
        link_directories(3rd_party/h264/lib/win64/release)
        link_directories(3rd_party/SiriusStreamSDK/lib/win64/release)
        link_directories(3rd_party/speex-1.2rc1/lib/win64/release)
        link_directories(3rd_party/portaudio/lib/win64/release)
   #elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
       # add here
   #elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
           # add here
  endif()
endif()

#x264 encoder
list(APPEND ADDITIONAL_LINK_TARGETS "x264_lib")

#h264 decoder
list(APPEND ADDITIONAL_LINK_TARGETS "H264_lib")

#speex audio
list(APPEND ADDITIONAL_LINK_TARGETS "speex_1_2rc1")

#portaudio pa_stable_v190600_20161030
list(APPEND ADDITIONAL_LINK_TARGETS "portaudio_x64")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        dialog/chatdialog.h
        dialog/chatdialog.cpp
        dialog/inputdialog.cpp
        dialog/inputdialog.h
        dialog/streamerdialog.cpp
        dialog/streamerdialog.h
        dialog/viewerdialog.h
        dialog/viewerdialog.cpp

        dialog/viewerdialog.ui
        dialog/streamerdialog.ui
        dialog/chatdialog.ui
        dialog/inputdialog.ui

        worker/videocaptureworker.cpp
        worker/videocaptureworker.h

        worker/streamerworker.cpp
        worker/streamerworker.h
        worker/audiocaptureworker.h
        worker/audiocaptureworker.cpp
        worker/videoviewermanager.cpp
        worker/videoviewermanager.h

        audio/audioadapter.h
        audio/iaudioprocessor.h
        audio/portaudiocapture.h
        audio/portaudiocapture.cpp
        audio/media.h
        audio/portaudioplayer.cpp
        audio/portaudioplayer.h
        audio/ivideoviewer.h

        backend/backend.cpp
        backend/backend.h
        backend/ivideoencoder.h
        backend/x264videoencoder.h
        backend/x264videoencoder.cpp
        backend/h264decoder.cpp
        backend/h264decoder.h
        backend/colorconverter.h
        backend/colorconverter.cpp
        backend/speexadapter.h
        backend/speexadapter.cpp

        demo_constants.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SiriusStreamDemo
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(SiriusStreamDemo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SiriusStreamDemo
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(SiriusStreamDemo
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${ADDITIONAL_LINK_TARGETS}
    ${CONAN_LIBS}

    ClientLib
    RoutingLib
    NetioLib
    CryptoLib
    CoreLib
    sirius_stream
    SiriusStreamSDK
    )
