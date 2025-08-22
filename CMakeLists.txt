cmake_minimum_required(VERSION 3.19)
project(
    qspm
    VERSION 0.1.0
    DESCRIPTION "System Power Manager"
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core DBus Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(qspm
    WIN32 MACOSX_BUNDLE
    config.hpp.in
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
    power.hpp
    power.cpp
)

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
configure_file(config.hpp.in config.hpp)

qt_add_translations(
    TARGETS qspm
    TS_FILES qspm_es_MX.ts
)

target_link_libraries(qspm
    PRIVATE
    Qt::Core
    Qt::DBus
    Qt::Widgets
)
target_link_libraries(qspm PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS qspm
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET qspm
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
