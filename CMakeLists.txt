cmake_minimum_required(VERSION 3.20)
project(ArmyChess)

find_package(Qt6 COMPONENTS Widgets)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

execute_process(COMMAND where qmake OUTPUT_VARIABLE QT_INSTALL_PATH)
string(REGEX REPLACE "\n$" "/../.." QT_INSTALL_PATH ${QT_INSTALL_PATH})
string(REGEX REPLACE "\\\\" "/" QT_INSTALL_PATH ${QT_INSTALL_PATH})
set(CMAKE_PREFIX_PATH "${QMAKE_PATH}/lib/cmake")

set(CMAKE_CXX_FLAGS "-Wall")
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif ()

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

add_executable(ArmyChess
        src/main.cpp
        src/mainwindow.cpp src/mainwindow.h src/mainwindow.ui
        src/board.cpp src/board.h src/board.ui
        resources/resources.qrc src/ChessLabel.cpp src/ChessLabel.h 
        src/createserverwindow.cpp src/createserverwindow.h src/createserverwindow.ui 
        src/connectserverwindow.cpp src/connectserverwindow.h src/connectserverwindow.ui)
target_link_libraries(ArmyChess
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if (WIN32)
    set(DEBUG_SUFFIX)
    #    if (CMAKE_BUILD_TYPE MATCHES "Debug")
    #        set(DEBUG_SUFFIX "d")
    #    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

add_definitions(-DscaleRatio=1.5)
add_definitions(-DPORT=7173)
add_definitions(-DCHEAT)
