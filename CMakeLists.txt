cmake_minimum_required(VERSION 3.16)

project(CANvis1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        libs/can-utils/dbc/dbc_parser.cpp  # Add the DBC parser source file
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CANvis1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(CANvis1 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CANvis1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Add the include directory for can-utils
target_include_directories(CANvis1 PRIVATE libs/can-utils)

target_link_libraries(CANvis1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CANvis1)
endif()
set_target_properties(CANvis1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CANvis1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CANvis1)
endif()

find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(CANvis1 PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
