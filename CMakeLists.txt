cmake_minimum_required(VERSION 3.15.0)

project(ExampleQt VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOCALIZATION_SOURCES "")

set(APP_SOURCES app/main.cpp
                ${LOCALIZATION_SOURCES})

configure_file (version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/app/version.h)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

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
