cmake_minimum_required(VERSION 3.24...3.30)
project(cauldron LANGUAGES C CXX)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
set(RAYLIB_VERSION 6.0)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

# Our Project
file(GLOB_RECURSE CORE_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.cxx)
list(FILTER CORE_SOURCE_FILES EXCLUDE REGEX "/main\\.cpp$")

add_library(cauldron_core STATIC ${CORE_SOURCE_FILES})
target_include_directories(cauldron_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
if (raylib_SOURCE_DIR)
    target_include_directories(cauldron_core SYSTEM PUBLIC ${raylib_SOURCE_DIR}/src)
endif()
target_compile_features(cauldron_core PUBLIC cxx_std_17)
target_link_libraries(cauldron_core PUBLIC raylib)

if (MSVC)
    target_compile_options(cauldron_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(cauldron_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE cauldron_core)

include(CTest)
if (BUILD_TESTING)
    add_executable(cauldron_tests tests/CombatTests.cpp)
    target_link_libraries(cauldron_tests PRIVATE cauldron_core)
    if (MSVC)
        target_compile_options(cauldron_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(cauldron_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME cauldron_combat_systems COMMAND cauldron_tests)
endif()

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
