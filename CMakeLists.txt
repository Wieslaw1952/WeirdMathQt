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
    # Compiled in its own object library so the .rc gets none of WeirdMath's
    # include dirs: MinGW windres does not quote them when calling the
    # preprocessor, which breaks builds in paths with spaces.
    add_library(WeirdMathIcon OBJECT resources/weirdmath.rc)
    set_target_properties(WeirdMathIcon PROPERTIES
        AUTOMOC OFF AUTOUIC OFF AUTORCC OFF
    )
    target_sources(WeirdMath PRIVATE $<TARGET_OBJECTS:WeirdMathIcon>)
endif()

target_link_libraries(WeirdMath PRIVATE Qt6::Widgets)

set_target_properties(WeirdMath PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
