
cmake_minimum_required(VERSION 3.10)
project(DummySnapshotDemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_WIN32_EXECUTABLE OFF CACHE BOOL "Force console app")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE OFF CACHE BOOL "Force console app")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

add_executable(DummySnapshotDemo
    src/main.cpp
    src/Dummy.cpp
    src/SnapshotManager.cpp
    src/ResetManager.cpp

    src/Dummy.h
    src/SnapshotManager.h
    src/ResetManager.h
    src/Resettable.h

    qml/Main.qml
)

target_link_libraries(DummySnapshotDemo
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
)

add_custom_command(TARGET DummySnapshotDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:DummySnapshotDemo>/qml
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/qml/Main.qml
    $<TARGET_FILE_DIR:DummySnapshotDemo>/qml/Main.qml
)

set(CMAKE_SH "CMAKE_SH-NOTFOUND")
