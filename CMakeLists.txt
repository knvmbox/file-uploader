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
    lib/common/logger/loggerfactory.cpp
    lib/common/qtwidgets/plaintextlogger.cpp
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
    lib/common/qtwidgets/plaintextlogger.hpp
    lib/common/logger/loggerfactory.hpp
    lib/common/queue.hpp
    model/abstractmodel.hpp
    model/urlsmodel.hpp
    mainwindow.hpp
    parametersdlg.hpp
    settings.hpp
    uploadimagesdlg.hpp
    utils/curlutils.hpp
    utils/webpdecoder.hpp
    widgets/fileselector.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(file-uploader
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    add_executable(
        ${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
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

include(cmake/webp.cmake)
include(cmake/qxlsx.cmake)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ext-lib
    ext-lib/fmt/include
    ext-lib/webp
    lib
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
    qxlsxqt5
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
