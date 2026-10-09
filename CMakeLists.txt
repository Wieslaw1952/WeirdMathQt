cmake_minimum_required(VERSION 3.16)

project(WeirdMath VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(WeirdMath
    main.cpp
    mainwindow.h mainwindow.cpp
    chaoscanvas.h chaoscanvas.cpp
)

qt_add_resources(WeirdMath "resources"
    PREFIX "/"
    FILES resources/weirdmath.ico
)

if(WIN32)
    target_sources(WeirdMath PRIVATE resources/weirdmath.rc)
endif()

target_link_libraries(WeirdMath PRIVATE Qt6::Widgets)

set_target_properties(WeirdMath PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
