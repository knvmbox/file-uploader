cmake_minimum_required(VERSION 3.5)

project(file-uploader VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
    ext-lib/fmt/src/format.cc
    model/abstractmodel.cpp
    model/urlsmodel.cpp
    main.cpp
    mainwindow.cpp
    parametersdlg.cpp
    settings.cpp
    uploadimagesdlg.cpp
    widgets/fileselector.cpp

    mainwindow.ui
    parametersdlg.ui
    uploadimagesdlg.ui

    file-uploader.qrc
)

set(PROJECT_HEADERS
    model/abstractmodel.hpp
    model/urlsmodel.hpp
    mainwindow.hpp
    parametersdlg.hpp
    settings.hpp
    uploadimagesdlg.hpp
    utils/curldownloader.hpp
    utils/webpdecoder.hpp
    widgets/fileselector.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(file-uploader
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET file-uploader APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(file-uploader SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(file-uploader
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set(requiredlibs)

find_package(CURL)
if(CURL_FOUND)
    target_include_directories(
        ${PROJECT_NAME} PRIVATE
        ${CURL_INCLUDE_DIR}
    )
    set(requiredlibs ${requiredlibs} ${CURL_LIBRARIES} )
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find the CURL library and development files.")
endif()

add_library(exampleutil STATIC IMPORTED)
set_target_properties(exampleutil PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libexampleutil.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imagedec STATIC IMPORTED)
set_target_properties(imagedec PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimagedec.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imageenc STATIC IMPORTED)
set_target_properties(imageenc PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimageenc.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imageioutil STATIC IMPORTED)
set_target_properties(imageioutil PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimageioutil.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(sharpyuv STATIC IMPORTED)
set_target_properties(sharpyuv PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libsharpyuv.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(webp STATIC IMPORTED)
set_target_properties(webp PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libwebp.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(webpdemux STATIC IMPORTED)
set_target_properties(webpdemux PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libwebpdemux.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ext-lib
    ext-lib/fmt/include
    ext-lib/webp
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${requiredlibs}
    exampleutil
    imagedec
    imageenc
    webpdemux
    png
    z
    imageioutil
    webp
    sharpyuv
    pthread
)

set_target_properties(file-uploader PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(file-uploader)
endif()
