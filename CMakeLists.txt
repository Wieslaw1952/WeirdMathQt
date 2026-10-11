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

# Installation and deployment, so the program runs on machines without Qt.
# The deploy script copies the needed Qt libraries and plugins next to the
# executable (windeployqt on Windows, the built-in deployer on Linux).
include(GNUInstallDirs)

install(TARGETS WeirdMath
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
    # Skip plugins a Widgets-only app never loads; some of them (vector images,
    # virtual keyboard, QML tooling) would drag in the whole Qt Quick stack.
    qt_generate_deploy_script(
        TARGET WeirdMath
        OUTPUT_SCRIPT deploy_script
        CONTENT "
qt_deploy_runtime_dependencies(
    EXECUTABLE \"$<TARGET_FILE:WeirdMath>\"
    GENERATE_QT_CONF
    NO_TRANSLATIONS
    INCLUDE_PLUGINS qwayland
    EXCLUDE_PLUGIN_TYPES vectorimageformats qmltooling networkinformation tls
                         egldeviceintegrations iconengines platforminputcontexts
)
")
else()
    qt_generate_deploy_app_script(
        TARGET WeirdMath
        OUTPUT_SCRIPT deploy_script
        NO_TRANSLATIONS
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
endif()
install(SCRIPT ${deploy_script})

if(UNIX AND NOT APPLE)
    install(FILES resources/weirdmath.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES resources/weirdmath.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps
        RENAME weirdmath.png)
endif()

# Linux packages: cpack in the build directory. The Windows installer is
# built with Inno Setup instead (installer/build-installer.ps1).
if(UNIX AND NOT APPLE)
    set(CPACK_PACKAGE_NAME "WeirdMath")
    set(CPACK_PACKAGE_VENDOR "Wieslaw1952")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Sierpinski-style fractals drawn with the chaos game")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "WeirdMath")
    set(CPACK_PACKAGE_EXECUTABLES "WeirdMath" "WeirdMath")
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/WeirdMath")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Wieslaw1952 <Wieslaw1952@users.noreply.github.com>")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxcb-cursor0")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_BINARY_DIR}/postinst;${CMAKE_CURRENT_BINARY_DIR}/postrm")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/postinst
        "#!/bin/sh\nln -sf /opt/WeirdMath/bin/WeirdMath /usr/bin/weirdmath\nln -sf /opt/WeirdMath/share/applications/weirdmath.desktop /usr/share/applications/weirdmath.desktop\nln -sf /opt/WeirdMath/share/icons/hicolor/32x32/apps/weirdmath.png /usr/share/pixmaps/weirdmath.png\n")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/postrm
        "#!/bin/sh\nrm -f /usr/bin/weirdmath /usr/share/applications/weirdmath.desktop /usr/share/pixmaps/weirdmath.png\n")
    file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/postinst ${CMAKE_CURRENT_BINARY_DIR}/postrm
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackOptions.cmake")
    include(CPack)
endif()
