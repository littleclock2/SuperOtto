cmake_minimum_required(VERSION 3.16)
project(SuperOtto VERSION 3.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network Multimedia)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/main.cpp
    src/core/Grid.cpp
    src/core/Robot.cpp
    src/core/AStar.cpp
    src/core/GameManager.cpp
    src/network/Protocol.cpp
    src/network/NetworkManager.cpp
    src/audio/MusicPlayer.cpp
    src/ui/GameWidget.cpp
    src/ui/MusicWidget.cpp
    src/ui/NetworkWidget.cpp
    src/ui/SettingsWidget.cpp
    src/ui/MainWindow.cpp
)

set(HEADERS
    src/core/Grid.h
    src/core/Robot.h
    src/core/AStar.h
    src/core/GameManager.h
    src/network/Protocol.h
    src/network/NetworkManager.h
    src/audio/MusicPlayer.h
    src/ui/GameWidget.h
    src/ui/MusicWidget.h
    src/ui/NetworkWidget.h
    src/ui/SettingsWidget.h
    src/ui/MainWindow.h
)

set(RESOURCES
    resources/resources.qrc
)

if(WIN32)
    set(WIN_RESOURCES resources/icon.rc)
endif()

add_executable(SuperOtto ${SOURCES} ${HEADERS} ${RESOURCES} ${WIN_RESOURCES})

target_include_directories(SuperOtto PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/src/ui
)

target_link_libraries(SuperOtto PRIVATE
    Qt5::Widgets
    Qt5::Network
    Qt5::Multimedia
)
