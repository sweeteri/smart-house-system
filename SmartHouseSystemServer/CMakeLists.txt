cmake_minimum_required(VERSION 3.14)

project(SmartHouseSystemServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql)

set(PROJECT_SOURCES
    SmartHouseSystemServer/main.cpp
    SmartHouseSystemServer/smarthousesystemserver.cpp
    DatabaseManager/databasemanager.cpp
)

set(PROJECT_HEADERS
    SmartHouseSystemServer/smarthousesystemserver.h
    DatabaseManager/databasemanager.h
)

add_executable(SmartHouseSystemServer ${PROJECT_SOURCES} ${PROJECT_HEADERS})

source_group("Header Files" FILES ${PROJECT_HEADERS})
source_group("Source Files" FILES ${PROJECT_SOURCES})


target_include_directories(SmartHouseSystemServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartHouseSystemServer
    ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseManager
)

target_link_libraries(SmartHouseSystemServer
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
)

include(GNUInstallDirs)
install(TARGETS SmartHouseSystemServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
