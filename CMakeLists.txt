cmake_minimum_required(VERSION 3.15.0)

project(ExampleQt VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# locate packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(spdlog CONFIG REQUIRED)

set(LIBRARY_SOURCES lib/LayerCake.cpp)

add_library(StaticLibrary STATIC ${LIBRARY_SOURCES})
if (MSVC)
    # we apply the static CRT for MSVC
    set_property(TARGET StaticLibrary PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # dismiss several warnings
    target_compile_options(StaticLibrary PRIVATE -wd4996)
else()
    # dismiss several warnings
    target_compile_options(StaticLibrary PRIVATE -Wno-deprecated-declarations)
endif()
target_include_directories(StaticLibrary PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_libraries(StaticLibrary PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

set(LOCALIZATION_SOURCES    app/localization/LanguageManager.cpp 
                            app/localization/StringIDs.cpp
                            app/localization/English.cpp
                            app/localization/German.cpp)

set(APP_SOURCES             app/main.cpp
                            app/MainDlg.cpp
                            ${LOCALIZATION_SOURCES})

set(APP_RESOURCE res/resource.qrc)
if (MSVC)
    # include MSVS's native resource file
    set(APP_RESOURCE app/Application.rc ${APP_RESOURCE})
endif()

configure_file (version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/app/version.h)

add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_RESOURCE})

if (MSVC)
    # we apply the static CRT for MSVC
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    if (CMAKE_BUILD_TYPE MATCHES ".*Rel.*")
        # this magic setting makes our app native to Windows (just for Release builds)
        # otherwise we'll create a "console" app
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
    endif()
    # dismiss several warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -wd4996)
else()
    # dismiss several warnings
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations -Wnarrowing)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_libraries(${PROJECT_NAME} PRIVATE StaticLibrary)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

