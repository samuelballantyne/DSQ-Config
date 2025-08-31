cmake_minimum_required(VERSION 3.16)

project(DemulEASY VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow CMake to locate a local Qt package directory
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    emulatorutils.cpp
    emulatorutils.h
    IniSyntaxHighlighter.cpp
    IniSyntaxHighlighter.h
    moc_IniSyntaxHighlighter.cpp
)

set_source_files_properties(IniSyntaxHighlighter.cpp PROPERTIES SKIP_AUTOMOC ON)
set_source_files_properties(IniSyntaxHighlighter.h PROPERTIES SKIP_AUTOMOC ON)

qt_add_executable(DemulEASY
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(DemulEASY PRIVATE Qt6::Widgets)

include(GNUInstallDirs)
install(TARGETS DemulEASY
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(DemulEASY)
