cmake_minimum_required(VERSION 3.20)
project(FreeML VERSION 0.0.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#[[ EXTERN ]]
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable REQUIRED NAMES glslc HINTS Vulkan::glslc)

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/public/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/*.c"
)
file(GLOB_RECURSE HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/private/*.h"
)

if (${BUILD_SHARED_LIBS})
    add_library(FreeML SHARED ${SOURCES} ${HEADERS})
else ()
    add_library(FreeML STATIC ${SOURCES} ${HEADERS})
endif ()
target_link_libraries(FreeML PUBLIC Vulkan::Vulkan)

if (MSVC)
    set_target_properties(FreeML PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} /W4")
else ()
    set_target_properties(FreeML PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

target_include_directories(FreeML PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(FreeML PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

add_subdirectory(tests)
