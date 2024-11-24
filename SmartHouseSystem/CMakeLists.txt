cmake_minimum_required(VERSION 3.5)

project(SmartHouseSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt5 COMPONENTS Core Widgets Sql REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Auth
    ${CMAKE_CURRENT_SOURCE_DIR}/NetworkManager
)

set(PROJECT_SOURCES
    Core/main.cpp
    Core/mainwindow.cpp
    Auth/loginwindow.cpp
    Auth/registrationwindow.cpp
    NetworkManager/networkmanager.cpp
)

set(PROJECT_HEADERS
    Core/mainwindow.h
    Auth/loginwindow.h
    Auth/registrationwindow.h
    NetworkManager/networkmanager.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SmartHouseSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    if(ANDROID)
        add_library(SmartHouseSystem SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    else()
        add_executable(SmartHouseSystem
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    endif()
endif()

target_link_libraries(SmartHouseSystem PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)

if(${QT_VERSION} VERSION_LESS 5.0.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SmartHouseSystem)
endif()

set_target_properties(SmartHouseSystem PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SmartHouseSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SmartHouseSystem)
endif()
