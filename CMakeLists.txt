cmake_minimum_required(VERSION 3.14)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version" FORCE)
    find_program(BREW_EXECUTABLE brew HINTS /usr/local/bin)
    if ("${BREW_EXECUTABLE}" STREQUAL "BREW_EXECUTABLE-NOTFOUND")
        message(STATUS "macOS: Homebrew not found.")
    else ()
        message(STATUS "macOS: Homebrew found at ${BREW_EXECUTABLE}.")
        execute_process(COMMAND ${BREW_EXECUTABLE} --prefix qt OUTPUT_VARIABLE QT_PREFIX_PATH)
        string(REGEX REPLACE "\n$" "" QT_PREFIX_PATH_STRIPPED "${QT_PREFIX_PATH}")
        if ("${QT_PREFIX_PATH}" STREQUAL "")
            message(STATUS "macOS: Homebrew: Qt not installed.")
        else ()
            list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH_STRIPPED}")
            message(STATUS "macOS: Homebrew Qt at ${QT_PREFIX_PATH_STRIPPED}.")
        endif ()
    endif ()
endif ()

set(QTCREATOR_SRC "" CACHE PATH "Path to Qt Creator sources")
set(QTCREATOR_BIN "" CACHE FILEPATH "Path to Qt Creator binary")
set(QTCREATOR_VERSION "4.9.0" CACHE STRING "Target version of Qt Creator")

if (NOT EXISTS "${QTCREATOR_SRC}/src/qtcreatorplugin.pri")
    message(FATAL_ERROR "QTCREATOR_SRC must point to Qt Creator sources.")
endif ()

if (NOT EXISTS "${QTCREATOR_BIN}")
    message(FATAL_ERROR "QTCREATOR_BIN must point to the Qt Creator executable.")
endif ()

get_filename_component(QTCREATOR_BIN_DIR "${QTCREATOR_BIN}" DIRECTORY)

enable_language(CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0071 NEW)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

project(onedark LANGUAGES CXX VERSION 0.1.0)

configure_file("${CMAKE_SOURCE_DIR}/onedark.json.in" "${CMAKE_CURRENT_BINARY_DIR}/onedark.json")

add_library(onedark SHARED plugin.cpp onedark.qrc)

set_target_properties(onedark PROPERTIES AUTOMOC ON AUTORCC ON)

target_include_directories(onedark PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${Qt5Widgets_INCLUDE_DIRS}"
    "${QTCREATOR_SRC}/src/libs"
    "${QTCREATOR_SRC}/src/plugins"
    )

if (APPLE)
    find_library(EXTENSIONSYSTEM_LIB NAMES ExtensionSystem PATHS "${QTCREATOR_BIN_DIR}/../Frameworks")
    find_library(QTWIDGETS_LIB NAMES QtWidgets PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    find_library(QTGUI_LIB NAMES QtGui PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    find_library(QTCORE_LIB NAMES QtCore PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    target_link_options(onedark PRIVATE
        "-Wl,-install_name,@rpath/PlugIns/"
        "-compatibility_version ${QTCREATOR_VERSION}"
    )
else ()
    find_library(EXTENSIONSYSTEM_LIB NAMES ExtensionSystem PATHS "${QTCREATOR_BIN_DIR}/../lib/qtcreator")
    set(QTWIDGETS_LIB "${Qt5Widgets_LIBRARIES}")
    set(QTGUI_LIB "${Qt5Gui_LIBRARIES}")
    set(QTCORE_LIB "${Qt5Core_LIBRARIES}")
    set_target_properties(onedark PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
endif ()

target_link_libraries(onedark PRIVATE
    "${EXTENSIONSYSTEM_LIB}"
    "${QTWIDGETS_LIB}"
    "${QTGUI_LIB}"
    "${QTCORE_LIB}"
)

if (APPLE)
    install(TARGETS onedark DESTINATION "${QTCREATOR_BIN_DIR}/../PlugIns")
    install(CODE "execute_process(COMMAND \"sudo xattr -rd com.apple.quarantine ${QTCREATOR_BIN_DIR}/../../\")")
else ()
    install(TARGETS onedark DESTINATION "${QTCREATOR_BIN_DIR}/../lib/qtcreator/plugins")
endif ()
