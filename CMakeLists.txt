cmake_minimum_required(VERSION 3.16)

project(DemulEASY VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(DemulEASY
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(DemulEASY
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(DemulEASY PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

include(GNUInstallDirs)
install(TARGETS DemulEASY
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DemulEASY)
endif()
