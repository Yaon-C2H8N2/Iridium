cmake_minimum_required(VERSION 3.25)
project(Iridium)

set(Iridium_VERSION_MAJOR 1)
set(Iridium_VERSION_MINOR 0)
set(Iridium_VERSION_PATCH 2)
set(Iridium_VERSION "${Iridium_VERSION_MAJOR}.${Iridium_VERSION_MINOR}.${Iridium_VERSION_PATCH}")

configure_file(src/Config/Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Config/Version.h @ONLY)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")



set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        LinguistTools
        Widgets)
find_package(Boost REQUIRED)
find_package(rclone_cpp REQUIRED)
find_package(CURL REQUIRED)
find_package(libzip REQUIRED)

message(STATUS "Qt version: ${Qt6_VERSION}")

file(GLOB_RECURSE SRC
        src/*.cpp
        src/*.h
        src/*.hpp
)

include_directories(src
        src/Task
        src/Config
        src/FileView
        src/FileView/TreeWidgets
        src/LeftSideBar
        src/MainWindow
        src/Other
        src/Rclone
        src/Search
        src/Application
        src/Sync
        src/Remote
        src/Remote/AddNewRemote
        src/Remote/ListRemote
        src/MainWindow/Menu
        src/MainWindow/Menu/Settings)

file(GLOB_RECURSE TRANSLATION
        languages/iridium_*.ts
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else ()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Translations)
    set_source_files_properties(${TRANSLATION} PROPERTIES
            OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/Translations)
endif ()

qt_add_translation(QM_FILES ${TRANSLATION})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_source_files_properties("${QM_FILES}" PROPERTIES MACOSX_PACKAGE_LOCATION "Translations")
    set(MACOSX_BUNDLE_ICON_FILE Iridium.icns)
    set(myApp_ICON resources/Iridium.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    qt6_add_executable(${PROJECT_NAME} MACOSX_BUNDLE application.qrc ${myApp_ICON} ${SRC} ${QM_FILES})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(WIN32_ICO ${CMAKE_CURRENT_SOURCE_DIR}/MyApp.rc)
    qt6_add_executable(${PROJECT_NAME} WIN32 application.qrc ${WIN32_ICO} ${SRC} ${QM_FILES})
else ()
    qt6_add_executable(${PROJECT_NAME} application.qrc ${SRC} ${QM_FILES})
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        rclone_cpp::rclone_cpp
        boost::boost
        Qt6::Core Qt6::Gui Qt6::Widgets
        CURL::libcurl libzip::zip
)


function(set_min_macos_version target version)
    if (APPLE)
        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET ${version})
        target_compile_options(${target} PUBLIC "-mmacosx-version-min=${version}")
        target_link_options(${target} PUBLIC "-mmacosx-version-min=${version}")
    endif ()
endfunction()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(Iridium)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_min_macos_version(${PROJECT_NAME} 12.0)
    find_library(COCOA Cocoa)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCOA})
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_NAME} ${Iridium_VERSION}"
            MACOSX_BUNDLE_COPYRIGHT "© 2023 ${PROJECT_NAME}"
            MACOSX_BUNDLE_ICON_FILE Iridium.icns
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist.in"
    )
endif ()