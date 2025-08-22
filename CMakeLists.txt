cmake_minimum_required(VERSION 3.19)
project(
    spm
    VERSION 0.1.0
    DESCRIPTION "System Power Manager"
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core DBus Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(spm
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
    config.hpp.in
)

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
configure_file(config.hpp.in config.hpp)

qt_add_translations(
    TARGETS spm
    TS_FILES spm_es_MX.ts
)

target_link_libraries(spm
    PRIVATE
    Qt::Core
    Qt::DBus
    Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS spm
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET spm
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
